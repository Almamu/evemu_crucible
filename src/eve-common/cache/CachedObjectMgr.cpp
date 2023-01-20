/*
    ------------------------------------------------------------------------------------
    LICENSE:
    ------------------------------------------------------------------------------------
    This file is part of EVEmu: EVE Online Server Emulator
    Copyright 2006 - 2021 The EVEmu Team
    For the latest information visit https://evemu.dev
    ------------------------------------------------------------------------------------
    This program is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any later
    version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
    this program; if not, write to the Free Software Foundation, Inc., 59 Temple
    Place - Suite 330, Boston, MA 02111-1307, USA, or go to
    http://www.gnu.org/copyleft/lesser.txt.
    ------------------------------------------------------------------------------------
    Author:     Zhur
*/

/** @todo this whole system needs updating....
 *
    [PyTuple 1 items]
      [PySubStream 134 bytes]
        [PyObjectData Name: objectCaching.CachedObject]
          [PyTuple 7 items]
            [PyTuple 2 items]
              [PyIntegerVar 129511422600825710]
              [PyInt 46587]
            [PyNone]
            [PyInt 704421]
            [PyInt 1]
            [PySubStream 34 bytes]
              [PyString "https://gate.eveonline.com/"]
            [PyInt 0]
            [PyTuple 3 items]
              [PyString "Method Call"]
              [PyString "server"]
              [PyTuple 2 items]
                [PyString "browserLockdownSvc"]
                [PyString "GetDefaultHomePage"]
    [PyNone]

    PyTuple* tuple = new PyTuple(7);
    tuple->SetItem(0, itr_1);
    tuple->SetItem(1, GenerateLockdownCachedObject());
    tuple->SetItem(2, new PyNone());

    // build the tuple based on above packet...may not need, as that is cached packet....
    PyTuple* first = new PyTuple(2);
        first->SetItem(0, new PyLong(Win32TimeNow()));
        first->SetItem(1, new PyInt(46587)); //unknown
    PyTuple* second = new PyTuple(3);
        second->SetItem(0, new PyString("Method Call"));
        second->SetItem(1, new PyString("server"));
    PyTuple* third = new PyTuple(2);
        third->SetItem(0, new PyString("browserLockdownSvc"));
        third->SetItem(1, new PyString("GetDefaultHomePage"));
        second->SetItem(2, third);
    PyTuple* data = new PyTuple(7);
        data->SetItem(0, first);
        data->SetItem(1, new PyNone()); //unknown
        data->SetItem(2, new PyInt(704421)); //unknown - nodeID?
        data->SetItem(3, PyStatic.NewOne()); //unknown
        data->SetItem(4, new PySubStream(new PyString("https://evemu.dev/")));
        data->SetItem(5, new PyInt(0)); //unknown
        data->SetItem(6, second);
    return new PyObject( "objectCaching.CachedMethodCallResult", data );

 */

#include "eve-common.h"

#include "python/Types.h"

#include "cache/CachedObjectMgr.h"
#include "marshal/EVEMarshal.h"
#include "marshal/EVEMarshalOpcodes.h"
#include "utils/EVEUtils.h"

const uint32 CacheFileMagic = 0xFF886622;
static const uint32 HackCacheNodeID = 333444;

CachedObjectMgr::~CachedObjectMgr ()
{
    for (auto& cur : m_cachedObjects)
        SafeDelete (cur.second);
}

/************************************************************************/
/* CacheRecord                                                          */
/************************************************************************/
CachedObjectMgr::CacheRecord::CacheRecord () :
    objectID (nullptr),
    timestamp(0),
    version(0),
    cache(nullptr) {
}

CachedObjectMgr::CacheRecord::~CacheRecord ()
{
    PyDecRef (objectID);
    PyDecRef (cache);
}

PyObject *CachedObjectMgr::CacheRecord::EncodeHint () const
{
    return new PyObject (
        new PyString ("util.CachedObject"),
        new PyTuple {
            objectID,
            new PyInt (HackCacheNodeID),
            new PyTuple {
                new PyInt (timestamp),
                new PyInt (version)
            },
            new PyBool (false)
        }
    );
}


