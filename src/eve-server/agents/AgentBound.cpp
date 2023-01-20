
 /**
  * @name AgentBound.cpp
  *   agent specific code
  *    removed from AgentMgrService.cpp
  *
  * @Author:        Allan
  * @date:      26 June 2018
  *
  */


/*
 * # Agent Logging:
 * AGENT__ERROR
 * AGENT__WARNING
 * AGENT__MESSAGE
 * AGENT__DEBUG
 * AGENT__INFO
 * AGENT__TRACE
 * AGENT__DUMP
 * AGENT__RSP_DUMP
 */

#include "eve-server.h"
#include "../../eve-common/EVE_Missions.h"
//#include "../../eve-common/EVE_Skills.h"
#include "../../eve-common/EVE_Standings.h"

#include "StaticDataMgr.h"
#include "account/AccountService.h"
#include "corporation/LPService.h"
#include "agents/AgentBound.h"
#include "agents/AgentMgrService.h"
#include "station/Station.h"
#include "services/ServiceManager.h"

AgentBound::AgentBound(EVEServiceManager& mgr, AgentMgrService& parent, Agent *agt) :
    EVEBoundObject(mgr, parent),
    m_agent(agt)
{
    this->Add("DoAction", &AgentBound::DoAction);
    this->Add("GetAgentLocationWrap", &AgentBound::GetAgentLocationWrap);
    this->Add("GetInfoServiceDetails", &AgentBound::GetInfoServiceDetails);
    this->Add("GetMissionBriefingInfo", &AgentBound::GetMissionBriefingInfo);
    this->Add("GetMissionObjectiveInfo", &AgentBound::GetMissionObjectiveInfo);
    this->Add("GetMissionKeywords", &AgentBound::GetMissionKeywords);
    this->Add("GetMissionJournalInfo", &AgentBound::GetMissionJournalInfo);
    this->Add("GetDungeonShipRestrictions", &AgentBound::GetDungeonShipRestrictions);
    this->Add("RemoveOfferFromJournal", &AgentBound::RemoveOfferFromJournal);
    this->Add("GetOfferJournalInfo", &AgentBound::GetOfferJournalInfo);
    this->Add("GetEntryPoint", &AgentBound::GetEntryPoint);
    this->Add("GotoLocation", &AgentBound::GotoLocation);
    this->Add("WarpToLocation", &AgentBound::WarpToLocation);
    this->Add("GetMyJournalDetails", &AgentBound::GetMyJournalDetails);
}

EVEResult AgentBound::GetAgentLocationWrap(EVECallArgs&call) {
    // this is detailed info on agent's location
    return m_agent->GetLocationWrap();
}

EVEResult AgentBound::GetInfoServiceDetails(EVECallArgs& call) {
    // this is agents personal info... level, division, station, etc.
    return m_agent->GetInfoServiceDetails();
}


/**     ***********************************************************************
 * @note   these below are partially coded
 */

