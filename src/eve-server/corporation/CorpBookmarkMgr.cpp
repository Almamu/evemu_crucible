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
    Author:        Ubiquitatis
    Rewrite:    Allan
*/

/* the distinction between bookmark Mgrs (corp/personal) is made on-the-fly when methods are called in client.
 * most (if not all) will use same code and returns.  so far, i havent seen a difference between them.
 * this may be used to separate corp and personal bm in db on live.
 * not sure if im gonna do that here or not yet.
 */

#include "eve-server.h"

#include "Client.h"

#include "packets/Bookmarks.h"
#include "cache/ObjCacheService.h"
#include "corporation/CorpBookmarkMgr.h"

CorpBookmarkMgr::CorpBookmarkMgr() :
    Service("corpBookmarkMgr", eAccessLevel_Corporation)
{
    this->Add("GetBookmarks", &CorpBookmarkMgr::GetBookmarks);
    this->Add("UpdateBookmark", &CorpBookmarkMgr::UpdateBookmark);
    this->Add("UpdatePlayerBookmark", &CorpBookmarkMgr::UpdatePlayerBookmark);
    this->Add("MoveBookmarksToFolder", &CorpBookmarkMgr::MoveBookmarksToFolder);
    this->Add("CreateFolder", &CorpBookmarkMgr::CreateFolder);
    this->Add("UpdateFolder", &CorpBookmarkMgr::UpdateFolder);
    this->Add("CopyBookmarks", &CorpBookmarkMgr::CopyBookmarks);
    this->Add("DeleteFolder", &CorpBookmarkMgr::DeleteFolder);
    this->Add("MoveFoldersToDB", &CorpBookmarkMgr::MoveFoldersToDB);
    this->Add("DeleteBookmarks", &CorpBookmarkMgr::DeleteBookmarks);
}

EVEResult CorpBookmarkMgr::GetBookmarks(EVECallArgs& call)
{
    /*
    ObjectCachedMethodID method_id(GetName(), "GetBookmarks");
    if (!m_manager->cache_service->IsCacheLoaded(method_id)) {
        PyTuple *tuple = new PyTuple(2);
            tuple->SetItem(0, m_db.GetBookmarks(call.client->GetCorporationID()));
            tuple->SetItem(1, m_db.GetFolders(call.client->GetCorporationID()));
        PyDataType* rep = tuple;

        m_manager->cache_service->GiveCache(method_id, &rep);
    }

    return(m_manager->cache_service->MakeObjectCachedMethodCallResult(method_id));
    */
    PyTuple* result = call.arena.Tuple ({
        m_db.GetBookmarks (call.client->GetCorporationID(), &call.arena),
        m_db.GetFolders (call.client->GetCorporationID(), &call.arena)
    });
    result->dump(BOOKMARK__RSP_DUMP, "    ");
    return result;
}

EVEResult CorpBookmarkMgr::UpdateBookmark(EVECallArgs& call, PyInt* bookmarkID, PyInt* ownerID, PyDataType* memo, PyDataType* comment, std::optional<PyInt*> folderID) {
    call.dump(BOOKMARK__CALL_DUMP);

    m_db.UpdateBookmark(bookmarkID->value(), ownerID->value(), folderID.has_value() ? folderID.value()->value() : 0, memo, comment);

    return PyStatic.NewNone();
}

EVEResult CorpBookmarkMgr::UpdatePlayerBookmark(EVECallArgs& call, PyInt* bookmarkID, PyInt* ownerID, PyDataType* memo, PyDataType* comment, std::optional<PyInt*> folderID) {
    call.dump(BOOKMARK__CALL_DUMP);

    m_db.UpdateBookmark(bookmarkID->value(), ownerID->value(), folderID.has_value() ? folderID.value()->value() : 0, memo, comment);

    return PyStatic.NewNone();
}