/*
 * # Cache Logging:
 * CACHE=1
 * CACHE__ERROR=1
 * CACHE__WARNING=0
 * CACHE__MESSAGE=0
 * CACHE__DEBUG=0
 * CACHE__INFO=0
 * CACHE__TRACE=0
 * CACHE__DUMP=0
 */
//extract out the string contents of the object ID... if its a single string,
// then this visit will be boring, but if its a nested structure of strings,
// its more interesting, either way we should come out with some string...
std::string CachedObjectMgr::OIDToString (const PyDataType *objectID)
{
    StringCollapseVisitor v;

    if (!objectID->visit (v)) {
        sLog.Error ("Cached Obj Mgr", "Failed to convert cache hind object ID into collapsed string:");
        objectID->dump (CACHE__DUMP, "    ");
        assert (false);
        return "";
    }

    return v.result;
}

bool CachedObjectMgr::HaveCached (const std::string &objectID) const
{
    // this is very sub-optimal, but it keeps things more consistent (in case StringCollapseVisitor ever gets more complicated)
    PyString *str = new PyString (objectID);
    bool ret = HaveCached (str);
    PyDecRef (str);
    return ret;
}

bool CachedObjectMgr::HaveCached (const PyDataType *objectID) const
{
    const std::string str = OIDToString (objectID);

    return (m_cachedObjects.find(str) != m_cachedObjects.end());
}

void CachedObjectMgr::InvalidateCache (const PyDataType *objectID)
{
    const std::string str = OIDToString (objectID);
    CachedObjMapItr res = m_cachedObjects.find(str);

    if (res == m_cachedObjects.end ())
        return;

    SafeDelete (res->second);
    m_cachedObjects.erase (res);
}

//#define RAW_CACHE_CONTENTS

void CachedObjectMgr::UpdateCacheFromSS (const std::string &objectID, PySubStream **in_cached_data)
{
    PyCachedObjectDecoder cache;
    if (!cache.Decode (in_cached_data)) {
        sLog.Error("CachedObjMgr","Failed to decode stream");
        return;
    }

    PyString* str = new PyString (objectID);
    PyBuffer* buf = new PyBuffer (cache.cache->encoded ());
    _UpdateCache (str, &buf);

    PyDecRef( str );
}

void CachedObjectMgr::UpdateCache (const std::string &objectID, PyDataType **in_cached_data)
{
    PyString *str = new PyString (objectID);
    UpdateCache (str, in_cached_data);
    PyDecRef(str);
}

void CachedObjectMgr::UpdateCache (const PyDataType *objectID, PyDataType **in_cached_data)
{
    PyDataType* cached_data(*in_cached_data);
    *in_cached_data = nullptr;

    //if (is_log_enabled(CACHE__DUMP)) {
    //  PyLogsysDump dumper(CACHE__DUMP, CACHE__DUMP, false, true);
        //cached_data->visit(&dumper, 0);
    //}

    Buffer* buf = new Buffer();
    bool res = MarshalDeflate (cached_data, *buf);

    if (res) {
        PyBuffer* pbuf = new PyBuffer (&buf);
        _UpdateCache (objectID, &pbuf);
    } else {
        sLog.Error ("Cached Obj Mgr", "Failed to marshal or deflate new cache object.");
    }

    SafeDelete (buf);
}

void CachedObjectMgr::_UpdateCache (const PyDataType *objectID, PyBuffer **pbuf)
{
    //this is the hard one..
    CacheRecord *r = new CacheRecord();
    r->timestamp = GetFileTimeNow();
    r->objectID = objectID;

    // retake ownership
    r->cache = *pbuf;
    *pbuf = nullptr;

    r->version = CRC32::Generate( &r->cache->content()[0], r->cache->content().size() );

    const std::string str = OIDToString(objectID);

    //find and destroy any older version of this object.
    CachedObjMapItr res = m_cachedObjects.find(str);

    if (res != m_cachedObjects.end()) {
        sLog.Debug("CachedObjMgr","Destroying old cached object with ID '%s' of length %u with checksum 0x%x", str.c_str(), res->second->cache->content().size(), res->second->version);
        SafeDelete( res->second );
    }

    sLog.Debug("CachedObjMgr","Registering new cached object with ID '%s' of length %u with checksum 0x%x", str.c_str(), r->cache->content().size(), r->version);

    m_cachedObjects[str] = r;
}