EVEResult AgentBound::DoAction(EVECallArgs&call, std::optional <PyInt*> actionID) {
    // this is first call when initiating agent convo
    _log(AGENT__DUMP,  "AgentBound::Handle_DoAction() - size=%lli", call.tuple->size());
    call.dump(AGENT__DUMP);

    // this actually returns a complicated tuple depending on other variables involving this agent and char.
    /*
        agentSays, dialogue, extraInfo = self.__GetConversation(agentDialogueWindow, actionID)

    def __GetConversation(self, wnd, actionID):
       tmp = wnd.sr.agentMoniker.DoAction(actionID)
        ret, wnd.sr.oob = tmp
        agentSays, wnd.sr.dialogue = ret
        firstActionID = wnd.sr.dialogue[0][0]
        firstActionDialogue = wnd.sr.dialogue[0][1]
        wnd.sr.agentSays = self.ProcessMessage(agentSays, wnd.sr.agentID)
        return (wnd.sr.agentSays, wnd.sr.dialogue, wnd.sr.oob)
     *
     */

    Character* pchar = call.client->GetChar().get();
    float charStanding = StandingDB::GetStanding(m_agent->GetID(), pchar->itemID());
    float quality = EvEMath::Agent::EffectiveQuality(m_agent->GetQuality(), pchar->GetSkillLevel(EvESkill::Connections), charStanding);
    float bonus = EvEMath::Agent::GetStandingBonus(charStanding, m_agent->GetFactionID(), pchar->GetSkillLevel(EvESkill::Connections), pchar->GetSkillLevel(EvESkill::Diplomacy), pchar->GetSkillLevel(EvESkill::CriminalConnections));
    float standing = EvEMath::Agent::EffectiveStanding(charStanding, bonus);

    std::string response = "";
    bool missionQuit = false, missionCompleted = false, missionDeclined = false;

    PyTuple* agentSays;
    // dialog is button info
    PyList* dialog = new PyList();

    using namespace Dialog::Button;

    // to set 'admin dialog options' (which i dont know wtf they are yet), i *think* you add a tuple of *something* that is NOT dict or int.
    if (false /*admin options*/) {
        dialog->add (new PyTuple {
            new PyInt (Admin),
            new PyString ("Admin Options")
        });
    }

    if (m_agent->CanUseAgent(call.client)) {
        switch (actionID.has_value() ? actionID.value()->value() : 0) {
            case 0: {
                //  if char has current mission with this agent, add this one.
                MissionOffer offer = MissionOffer();
                if (m_agent->HasMission(pchar->itemID(), offer)) {
                    dialog->add (new PyTuple {
                        new PyInt (ViewMission), // this are buttonIDs which are unique and sequential to each agent, regardless of chars
                        new PyInt (ViewMission)
                    });
                    if (call.client->IsMissionComplete(offer))  {
                        dialog->add (new PyTuple {
                            new PyInt (Complete),
                            new PyInt (Complete)
                        });
                    }
                    agentSays = new PyTuple {
                        new PyInt (offer.briefingID),
                        new PyInt (offer.characterID)
                    };
                } else {
                    // dialogue data.  if RequestMission is only option, client auto-responds with DoAction(RequestMission optionID)
                    dialog->add (new PyTuple {
                        new PyInt (RequestMission),
                        new PyInt (RequestMission)
                    });
                // response as string for custom data.  response as pyint to use client data (using getlocale shit)
                    // default initial agent response based on agent location, level, bloodline, quality, and char/agent standings
                    //  this will be modeled after UO speech data, in tiers and levels.
                    // if RequestMission is only option, this is ignored.  see note under 'dialog data'
                    response = "Why the fuck am I looking at you again, ";
                    response += call.client->GetName();
                    response += "?";
                    agentSays = new PyTuple {
                        new PyString (response), // msgInfo  -- if tuple[0].string then return msgInfo
                        PyStatic.NewNone () // ContentID  -- PyNone used when msgInfo is string (mostly for initial greetings)
                    };
                }

                // if agent does location, add this one...
                if (m_agent->IsLocator()) {
                    dialog->add (new PyTuple {
                        new PyInt (LocateCharacter),
                        new PyInt (LocateCharacter)
                    });
                }

                // if agent does research, add this one...
                if (m_agent->IsResearch()) {
                    dialog->add (new PyTuple {
                        new PyInt (StartResearch),
                        new PyInt (StartResearch)
                    });
                }
            } break;
            case RequestMission: {  //2
                MissionOffer offer = MissionOffer();
                m_agent->MakeOffer(pchar->itemID(), offer);
                m_agent->SendMissionUpdate(call.client, "offered");

                //  this one will get complicated and is based on agent/char interaction
                //   detail in /eve/client/script/ui/station/agents/agents.py

                /*  agentSays is a tuple of msgData and contentID
                *      msgData can be single integer of briefingID, a string literal, or a tuple as defined above.
                *   contentID is used for specific char's mission keywords.  we're not using it like there here....
                */

                agentSays = new PyTuple {
                    new PyInt (offer.briefingID),
                    new PyInt (offer.characterID)
                };

                // dialog can also contain mission data.
                //   set a dialog tuple[1] to dict and fill with MissionBriefingInfo
                dialog->add (new PyTuple {
                    new PyInt (Accept),
                    new PyInt (Accept)
                });
                dialog->add (new PyTuple {
                    new PyInt (Decline),
                    new PyInt (Decline)
                });
                dialog->add (new PyTuple {
                    new PyInt (Defer),
                    new PyInt (Defer)
                });
            } break;
            case ViewMission: { //1
                MissionOffer offer = MissionOffer();
                m_agent->GetOffer(pchar->itemID(), offer);
                agentSays = new PyTuple {
                    new PyInt (offer.briefingID),
                    new PyInt (offer.characterID)
                };
                if (offer.stateID < Mission::State::Accepted) {
                    dialog->add (new PyTuple {
                        new PyInt (Accept),
                        new PyInt (Accept)
                    });
                    dialog->add (new PyTuple {
                        new PyInt (Decline),
                        new PyInt (Decline)
                    });
                    dialog->add (new PyTuple {
                        new PyInt (Defer),
                        new PyInt (Defer)
                    });
                } else if (offer.stateID == Mission::State::Accepted) {
                    dialog->add (new PyTuple {
                        new PyInt (Quit),
                        new PyInt (Quit)
                    });
                    if (call.client->IsMissionComplete(offer))  {
                        dialog->add (new PyTuple {
                            new PyInt (Complete),
                            new PyInt (Complete)
                        });
                    }
                }
            } break;
            case Accept:            //3
            case AcceptRemotely: {  //5
                MissionOffer offer = MissionOffer();
                m_agent->GetOffer(pchar->itemID(), offer);
                offer.stateID = Mission::State::Accepted;
                offer.dateAccepted = GetFileTimeNow();
                offer.expiryTime = GetFileTimeNow() + (30 * m_agent->GetLevel() * EvE::Time::Minute);  // 30m per agent level  ?  test this.
                if (offer.courierTypeID) {
                    // add item to players hangar
                    sItemFactory.SetUsingClient(call.client);
                    ItemData data(offer.courierTypeID, pchar->itemID(), locTemp, flagNone, offer.courierAmount);
                    InventoryItemRef iRef = sItemFactory.SpawnItem(data);
                    iRef->Move(offer.originID, flagHangar, true);
                    sItemFactory.UnsetUsingClient();
                }
                m_agent->UpdateOffer(pchar->itemID(), offer);
                m_agent->SendMissionUpdate(call.client, "offer_accepted");
                agentSays = new PyTuple {
                    new PyInt (m_agent->GetAcceptRsp(pchar->itemID())),
                    new PyInt (pchar->itemID())
                };
            } break;
            case Complete:              //6
            case CompleteRemotely: {    //7
                //  need to verify all requirements have been met.
                MissionOffer offer = MissionOffer();
                m_agent->GetOffer(pchar->itemID(), offer);
                offer.stateID = Mission::State::Completed;
                offer.dateCompleted = GetFileTimeNow();
                m_agent->UpdateOffer(pchar->itemID(), offer);
                m_agent->SendMissionUpdate(call.client, "completed");
                agentSays = new PyTuple {
                    new PyInt (m_agent->GetCompleteRsp(pchar->itemID())),
                    new PyInt (pchar->itemID())
                };
                if (offer.courierTypeID) {
                    // remove item from player possession
                    call.client->RemoveMissionItem(offer.courierTypeID, offer.courierAmount);
                }
                if (offer.rewardItemID) {
                    // add reward item to players hangar
                    sItemFactory.SetUsingClient(call.client);
                    ItemData data(offer.rewardItemID, pchar->itemID(), locTemp, flagNone, offer.rewardItemQty);
                    InventoryItemRef iRef = sItemFactory.SpawnItem(data);
                    iRef->Move(m_agent->GetStationID(), flagHangar, true);
                    sItemFactory.UnsetUsingClient();
                }
                /** @todo  add fleet sharing  */
                if (offer.rewardISK)
                    AccountService::TransferFunds(m_agent->GetID(), pchar->itemID(), offer.rewardISK, "Mission Reward", Journal::EntryType::AgentMissionReward, m_agent->GetID());
                if ((offer.bonusTime > 0) and (offer.bonusTime < (offer.dateAccepted - GetFileTimeNow())))
                    AccountService::TransferFunds(m_agent->GetID(), pchar->itemID(), offer.bonusISK, "Mission Bonus Reward", Journal::EntryType::AgentMissionTimeBonusReward, m_agent->GetID());
                /** @todo  add lp, etc, etc  */
                if (offer.rewardLP)
                    LPService::AddLP(pchar->itemID(), m_agent->GetCorpID(), offer.rewardLP);
                m_agent->UpdateStandings(call.client, Standings::MissionCompleted, offer.important);
            } break;
            case Defer: {   //10
                // extend expiry time and close
                MissionOffer offer = MissionOffer();
                if (m_agent->HasMission(pchar->itemID(), offer)) {
                    offer.stateID = Mission::State::Allocated; //Defered
                    offer.expiryTime += EvE::Time::Day;
                    m_agent->UpdateOffer(pchar->itemID(), offer);
                    m_agent->SendMissionUpdate(call.client, "prolong");
                    agentSays = new PyTuple {
                        new PyString("I can give you 24 hours to think about it."), // msgInfo  -- if tuple[0].string then return msgInfo
                        PyStatic.NewNone () // ContentID  -- PyNone used when msgInfo is string to return without processing
                    };
                }
            } break;
            case Decline: { //9
                missionDeclined = true;
                m_agent->DeleteOffer(pchar->itemID());
                m_agent->SendMissionUpdate(call.client, "offer_declined");
                agentSays = new PyTuple {
                    new PyInt (m_agent->GetDeclineRsp(pchar->itemID())),
                    new PyInt (pchar->itemID())
                };
                /** @todo  add lp, etc, etc  */
                m_agent->UpdateStandings(call.client, Standings::MissionDeclined);
            } break;
            case Quit: {    //11
                missionQuit = true;
                MissionOffer offer = MissionOffer();
                m_agent->GetOffer(pchar->itemID(), offer);
                if (offer.courierTypeID) {
                    // remove item from player possession
                    call.client->RemoveMissionItem(offer.courierTypeID, offer.courierAmount);
                }
                // remove mission offer and set standings accordingly
                m_agent->DeleteOffer(pchar->itemID());
                m_agent->SendMissionUpdate(call.client, "quit");
                agentSays = new PyTuple {
                    new PyInt (m_agent->GetDeclineRsp(pchar->itemID())),
                    new PyInt (pchar->itemID())
                };
                /** @todo  add lp, etc, etc  */
                m_agent->UpdateStandings(call.client, Standings::MissionFailure, offer.important);
            } break;
            case Continue: {    //8
                // not sure what to do here.
            } break;
            case StartResearch: {   //12
                // not sure what to do here.
            } break;
            case CancelResearch: {  //13
                // not sure what to do here.
            } break;
            case BuyDatacores: {    //14
                // not sure what to do here.
            } break;
            case LocateCharacter:   //15
            case LocateAccept: {    //16
                // not sure what to do here.
            } break;
            case LocateReject: {    //17
                // not sure what to do here.
            } break;
            case Yes: {             //18
                // not sure what to do here.
            } break;
            case No: {              //19
                // not sure what to do here.
            } break;
            case AcceptChoice:{     //4
                // i think this is for options
            } break;
            case Admin: {           //20
                // not sure what to do here.
            } break;
            default: {
                // error
                _log(AGENT__ERROR, "AgentBound::Handle_DoAction() - unhandled buttonID %u", actionID );
                call.client->SendErrorMsg("Internal Server Error. Ref: ServerError xxxxx.");
                PySafeDecRef(dialog);
                PySafeDecRef(agentSays);
                return nullptr;
            }
        }
    } else {
        agentSays = new PyTuple {
            new PyInt (m_agent->GetStandingsRsp(pchar->itemID())),
            PyStatic.NewNone() /*new PyInt (pchar->itemID())*/
        };
    }

    if (agentSays == nullptr) {
        agentSays = new PyTuple {
            PyStatic.NewNone(), // briefingID
            PyStatic.NewNone() // contentID
        };
    }

    PyTuple* outer = new PyTuple {
        new PyTuple {
            agentSays,
            dialog
        },
        new PyDict {
            {"loyaltyPoints", new PyInt(LPService::GetLPBalanceForCorp(pchar->itemID(),m_agent->GetCorpID()))},  // this is char current LP
            {"missionCompleted", new PyBool(missionCompleted)},
            {"missionQuit", new PyBool(missionQuit)},
            {"missionDeclined", new PyBool(missionDeclined)}
        }
    };
    if (is_log_enabled(AGENT__RSP_DUMP)) {
        _log(AGENT__RSP_DUMP, "AgentBound::Handle_DoAction RSP:" );
        outer->dump(AGENT__RSP_DUMP, "    ");
    }

    PySafeDecRef(agentSays);

    return outer;
}

