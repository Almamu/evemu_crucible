/*
 *  CalendarDB.h
 *
 *  Allan
*/


#ifndef EVEMU_SYSTEM_CALENDARDB_H_
#define EVEMU_SYSTEM_CALENDARDB_H_


#include "ServiceDB.h"



class CalendarDB {
  public:

      static void DeleteEvent(uint32 eventID);
      static uint32 SaveSystemEvent(uint32 ownerID, uint32 creatorID, int64 startDateTime, uint8 autoEventType,
                                    std::string title, std::string description, bool important=false);
      static void SaveEventResponse(uint32 charID, uint32 eventID, uint32 response);
      static void UpdateEventParticipants(); // TODO: implement this

      static PyDataType* SaveNewEvent(uint32 ownerID, Call_CreateEventWithInvites &args);
      static PyDataType* SaveNewEvent(uint32 ownerID, uint32 creatorID, Call_CreateEvent &args);
      static PyDataType* GetEventList(uint32 ownerID, uint32 month, uint32 year);
      static PyDataType* GetEventDetails(uint32 eventID);

      static PyDataType* GetResponsesToEvent(uint32 eventID);
      static PyDataType* GetResponsesForCharacter(uint32 charID);

  protected:

};

#endif // EVEMU_SYSTEM_CALENDARDB_H_