PyObject *CachedObjectMgr::MakeCacheHint (const std::string &objectID)
{
    //this is sub-optimal, but it keeps things more consistent (in case StringCollapseVisitor ever gets more complicated)
    PyString* str = new PyString (objectID);
    PyObject* obj (MakeCacheHint (str));
    PyDecRef (str);
    return obj;
}

PyObject *CachedObjectMgr::MakeCacheHint (const PyDataType *objectID)
{
    const std::string str = OIDToString (objectID);

    CachedObjMapItr res = m_cachedObjects.find (str);

    if (res == m_cachedObjects.end ())
        return nullptr;

    return res->second->EncodeHint ();
}

PyObject *CachedObjectMgr::GetCachedObject (const std::string &objectID)
{
    //this is sub-optimal, but it keeps things more consistent (in case StringCollapseVisitor ever gets more complicated)
    PyString* str = new PyString (objectID);
    PyObject* obj (GetCachedObject (str));
    PyDecRef (str);
    return obj;
}

PyObject *CachedObjectMgr::GetCachedObject (const PyDataType *objectID)
{
    const std::string str = OIDToString (objectID);

    CachedObjMapItr res = m_cachedObjects.find (str);

    if (res == m_cachedObjects.end ())
        return nullptr;

    PyCachedObject co;
    co.timestamp = res->second->timestamp;
    co.version = res->second->version;
    co.nodeID = HackCacheNodeID;    //hack, doesn't matter until we have multi-node networks.
    co.shared = true;
    co.objectID = res->second->objectID;
    co.cache = res->second->cache;

    if (res->second->cache->content ().size () == 0 || res->second->cache->content () [0] == MarshalHeaderByte)
        co.compressed = false;
    else
        co.compressed = true;

    sLog.Debug ("CachedObjMgr","Returning cached object '%s' with checksum 0x%x", str.c_str (), co.version);

    PyObject* result = co.Encode ();
    co.cache = nullptr;    //avoid a copy

    return result;
}

bool CachedObjectMgr::IsCacheUpToDate (const PyDataType *objectID, uint32 version, int64 timestamp)
{
    const std::string str = OIDToString (objectID);

    CachedObjMapItr res = m_cachedObjects.find (str);

    if (res == m_cachedObjects.end())
        return false;

    //for now, only support exact matches...
    return res->second->version == version &&
           res->second->timestamp == timestamp;
}

bool CachedObjectMgr::LoadCachedFromFile (const std::string &cacheDir, const std::string &objectID)
{
    //this is sub-optimal, but it keeps things more consistent (in case StringCollapseVisitor ever gets more complicated)
    PyString* str = new PyString (objectID);
    bool ret = LoadCachedFromFile (cacheDir, str);
    PyDecRef (str);
    return ret;
}

/**
 * LoadCachedFromFile
 *
 * Load a cached object from file.
 */
bool CachedObjectMgr::LoadCachedFromFile (const std::string &cacheDir, const PyDataType *objectID)
{
    const std::string str = OIDToString (objectID);

    std::string filename (cacheDir);
    filename += "/" + str + ".cache";

    FILE *f = fopen (filename.c_str (), "rb");

    if (f == nullptr)
        return false;

    CacheFileHeader header;
    if (fread (&header, sizeof (header), 1, f) != 1) {
        fclose (f);
        return false;
    }

    /* check if its a valid cache file */
    if (header.magic != CacheFileMagic) {
        fclose(f);
        return false;
    }

    Buffer* buf = new Buffer (header.length);

    if ( fread (&(*buf) [0], sizeof (uint8), header.length, f) != header.length) {
        SafeDelete (buf);
        fclose (f);
        return false;
    }

    fclose (f);

    CachedObjMapItr res = m_cachedObjects.find (str);

    if (res != m_cachedObjects.end ())
        SafeDelete (res->second);

    CacheRecord* cache = m_cachedObjects [str] = new CacheRecord;
    cache->objectID = objectID;
    cache->cache = new PyBuffer (&buf);
    cache->timestamp = header.timestamp;
    cache->version = header.version;

    m_cachedObjects [str] = cache;

    SafeDelete (buf);
    return true;
}

