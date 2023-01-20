
 /**
  * @name FactoryDB.h
  *   db query methods for R.A.M. activities
  *
  * @Author:         Allan
  * @date:          9Jan18
  */


#ifndef EVE_MANUF_FACTORYDB_H
#define EVE_MANUF_FACTORYDB_H

#include "ServiceDB.h"
#include "../eve-common/EVE_RAM.h"
#include "packets/Manufacturing.h"
#include "inventory/InventoryItem.h"

class Character;

class FactoryDB
: public ServiceDB
{
public:
    // client calls
    static PyDataType* GetJobs2(const int32 ownerID, const bool completed);
    static PyDataType* AssemblyLinesSelectPublic(const uint32 regionID);
    static PyDataType* AssemblyLinesSelectPersonal(const uint32 charID);
    static PyDataType* AssemblyLinesSelectPrivate(const uint32 charID);
    static PyDataType* AssemblyLinesSelectCorporation(const uint32 corporationID);
    static PyDataType* AssemblyLinesSelectAlliance(const int32 allianceID);
    static PyDataType* AssemblyLinesGet(const uint32 containerID);
    static PyDataType* GetMaterialCompositionOfItemType(const uint32 typeID);
    static void GetOutpostMaterialCompositionOfItemType(const uint32 typeID, DBQueryResult& res);

    // for static data mgr
    static bool IsRefinable(const uint16 typeID);
    static bool IsRecyclable(const uint16 typeID);
    static void GetSalvage(DBQueryResult& res);
    static void GetMinerals(DBQueryResult& res);
    static void GetCompounds(DBQueryResult& res);
    static void GetResources(DBQueryResult& res);
    static void GetComponents(DBQueryResult& res);
    static void GetCommodities(DBQueryResult& res);
    static void GetMiscCommodities(DBQueryResult& res);
    static void GetRAMMaterials(DBQueryResult& res);
    static void GetBlueprintType(DBQueryResult& res);
    static void GetRAMRequirements(DBQueryResult& res);

    // InstallJob stuff
    static bool GetAssemblyLineProperties(const uint32 assemblyLineID, Character *pChar, Rsp_InstallJob &into, bool isCorpJob=false);
    static bool GetAssemblyLineRestrictions(const int32 assemblyLineID, EvERam::LineRestrictions& data);
    static uint32 InstallJob(const uint32 ownerID, const uint32 installerID, Call_InstallJob &args, const int64 beginTime, const int64 endTime, const uint32 systemID);

    // CompleteJob stuff
    static bool GetJobProperties(const uint32 jobID, EvERam::JobProperties& data);
    static bool CompleteJob(const uint32 jobID, const int8 completedStatus);

    // misc queries
    static bool DeleteBlueprint(uint32 blueprintID);
    static bool GetBlueprintData(uint32 blueprintID, EvERam::bpData& into);
    static bool SaveBlueprintData(uint32 blueprintID, EvERam::bpData& data);
    static bool IsProducableBy(const uint32 assemblyLineID, const ItemType *pType);
    static bool GetMultipliers(const uint32 assemblyLineID, const ItemType *pType, Rsp_InstallJob &into);

    static uint32 CountManufacturingJobs(const uint32 installerID);
    static uint32 CountResearchJobs(const uint32 installerID);
    static uint32 GetTech2Blueprint(const uint32 blueprintTypeID);

    static int64 GetNextFreeTime(const uint32 assemblyLineID);

    // for calendar events
    static void SetJobEventID(const uint32 jobID, const uint32 eventID);

};

#endif  // EVE_MANUF_FACTORYDB_H

