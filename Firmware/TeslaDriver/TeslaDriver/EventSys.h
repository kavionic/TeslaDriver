// This file is part of TeslaDriver.
//
// Copyright (C) 2016 Kurt Skauen <http://kavionic.com/>
//
// TeslaDriver is free software : you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// TeslaDriver is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with TeslaDriver. If not, see < http://www.gnu.org/licenses/>.
///////////////////////////////////////////////////////////////////////////////


#ifndef __EVENTSYS_H__
#define __EVENTSYS_H__

#include "Misc/EventQueue.h"

namespace EventID
{
    enum Enum
    {
        e_None,
        e_TimeTick100,
        e_TimeTick500,
        e_TimeTick1000,
        e_KeyDown,
        e_KeyRepeat,
        e_KeyUp,
        e_KnobTurned,
        e_AccessPointSelected,
        e_ConnectToAccessPoint,
        e_MenuItemEdited,
        e_MenuSelected,
        e_ValueChanged,
        e_ValueSelected,
        e_TextEditCanceled,
        e_TextEditDone,
        e_SetUIState
    };
}

struct Event
{
    Event() {}
    Event(EventID::Enum t, int8_t v) : type(t), value(v) {}
    
    EventID::Enum type;
    int8_t        value;
};

class EventSys
{
public:
    static void AddEvent(const Event& event);
    static bool GetEvent(Event* event);
    static Event* PeekEvent();
    
private:
    EventSys( const EventSys &c );
    EventSys& operator=( const EventSys &c );


    static EventQueue<Event, 8> s_EventQueue;

};


#endif //__EVENTSYS_H__