EVEResult AgentBound::GetMissionBriefingInfo(EVECallArgs&call) {
    // called from iniate agent convo... should be populated when mission available
    // will return PyNone if no mission avalible
    _log(AGENT__MESSAGE,  "AgentBound::Handle_GetMissionBriefingInfo()");

    MissionOffer offer = MissionOffer();
    if (!m_agent->HasMission(call.client->GetCharacterID(), offer))
        return PyStatic.NewNone();

    switch (offer.stateID) {
        //case Mission::State::Allocated:
        case Mission::State::Accepted:
        case Mission::State::Failed:
        case Mission::State::Completed:
        case Mission::State::Rejected:
        case Mission::State::Defered:
        case Mission::State::Expired: {
            return PyStatic.NewNone();
        }
    }

    // these are found in the client data by MessageIDs ....  i.e.  {[location]objectiveDestinationID.name}
    // contentID is the key for the keywords data on live...not used here
    PyDict* keywords = new PyDict();
        keywords->set ("objectiveLocationID", new PyInt(offer.originID));
        keywords->set ("objectiveLocationSystemID", new PyInt(offer.originSystemID));
        keywords->set ("objectiveTypeID", new PyInt(offer.courierTypeID));
        keywords->set ("objectiveQuantity", new PyInt(offer.courierAmount));
        keywords->set ("objectiveDestinationID", new PyInt(offer.destinationID));
        keywords->set ("objectiveDestinationSystemID", new PyInt(offer.destinationSystemID));
        if (offer.rewardISK) {
            keywords->set ("rewardTypeID", new PyInt(itemTypeCredits));
            keywords->set ("rewardQuantity", new PyInt(offer.rewardISK));
        } else {
            // wouldnt these be in 'extra' or ?
            keywords->set ("rewardTypeID", new PyInt(offer.rewardItemID));
            keywords->set ("rewardQuantity", new PyInt(offer.rewardItemQty));
        }
        keywords->set ("dungeonLocationID", new PyInt(offer.dungeonLocationID));
        keywords->set ("dungeonSolarSystemID", new PyInt(offer.dungeonSolarSystemID));
    PyDict *briefingInfo = new PyDict();
        briefingInfo->set ("ContentID", new PyInt(offer.characterID));
        briefingInfo->set ("Mission Keywords", keywords);
        briefingInfo->set ("Mission Title ID", new PyInt(offer.missionID));
        briefingInfo->set ("Mission Briefing ID", new PyInt(offer.briefingID));
        switch(offer.typeID) {
            case Mission::Type::Courier:
                briefingInfo->set ("Mission Image", sMissionDataMgr.GetCourierRes()); break;
            case Mission::Type::Mining:
                briefingInfo->set ("Mission Image", sMissionDataMgr.GetMiningRes()); break;
            case Mission::Type::Encounter:
                briefingInfo->set ("Mission Image", sMissionDataMgr.GetKillRes()); break;
        }
        // decline time OR expiration time.  if not decline then expiration
        briefingInfo->set ("Decline Time", PyStatic.NewNone());   // -1 is generic decline msg
        briefingInfo->set ("Expiration Time", new PyInt(offer.expiryTime) );

    if (is_log_enabled(AGENT__RSP_DUMP)) {
        _log(AGENT__RSP_DUMP, "AgentBound::Handle_GetMissionBriefingInfo() RSP:" );
        briefingInfo->dump(AGENT__RSP_DUMP, "    ");
    }

    return briefingInfo;
}

