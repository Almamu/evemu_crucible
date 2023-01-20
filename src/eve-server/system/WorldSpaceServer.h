
/**
 * @name WorldSpaceServer.h
 *   Specific Class for
 *
 * @Author:         Allan
 * @date:   31August17
 */


#ifndef __EVE_SYSTEM_WORLDSPACE_H_
#define __EVE_SYSTEM_WORLDSPACE_H_

#include "services/Service.h"

class WorldSpaceServer : public Service <WorldSpaceServer>
{
public:
    WorldSpaceServer();

protected:
  EVEResult GetWorldSpaceTypeIDFromWorldSpaceID(EVECallArgs& call, PyInt* worldSpaceID);
  EVEResult GetWorldSpaceMachoAddress(EVECallArgs& call, PyString* address);
};

#endif  // __EVE_SYSTEM_WORLDSPACE_H_