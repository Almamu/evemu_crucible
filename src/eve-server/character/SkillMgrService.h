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

#ifndef _SKILLMGR_SERVICE_H
#define _SKILLMGR_SERVICE_H

#include "character/CharacterDB.h"

#include "services/BoundService.h"
#include "services/ServiceManager.h"
#include "Client.h"

class SkillMgrBound;

class SkillMgrService : public BindableService <SkillMgrService, SkillMgrBound> {
public:
    SkillMgrService(EVEServiceManager& mgr);

    void BoundReleased (SkillMgrBound* bound) override;

protected:
    CharacterDB m_db;

    BoundDispatcher* BindObject(Client *client, PyDataType* bindParameters) override;
};

class SkillMgrBound : public EVEBoundObject <SkillMgrBound>
{
public:
    SkillMgrBound(EVEServiceManager& mgr, SkillMgrService& parent, CharacterDB &db);

protected:
    CharacterDB &m_db;

    EVEResult GetRespecInfo(EVECallArgs& call);
    EVEResult GetSkillQueueAndFreePoints(EVECallArgs& call);
    EVEResult GetEndOfTraining(EVECallArgs& call);
    EVEResult GetSkillHistory(EVECallArgs& call);
    EVEResult CharStopTrainingSkill(EVECallArgs& call);
    EVEResult CharStartTrainingSkill(EVECallArgs& call, PyInt* itemID, PyInt* locationID);
    EVEResult AddToEndOfSkillQueue(EVECallArgs& call, PyInt* skillID, PyInt* nextLevel);
    EVEResult InjectSkillIntoBrain(EVECallArgs& call, PyList* skillItemIDs, PyInt* stationID);
    EVEResult SaveSkillQueue(EVECallArgs& call, PyList* skillQueue);
    EVEResult CharStartTrainingSkillByTypeID(EVECallArgs& call, PyInt* skillTypeID);
    EVEResult RespecCharacter(EVECallArgs& call, PyInt* charisma, PyInt* intelligence, PyInt* memory, PyInt* perception, PyInt* willpower);
    EVEResult GetCharacterAttributeModifiers(EVECallArgs& call, PyInt* attr);
    EVEResult CharAddImplant(EVECallArgs& call, PyInt* itemID);
    EVEResult RemoveImplantFromCharacter(EVECallArgs& call, PyInt* itemID);
};

#endif