//this is sub-optimal, but it keeps things more consistent (in case StringCollapseVisitor ever gets more complicated)
bool CachedObjectMgr::SaveCachedToFile (const std::string &cacheDir, const std::string &objectID) const
{
    PyString* str = new PyString (objectID);
    bool ret = SaveCachedToFile (cacheDir, str);
    PyDecRef (str);
    return ret;
}

bool CachedObjectMgr::SaveCachedToFile (const std::string &cacheDir, const PyDataType *objectID) const
{
    const std::string str = OIDToString (objectID);
    CachedObjMapConstItr res = m_cachedObjects.find (str);

    /* make sure we don't try to save a object we don't have */
    if (res == m_cachedObjects.end ())
        return false;

    std::string filename (cacheDir);
    filename += "/";
    filename += str;
    filename += ".cache";

    FILE *f = fopen (filename.c_str (), "wb");

    if (f == nullptr)
        return false;

    CacheFileHeader header;
    header.timestamp = res->second->timestamp;
    header.version = res->second->version;
    header.magic = CacheFileMagic;
    header.length = res->second->cache->content ().size ();

    if (fwrite (&header, sizeof (header), 1, f) != 1) {
        fclose (f);
        return false;
    }

    if (fwrite (&res->second->cache->content () [0], sizeof (uint8), header.length, f) != header.length) {
        assert (false);
        fclose (f);
        return false;
    }
    fclose (f);
    return true;
}

/*
void CachedObjectMgr::AddCacheHint(const char *oname, const char *key, PyDict *into) {
    PyDataType *t = _MakeCacheHint(oname);
    if (t == nullptr)
        return;
    into->add(key, t);
}
*/


/*bool CachedObjectMgr::AddCachedFileContents(const char *filename, const char
    *oname, PySubStream *into) { PySubStream *cache;
    if (!LoadCachedFile(filename, oname, cache))
        return false;

}*/

PySubStream* CachedObjectMgr::LoadCachedFile (const char *obj_name)
{
    PyString* str = new PyString (obj_name);
    PySubStream* ret = LoadCachedFile (str, obj_name);
    PyDecRef (str);
    return ret;
}

PySubStream *CachedObjectMgr::LoadCachedFile (PyDataType *key, const char *oname)
{
    std::string fname;
    GetCacheFileName (key, fname);

    std::string abs_fname = "../data/cache/";
    abs_fname += fname;

    return LoadCachedFile (abs_fname.c_str (), oname);
}

PySubStream* CachedObjectMgr::LoadCachedFile (const char* abs_fname, const char* oname)
{
    FILE* f = fopen (abs_fname, "rb");

    if (f == nullptr) {
        sLog.Error ("CachedObjMgr","Unable to open cache file '%s' for oname '%s': %s", abs_fname, oname, strerror (errno));
        return 0;
    }

    int64 file_length = filesize (f);
    if (file_length == 0)
    {
        sLog.Error ("CachedObjMgr","Unable to stat cache file '%s' for oname '%s'", abs_fname, oname);
        fclose (f);
        return 0;
    }

    Buffer* buf = new Buffer (static_cast <size_t> (file_length));

    if (file_length != fread (&(*buf)[0], sizeof (uint8), static_cast <size_t> (file_length), f)) {
        sLog.Error ("CachedObjMgr","Unable to read cache file '%s' for oname '%s%': %s", abs_fname, oname, strerror (errno));
        SafeDelete (buf);
        fclose (f);
        return 0;
    }

    fclose (f);
    sLog.Debug ("CachedObjMgr","Loaded cache file for '%s': length %u", oname, file_length);

    /** @todo Mem leak.  `new PyBuffer()` never freed */
    PySubStream* res = new PySubStream (new PyBuffer (&buf));
    SafeDelete (buf);
    return res;
}

PyCachedObjectDecoder *CachedObjectMgr::LoadCachedObject (const char *filename, const char *oname)
{
    PySubStream* ss = LoadCachedFile (filename, oname);

    if (ss == nullptr )
        return nullptr;

    PyCachedObjectDecoder *obj = new PyCachedObjectDecoder ();

    if (!obj->Decode(&ss)) {   //ss is consumed.
        SafeDelete( obj );
        return nullptr;
    }

    return obj;
}