EVEResult AgentBound::GetMissionKeywords(EVECallArgs&call, PyInt* contentID) {
    // thse are the variables embedded in the messageIDs
    //self.missionArgs[contentID] = self.GetAgentMoniker(agentID).GetMissionKeywords(contentID)
    _log(AGENT__DUMP,  "AgentBound::Handle_GetMissionKeywords() - size=%lli", call.tuple->size());
    call.dump(AGENT__DUMP);

    /*   none of this really matters as we're not using 'contentID' like live does
    Call_SingleArg args;
    if (!args.Decode(&call.tuple)) {
        _log(SERVICE__ERROR, "%s: Failed to decode arguments.", GetName());
        return nullptr;
    }

    uint32 contentID = PyRep::IntegerValueU32(args.arg);
    if (contentID == 0)
        return PyStatic.NewNone();
    */
    MissionOffer offer = MissionOffer();
    if (!m_agent->HasMission(call.client->GetCharacterID(), offer))
        return PyStatic.NewNone();

    PyDict* keywords = new PyDict();
    keywords->set ("objectiveLocationID", new PyInt(offer.originID));
    keywords->set ("objectiveLocationSystemID", new PyInt(offer.originSystemID));
    keywords->set ("objectiveTypeID", new PyInt(offer.courierTypeID));
    keywords->set ("objectiveQuantity", new PyInt(offer.courierAmount));
    keywords->set ("objectiveDestinationID", new PyInt(offer.destinationID));
    keywords->set ("objectiveDestinationSystemID", new PyInt(offer.destinationSystemID));
    if (offer.rewardISK) {
        keywords->set ("rewardTypeID", new PyInt(itemTypeCredits));
        keywords->set ("rewardQuantity", new PyInt(offer.rewardISK));
    } else {
        // wouldnt these be in 'extra' or ?
        keywords->set ("rewardTypeID", new PyInt(offer.rewardItemID));
        keywords->set ("rewardQuantity", new PyInt(offer.rewardItemQty));
    }
    keywords->set ("dungeonLocationID", new PyInt(offer.dungeonLocationID));
    keywords->set ("dungeonSolarSystemID", new PyInt(offer.dungeonSolarSystemID));

    if (is_log_enabled(AGENT__RSP_DUMP)) {
        _log(AGENT__RSP_DUMP, "AgentBound::Handle_GetMissionKeywords() RSP:" );
        keywords->dump(AGENT__RSP_DUMP, "    ");
    }

    return keywords;
}

