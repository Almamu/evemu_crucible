
 /**
  * @name FleetBound.h
  *     Fleet Bound code for EVEmu
  *
  * @Author:        Allan
  * @date:          05 August 2014 (original skeleton outline)
  * @update:        21 November 2017 (begin actual implementation)
  *
  */


#ifndef EVEMU_SRC_FLEET_BOUND_H_
#define EVEMU_SRC_FLEET_BOUND_H_


#include "services/BoundService.h"
#include "fleet/FleetManager.h"

class FleetObject;

class FleetBound : public EVEBoundObject <FleetBound>
{
public:
    FleetBound(EVEServiceManager& mgr, FleetObject& parent, uint32 fleetID);

    uint32 GetFleetID() { return this->m_fleetID; }

protected:
  EVEResult Init(EVECallArgs& call, std::optional <PyInt*> shipTypeID);
  EVEResult GetInitState(EVECallArgs& call);
  EVEResult GetFleetID(EVECallArgs& call);
  EVEResult Invite(EVECallArgs& call, PyInt* characterID, std::optional<PyInt*> wingID, std::optional <PyInt*> squadID, std::optional <PyInt*> role);
  EVEResult AcceptInvite(EVECallArgs& call, std::optional <PyInt*> shipTypeID);
  EVEResult RejectInvite(EVECallArgs& call);
  EVEResult ChangeWingName(EVECallArgs& call, PyInt* wingID, PyDataType* name);
  EVEResult ChangeSquadName(EVECallArgs& call, PyInt* squadID, PyDataType* name);
  EVEResult SetOptions(EVECallArgs& call, PyObject* options);
  EVEResult GetJoinRequests(EVECallArgs& call);
  EVEResult RejectJoinRequest(EVECallArgs& call, PyInt* characterID);
  EVEResult AddToVoiceChat(EVECallArgs& call, PyDataType* channelName);
  EVEResult SetVoiceMuteStatus(EVECallArgs& call, PyDataType* status, PyDataType* channel);
  EVEResult ExcludeFromVoiceMute(EVECallArgs& call, PyInt* characterID, PyDataType* channel);
  EVEResult GetFleetComposition(EVECallArgs& call);
  EVEResult GetWings(EVECallArgs& call);
  EVEResult SendBroadcast(EVECallArgs& call, PyDataType* message, PyInt* group, PyInt* itemID);
  EVEResult UpdateMemberInfo(EVECallArgs& call, std::optional <PyInt*> shipTypeID);
  EVEResult SetMotdEx(EVECallArgs& call, PyDataType* motd);
  EVEResult GetMotd(EVECallArgs& call);
  EVEResult LeaveFleet(EVECallArgs& call);
  EVEResult MakeLeader(EVECallArgs& call, PyInt* characterID);
  EVEResult SetBooster(EVECallArgs& call, PyInt* characterID, std::optional <PyInt*> roleBooster);
  EVEResult MoveMember(EVECallArgs& call, PyInt* characterID, std::optional <PyInt*> wingID, std::optional <PyInt*> squadID, std::optional <PyInt*> role, std::optional <PyInt*> booster);
  EVEResult KickMember(EVECallArgs& call, PyInt* characterID);
  EVEResult CreateWing(EVECallArgs& call);
  EVEResult CreateSquad(EVECallArgs& call, PyInt* wingID);
  EVEResult DeleteWing(EVECallArgs& call, PyInt* wingID);
  EVEResult DeleteSquad(EVECallArgs& call, PyInt* squadID);
  EVEResult Reconnect(EVECallArgs& call);

private:
    uint32 m_fleetID;

};

#endif  // EVEMU_SRC_FLEET_BOUND_H_
