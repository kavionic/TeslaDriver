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


#ifndef __MAINMENU_H__
#define __MAINMENU_H__

#include "ScrollMenu.h"

class MainMenu
{
public:
    void Initialize();
    void Shutdown() {}
    bool Run(const Event& event);

private:
    ScrollMenu m_Menu;
    static int8_t s_Selection;
}; //MainMenu

#endif //__MAINMENU_H__