EVEResult AgentBound::GetMissionObjectiveInfo(EVECallArgs&call, std::optional <PyInt*> characterID, std::optional <PyInt*> contentID)
{
    // sends charID, contentID (although there's another call without any parameters, hence the optionals used)
    // returns PyDict loaded with mission info  or PyNone
    //  returning mission info sets double-pane view, where PyNone sets single-pane view
    _log(AGENT__DUMP,  "AgentBound::Handle_GetMissionObjectiveInfo() - size=%lli", call.tuple->size());
    call.dump(AGENT__DUMP);

    MissionOffer offer = MissionOffer();
    if (call.tuple->size() == 0)
        if (!m_agent->HasMission(call.client->GetCharacterID(), offer))
            return PyStatic.NewNone();

        switch (offer.stateID) {
            //case Mission::State::Allocated:
            case Mission::State::Accepted:
            //case Mission::State::Failed:
            case Mission::State::Completed:
            case Mission::State::Rejected:
            case Mission::State::Defered:
            case Mission::State::Expired: {
                return PyStatic.NewNone();
            }
        }

    return GetMissionObjectiveInfo(call.client, offer);
}

EVEResult AgentBound::GetMyJournalDetails(EVECallArgs&call) {
    //parallelCalls.append((sm.GetService('agents').GetAgentMoniker(agentID).GetMyJournalDetails, ()))
    //missionState, importantMission, missionType, missionName, agentID, expirationTime, bookmarks, remoteOfferable, remoteCompletable = each
    // this is to update ONLY info with this agent....
    _log(AGENT__DUMP,  "AgentBound::Handle_GetMyJournalDetails() - size=%lli", call.tuple->size());
    call.dump(AGENT__DUMP);

    //missions:
    PyList* missions = new PyList();
    MissionOffer offer = MissionOffer();
    if (m_agent->HasMission(call.client->GetCharacterID(), offer)) {
        if (offer.stateID < Mission::State::Completed) {
            missions->add (
                new PyTuple {
                    new PyInt (offer.stateID), // missionState  .. these may be wrong also.
                    new PyInt (offer.important ? 1 : 0), // importantMission  -- integer boolean
                    new PyString (sMissionDataMgr.GetTypeLabel (offer.typeID)), // missionTypeLabel
                    new PyString (offer.name), // missionName
                    new PyInt (offer.agentID), // agentID
                    new PyInt (offer.expiryTime), // expirationTime
                    offer.bookmarks->clone (), // bookmarks -- if populated, this is PyList of PyDicts as defined below...
                    new PyBool (offer.remoteOfferable), // remoteOfferable
                    new PyBool (offer.remoteCompletable) // remoteCompletable
                }
            );
        }
    }
    PyTuple* tuple = new PyTuple {
        missions,
        new PyList() // research
    };

    if (is_log_enabled(AGENT__RSP_DUMP))
        tuple->dump(AGENT__RSP_DUMP, "   ");
    return tuple;
}

EVEResult AgentBound::GetMissionJournalInfo(EVECallArgs&call, std::optional <PyInt*> characterID, std::optional <PyInt*> contentID) {
    //called on rclick in journal to "read details"
    //ret = self.GetAgentMoniker(agentID).GetMissionJournalInfo(charID, contentID)
    _log(AGENT__DUMP,  "AgentBound::Handle_GetMissionJournalInfo() - size=%lli", call.tuple->size());
    call.dump(AGENT__DUMP);

    MissionOffer offer = MissionOffer();
    if (!m_agent->HasMission(call.client->GetCharacterID(), offer))
        return PyStatic.NewNone();

    PyDict* journalInfo = new PyDict();
    journalInfo->set ("contentID", new PyInt(offer.characterID));
    journalInfo->set ("missionNameID", new PyInt(offer.missionID));
    journalInfo->set ("briefingTextID", new PyInt(offer.briefingID));
    journalInfo->set ("missionState", new PyInt(offer.stateID));
    journalInfo->set ("expirationTime", new PyInt(offer.expiryTime) );
    journalInfo->set ("objectives", GetMissionObjectiveInfo(call.client, offer));
    switch(offer.typeID) {
        case Mission::Type::Courier:
            journalInfo->set ("missionImage", sMissionDataMgr.GetCourierRes()); break;
        case Mission::Type::Mining:
            journalInfo->set ("missionImage", sMissionDataMgr.GetMiningRes()); break;
        case Mission::Type::Encounter:
            journalInfo->set ("missionImage", sMissionDataMgr.GetKillRes()); break;
    }

    if (is_log_enabled(AGENT__RSP_DUMP)) {
        _log(AGENT__RSP_DUMP, "AgentBound::Handle_GetMissionJournalInfo() RSP:" );
        journalInfo->dump(AGENT__RSP_DUMP, "    ");
    }

    return journalInfo;
}

