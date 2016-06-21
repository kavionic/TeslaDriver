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


#ifndef __ACCESSPOINTSELECTOR_H__
#define __ACCESSPOINTSELECTOR_H__

#include <stdio.h>
#include "ESP8266/ESP8266.h"

class Event;

class AccessPointSelector
{
public:
    void Initialize();
    bool Run(const Event& event);
    const WifiAccessPoint* GetSelectedAccessPoint() const;
private:
    void Print();
    void SelectItem(int8_t selection);
    static const uint8_t MAX_ACCESS_POINTS = 30;
    WifiAccessPoint m_AccessPoints[MAX_ACCESS_POINTS];
    int8_t          m_AccessPointCount;
    int8_t          m_SelectedItem;
    int8_t          m_FirstVisibleItem;
    int8_t          m_ScrollOffset;
};

#endif //__ACCESSPOINTSELECTOR_H__
