
 /**
  * @name FleetProxy.h
  *     Fleet Proxy code for EVEmu
  *
  * @Author:        Allan
  * @date:          05 August 2014 (original skeleton outline)
  * @update:        21 November 2017 (begin actual implementation)
  *
  */

#ifndef EVEMU_SRC_FLEET_PROXY_H_
#define EVEMU_SRC_FLEET_PROXY_H_

#include "services/Service.h"
#include "fleet/FleetManager.h"

class FleetProxy : public Service <FleetProxy>
{
public:
    FleetProxy();

protected:
  EVEResult GetAvailableFleets(EVECallArgs& call);
  EVEResult ApplyToJoinFleet(EVECallArgs& call, PyInt* fleetID);
  EVEResult AddFleetFinderAdvert(EVECallArgs& call, PyObject* info);
  EVEResult RemoveFleetFinderAdvert(EVECallArgs& call);
  EVEResult GetMyFleetFinderAdvert(EVECallArgs& call);
  EVEResult UpdateAdvertInfo(EVECallArgs& call, PyInt* numMembers);
};

#endif  // EVEMU_SRC_FLEET_PROXY_H_
