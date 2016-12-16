// This file is part of TeslaTuner.
//
// Copyright (C) 2016 Kurt Skauen <http://kavionic.com/>
//
// TeslaTuner is free software : you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// TeslaTuner is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with TeslaTuner. If not, see < http://www.gnu.org/licenses/>.
///////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "TeslaTuner.h"
#include <QtWidgets/QApplication>

#define CPU_FREQ 32000000

#define I8(value) static_cast<int8_t>(value)
#define U8(value) static_cast<uint8_t>(value)

#define I16(value) static_cast<int16_t>(value)
#define U16(value) static_cast<uint16_t>(value)

#define I32(value) static_cast<int32_t>(value)
#define U32(value) static_cast<uint32_t>(value)

#define I64(value) static_cast<int64_t>(value)
#define U64(value) static_cast<uint64_t>(value)

#define BIT(pos, value) ((value)<<(pos))
#define BIT8(pos, value)  U8((value)<<(pos))
#define BIT16(pos, value) U16((value)<<(pos))
#define BIT32(pos, value) U32((value)<<(pos))

#define DIV_ROUND(a,divider) (((a) + ((divider) >> 1)) / (divider))

int32_t CalculateBaudrate(int8_t bscale, uint16_t bsel, int8_t clockShifter)
{
    // double scaleP2 = pow(2.0, bscale);
    if (bscale >= 0)
    {
        //            baudrateF = double(CPU_FREQ) / (scaleP2 * 8.0*(double(bsel) + 1.0));
        int32_t divider = (bsel + 1) << bscale;
        return DIV_ROUND((CPU_FREQ >> clockShifter), divider);
    }
    else
    {
        //            double tmp = scaleP2 * double(bsel);
        //            baudrateF = (CPU_FREQ/clockScale) / (scaleP2 * bsel + 1.0);
        //            baudrateF = (CPU_FREQ/clockScale/scaleP2) / (bsel + 1.0*scaleP2);
        //            baudrateF = (CPU_FREQ/clockScale/scaleP2) / (bsel + scaleP2);
        int8_t scaleShift = -bscale;
        int8_t fShift = scaleShift - clockShifter;
        uint32_t f = (fShift >= 0) ? (CPU_FREQ << fShift) : (CPU_FREQ >> (-fShift));
        int32_t divider = bsel + (1 << scaleShift);
        return DIV_ROUND(f, divider);
    }
}


void CalculateBaudrateParamsOld(uint32_t baudrate, int8_t& bscale, uint16_t& bsel)
{
    bscale = 0;
    bsel = 0;
    int32_t smallestOffset = 10000000;

    for (int8_t bscaleCur = -7 ; bscaleCur <= 7 ; ++bscaleCur)
    {
        if (bscaleCur < 0)
        {
            float bscalePow2 = U16(U16(1) << (-bscaleCur));
            uint8_t bscalePow22 = 1 << (-bscaleCur);
            //            ESP8266_LOG_ERROR(PSTRL("%d\n"), int(bscalePow2 * 10.0));

            uint32_t bselCur = uint32_t(((double(CPU_FREQ)/(8.0*baudrate) - 1) * bscalePow2) + 0.5);
            if (bselCur <= 4095)
            {
                //            uint16_t bselCur = round(((double(CPU_FREQ)/(8*baudrate) - 1)/pow(2.0, bscaleCur)));
                //            uint16_t bselCur = round((1.0/pow(2.0, bscaleCur)) * (CPU_FREQ/(8*baudrate) - 1));
                //            uint32_t realBaudrate = CPU_FREQ / (8*( pow(2.0, bscaleCur) * bselCur + 1 ));
                //                uint32_t realBaudrate = double(CPU_FREQ) / (8.0*(float(bselCur) / bscalePow2 + 1));
                uint32_t realBaudrate = U32(double(CPU_FREQ) / (8.0 * (float(bselCur) / bscalePow2 + 1)));
                uint32_t realBaudrate2 = U32(double(CPU_FREQ) / (8.0 * (float(bselCur) / bscalePow22 + 1)));
                if (realBaudrate != realBaudrate2) {
//                    ESP8266_LOG_ERROR(PSTRL("  bscale: %d, bsel: %ld (%ld / %ld)\n"), bscaleCur, bselCur, realBaudrate, realBaudrate2);
                }
                int32_t offset = realBaudrate - baudrate;
                if (offset < 0) offset = -offset;
                if (offset < smallestOffset)
                {
                    bscale = bscaleCur;
                    bsel = bselCur;
                    smallestOffset = offset;
                }
            }
        }
        else
        {
            float bscalePow2 = U16(1) << bscaleCur;
            uint16_t bselCur = double(CPU_FREQ) / (bscalePow2 * 8 * baudrate) - 1;
            if (bselCur <= 4095)
            {
                uint32_t realBaudrate = CPU_FREQ / (bscalePow2 * 8 * (bselCur + 1));
                int32_t offset = realBaudrate - baudrate;
                if (offset < 0) offset = -offset;
                //ESP8266_LOG_ERROR(PSTRL("  bscale: %d, bsel: %d (%ld, %ld)\n"), bscaleCur, bselCur, realBaudrate, offset);
                if (offset < smallestOffset)
                {
                    bscale = bscaleCur;
                    bsel = bselCur;
                    smallestOffset = offset;
                }
            }
        }
    }
}

