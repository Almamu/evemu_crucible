
/**
 * @name WorldSpaceServer.cpp
 *   Specific Class for
 *
 * @Author:         Allan
 * @date:   31August17
 */


#include "eve-server.h"



#include "system/WorldSpaceServer.h"

WorldSpaceServer::WorldSpaceServer() :
    Service("worldSpaceServer")
{
    this->Add("GetWorldSpaceTypeIDFromWorldSpaceID", &WorldSpaceServer::GetWorldSpaceTypeIDFromWorldSpaceID);
    this->Add("GetWorldSpaceMachoAddress", &WorldSpaceServer::GetWorldSpaceMachoAddress);

    /*
        ws = world.GetWorldSpace(worldSpaceTypeID)
        return ws.GetDistrictID()


        currentRevs = sm.GetService('jessicaWorldSpaceClient').GetWorldSpace(self.id).GetWorldSpaceSpawnRevisionsList()
        */
}

EVEResult WorldSpaceServer::GetWorldSpaceTypeIDFromWorldSpaceID(EVECallArgs&call, PyInt* worldSpaceID) {
    /**
     *        worldSpaceTypeID = self.GetWorldSpaceTypeIDFromWorldSpaceID(worldSpaceID)
     */
    sLog.White( "WorldSpaceServer::Handle_GetWorldSpaceTypeIDFromWorldSpaceID()", "size=%lu", call.tuple->size());
    call.dump(SERVICE__CALL_DUMP);

    return PyStatic.NewNone();
}

EVEResult WorldSpaceServer::GetWorldSpaceMachoAddress(EVECallArgs&call, PyString* address) {
    /**
     *       service, address = wss.GetWorldSpaceMachoAddress(address)
     */
    sLog.White( "WorldSpaceServer::Handle_GetWorldSpaceMachoAddress()", "size=%lu", call.tuple->size());
    call.dump(SERVICE__CALL_DUMP);

    return PyStatic.NewNone();
}

