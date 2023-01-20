/*
 *
 *
 */

#ifndef EVE_CHARACTER_FITTING_MGR_H
#define EVE_CHARACTER_FITTING_MGR_H

#include "services/Service.h"

class CharFittingMgr : public Service <CharFittingMgr>
{
public:
    CharFittingMgr();

protected:
  EVEResult GetFittings(EVECallArgs& call, PyInt* ownerID);
  EVEResult SaveFitting(EVECallArgs& call, PyInt* ownerID, PyObject* fitting);
  EVEResult SaveManyFittings(EVECallArgs& call, PyInt* ownerID, PyDict* fittingsToSave);
  EVEResult DeleteFitting(EVECallArgs& call, PyInt* ownerID, PyInt* fittingID);
  EVEResult UpdateNameAndDescription(EVECallArgs& call, PyInt* fittingID, PyInt* ownerID, PyString* name, PyString* description);

};

#endif  // EVE_CHARACTER_FITTING_MGR_H