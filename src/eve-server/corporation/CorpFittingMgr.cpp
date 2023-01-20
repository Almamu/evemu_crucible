/*
 *
 *
 * note:  these will be same calls as in charFittingMgr
 */

//work in progress


#include "eve-server.h"


#include "corporation/CorpFittingMgr.h"

CorpFittingMgr::CorpFittingMgr() :
    Service("corpFittingMgr", eAccessLevel_Corporation)
{
    this->Add("GetFittings", &CorpFittingMgr::GetFittings);
    this->Add("SaveFitting", &CorpFittingMgr::SaveFitting);
    this->Add("SaveManyFittings", &CorpFittingMgr::SaveManyFittings);
    this->Add("DeleteFitting", &CorpFittingMgr::DeleteFitting);
    this->Add("UpdateNameAndDescription", &CorpFittingMgr::UpdateNameAndDescription);
}

EVEResult CorpFittingMgr::GetFittings(EVECallArgs&call, PyInt* ownerID)
{
    //self.fittings[ownerID] = self.GetFittingMgr(ownerID).GetFittings(ownerID)
    _log(CORP__CALL, "CorpFittingMgr::Handle_GetFittings()");
    call.dump(CORP__CALL_DUMP);

    return nullptr;
}

EVEResult CorpFittingMgr::SaveFitting(EVECallArgs&call, PyInt* ownerID, PyObject* fitting)
{
    //    fitting.ownerID = ownerID
    //    fitting.fittingID = self.GetFittingMgr(ownerID).SaveFitting(ownerID, fitting)
    _log(CORP__CALL, "CorpFittingMgr::Handle_SaveFitting()");
    call.dump(CORP__CALL_DUMP);

    return nullptr;
}

EVEResult CorpFittingMgr::SaveManyFittings(EVECallArgs&call, PyInt* ownerID, PyDict* fittingsToSave)
{
    /*
        newFittingIDs = self.GetFittingMgr(ownerID).SaveManyFittings(ownerID, fittingsToSave)
        for row in newFittingIDs:
            self.fittings[ownerID][row.realFittingID] = fittingsToSave[row.tempFittingID]
            self.fittings[ownerID][row.realFittingID].fittingID = row.realFittingID
        */
    _log(CORP__CALL, "CorpFittingMgr::Handle_SaveManyFittings()");
    call.dump(CORP__CALL_DUMP);

    return nullptr;
}

EVEResult CorpFittingMgr::DeleteFitting(EVECallArgs&call, PyInt* ownerID, PyInt* fittingID)
{
    // self.GetFittingMgr(ownerID).DeleteFitting(ownerID, fittingID)
    _log(CORP__CALL, "CorpFittingMgr::Handle_DeleteFitting()");
    call.dump(CORP__CALL_DUMP);

    return nullptr;
}

EVEResult CorpFittingMgr::UpdateNameAndDescription(EVECallArgs&call, PyInt* fittingID, PyInt* ownerID, PyString* name, PyString* description)
{
    // self.GetFittingMgr(ownerID).UpdateNameAndDescription(fittingID, ownerID, name, description)
    _log(CORP__CALL, "CorpFittingMgr::Handle_UpdateNameAndDescription()");
    call.dump(CORP__CALL_DUMP);

    return nullptr;
}