PyDict* AgentBound::GetMissionObjectiveInfo(Client* pClient, MissionOffer& offer)
{
    PyDict* objectiveData = new PyDict();
    objectiveData->set ("missionTitleID", new PyInt(offer.missionID));
    objectiveData->set ("contentID", new PyInt(offer.characterID));
    objectiveData->set ("importantStandings", new PyInt(offer.important));     // boolean integer
    // will need to test for this to set correctly.....
    if (pClient->IsMissionComplete(offer)) {     // Mission::Status:: data here 0=no, 1=yes, 2=cheat
        objectiveData->set ("completionStatus", new PyInt(Mission::Status::Complete));
    } else {
        objectiveData->set ("completionStatus", new PyInt(Mission::Status::Incomplete));
    }
    objectiveData->set ("missionState", new PyInt(offer.stateID /*Mission::State::Offered*/));   // Mission::State:: data here for agentGift populating.  Accepted/failed to display gift items as accepted
    objectiveData->set ("loyaltyPoints", new PyInt(offer.rewardLP));
    objectiveData->set ("researchPoints", new PyInt(0));

    /*  this puts title/msg at bottom of right pane
    if (offer.stateID == Mission::State::Accepted)
        if (offer.typeID == Mission::Type::Courier) {
            PyTuple* missionExtra = new PyTuple(2);  // this is tuple(2)  headerID, bodyID    -- std locale msgIDs
                missionExtra->SetItem(0, new PyString("Reminder...."));   // this should be separate title from mission name
                missionExtra->SetItem(1, new PyString("Remember to get the %s from your hangar before you leave.", call.client->GetCourierItemRef(m_agent->GetID())->name()));   // this is additional info about mission, etc.
            objectiveData->set ("missionExtra", missionExtra);
        } */

    objectiveData->set (
        "locations",
        new PyList { // tuple of list of locationIDs (pickup and dropoff)
            new PyInt (offer.originSystemID),
            new PyInt (offer.destinationSystemID)
        }
    );

    PyList* giftList = new PyList();    // this is list of tuple(3)  typeID, quantity, extra
    /*
    PyDict* extra = new PyDict();    // 'extra' is either specificItemID or blueprint data.
        extra->set ("specificItemID", PyStatic.NewNone());
        extra->set ("blueprintInfo", PyStatic.NewNone());
    PyTuple* agentGift = new PyTuple(3);
        agentGift->SetItem(0, PyStatic.NewNone());
        agentGift->SetItem(1, PyStatic.NewNone());
        agentGift->SetItem(2, extra);
        giftList->AddItem(agentGift);
    */
    objectiveData->set ("agentGift", giftList);

    PyList* normList = new PyList();    // this is list of tuple(3)  typeID, quantity, extra
    if (offer.rewardISK) {
        normList->add (
            new PyTuple {
                new PyInt (itemTypeCredits),
                new PyInt (offer.rewardISK),
                new PyDict { // 'extra' is either specificItemID or blueprint data.
                    // {"specificItemID", PyStatic.NewNone()},
                    // {"blueprintInfo", PyStatic.NewNone()}
                }
            }
        );
    }
    if (offer.rewardItemID) {
        normList->add (
            new PyTuple {
                new PyInt (offer.rewardItemID),
                new PyInt (offer.rewardItemQty),
                new PyDict { // 'extra' is either specificItemID or blueprint data.
                    // {"specificItemID", PyStatic.NewNone()},
                    // {"blueprintInfo", PyStatic.NewNone()}
                }
            }
        );
    }
    objectiveData->set ("normalRewards", normList);

    PyList* collateralList = new PyList(); // this is list of tuple(3)  typeID, quantity, extra
    /*
    PyDict* extra = new PyDict();    // 'extra' is either specificItemID or blueprint data.
        extra->set ("specificItemID", PyStatic.NewNone());
        extra->set ("blueprintInfo", PyStatic.NewNone());
    PyTuple* collateral = new PyTuple(3);
        collateral->SetItem(0, PyStatic.NewNone());
        collateral->SetItem(1, PyStatic.NewNone());
        collateral->SetItem(2, extra);
        */
    objectiveData->set ("collateral", collateralList);

    PyList* bonusList = new PyList();   // this is list of tuple(4)  timeRemaining, typeID, quantity, extra
    if (offer.bonusTime > 0) {
        bonusList->add(
            new PyTuple {
                offer.dateAccepted > 0
                    ? new PyInt(offer.bonusTime - (offer.dateAccepted - offer.dateIssued) * EvE::Time::Minute) // bonus time - elapsed time * minutes
                    : new PyInt(offer.bonusTime * EvE::Time::Minute), // bonus time * minutes
                new PyInt (itemTypeCredits), // bonus is *usually* isk.  for now, we'll keep it as isk (easier)
                new PyInt (offer.rewardISK * 2),
                new PyDict { // 'extra' is either specificItemID or blueprint data.
                    // {"specificItemID", PyStatic.NewNone()},
                    // {"blueprintInfo", PyStatic.NewNone()}
                }
            }
        );
    }
    // bonusList can be multiple items, usualy only item or isk for time bonus
    if (false/*bonus2*/) {
        bonusList->add(
            new PyTuple {
                new PyInt (12000000000), // 20m
                new PyInt (itemTypeTrit),
                new PyInt (offer.rewardISK),
                new PyDict { // 'extra' is either specificItemID or blueprint data.
                    // {"specificItemID", PyStatic.NewNone()},
                    // {"blueprintInfo", PyStatic.NewNone()}
                }
            }
        );

    }
    objectiveData->set ("bonusRewards", bonusList);
    /*  for collateral and rewards, as follows...
    typeID, quantity, extra in objectiveData['normalRewards']
    typeID, quantity, extra in objectiveData['collateral']
    typeID, quantity, extra in objectiveData['agentGift']
    or
    timeRemaining, typeID, quantity, extra in objectiveData['bonusRewards']

        specificItemID = extra.get('specificItemID', 0)
        blueprintInfo = extra.get('blueprintInfo', None)
        */

    objectiveData->set ("objectives", GetMissionObjectives(pClient, offer));
    PyList* dunList = new PyList();  // this is a list of dunData dicts
    /*
    PyDict* dunData = new PyDict();
        dunData->set ("dungeonID", new PyInt(1000));
        dunData->set ("completionStatus", new PyInt(Dungeon::Status::Started));
        dunData->set ("optional", new PyInt());
        dunData->set ("briefingMessage", new PyInt());
        dunData->set ("objectiveCompleted", new PyBool(false));
        dunData->set ("ownerID", new PyInt(m_agent->GetID()));
        dunData->set ("shipRestrictions", new PyInt(0));   // 0=normal 1=special with link to *something else*
        dunData->set ("location", m_agent->GetLocationWrap());
    */
    objectiveData->set ("dungeons", dunList);
    /* dunData data....
     * dungeonID
     * completionStatus
     * optional
     * briefingMessage
     * objectiveCompleted
     * ownerID
     * location
        location['locationID']
        location['locationType']
        location['solarsystemID']
        location['coords']
        location['agentID']
        ?location['referringAgentID']
        ?location['shipTypeID']
     * shipRestrictions  0=normal 1=special with link to *something else*
     */

    if (is_log_enabled(AGENT__RSP_DUMP)) {
        _log(AGENT__RSP_DUMP, "AgentBound::Handle_GetMissionObjectiveInfo() RSP:" );
        objectiveData->dump(AGENT__RSP_DUMP, "    ");
    }

    return objectiveData;
}

