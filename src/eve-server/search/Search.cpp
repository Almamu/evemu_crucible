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
    Author:        Allan
*/


#include "eve-server.h"

#include "search/Search.h"

Search::Search() :
    Service("search")
{
    this->Add("Query", &Search::Query);
    this->Add("QuickQuery", &Search::QuickQuery);
}

EVEResult Search::Query(EVECallArgs& call, PyString* filter, PyList* data) {
    std::string str = filter->content();
    Replace(str);

    // this hits db directly, so test for possible sql injection code
    for (const auto cur : badCharsSearch)
        if (EvE::icontains(str, cur))
            throw CustomError ("Search String contains invalid characters");

    // TODO: this should be possible to improve once there's updates to the type system
    // all the collections needs some overhaul on how they work
    std::vector<int> ids;
    
    PyList::const_iterator list_2_cur = data->begin();
    for (size_t list_2_index(0); list_2_cur != data->end(); ++list_2_cur, ++list_2_index) {
        if (!(*list_2_cur)->is<PyInt>()) {
            _log(XMLP__DECODE_ERROR, "Decode Call_SearchQuery failed: Element %u in list list_2 is not an integer: %s", list_2_index, (*list_2_cur)->TypeString());
            return nullptr;
        }

        const PyInt* t = (*list_2_cur)->as<PyInt>();
        ids.push_back(t->value());
    }

    return m_db->Query( str, &ids, call.client->GetCharacterID() );
}

EVEResult Search::QuickQuery(EVECallArgs& call, PyString* filter, PyList* data)  {
/*         QuickQuery(query, groupIDList, hideNPC=hideNPC, onlyAltName=onlyAltName)
*/
    std::string str = filter->content();
	Replace(str);

    bool hideNPC = true, onlyAltName = false;
    if (call.byname.find("hideNPC") != call.byname.end())
        hideNPC = (call.byname.find("hideNPC")->second->i64() != 0);

    if (call.byname.find("onlyAltName") != call.byname.end())
        onlyAltName = (call.byname.find("onlyAltName")->second->i64() != 0);

    // this hits db directly, so test for possible sql injection code
    for (const auto cur : badCharsSearch)
        if (EvE::icontains(str, cur))
            throw CustomError ("Search String contains invalid characters");


    // TODO: this should be possible to improve once there's updates to the type system
    // all the collections needs some overhaul on how they work
    std::vector<int> ids;

    PyList::const_iterator list_2_cur = data->begin();
    for (size_t list_2_index(0); list_2_cur != data->end(); ++list_2_cur, ++list_2_index) {
        if (!(*list_2_cur)->is<PyInt>()) {
            _log(XMLP__DECODE_ERROR, "Decode Call_SearchQuery failed: Element %u in list list_2 is not an integer: %s", list_2_index, (*list_2_cur)->TypeString());
            return nullptr;
        }

        const PyInt* t = (*list_2_cur)->as<PyInt>();
        ids.push_back(t->value());
    }

    return m_db->QuickQuery( str, &ids, call.client->GetCharacterID(), hideNPC, onlyAltName);
}

void Search::Replace(std::string &str) {
    for (uint i = 0; i < str.length(); ++i)
        switch (str[i]) {
			case '*':
                str[i] = '%';
		}
}