PyCachedCall *CachedObjectMgr::LoadCachedCall (const char *filename, const char *oname)
{
    PySubStream* ss = LoadCachedFile(filename, oname);

    if (ss == nullptr)
        return nullptr;

    PyCachedCall *obj = new PyCachedCall ();

    if (!obj->Decode(&ss)) {   //ss is consumed.
        SafeDelete( obj );
        return nullptr;
    }

    return obj;
}

// Base64 encoding utilities
#include <Base64.h>
void CachedObjectMgr::GetCacheFileName (PyDataType *key, std::string &into)
{
    Buffer data;
    Marshal (key, data);

    Base64::encode (&data[0], data.size (), into, false);

    std::string::size_type epos = into.find ('=');
    if (epos != std::string::npos)
        into.resize(epos);

    into += ".cache";
}

/*
PyDataType *CachedObjectMgr::_MakeCacheHint(const char *oname) {
    //this is a ton of work we are doing here to generate a simple hint, but
    //theres not really a better way to do it while we are using the on-disk
    //cache files as the data source.
    PyCachedObject *obj = LoadCachedObject(oname);
    if (obj == nullptr) {
        _log(SERVICE__ERROR, "Unable to load cache file for '%s' in order to build hint", oname);
        return nullptr;
    }

    PyDataType *hint = obj->EncodeHint();
    delete obj;
    return(hint);
}
*/

PyCachedObjectDecoder::PyCachedObjectDecoder ()
: timestamp (0),
  version (0),
  nodeID (0),
  shared (false),
  cache (nullptr),
  compressed (false),
  objectID (nullptr)
{
}

PyCachedObjectDecoder::~PyCachedObjectDecoder ()
{
    PySafeDecRef (cache);
    PySafeDecRef (objectID);
}

PyCachedObject::PyCachedObject ()
: timestamp (0),
  version (0),
  nodeID (0),
  shared (false),
  cache (nullptr),
  compressed (false),
  objectID (nullptr)
{
}

PyCachedObject::~PyCachedObject ()
{
    PySafeDecRef (cache);
    PySafeDecRef (objectID);
}

PyCachedObject *PyCachedObject::Clone () const
{
    PyCachedObject *res = new PyCachedObject();
    res->timestamp = timestamp;
    res->version  = version;
    res->nodeID  = nodeID;
    res->shared = shared;
    res->cache = (PyBuffer *) cache;
    res->compressed = compressed;
    res->objectID = objectID;
    return res;
}

void PyCachedObjectDecoder::Dump (FILE *into, const char *pfx, bool contents_too)
{
    std::string s(pfx);
    s += "    ";
    fprintf(into, "%sCached Object:\n", pfx);
    fprintf(into, "%s  ObjectID:\n", pfx);
    objectID->dump(into, s.c_str());
    fprintf(into, "%s  Version Time: %" PRIu64 "\n", pfx, timestamp);
    fprintf(into, "%s  Version: %u\n", pfx, version);
    fprintf(into, "%s  NodeID: %u\n", pfx, nodeID);
    fprintf(into, "%s  Shared: %s\n", pfx, shared?"yes":"no");
    fprintf(into, "%s  Compressed: %s\n", pfx, compressed?"yes":"no");
    if (contents_too) {
        fprintf(into, "%s  Contents:\n", pfx);
            cache->dump(into, s.c_str());
    }
}

void PyCachedObject::Dump (FILE *into, const char *pfx, bool contents_too)
{
    std::string s(pfx);
    s += "    ";
    fprintf(into, "%sCached Object:\n", pfx);
    fprintf(into, "%s  ObjectID:\n", pfx);
        objectID->dump(into, s.c_str());
    fprintf(into, "%s  Version Time: %" PRIu64 "\n", pfx, timestamp);
    fprintf(into, "%s  Version: %u\n", pfx, version);
    fprintf(into, "%s  NodeID: %u\n", pfx, nodeID);
    fprintf(into, "%s  Shared: %s\n", pfx, shared?"yes":"no");
    fprintf(into, "%s  Compressed: %s\n", pfx, compressed?"yes":"no");
    if (contents_too) {
        fprintf(into, "%s  Contents:\n", pfx);
            cache->dump(into, s.c_str());
    }
}