PyTuple* AgentBound::GetMissionObjectives(Client* pClient, MissionOffer& offer)
{
    // set mission objectiveData based on mission type.
    PyDict* dropoffLocation;
    if (sDataMgr.IsStation(offer.destinationID)) {
        dropoffLocation = new PyDict {
            {"typeID", new PyInt (offer.destinationTypeID)},
            {"locationID", new PyInt (offer.destinationID)},
            {"solarsystemID", new PyInt (offer.destinationSystemID)}
        };
    } else {
        dropoffLocation = new PyDict {
            {"shipTypeID", new PyInt (offer.destinationTypeID)},
            {"agentID", new PyInt (offer.destinationOwnerID)},
            {"coords", new PyTuple {
                    new PyFloat (0), // x
                    new PyFloat (0), // y
                    new PyFloat (0)  // z
                }
            },
            {"referringAgentID", new PyInt (offer.agentID)}
        };
    }

    PyTuple* objectives;
    switch (offer.typeID) {
        case Mission::Type::Trade:
        case Mission::Type::Courier: {
            objectives = new PyTuple {
                new PyTuple {
                    new PyString ("transport"),
                    new PyTuple {
                        new PyInt (offer.originOwnerID),
                        new PyDict {
                            {"typeID", new PyInt (m_agent->GetLocTypeID())},
                            {"locationID", new PyInt (offer.originID)},
                            {"solarsystemID", new PyInt (offer.originSystemID)}
                        }/*m_agent->GetLocationWrap()*/,
                        new PyInt (offer.destinationOwnerID),
                        dropoffLocation/*m_agent->GetLocationWrap()*/,
                        new PyDict {
                            {"hasCargo", new PyBool (pClient->ContainsTypeQty (offer.courierTypeID, offer.courierAmount))},
                            {"typeID", new PyInt (offer.courierTypeID)},
                            {"quantity", new PyInt (offer.courierAmount)},
                            {"volume", new PyFloat (offer.courierItemVolume * offer.courierAmount)} // calculated shipment volume. *this is direct to window*
                        }
                    }
                }
            };
        } break;
        case Mission::Type::Encounter:
        case Mission::Type::Mining: {
            objectives = new PyTuple {
                new PyTuple {
                    new PyString ("fetch"),
                    new PyTuple {
                        new PyInt (offer.destinationOwnerID),
                        dropoffLocation/*m_agent->GetLocationWrap()*/,
                        new PyDict {
                            {"hasCargo", new PyBool (pClient->ContainsTypeQty (offer.courierTypeID, offer.courierAmount))},
                            {"typeID", new PyInt (offer.courierTypeID)},
                            {"quantity", new PyInt (offer.courierAmount)},
                            {"volume", new PyFloat (offer.courierItemVolume * offer.courierAmount)} // calculated shipment volume. *this is direct to window*
                        }
                    }
                }
            };
        } break;
        case Mission::Type::Anomic:
        case Mission::Type::EpicArc:
        case Mission::Type::Burner:
        case Mission::Type::Cosmos:
        case Mission::Type::Data:
        case Mission::Type::Research:
        case Mission::Type::Storyline:
        case Mission::Type::Tutorial: {
            objectives = new PyTuple {
                PyStatic.NewNone()
            };
        } break;
    }

    // cleanup
    PySafeDecRef(dropoffLocation);

    return objectives;

    /*  objectives data...
    if objType == 'agent':      -- report to agent
        agentID, agentLocation = objData
        agentLocation['locationID']
        agentLocation['locationType']
        agentLocation['solarsystemID']
        if not in station
            agentLocation['coords']
            agentLocation['agentID']
            ?agentLocation['referringAgentID']
            ?agentLocation['shipTypeID']

    elif objType == 'transport':        -- courier and trade missions
        pickupOwnerID, pickupLocation, dropoffOwnerID, dropoffLocation, cargo = objData
        pickupLocation['locationID']
        pickupLocation['locationType']
        pickupLocation['solarsystemID']
        dropoffLocation['locationID']
        dropoffLocation['locationType']
        dropoffLocation['solarsystemID']
        if not in station
            dropoffLocation['coords']
            dropoffLocation['agentID']
            ?dropoffLocation['referringAgentID']
            ?dropoffLocation['shipTypeID']
        cargo['hasCargo']
        cargo['typeID']
        cargo['volume']
        cargo['quantity']

    elif objType == 'fetch':            -- encounter and mining missions
        dropoffOwnerID, dropoffLocation, cargo = objData
        dropoffLocation['locationID']
        dropoffLocation['locationType']
        dropoffLocation['solarsystemID']
        if not in station
            dropoffLocation['coords']
            dropoffLocation['agentID']
            ?dropoffLocation['referringAgentID']
            ?dropoffLocation['shipTypeID']
        cargo['hasCargo']
        cargo['typeID']
        cargo['volume']
        cargo['quantity']
        */
}