void CalculateBaudrateParamsOld2(uint32_t baudrate, int8_t& bscale, uint16_t& bsel)
{
    bscale = 0;
    bsel = 0;
    int32_t smallestOffset = 10000000;

    for (int8_t bscaleCur = -7 ; bscaleCur <= 7 ; ++bscaleCur)
    {
        if (bscaleCur < 0)
        {
            float bscalePow2 = U16(U16(1) << (-bscaleCur));
            uint8_t bscalePow22 = 1 << (-bscaleCur);
            //            ESP8266_LOG_ERROR(PSTRL("%d\n"), int(bscalePow2 * 10.0));

            uint32_t bselCur = uint32_t(((double(CPU_FREQ)/(8.0*baudrate) - 1) * bscalePow2) + 0.5);
            if (bselCur <= 4095)
            {
                //            uint16_t bselCur = round(((double(CPU_FREQ)/(8*baudrate) - 1)/pow(2.0, bscaleCur)));
                //            uint16_t bselCur = round((1.0/pow(2.0, bscaleCur)) * (CPU_FREQ/(8*baudrate) - 1));
                //            uint32_t realBaudrate = CPU_FREQ / (8*( pow(2.0, bscaleCur) * bselCur + 1 ));
                //                uint32_t realBaudrate = double(CPU_FREQ) / (8.0*(float(bselCur) / bscalePow2 + 1));
                uint32_t realBaudrate  = U32(double(CPU_FREQ) / (8.0 * (float(bselCur) / bscalePow2 + 1)));
                uint32_t realBaudrate2 = U32(double(CPU_FREQ) / (8.0 * (float(bselCur) / bscalePow22 + 1)));
                if (realBaudrate != realBaudrate2) {
//                    ESP8266_LOG_ERROR(PSTRL("  bscale: %d, bsel: %ld (%ld / %ld)\n"), bscaleCur, bselCur, realBaudrate, realBaudrate2);
                }
                int32_t offset = realBaudrate - baudrate;
                if (offset < 0) offset = -offset;
                if (offset < smallestOffset)
                {
                    bscale = bscaleCur;
                    bsel = bselCur;
                    smallestOffset = offset;
                }
            }
        }
        else
        {
            float bscalePow2 = U16(1) << bscaleCur;
//            uint32_t bselCur = uint32_t(((double(CPU_FREQ)/(8.0*baudrate) - 1) * bscalePow2) + 0.5);
            uint16_t bselCur = double(CPU_FREQ) / (bscalePow2 * 8 * baudrate) - 1;
            if (bselCur <= 4095)
            {
                uint32_t realBaudrate = U32(double(CPU_FREQ) / (8.0 * (float(bselCur) / bscalePow2 + 1)));
//                uint32_t realBaudrate = CPU_FREQ / (bscalePow2 * 8 * (bselCur + 1));
                int32_t offset = realBaudrate - baudrate;
                if (offset < 0) offset = -offset;
                //ESP8266_LOG_ERROR(PSTRL("  bscale: %d, bsel: %d (%ld, %ld)\n"), bscaleCur, bselCur, realBaudrate, offset);
                if (offset < smallestOffset)
                {
                    bscale = bscaleCur;
                    bsel = bselCur;
                    smallestOffset = offset;
                }
            }
        }
    }
}

