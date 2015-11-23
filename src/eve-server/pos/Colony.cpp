/*
    ------------------------------------------------------------------------------------
    LICENSE:
    ------------------------------------------------------------------------------------
    This file is part of EVEmu: EVE Online Server Emulator
    Copyright 2006 - 2011 The EVEmu Team
    For the latest information visit http://evemu.org
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
    Author:        Cometo
*/

#include "Colony.h"

PyTuple *testContainer = new_tuple(2268, 1);
uint32 charID;
uint32 planetID;

Colony::Colony(uint32 cID, uint32 pID) {
    charID = cID;
    planetID = pID;
}

void Colony::CreateCommandPin(uint32 pinID, uint32 typeID, float latitude, float longitude) {
    Pin cc;
    cc.id = pinID;
    cc.typeID = typeID;
    cc.latitude = latitude;
    cc.longitude = longitude;
    cc.ownerID = charID;
    cc.state = STATE_IDLE; // ?
    cc.lastRunTime = 0L;
    cc.lastLaunchTime = 0L;
    ccPin.level = 0;
    ccPin.currentSimTime = CurrentBlueTime();
    ccPin.pins.push_back(cc);
}


PyResult Colony::GetColony() {
    return NULL;
}