
 /**
  * @name FleetManager.h
  *     Fleet Manager code for EVEmu
  *
  * @Author:        Allan
  * @date:          05 August 2014 (original skeleton outline)
  * @update:        21 November 2017 (begin actual implementation)
  *
  */

#ifndef EVEMU_SRC_FLEET_MGR_H_
#define EVEMU_SRC_FLEET_MGR_H_

#include "services/Service.h"
#include "fleet/FleetService.h"

class FleetManager: public Service <FleetManager>
{
public:
    FleetManager();

protected:
  EVEResult ForceLeaveFleet(EVECallArgs& call);
  EVEResult GetActiveStatus(EVECallArgs& call);
  EVEResult BroadcastToBubble(EVECallArgs& call, PyString* name, PyInt* groupID, PyInt* itemID);
  EVEResult BroadcastToSystem(EVECallArgs& call, PyString* name, PyInt* groupID, PyInt* itemID);
  EVEResult AddToWatchlist(EVECallArgs& call, PyInt* characterID, PyDataType* fav);
  EVEResult RemoveFromWatchlist(EVECallArgs& call, PyInt* characterID, PyDataType* fav);
  EVEResult RegisterForDamageUpdates(EVECallArgs& call, PyDataType* fav);
};

#endif  // EVEMU_SRC_FLEET_MGR_H_
