
 /**
  * @name zActionServer.h
  *     *not sure yet* system for EVEmu
  *
  * @Author:        Allan
  * @date:          13 December 2018
  *
  */



#ifndef _EVEMU_QATOOLS_ZACTION_SERVER_H
#define _EVEMU_QATOOLS_ZACTION_SERVER_H

#include "services/Service.h"

class zActionServer : public Service <zActionServer>
{
public:
    zActionServer();

protected:
  EVEResult RequestActionStart(EVECallArgs& call, PyInt* entityID, PyInt* actionID, PyBool* interrupt, PyDict* clientProps);
  EVEResult QA_RequestForceActionStart(EVECallArgs& call, PyInt* entityID, PyInt* actionID);

};


#endif  // _EVEMU_QATOOLS_ZACTION_SERVER_H


