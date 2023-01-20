/**
 * @name BulkMgrService.h
 *   This file is for processing bulkdata
 *   Copyright 2017  Alasiya-EVEmu Team
 *
 * @Author:    ozatomic (hacked for static client data)
 * @Author:    Allan (added calls and (hacked) updates for new dgm data)
 * @date:      24 January 2017
 *
 */

#ifndef __BULKMGR_SERVICE_H_INCL__
#define __BULKMGR_SERVICE_H_INCL__

#include "services/Service.h"
#include "cache/BulkDB.h"

class BulkMgrService : public Service <BulkMgrService>
{
public:
    BulkMgrService();

protected:
  EVEResult UpdateBulk(EVECallArgs& call, PyInt* changeID, std::optional<PyString*> hashValue, PyInt* branch);
  EVEResult GetFullFiles(EVECallArgs& call, std::optional<PyList*> toGet);
  EVEResult GetFullFilesChunk(EVECallArgs& call, PyInt* chunkSetID, PyInt* chunkNumber);
  EVEResult GetVersion(EVECallArgs& call);
  EVEResult GetAllBulkIDs(EVECallArgs& call);
  EVEResult GetChunk(EVECallArgs& call, PyInt* changeID, PyInt* chunkNumber);
  EVEResult GetUnsubmittedChunk(EVECallArgs& call, PyInt* chunkNumber);
  EVEResult GetUnsubmittedChanges(EVECallArgs& call);

private:

    enum bulkStatus {
        updateBulkStatusOK               = 0,   // client data == server data  - no change
        updateBulkStatusWrongBranch      = 1,   // client != server.  calls GetFullFiles then GetVersion
        updateBulkStatusHashMismatch     = 2,   // client missing files  - compares server (returned) fileIDs with local fileIDs
        updateBulkStatusClientNewer      = 3,   // client != server.  calls GetFullFiles then GetVersion
        updateBulkStatusNeedToUpdate     = 4,   // this one will be complicated.  see notes in cpp
        updateBulkStatusTooManyRevisions = 5    // server has too many updates to bring client files up-to-date.  calls GetFullFiles then GetVersion
    };
};

#endif