bool PyCachedObjectDecoder::Decode(PySubStream **in_ss)
{
    PySubStream *ss = *in_ss;    //consume
    *in_ss = nullptr;

    PySafeDecRef (cache);
    PySafeDecRef (objectID);

    if (!ss->decoded()->Is <PyObject> ()) {
        sLog.Error("PyCachedObjectDecoder","Cache substream does not contain an object: %s", ss->decoded()->TypeString());

        PyDecRef( ss );
        return false;
    }
    PyObject *po = (PyObject *) ss->decoded();
    //TODO: could check type string, dont care... (should be objectCaching.CachedObject)

    if (!po->arguments()->Is <PyTuple> ()) {
        sLog.Error("PyCachedObjectDecoder","Cache object's args is not a tuple: %s", po->arguments()->TypeString());

        PyDecRef( ss );
        return false;
    }
    PyTuple *args = (PyTuple *) po->arguments();

    if (args->size () != 7) {
        sLog.Error("PyCachedObjectDecoder","Cache object's args tuple has %lu elements instead of 7", args->size());

        PyDecRef( ss );
        return false;
    }

    PyTuple* objVt = args->at <PyTuple> (0);
    PyInt *nodeidr = args->at <PyInt> (2);
    PyInt *sharedr = args->at <PyInt> (3);
    PyInt *compressedr = args->at <PyInt> (5);
    PyInt *timer = objVt->at <PyInt> (0);
    PyInt *versionr = objVt->at <PyInt> (1);
    PyDataType* fourth = args->at (4);

    timestamp = timer->value();
    version = versionr->value();
    nodeID = nodeidr->value();
    shared = sharedr->value() != 0;
    compressed = compressedr->value() != 0;

    //content (do this as the last thing, since its the heavy lifting):
    if (fourth->Is <PySubStream> ()) {
        cache = fourth->As <PySubStream> ();
    } else if (fourth->Is <PyBuffer> ()) {
        //this is a data buffer, likely compressed.
        PyBuffer* buf = fourth->As <PyBuffer> ();
        cache = new PySubStream (buf->content ());
    } else if (fourth->Is <PyString> ()) {
        //this is a data buffer, likely compressed, not sure why it comes through as a string...
        cache = new PySubStream (Buffer (fourth->string ().begin (), fourth->string ().end ()));
    } else {
        sLog.Error ("PyCachedObjectMgr", "Cache object's arg %d is not a substream or buffer: %s", 4, fourth->TypeString ());
        PyDecRef (ss);
        return false;
    }

    objectID = args->at (6);

    PyDecRef (ss);

    return true;
}

PyObject *PyCachedObject::Encode()
{
    return new PyObject (
        new PyString ("objectCaching.CachedObject"),
        new PyTuple {
            new PyTuple {new PyInt (timestamp), new PyInt (version)},
            PyStatic.NewNone (),
            new PyInt (nodeID),
            new PyInt (shared ? 1 : 0),
            cache,
            new PyInt (compressed ? 1 : 0),
            objectID
        }
    );
}

PyObject *PyCachedObjectDecoder::EncodeHint() {
    return new PyObject (
        new PyString ("util.CachedObject"),
        new PyTuple {
            objectID,
            new PyInt (nodeID),
            new PyTuple {
                new PyInt (timestamp),
                new PyInt (version)
            }
        }
    );
}


PyCachedCall::PyCachedCall () : result(nullptr) {}
PyCachedCall::~PyCachedCall ()
{
    PySafeDecRef (result);
}

PyCachedCall *PyCachedCall::Clone() const {
    PyCachedCall *res = new PyCachedCall();
    res->result = result;
    return res;
}

void PyCachedCall::Dump(FILE *into, const char *pfx, bool contents_too)
{
    std::string s(pfx);
    s += "    ";
    fprintf(into, "%sCached Call: (empty right now) \n", pfx);
    if (contents_too) {
        fprintf(into, "%s  Contents:\n", pfx);
        result->dump(into, s.c_str());
    }
}

bool PyCachedCall::Decode(PySubStream **in_ss)
{
    PySubStream *ss = *in_ss;    //consume
    *in_ss = nullptr;

    PySafeDecRef (result);

    for (const auto cur : *ss->decoded ()->As <PyDict> ()) {
        if (cur.first->Is <PyString> () == false)
            continue;
        if (cur.first->string () == "lret")
            result = cur.second;
    }

    PyDecRef (ss);

    return result != nullptr;
}