/**     ***********************************************************************
 * @note   these do absolutely nothing at this time....
 */

EVEResult AgentBound::GetDungeonShipRestrictions(EVECallArgs&call, PyInt* dungeonID) {
    //restrictions = self.GetAgentMoniker(agentID).GetDungeonShipRestrictions(dungeonID)
    _log(AGENT__DUMP,  "AgentBound::Handle_GetDungeonShipRestrictions() - size=%lli", call.tuple->size());
    call.dump(AGENT__DUMP);

    return nullptr;
}

EVEResult AgentBound::RemoveOfferFromJournal(EVECallArgs&call) {
    //called on rclick in journal to "remove offer"
    //self.GetAgentMoniker(agentID).RemoveOfferFromJournal()
    _log(AGENT__DUMP,  "AgentBound::Handle_RemoveOfferFromJournal() - size=%lli", call.tuple->size());
    call.dump(AGENT__DUMP);

    return nullptr;
}

EVEResult AgentBound::GetOfferJournalInfo(EVECallArgs&call) {
    //html = self.GetAgentMoniker(agentID).GetOfferJournalInfo()
    _log(AGENT__DUMP,  "AgentBound::Handle_GetOfferJournalInfo() - size=%lli", call.tuple->size());
    call.dump(AGENT__DUMP);

    return nullptr;
}

EVEResult AgentBound::GetEntryPoint(EVECallArgs&call) {
    //entryPoint = sm.StartService('agents').GetAgentMoniker(bookmark.agentID).GetEntryPoint()
    _log(AGENT__DUMP,  "AgentBound::Handle_GetEntryPoint() - size=%lli", call.tuple->size());
    call.dump(AGENT__DUMP);

    return nullptr;
}

EVEResult AgentBound::GotoLocation(EVECallArgs&call, PyInt* locationType, PyInt* locationNumber, PyInt* referringAgentID) {
    //sm.StartService('agents').GetAgentMoniker(bookmark.agentID).GotoLocation(bookmark.locationType, bookmark.locationNumber, referringAgentID)
    _log(AGENT__DUMP,  "AgentBound::Handle_GotoLocation() - size=%lli", call.tuple->size());
    call.dump(AGENT__DUMP);

    return nullptr;
}

EVEResult AgentBound::WarpToLocation(EVECallArgs&call, PyInt* locationType, PyInt* locationNumber, PyFloat* warpRange, PyBool* fleet, PyInt* referringAgentID) {
    //sm.StartService('agents').GetAgentMoniker(bookmark.agentID).WarpToLocation(bookmark.locationType, bookmark.locationNumber, warpRange, fleet, referringAgentID)
    _log(AGENT__DUMP,  "AgentBound::Handle_WarpToLocation() - size=%lli", call.tuple->size());
    call.dump(AGENT__DUMP);

    return nullptr;
}
