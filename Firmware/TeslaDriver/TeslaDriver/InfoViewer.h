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


#ifndef __INFOVIEWER_H__
#define __INFOVIEWER_H__

class Event;

namespace InfoViewPage
{
    enum Enum
    {
        e_PageSummary,
        e_PageNetwork1,
        e_PageNetwork2,
        e_PagePWM,
        e_PageSystem,
        e_PageCount
    };
}

class InfoViewer
{
public:
    void Initialize();
    bool Run(const Event& event);

private:
    bool DisplaySummary(const Event& event, bool initialize);
    bool DisplayNetwork1(const Event& event, bool initialize);
    bool DisplayNetwork2(const Event& event, bool initialize);
    bool DisplayPWM(const Event& event, bool initialize);
    bool DisplaySystem(const Event& event, bool initialize);
    
    InfoViewPage::Enum m_CurrentPage;
    InfoViewPage::Enum m_PrevPage;
};

#endif //__INFOVIEWER_H__