EVEResult CorpBookmarkMgr::MoveBookmarksToFolder(EVECallArgs& call, PyInt* folderID, std::optional<PyObjectEx*> bookmarkIDs)
{
    // rows = bookmarkMgr.MoveBookmarksToFolder(folderID, bookmarkIDs)
    call.dump(BOOKMARK__CALL_DUMP);
    //args.Dump(BOOKMARK__CALL_DUMP, "    ");

    if (bookmarkIDs.has_value() == false)
        return PyStatic.NewNone();

    PyList* bmList = bookmarkIDs.value()->header()->as<PyTuple>()->at (1)->as<PyTuple>()->at (0)->as<PyList>();

    std::vector<int32> bmIDs;
    for (size_t i = 0; i < bmList->size(); ++i)
        bmIDs.push_back(bmList->at (i)->as<PyInt>()->value());

    m_db.MoveBookmarkToFolder(folderID->value(), bmIDs);

    return m_db.GetBookmarksInFolder(folderID->value());
}

EVEResult CorpBookmarkMgr::UpdateFolder(EVECallArgs& call, PyInt* folderID, PyDataType* folderName)
{
    //if bookmarkMgr.UpdateFolder(folderID, folderName):
    call.dump(BOOKMARK__CALL_DUMP);
    /** @todo sanitize name */
    if (!m_db.UpdateFolder(folderID->value(), folderName->string()))
        return PyStatic.NewFalse();

    return PyStatic.NewTrue();
}

EVEResult CorpBookmarkMgr::CreateFolder(EVECallArgs& call, PyDataType* folderName)
{
    //folder = bookmarkMgr.CreateFolder(folderName)
    call.dump(BOOKMARK__CALL_DUMP);
    /** @todo sanitize name */
    std::string name = folderName->string();

    uint32 ownerID = call.client->GetCorporationID();
    Rsp_CreateFolder result;
        result.ownerID = ownerID;
        result.folderID = m_db.SaveNewFolder(name, ownerID, call.client->GetCharacterID());
        result.folderName = name;
        result.creatorID = call.client->GetCharacterID();

    result.Dump(BOOKMARK__RSP_DUMP, "    ");
    return result.Encode();
}

EVEResult CorpBookmarkMgr::CopyBookmarks(EVECallArgs& call, std::optional<PyDataType*> bookmarksToCopy, PyInt* folderID)
{
    call.dump(BOOKMARK__CALL_DUMP);
    return PyStatic.NewNone();
}

EVEResult CorpBookmarkMgr::DeleteFolder(EVECallArgs& call, PyInt* folderID, PyDataType* unused)
{
    //deleteFolder, bookmarks = self.corpBookmarkMgr.DeleteFolder(folderID, bookmarkIDs)
    call.dump(BOOKMARK__CALL_DUMP);

    // call db to get list of bmIDs in deleted folder.  return result with this data
    std::vector< int32 > bmIDs;
    bmIDs.clear();
    m_db.GetBookmarkByFolderID(folderID->value(), bmIDs);

    PyList* list = new PyList();
    for (auto cur : bmIDs) {
        list->add(
            new PyObject(
                "util.KeyVal",
                new PyDict {
                    {"bookmarkID", new PyInt (cur)}
                }
            )
        );
    }

    m_db.DeleteFolder(folderID->value());

    return list;
}

EVEResult CorpBookmarkMgr::MoveFoldersToDB(EVECallArgs& call, PyDataType* info)
{
    //rows, folders = self.corpBookmarkMgr.MoveFoldersToDB(info)
    call.dump(BOOKMARK__CALL_DUMP);
    return PyStatic.NewNone();
}

EVEResult CorpBookmarkMgr::DeleteBookmarks(EVECallArgs& call, std::optional<PyObjectEx*> bookmarkIDs)
{
    //deletedBookmarks = self.corpBookmarkMgr.DeleteBookmarks(bookmarkIDs)
    call.dump(BOOKMARK__CALL_DUMP);

    if (bookmarkIDs.has_value() == false)
        return PyStatic.NewNone();

    PyList* bmList = bookmarkIDs.value()->header()->as<PyTuple>()->at (1)->as<PyTuple>()->at (0)->as<PyList>();

    PyList* list = new PyList();
    for (size_t i = 0; i < bmList->size(); ++i) {
        uint32 bmID = bmList->at (i)->as<PyInt>()->value();
        m_db.ChangeOwner(bmID, 1);
        list->add(
            new PyObject(
                "util.KeyVal",
                new PyDict {
                    {"bookmarkID", new PyInt (bmID)}
                }
            )
        );
    }
    return list;
}