void CalculateBaudrateParams(uint32_t baudrate, int8_t& bscale, uint16_t& bsel)
{
    bscale = 0;
    bsel = 0;
    int32_t smallestOffset = 10000000;

    int8_t clockShifter = 3;

    // Check negative bscale values:
    for (int8_t bscaleCurNeg = 1 ; bscaleCurNeg <= 7 ; ++bscaleCurNeg)
    {
        int8_t fShift = bscaleCurNeg - clockShifter;
        uint32_t f = (fShift >= 0) ? (CPU_FREQ << fShift) : (CPU_FREQ >> (-fShift));
        uint16_t bselCur = DIV_ROUND(f, baudrate) - (1 << bscaleCurNeg);
        if (bselCur <= 4095)
        {
            //            baudrateF = (CPU_FREQ/clockScale) / (scaleP2 * bsel + 1.0);
            //            baudrateF = (CPU_FREQ/clockScale/scaleP2) / (bsel + 1.0*scaleP2);
            //            baudrateF = (CPU_FREQ/clockScale/scaleP2) / (bsel + scaleP2);
            int16_t divider = bselCur + (1 << bscaleCurNeg);
            uint32_t realBaudrate = DIV_ROUND(f, divider);

            int32_t offset = realBaudrate - baudrate;
            if (offset < 0) offset = -offset;
            if (offset < smallestOffset)
            {
                bscale = -bscaleCurNeg;
                bsel = bselCur;
                smallestOffset = offset;
            }
        }
    }
    // Check positive bscale values:
    for (int8_t bscaleCur = 0 ; bscaleCur <= 7 ; ++bscaleCur)
    {
        uint32_t divider = baudrate << (bscaleCur + clockShifter);
        uint16_t bselCur = DIV_ROUND(CPU_FREQ, divider) - 1;
        if (bselCur <= 4095)
        {
            divider = (bselCur + 1) << bscaleCur;
            uint32_t realBaudrate = ((CPU_FREQ >> clockShifter) + (divider >> 1)) / divider;

            int32_t offset = realBaudrate - baudrate;
            if (offset < 0) offset = -offset;
            if (offset < smallestOffset)
            {
                bscale = bscaleCur;
                bsel = bselCur;
                smallestOffset = offset;
            }
        }
    }
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    double largestError = 0.000438596;
    int32_t worstBaudrate = 0;
    int8_t divider = 8;
    int8_t clockShifter = 3;
    for (int32_t i = 10; i < 4000000; ++i)
    {
        int8_t bscaleOld;
        uint16_t bselOld;
        int8_t bscale;
        uint16_t bsel;

//        CalculateBaudrateParamsOld(i, bscaleOld, bselOld);
        CalculateBaudrateParams(i, bscale, bsel);
        double baudrateF = CalculateBaudrate(bscale, bsel, clockShifter);
/*        double scaleP2 = pow(2.0, bscale);
        if (bscale >= 0)
        {
//            baudrateF = double(CPU_FREQ) / (scaleP2 * 8.0*(double(bsel) + 1.0));
            baudrateF = (CPU_FREQ >> clockShifter) / ((bsel + 1) << bscale);
        }
        else
        {
//            double tmp = scaleP2 * double(bsel);
//            baudrateF = (CPU_FREQ/clockScale) / (scaleP2 * bsel + 1.0);
//            baudrateF = (CPU_FREQ/clockScale/scaleP2) / (bsel + 1.0*scaleP2);
//            baudrateF = (CPU_FREQ/clockScale/scaleP2) / (bsel + scaleP2);
            int8_t scaleShift = -bscale;
            int8_t fShift = scaleShift - clockShifter;
            uint32_t f = (fShift >= 0) ? (CPU_FREQ << fShift) : (CPU_FREQ >> (-fShift));
            baudrateF = f / (bsel + (1 << scaleShift));
       }*/
        double error = abs(double(i) / baudrateF - 1.0);
        //  int32_t baudrate = int32_t(baudrateF);
        if (error > largestError)
        {
            largestError = error;
            worstBaudrate = i;
 //           CalculateBaudrateParams(i, bscale, bsel);
        }
        /*        if (bscale != bscaleOld || bsel != bselOld)
        {
            CalculateBaudrateParamsOld(i, bscaleOld, bselOld);
            CalculateBaudrateParams(i, bscale, bsel);
        }*/
    }

    app.setApplicationName("TeslaTuner");
    app.setApplicationVersion("0.1");
    app.setOrganizationName("Kavionic");
    app.setOrganizationDomain("kavionic.com");

    TeslaTuner w;
    w.show();
    return app.exec();
}
