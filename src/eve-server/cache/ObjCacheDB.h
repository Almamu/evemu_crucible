/*
    ------------------------------------------------------------------------------------
    LICENSE:
    ------------------------------------------------------------------------------------
    This file is part of EVEmu: EVE Online Server Emulator
    Copyright 2006 - 2021 The EVEmu Team
    For the latest information visit https://evemu.dev
    ------------------------------------------------------------------------------------
    This program is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any later
    version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
    this program; if not, write to the Free Software Foundation, Inc., 59 Temple
    Place - Suite 330, Boston, MA 02111-1307, USA, or go to
    http://www.gnu.org/copyleft/lesser.txt.
    ------------------------------------------------------------------------------------
    Author:        Zhur
*/

#ifndef __OBJCACHEDB_H_INCL__
#define __OBJCACHEDB_H_INCL__

#include "ServiceDB.h"

class PyRep;
class DBQueryResult;

class ObjCacheDB
: public ServiceDB
{
public:
    ObjCacheDB();

    PyDataType *GetCachableObject(const std::string &type);

protected:
    typedef PyDataType *(ObjCacheDB::* genFunc)();
    std::map<std::string, genFunc> m_generators;

    //hack:
    PyDataType *DBResultToRowsetTuple(DBQueryResult &result);

    //declare all the generators
    PyDataType *Generate_PaperdollResources();
    PyDataType *Generate_PaperdollColors();
    PyDataType *Generate_PaperdollModifierLocations();
    PyDataType *Generate_PaperdollSculptingLocations();
    PyDataType *Generate_PaperdollColorNames();
    PyDataType *Generate_PaperdollColorRestrictions();
    PyDataType *Generate_BloodlineNames();
    PyDataType *Generate_Locationscenes();
    PyDataType *Generate_OverviewDefaults();
    PyDataType *Generate_Schematicspinmap();
    PyDataType *Generate_OverviewDefaultGroups();
    PyDataType *Generate_Schematics();
    PyDataType *Generate_Schematicstypemap();
    PyDataType *Generate_Sounds();
    PyDataType *Generate_Invtypematerials();
    PyDataType *Generate_Ownericons();
    PyDataType *Generate_Icons();
    PyDataType *Generate_CharNewExtraRaceSkills();
    PyDataType *Generate_CharNewExtraCareerSkills();
    PyDataType *Generate_CharNewExtraSpecialitySkills();
    PyDataType *Generate_CharNewExtraCareers();
    PyDataType *Generate_CharNewExtraSpecialities();

    PyDataType *Generate_BillTypes();
    PyDataType *Generate_AllianceShortnames();
    PyDataType *Generate_invCategories();
    PyDataType *Generate_invTypeReactions();

    PyDataType *Generate_dgmTypeAttribs();
    PyDataType *Generate_dgmTypeEffects();
    PyDataType *Generate_dgmEffects();
    PyDataType *Generate_dgmAttribs();
    PyDataType *Generate_dgmExpressions();

    PyDataType *Generate_invMetaGroups();

    PyDataType *Generate_ramActivities();
    PyDataType *Generate_ramALTypeGroup();
    PyDataType *Generate_ramALTypeCategory();
    PyDataType *Generate_ramALTypes();
    PyDataType *Generate_ramCompletedStatuses();
    PyDataType *Generate_ramTypeRequirements();

    PyDataType *Generate_mapCelestialDescriptions();
    PyDataType *Generate_tickerNames();
    PyDataType *Generate_invGroups();
    PyDataType *Generate_certificates();
    PyDataType *Generate_certificateRelationships();
    PyDataType *Generate_invShipTypes();
    PyDataType *Generate_cacheLocations();
    PyDataType *Generate_locationWormholeClasses();
    PyDataType *Generate_invBlueprintTypes();
    PyDataType *Generate_eveGraphics();
    PyDataType *Generate_invTypes();
    PyDataType *Generate_invMetaTypes();
    PyDataType *Generate_chrBloodlines();
    PyDataType *Generate_eveUnits();
    PyDataType *Generate_eveBulkDataUnits();
    PyDataType *Generate_cacheOwners();
    PyDataType *Generate_eveStaticOwners();
    PyDataType *Generate_chrRaces();
    PyDataType *Generate_chrAttributes();
    PyDataType *Generate_invFlags();
    PyDataType *Generate_eveStaticLocations();
    PyDataType *Generate_invContrabandTypes();

    PyDataType *Generate_c_chrBloodlines();
    PyDataType *Generate_c_chrRaces();
    PyDataType *Generate_c_chrAncestries();
    PyDataType *Generate_c_chrSchools();
    PyDataType *Generate_c_chrAttributes();
    PyDataType *Generate_bl_accessories();
    PyDataType *Generate_bl_lights();
    PyDataType *Generate_bl_skins();
    PyDataType *Generate_bl_beards();
    PyDataType *Generate_bl_eyes();
    PyDataType *Generate_bl_lipsticks();
    PyDataType *Generate_bl_makeups();
    PyDataType *Generate_bl_hairs();
    PyDataType *Generate_bl_backgrounds();
    PyDataType *Generate_bl_decos();
    PyDataType *Generate_bl_eyebrows();
    PyDataType *Generate_bl_costumes();

    PyDataType *Generate_a_eyebrows();
    PyDataType *Generate_a_eyes();
    PyDataType *Generate_a_decos();
    PyDataType *Generate_a_hairs();
    PyDataType *Generate_a_backgrounds();
    PyDataType *Generate_a_accessories();
    PyDataType *Generate_a_lights();
    PyDataType *Generate_a_costumes();
    PyDataType *Generate_a_makeups();
    PyDataType *Generate_a_beards();
    PyDataType *Generate_a_skins();
    PyDataType *Generate_a_lipsticks();
};

#endif
