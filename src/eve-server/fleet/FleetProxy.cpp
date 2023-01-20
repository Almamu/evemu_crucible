
 /**
  * @name FleetProxy.cpp
  *     Fleet Proxy code for EVEmu
  *     This file deals with very generic fleet functions
  *
  * @Author:        Allan
  * @date:          05 August 2014 (original skeleton outline)
  * @update:        21 November 2017 (begin actual implementation)
  * @finished:      02 December 2017
  *
  */

#include "eve-server.h"

#include "fleet/FleetProxy.h"

FleetProxy::FleetProxy() :
    Service("fleetProxy")
{
    this->Add("GetAvailableFleets", &FleetProxy::GetAvailableFleets);
    this->Add("AddFleetFinderAdvert", &FleetProxy::AddFleetFinderAdvert);
    this->Add("RemoveFleetFinderAdvert", &FleetProxy::RemoveFleetFinderAdvert);
    this->Add("GetMyFleetFinderAdvert", &FleetProxy::GetMyFleetFinderAdvert);
    this->Add("UpdateAdvertInfo", &FleetProxy::UpdateAdvertInfo);
    this->Add("ApplyToJoinFleet", &FleetProxy::ApplyToJoinFleet);
}

/*
FLEET__ERROR
FLEET__WARNING
FLEET__MESSAGE
FLEET__DEBUG
FLEET__INFO
FLEET__TRACE
FLEET__DUMP
FLEET__BIND_DUMP
*/
EVEResult FleetProxy::GetAvailableFleets(EVECallArgs&call) {
    return sFltSvc.GetAvailableFleets();
}

EVEResult FleetProxy::ApplyToJoinFleet(EVECallArgs&call, PyInt* fleetID) {
  // ret = sm.ProxySvc('fleetProxy').ApplyToJoinFleet(fleetID)
   // sLog.White("FleetProxy", "Handle_ApplyToJoinFleet() size=%lli", call.tuple->size());
   // call.dump(FLEET__DUMP);

    // returns boolean
    return new PyBool(sFltSvc.AddJoinRequest(fleetID->value(), call.client));
}

    // this is also used to update advert info
EVEResult FleetProxy::AddFleetFinderAdvert(EVECallArgs&call, PyObject* info) {
 //  sm.ProxySvc('fleetProxy').AddFleetFinderAdvert(info)
    //sLog.White("FleetProxy", "Handle_AddFleetFinderAdvert() size=%lli", call.tuple->size());
    //call.dump(FLEET__DUMP);

    /** @todo  this needs to be updated....check standings and verifly scope */

    FleetAdvertCall args;
    if (!args.Decode(&call.tuple)) {
        codelog(SERVICE__ERROR, "%s: Failed to decode args.", call.client->GetChar()->name());
        return nullptr;
    }

    int32 fleetID = call.client->GetChar()->fleetID();

    FleetData fData = FleetData();
    sFltSvc.GetFleetData(fleetID, fData);

    // TODO: update this sometime in the future, updating this is way out of the scope of the service changes
    FleetAdvert adata = FleetAdvert();
        adata.fleetID = fleetID;
        adata.hideInfo = args.hideInfo;
        adata.inviteScope = args.inviteScope;
        adata.leader = call.client;
        adata.fleetName = args.fleetName->string();
        adata.advertTime = GetFileTimeNow();
        adata.dateCreated = fData.dateCreated;
        adata.description = args.description->string();
        adata.solarSystemID = call.client->GetSystemID();
        adata.joinNeedsApproval = args.joinNeedsApproval;
        adata.local_minSecurity = args.local_minSecurity;
        adata.local_minStanding = args.local_minStanding;
        adata.public_minSecurity = args.public_minSecurity;
        adata.public_minStanding = args.public_minStanding;
    PyList* localList = args.local_allowedEntities->header()->as<PyTuple>()->at (1)->as<PyTuple>()->at (0)->as<PyList>();
        adata.local_allowedEntities.clear();
    for (PyList::const_iterator itr = localList->begin(); itr != localList->end(); ++itr)
        adata.local_allowedEntities.push_back((*itr)->u32());
    PyList* publicList = args.public_allowedEntities->header()->as<PyTuple>()->at (1)->as<PyTuple>()->at (0)->as<PyList>();
        adata.public_allowedEntities.clear();
    for (PyList::const_iterator itr = publicList->begin(); itr != publicList->end(); ++itr)
        adata.public_allowedEntities.push_back((*itr)->u32());
    sFltSvc.CreateFleetAdvert(fleetID, adata);

    return nullptr;
}

EVEResult FleetProxy::RemoveFleetFinderAdvert(EVECallArgs&call) {
   // sLog.White("FleetProxy", "Handle_RemoveFleetFinderAdvert() size=%lli", call.tuple->size());
    //call.dump(FLEET__DUMP);

    sFltSvc.RemoveFleetAdvert(call.client->GetChar()->fleetID());

    return nullptr;
}

EVEResult FleetProxy::GetMyFleetFinderAdvert(EVECallArgs&call) {
   // sLog.White("FleetProxy", "Handle_GetMyFleetFinderAdvert() size=%lli", call.tuple->size());
   // call.dump(FLEET__DUMP);

    return sFltSvc.GetFleetAdvert(call.client->GetChar()->fleetID());
}

EVEResult FleetProxy::UpdateAdvertInfo(EVECallArgs&call, PyInt* numMembers) {
  //   sm.ProxySvc('fleetProxy').UpdateAdvertInfo(numMembers)
    // this call just updates member count in fleet advert.  not needed here, as that is dynamic data.
    //sLog.White("FleetProxy", "Handle_UpdateAdvertInfo() size=%lli", call.tuple->size());
    //call.dump(FLEET__DUMP);

    return nullptr;
}
