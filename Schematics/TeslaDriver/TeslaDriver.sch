EESchema Schematic File Version 2
LIBS:MyDiodes
LIBS:MyTransistors
LIBS:MySymbols
LIBS:MyMagnetics
LIBS:MyPower
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:TeslaDriver-cache
EELAYER 25 0
EELAYER END
$Descr A3 16535 11693
encoding utf-8
Sheet 1 1
Title "VPower"
Date "2015-07-01"
Rev "1"
Comp "Kavionic"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L STP65NF06 Q1
U 1 1 557FCE99
P 7450 2250
F 0 "Q1" H 7250 2450 50  0000 L CNN
F 1 "STP65NF06" H 7650 2250 50  0000 L CNN
F 2 "MyFootprints:TO-220" H 7650 2175 50  0001 L CIN
F 3 "" H 7450 2250 50  0000 L CNN
	1    7450 2250
	1    0    0    -1  
$EndComp
$Comp
L +24V #PWR01
U 1 1 5581F374
P 7550 1850
F 0 "#PWR01" H 7550 1700 50  0001 C CNN
F 1 "+24V" H 7550 1990 50  0000 C CNN
F 2 "" H 7550 1850 60  0000 C CNN
F 3 "" H 7550 1850 60  0000 C CNN
	1    7550 1850
	1    0    0    -1  
$EndComp
$Comp
L C_Small C8
U 1 1 558357A5
P 2900 3200
F 0 "C8" H 2910 3270 50  0000 L CNN
F 1 "100n" H 2910 3120 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 2900 3200 60  0001 C CNN
F 3 "" H 2900 3200 60  0000 C CNN
	1    2900 3200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 55835B67
P 1750 3450
F 0 "#PWR02" H 1750 3200 50  0001 C CNN
F 1 "GND" H 1750 3300 50  0000 C CNN
F 2 "" H 1750 3450 60  0000 C CNN
F 3 "" H 1750 3450 60  0000 C CNN
	1    1750 3450
	1    0    0    -1  
$EndComp
$Comp
L C_Small C4
U 1 1 55836BE0
P 2700 3200
F 0 "C4" H 2710 3270 50  0000 L CNN
F 1 "1u" H 2710 3120 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 2700 3200 60  0001 C CNN
F 3 "" H 2700 3200 60  0000 C CNN
	1    2700 3200
	1    0    0    -1  
$EndComp
$Comp
L CP_Small C27
U 1 1 558396E2
P 3600 1350
F 0 "C27" V 3650 1100 50  0000 L CNN
F 1 "1000u/63V" V 3450 1200 50  0000 L CNN
F 2 "MyCapacitors:C_Radial_D16_L25_P7.5" H 3600 1350 60  0001 C CNN
F 3 "" H 3600 1350 60  0000 C CNN
	1    3600 1350
	1    0    0    -1  
$EndComp
$Comp
L CP_Small C28
U 1 1 558396E8
P 3900 1350
F 0 "C28" V 3950 1100 50  0000 L CNN
F 1 "1000u/63V" V 3750 1200 50  0000 L CNN
F 2 "MyCapacitors:C_Radial_D16_L25_P7.5" H 3900 1350 60  0001 C CNN
F 3 "" H 3900 1350 60  0000 C CNN
	1    3900 1350
	1    0    0    -1  
$EndComp
$Comp
L C_Small C33
U 1 1 5583973B
P 4200 1350
F 0 "C33" V 4250 1100 50  0000 L CNN
F 1 "2.2u/63V" V 4100 1200 50  0000 L CNN
F 2 "MyCapacitors:C_Rect_L18_W8_P15" H 4200 1350 60  0001 C CNN
F 3 "" H 4200 1350 60  0000 C CNN
	1    4200 1350
	1    0    0    -1  
$EndComp
$Comp
L C_Small C38
U 1 1 5583998A
P 4500 1350
F 0 "C38" V 4550 1100 50  0000 L CNN
F 1 "2.2u/63V" V 4400 1200 50  0000 L CNN
F 2 "MyCapacitors:C_Rect_L18_W8_P15" H 4500 1350 60  0001 C CNN
F 3 "" H 4500 1350 60  0000 C CNN
	1    4500 1350
	1    0    0    -1  
$EndComp
Text Label 700  950  1    47   ~ 0
IInSense
Text Label 2700 2700 2    47   ~ 0
GatePWM1H
Text Label 2700 3750 2    47   ~ 0
GatePWM1L
$Comp
L DS18B20 U1
U 1 1 558B3E36
P 2300 9850
F 0 "U1" H 2300 9700 60  0000 C CNN
F 1 "DS18B20" H 2275 10000 60  0000 C CNN
F 2 "MyFootprints:TO-92_Inline_Narrow_Oval" H 2300 9850 60  0001 C CNN
F 3 "" H 2300 9850 60  0000 C CNN
	1    2300 9850
	0    1    -1   0   
$EndComp
$Comp
L R_Small R1
U 1 1 558B3FF1
P 2050 9900
F 0 "R1" H 2080 9920 50  0000 L CNN
F 1 "4k7" H 2080 9860 50  0000 L CNN
F 2 "MyResistors:R_0603" H 2050 9900 60  0001 C CNN
F 3 "" H 2050 9900 60  0000 C CNN
	1    2050 9900
	-1   0    0    1   
$EndComp
Text GLabel 2350 9350 1    47   Input ~ 0
TEMP_MOSFET1
$Comp
L DS18B20 U2
U 1 1 558B587C
P 3000 9850
F 0 "U2" H 3000 9700 60  0000 C CNN
F 1 "DS18B20" H 2975 10000 60  0000 C CNN
F 2 "MyFootprints:TO-92_Inline_Narrow_Oval" H 3000 9850 60  0001 C CNN
F 3 "" H 3000 9850 60  0000 C CNN
	1    3000 9850
	0    1    -1   0   
$EndComp
$Comp
L R_Small R2
U 1 1 558B5882
P 2750 9900
F 0 "R2" H 2780 9920 50  0000 L CNN
F 1 "4k7" H 2780 9860 50  0000 L CNN
F 2 "MyResistors:R_0603" H 2750 9900 60  0001 C CNN
F 3 "" H 2750 9900 60  0000 C CNN
	1    2750 9900
	-1   0    0    1   
$EndComp
Text GLabel 3050 9350 1    47   Input ~ 0
TEMP_MOSFET2
$Comp
L +3.3V #PWR03
U 1 1 558B6D8A
P 3350 10050
F 0 "#PWR03" H 3350 9900 50  0001 C CNN
F 1 "+3.3V" H 3350 10190 50  0000 C CNN
F 2 "" H 3350 10050 60  0000 C CNN
F 3 "" H 3350 10050 60  0000 C CNN
	1    3350 10050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR04
U 1 1 558CA78F
P 2600 10650
F 0 "#PWR04" H 2600 10400 50  0001 C CNN
F 1 "GND" H 2600 10500 50  0000 C CNN
F 2 "" H 2600 10650 60  0000 C CNN
F 3 "" H 2600 10650 60  0000 C CNN
	1    2600 10650
	1    0    0    -1  
$EndComp
Text GLabel 1750 2000 2    47   Input ~ 0
TEMP_MOSFET1
Text GLabel 1750 1900 2    47   Input ~ 0
TEMP_MOSFET2
$Comp
L CONN_01X16 P1
U 1 1 55A84FA1
P 1450 2650
F 0 "P1" H 1450 3500 50  0000 C CNN
F 1 "CONTROL" V 1550 2650 50  0000 C CNN
F 2 "MyConnectors:PinHeaderMale_2x08" H 1450 2650 60  0001 C CNN
F 3 "" H 1450 2650 60  0000 C CNN
	1    1450 2650
	-1   0    0    1   
$EndComp
$Comp
L GNDPWR #PWR05
U 1 1 55C2BB8B
P 3450 1800
F 0 "#PWR05" H 3450 1600 50  0001 C CNN
F 1 "GNDPWR" H 3450 1670 50  0000 C CNN
F 2 "" H 3450 1750 60  0000 C CNN
F 3 "" H 3450 1750 60  0000 C CNN
	1    3450 1800
	1    0    0    -1  
$EndComp
$Comp
L GNDPWR #PWR06
U 1 1 55C2C61C
P 7550 6450
F 0 "#PWR06" H 7550 6250 50  0001 C CNN
F 1 "GNDPWR" H 7550 6320 50  0000 C CNN
F 2 "" H 7550 6400 60  0000 C CNN
F 3 "" H 7550 6400 60  0000 C CNN
	1    7550 6450
	1    0    0    -1  
$EndComp
$Comp
L GateTR10x4 T1
U 1 1 5683615A
P 5400 2450
F 0 "T1" H 5400 2680 50  0000 C CNN
F 1 "GateTR10x4" H 5410 2225 50  0000 C CNN
F 2 "MyMagnetics:GateTR10x4" H 5400 2400 60  0001 C CNN
F 3 "" H 5400 2400 60  0000 C CNN
	1    5400 2450
	1    0    0    -1  
$EndComp
$Comp
L STP65NF06 Q2
U 1 1 5685F438
P 7450 3300
F 0 "Q2" H 7250 3500 50  0000 L CNN
F 1 "STP65NF06" H 7650 3300 50  0000 L CNN
F 2 "MyFootprints:TO-220" H 7650 3225 50  0001 L CIN
F 3 "" H 7450 3300 50  0000 L CNN
	1    7450 3300
	1    0    0    -1  
$EndComp
$Comp
L GateTR10x4 T2
U 1 1 5685F43E
P 5400 3500
F 0 "T2" H 5400 3730 50  0000 C CNN
F 1 "GateTR10x4" H 5410 3275 50  0000 C CNN
F 2 "MyMagnetics:GateTR10x4" H 5400 3450 60  0001 C CNN
F 3 "" H 5400 3450 60  0000 C CNN
	1    5400 3500
	1    0    0    -1  
$EndComp
$Comp
L STP65NF06 Q3
U 1 1 568615A1
P 7450 5050
F 0 "Q3" H 7250 5250 50  0000 L CNN
F 1 "STP65NF06" H 7650 5050 50  0000 L CNN
F 2 "MyFootprints:TO-220" H 7650 4975 50  0001 L CIN
F 3 "" H 7450 5050 50  0000 L CNN
	1    7450 5050
	1    0    0    -1  
$EndComp
$Comp
L +24V #PWR07
U 1 1 568615A7
P 7550 4700
F 0 "#PWR07" H 7550 4550 50  0001 C CNN
F 1 "+24V" H 7550 4840 50  0000 C CNN
F 2 "" H 7550 4700 60  0000 C CNN
F 3 "" H 7550 4700 60  0000 C CNN
	1    7550 4700
	1    0    0    -1  
$EndComp
$Comp
L GateTR10x4 T3
U 1 1 568615C8
P 5400 5250
F 0 "T3" H 5400 5480 50  0000 C CNN
F 1 "GateTR10x4" H 5410 5025 50  0000 C CNN
F 2 "MyMagnetics:GateTR10x4" H 5400 5200 60  0001 C CNN
F 3 "" H 5400 5200 60  0000 C CNN
	1    5400 5250
	1    0    0    1   
$EndComp
$Comp
L STP65NF06 Q4
U 1 1 5686160E
P 7450 5900
F 0 "Q4" H 7250 6100 50  0000 L CNN
F 1 "STP65NF06" H 7650 5900 50  0000 L CNN
F 2 "MyFootprints:TO-220" H 7650 5825 50  0001 L CIN
F 3 "" H 7450 5900 50  0000 L CNN
	1    7450 5900
	1    0    0    -1  
$EndComp
$Comp
L GateTR10x4 T4
U 1 1 56861614
P 5400 6100
F 0 "T4" H 5400 6330 50  0000 C CNN
F 1 "GateTR10x4" H 5410 5875 50  0000 C CNN
F 2 "MyMagnetics:GateTR10x4" H 5400 6050 60  0001 C CNN
F 3 "" H 5400 6050 60  0000 C CNN
	1    5400 6100
	1    0    0    1   
$EndComp
$Comp
L GND #PWR08
U 1 1 56861ED0
P 2900 3450
F 0 "#PWR08" H 2900 3200 50  0001 C CNN
F 1 "GND" H 2900 3300 50  0000 C CNN
F 2 "" H 2900 3450 60  0000 C CNN
F 3 "" H 2900 3450 60  0000 C CNN
	1    2900 3450
	1    0    0    -1  
$EndComp
$Comp
L GNDPWR #PWR09
U 1 1 56863B45
P 7550 3950
F 0 "#PWR09" H 7550 3750 50  0001 C CNN
F 1 "GNDPWR" H 7550 3820 50  0000 C CNN
F 2 "" H 7550 3900 60  0000 C CNN
F 3 "" H 7550 3900 60  0000 C CNN
	1    7550 3950
	1    0    0    -1  
$EndComp
$Comp
L C_Small C15
U 1 1 56872931
P 4800 2100
F 0 "C15" H 4810 2170 50  0000 L CNN
F 1 "100n" H 4810 2020 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 4800 2100 60  0001 C CNN
F 3 "" H 4800 2100 60  0000 C CNN
	1    4800 2100
	0    -1   -1   0   
$EndComp
$Comp
L +12V #PWR010
U 1 1 5688D589
P 1850 2500
F 0 "#PWR010" H 1850 2350 50  0001 C CNN
F 1 "+12V" V 1850 2600 50  0000 L CNN
F 2 "" H 1850 2500 60  0000 C CNN
F 3 "" H 1850 2500 60  0000 C CNN
	1    1850 2500
	0    1    1    0   
$EndComp
$Comp
L +3.3V #PWR011
U 1 1 5688D630
P 1850 2300
F 0 "#PWR011" H 1850 2150 50  0001 C CNN
F 1 "+3.3V" V 1850 2400 50  0000 L CNN
F 2 "" H 1850 2300 60  0000 C CNN
F 3 "" H 1850 2300 60  0000 C CNN
	1    1850 2300
	0    1    1    0   
$EndComp
$Comp
L +24V #PWR012
U 1 1 5688E52A
P 3450 800
F 0 "#PWR012" H 3450 650 50  0001 C CNN
F 1 "+24V" H 3450 940 50  0000 C CNN
F 2 "" H 3450 800 60  0000 C CNN
F 3 "" H 3450 800 60  0000 C CNN
	1    3450 800 
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X01 P2
U 1 1 5689BCD3
P 2950 1000
F 0 "P2" H 2950 1100 50  0000 C CNN
F 1 "PWR_IN+" V 3050 1000 50  0000 C CNN
F 2 "MyConnectors:PowerPad15_5" H 2950 1000 50  0001 C CNN
F 3 "" H 2950 1000 50  0000 C CNN
	1    2950 1000
	-1   0    0    1   
$EndComp
$Comp
L CONN_01X01 P3
U 1 1 5689BE27
P 2950 1650
F 0 "P3" H 2950 1750 50  0000 C CNN
F 1 "PWR_GND" V 3050 1650 50  0000 C CNN
F 2 "MyConnectors:PowerPad15_5" H 2950 1650 50  0001 C CNN
F 3 "" H 2950 1650 50  0000 C CNN
	1    2950 1650
	-1   0    0    1   
$EndComp
$Comp
L CONN_01X01 P5
U 1 1 5689E340
P 8150 5400
F 0 "P5" H 8150 5500 50  0000 C CNN
F 1 "PWR_OUT_L2" V 8250 5400 50  0000 C CNN
F 2 "MyConnectors:PowerPad15_5" H 8150 5400 50  0001 C CNN
F 3 "" H 8150 5400 50  0000 C CNN
	1    8150 5400
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X01 P4
U 1 1 5689F756
P 8150 2600
F 0 "P4" H 8150 2700 50  0000 C CNN
F 1 "PWR_OUT_L1" V 8250 2600 50  0000 C CNN
F 2 "MyConnectors:PowerPad15_5" H 8150 2600 50  0001 C CNN
F 3 "" H 8150 2600 50  0000 C CNN
	1    8150 2600
	1    0    0    -1  
$EndComp
Text Label 7700 2600 0    60   ~ 0
OUTL1
Text Label 7700 5400 0    60   ~ 0
OUTL2
$Comp
L C_Small C10
U 1 1 5690305E
P 3050 10500
F 0 "C10" H 3060 10570 50  0000 L CNN
F 1 "100n" H 3060 10420 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 3050 10500 60  0001 C CNN
F 3 "" H 3050 10500 60  0000 C CNN
	1    3050 10500
	1    0    0    -1  
$EndComp
$Comp
L C_Small C1
U 1 1 569031F4
P 2350 10500
F 0 "C1" H 2360 10570 50  0000 L CNN
F 1 "100n" H 2360 10420 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 2350 10500 60  0001 C CNN
F 3 "" H 2350 10500 60  0000 C CNN
	1    2350 10500
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR013
U 1 1 569033E5
P 2350 10650
F 0 "#PWR013" H 2350 10400 50  0001 C CNN
F 1 "GND" H 2350 10500 50  0000 C CNN
F 2 "" H 2350 10650 60  0000 C CNN
F 3 "" H 2350 10650 60  0000 C CNN
	1    2350 10650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR014
U 1 1 5690348F
P 3050 10650
F 0 "#PWR014" H 3050 10400 50  0001 C CNN
F 1 "GND" H 3050 10500 50  0000 C CNN
F 2 "" H 3050 10650 60  0000 C CNN
F 3 "" H 3050 10650 60  0000 C CNN
	1    3050 10650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR015
U 1 1 576DBD15
P 3300 3200
F 0 "#PWR015" H 3300 2950 50  0001 C CNN
F 1 "GND" H 3300 3050 50  0000 C CNN
F 2 "" H 3300 3200 60  0000 C CNN
F 3 "" H 3300 3200 60  0000 C CNN
	1    3300 3200
	1    0    0    -1  
$EndComp
$Comp
L NCP81071A U3
U 1 1 576DBC1A
P 5050 3350
F 0 "U3" H 3900 4200 60  0000 C CNN
F 1 "NCP81071A" H 3900 3450 60  0000 C CNN
F 2 "MyFootprints:SOIC-8_3.9x4.9mm_Pitch1.27mm" H 5050 3350 60  0001 C CNN
F 3 "" H 5050 3350 60  0000 C CNN
	1    5050 3350
	1    0    0    -1  
$EndComp
$Comp
L +12V #PWR016
U 1 1 576DDEF3
P 3200 2500
F 0 "#PWR016" H 3200 2350 50  0001 C CNN
F 1 "+12V" V 3200 2600 50  0000 L CNN
F 2 "" H 3200 2500 60  0000 C CNN
F 3 "" H 3200 2500 60  0000 C CNN
	1    3200 2500
	1    0    0    -1  
$EndComp
$Comp
L C_Small C16
U 1 1 576DF0BA
P 4800 2300
F 0 "C16" H 4810 2370 50  0000 L CNN
F 1 "1u" H 4810 2220 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 4800 2300 60  0001 C CNN
F 3 "" H 4800 2300 60  0000 C CNN
	1    4800 2300
	0    -1   -1   0   
$EndComp
Text Label 2200 2100 2    47   ~ 0
GateEnable
$Comp
L GND #PWR017
U 1 1 576E09F1
P 3300 4250
F 0 "#PWR017" H 3300 4000 50  0001 C CNN
F 1 "GND" H 3300 4100 50  0000 C CNN
F 2 "" H 3300 4250 60  0000 C CNN
F 3 "" H 3300 4250 60  0000 C CNN
	1    3300 4250
	1    0    0    -1  
$EndComp
$Comp
L +12V #PWR018
U 1 1 576E0A09
P 3200 3600
F 0 "#PWR018" H 3200 3450 50  0001 C CNN
F 1 "+12V" H 3150 3750 50  0000 L CNN
F 2 "" H 3200 3600 60  0000 C CNN
F 3 "" H 3200 3600 60  0000 C CNN
	1    3200 3600
	1    0    0    -1  
$EndComp
$Comp
L C_Small C12
U 1 1 576E1068
P 4700 3350
F 0 "C12" H 4710 3420 50  0000 L CNN
F 1 "1u" H 4710 3270 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 4700 3350 60  0001 C CNN
F 3 "" H 4700 3350 60  0000 C CNN
	1    4700 3350
	0    -1   -1   0   
$EndComp
$Comp
L C_Small C11
U 1 1 576E114B
P 4700 3150
F 0 "C11" H 4710 3220 50  0000 L CNN
F 1 "100n" H 4710 3070 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 4700 3150 60  0001 C CNN
F 3 "" H 4700 3150 60  0000 C CNN
	1    4700 3150
	0    -1   -1   0   
$EndComp
$Comp
L NCP81071A U4
U 1 1 576E09FF
P 5050 4400
F 0 "U4" H 3900 5250 60  0000 C CNN
F 1 "NCP81071A" H 3900 4500 60  0000 C CNN
F 2 "MyFootprints:SOIC-8_3.9x4.9mm_Pitch1.27mm" H 5050 4400 60  0001 C CNN
F 3 "" H 5050 4400 60  0000 C CNN
	1    5050 4400
	1    0    0    -1  
$EndComp
Text Label 2650 6350 2    47   ~ 0
GatePWM2L
Text Label 2700 5300 2    47   ~ 0
GatePWM2H
$Comp
L C_Small C17
U 1 1 576E5427
P 4800 4900
F 0 "C17" H 4810 4970 50  0000 L CNN
F 1 "100n" H 4810 4820 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 4800 4900 60  0001 C CNN
F 3 "" H 4800 4900 60  0000 C CNN
	1    4800 4900
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR019
U 1 1 576E5430
P 3300 5700
F 0 "#PWR019" H 3300 5450 50  0001 C CNN
F 1 "GND" H 3300 5550 50  0000 C CNN
F 2 "" H 3300 5700 60  0000 C CNN
F 3 "" H 3300 5700 60  0000 C CNN
	1    3300 5700
	1    0    0    -1  
$EndComp
$Comp
L NCP81071A U5
U 1 1 576E543E
P 5050 5850
F 0 "U5" H 3900 6700 60  0000 C CNN
F 1 "NCP81071A" H 3900 5950 60  0000 C CNN
F 2 "MyFootprints:SOIC-8_3.9x4.9mm_Pitch1.27mm" H 5050 5850 60  0001 C CNN
F 3 "" H 5050 5850 60  0000 C CNN
	1    5050 5850
	1    0    0    -1  
$EndComp
$Comp
L +12V #PWR020
U 1 1 576E5449
P 3200 5000
F 0 "#PWR020" H 3200 4850 50  0001 C CNN
F 1 "+12V" V 3200 5100 50  0000 L CNN
F 2 "" H 3200 5000 60  0000 C CNN
F 3 "" H 3200 5000 60  0000 C CNN
	1    3200 5000
	1    0    0    -1  
$EndComp
$Comp
L C_Small C18
U 1 1 576E5456
P 4800 5100
F 0 "C18" H 4810 5170 50  0000 L CNN
F 1 "1u" H 4810 5020 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 4800 5100 60  0001 C CNN
F 3 "" H 4800 5100 60  0000 C CNN
	1    4800 5100
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR021
U 1 1 576E5467
P 3300 6750
F 0 "#PWR021" H 3300 6500 50  0001 C CNN
F 1 "GND" H 3300 6600 50  0000 C CNN
F 2 "" H 3300 6750 60  0000 C CNN
F 3 "" H 3300 6750 60  0000 C CNN
	1    3300 6750
	1    0    0    -1  
$EndComp
$Comp
L +12V #PWR022
U 1 1 576E5478
P 3200 6100
F 0 "#PWR022" H 3200 5950 50  0001 C CNN
F 1 "+12V" H 3150 6250 50  0000 L CNN
F 2 "" H 3200 6100 60  0000 C CNN
F 3 "" H 3200 6100 60  0000 C CNN
	1    3200 6100
	1    0    0    -1  
$EndComp
$Comp
L C_Small C14
U 1 1 576E5486
P 4700 5950
F 0 "C14" H 4710 6020 50  0000 L CNN
F 1 "1u" H 4710 5870 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 4700 5950 60  0001 C CNN
F 3 "" H 4700 5950 60  0000 C CNN
	1    4700 5950
	0    -1   -1   0   
$EndComp
$Comp
L C_Small C13
U 1 1 576E548C
P 4700 5750
F 0 "C13" H 4710 5820 50  0000 L CNN
F 1 "100n" H 4710 5670 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 4700 5750 60  0001 C CNN
F 3 "" H 4700 5750 60  0000 C CNN
	1    4700 5750
	0    -1   -1   0   
$EndComp
$Comp
L NCP81071A U6
U 1 1 576E5492
P 5050 6900
F 0 "U6" H 3900 7750 60  0000 C CNN
F 1 "NCP81071A" H 3900 7000 60  0000 C CNN
F 2 "MyFootprints:SOIC-8_3.9x4.9mm_Pitch1.27mm" H 5050 6900 60  0001 C CNN
F 3 "" H 5050 6900 60  0000 C CNN
	1    5050 6900
	1    0    0    -1  
$EndComp
$Comp
L SS210 D8
U 1 1 576E6D7F
P 6550 5850
F 0 "D8" H 6500 5930 50  0000 L CNN
F 1 "SS210" H 6450 5750 50  0000 L CNN
F 2 "MyDiodes:Diode-SMA_Standard" V 6550 5850 60  0001 C CNN
F 3 "" V 6550 5850 60  0000 C CNN
	1    6550 5850
	0    -1   -1   0   
$EndComp
$Comp
L ZENERsmall D4
U 1 1 576E6E8C
P 6450 6100
F 0 "D4" H 6450 6200 50  0000 C CNN
F 1 "ZMY12" H 6450 6000 50  0000 C CNN
F 2 "MyDiodes:MELF_Standard" H 6450 6100 50  0001 C CNN
F 3 "" H 6450 6100 50  0000 C CNN
	1    6450 6100
	0    1    1    0   
$EndComp
$Comp
L C_Small C32
U 1 1 576E7041
P 6650 6100
F 0 "C32" H 6660 6170 50  0000 L CNN
F 1 "1u" H 6660 6020 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 6650 6100 60  0001 C CNN
F 3 "" H 6650 6100 60  0000 C CNN
	1    6650 6100
	1    0    0    -1  
$EndComp
$Comp
L ZENERsmall D12
U 1 1 576E7B20
P 6900 6100
F 0 "D12" H 6900 6200 50  0000 C CNN
F 1 "ZMY12" H 6900 6000 50  0000 C CNN
F 2 "MyDiodes:MELF_Standard" H 6900 6100 50  0001 C CNN
F 3 "" H 6900 6100 50  0000 C CNN
	1    6900 6100
	0    -1   -1   0   
$EndComp
$Comp
L C_Small C37
U 1 1 576E7B26
P 7100 6100
F 0 "C37" H 7110 6170 50  0000 L CNN
F 1 "1u" H 7110 6020 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 7100 6100 60  0001 C CNN
F 3 "" H 7100 6100 60  0000 C CNN
	1    7100 6100
	1    0    0    -1  
$EndComp
$Comp
L SS210 D16
U 1 1 576E7E21
P 7000 5850
F 0 "D16" H 6950 5930 50  0000 L CNN
F 1 "SS210" H 6900 5750 50  0000 L CNN
F 2 "MyDiodes:Diode-SMA_Standard" V 7000 5850 60  0001 C CNN
F 3 "" V 7000 5850 60  0000 C CNN
	1    7000 5850
	0    1    1    0   
$EndComp
$Comp
L C_Small C26
U 1 1 576E89D6
P 6050 5950
F 0 "C26" H 6060 6020 50  0000 L CNN
F 1 "1u" H 6060 5870 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 6050 5950 50  0001 C CNN
F 3 "" H 6050 5950 50  0000 C CNN
	1    6050 5950
	0    1    1    0   
$EndComp
$Comp
L C_Small C25
U 1 1 576E89DC
P 6050 5750
F 0 "C25" H 6060 5820 50  0000 L CNN
F 1 "100n" H 6060 5670 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 6050 5750 60  0001 C CNN
F 3 "" H 6050 5750 60  0000 C CNN
	1    6050 5750
	0    -1   -1   0   
$EndComp
$Comp
L SS210 D7
U 1 1 576ECBDB
P 6550 5000
F 0 "D7" H 6500 5080 50  0000 L CNN
F 1 "SS210" H 6450 4900 50  0000 L CNN
F 2 "MyDiodes:Diode-SMA_Standard" V 6550 5000 60  0001 C CNN
F 3 "" V 6550 5000 60  0000 C CNN
	1    6550 5000
	0    -1   -1   0   
$EndComp
$Comp
L ZENERsmall D3
U 1 1 576ECBE1
P 6450 5250
F 0 "D3" H 6450 5350 50  0000 C CNN
F 1 "ZMY12" H 6450 5150 50  0000 C CNN
F 2 "MyDiodes:MELF_Standard" H 6450 5250 50  0001 C CNN
F 3 "" H 6450 5250 50  0000 C CNN
	1    6450 5250
	0    1    1    0   
$EndComp
$Comp
L C_Small C31
U 1 1 576ECBE7
P 6650 5250
F 0 "C31" H 6660 5320 50  0000 L CNN
F 1 "1u" H 6660 5170 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 6650 5250 60  0001 C CNN
F 3 "" H 6650 5250 60  0000 C CNN
	1    6650 5250
	1    0    0    -1  
$EndComp
$Comp
L ZENERsmall D11
U 1 1 576ECBF2
P 6900 5250
F 0 "D11" H 6900 5350 50  0000 C CNN
F 1 "ZMY12" H 6900 5150 50  0000 C CNN
F 2 "MyDiodes:MELF_Standard" H 6900 5250 50  0001 C CNN
F 3 "" H 6900 5250 50  0000 C CNN
	1    6900 5250
	0    -1   -1   0   
$EndComp
$Comp
L C_Small C36
U 1 1 576ECBF8
P 7100 5250
F 0 "C36" H 7110 5320 50  0000 L CNN
F 1 "1u" H 7110 5170 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 7100 5250 60  0001 C CNN
F 3 "" H 7100 5250 60  0000 C CNN
	1    7100 5250
	1    0    0    -1  
$EndComp
$Comp
L SS210 D15
U 1 1 576ECC03
P 7000 5000
F 0 "D15" H 6950 5080 50  0000 L CNN
F 1 "SS210" H 6900 4900 50  0000 L CNN
F 2 "MyDiodes:Diode-SMA_Standard" V 7000 5000 60  0001 C CNN
F 3 "" V 7000 5000 60  0000 C CNN
	1    7000 5000
	0    1    1    0   
$EndComp
$Comp
L C_Small C24
U 1 1 576ECC0E
P 6050 5100
F 0 "C24" H 6060 5170 50  0000 L CNN
F 1 "1u" H 6060 5020 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 6050 5100 50  0001 C CNN
F 3 "" H 6050 5100 50  0000 C CNN
	1    6050 5100
	0    1    1    0   
$EndComp
$Comp
L C_Small C23
U 1 1 576ECC14
P 6050 4900
F 0 "C23" H 6060 4970 50  0000 L CNN
F 1 "100n" H 6060 4820 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 6050 4900 60  0001 C CNN
F 3 "" H 6050 4900 60  0000 C CNN
	1    6050 4900
	0    -1   -1   0   
$EndComp
$Comp
L SS210 D6
U 1 1 576ED8DD
P 6550 3250
F 0 "D6" H 6500 3330 50  0000 L CNN
F 1 "SS210" H 6450 3150 50  0000 L CNN
F 2 "MyDiodes:Diode-SMA_Standard" V 6550 3250 60  0001 C CNN
F 3 "" V 6550 3250 60  0000 C CNN
	1    6550 3250
	0    -1   -1   0   
$EndComp
$Comp
L ZENERsmall D2
U 1 1 576ED8E3
P 6450 3500
F 0 "D2" H 6450 3600 50  0000 C CNN
F 1 "ZMY12" H 6450 3400 50  0000 C CNN
F 2 "MyDiodes:MELF_Standard" H 6450 3500 50  0001 C CNN
F 3 "" H 6450 3500 50  0000 C CNN
	1    6450 3500
	0    1    1    0   
$EndComp
$Comp
L C_Small C30
U 1 1 576ED8E9
P 6650 3500
F 0 "C30" H 6660 3570 50  0000 L CNN
F 1 "1u" H 6660 3420 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 6650 3500 60  0001 C CNN
F 3 "" H 6650 3500 60  0000 C CNN
	1    6650 3500
	1    0    0    -1  
$EndComp
$Comp
L ZENERsmall D10
U 1 1 576ED8F4
P 6900 3500
F 0 "D10" H 6900 3600 50  0000 C CNN
F 1 "ZMY12" H 6900 3400 50  0000 C CNN
F 2 "MyDiodes:MELF_Standard" H 6900 3500 50  0001 C CNN
F 3 "" H 6900 3500 50  0000 C CNN
	1    6900 3500
	0    -1   -1   0   
$EndComp
$Comp
L C_Small C35
U 1 1 576ED8FA
P 7100 3500
F 0 "C35" H 7110 3570 50  0000 L CNN
F 1 "1u" H 7110 3420 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 7100 3500 60  0001 C CNN
F 3 "" H 7100 3500 60  0000 C CNN
	1    7100 3500
	1    0    0    -1  
$EndComp
$Comp
L SS210 D14
U 1 1 576ED905
P 7000 3250
F 0 "D14" H 6950 3330 50  0000 L CNN
F 1 "SS210" H 6900 3150 50  0000 L CNN
F 2 "MyDiodes:Diode-SMA_Standard" V 7000 3250 60  0001 C CNN
F 3 "" V 7000 3250 60  0000 C CNN
	1    7000 3250
	0    1    1    0   
$EndComp
$Comp
L C_Small C22
U 1 1 576ED910
P 6050 3350
F 0 "C22" H 6060 3420 50  0000 L CNN
F 1 "1u" H 6060 3270 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 6050 3350 50  0001 C CNN
F 3 "" H 6050 3350 50  0000 C CNN
	1    6050 3350
	0    1    1    0   
$EndComp
$Comp
L C_Small C21
U 1 1 576ED916
P 6050 3150
F 0 "C21" H 6060 3220 50  0000 L CNN
F 1 "100n" H 6060 3070 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 6050 3150 60  0001 C CNN
F 3 "" H 6050 3150 60  0000 C CNN
	1    6050 3150
	0    -1   -1   0   
$EndComp
$Comp
L SS210 D5
U 1 1 576ED92C
P 6550 2200
F 0 "D5" H 6500 2280 50  0000 L CNN
F 1 "SS210" H 6450 2100 50  0000 L CNN
F 2 "MyDiodes:Diode-SMA_Standard" V 6550 2200 60  0001 C CNN
F 3 "" V 6550 2200 60  0000 C CNN
	1    6550 2200
	0    -1   -1   0   
$EndComp
$Comp
L ZENERsmall D1
U 1 1 576ED932
P 6450 2450
F 0 "D1" H 6450 2550 50  0000 C CNN
F 1 "ZMY12" H 6450 2350 50  0000 C CNN
F 2 "MyDiodes:MELF_Standard" H 6450 2450 50  0001 C CNN
F 3 "" H 6450 2450 50  0000 C CNN
	1    6450 2450
	0    1    1    0   
$EndComp
$Comp
L C_Small C29
U 1 1 576ED938
P 6650 2450
F 0 "C29" H 6660 2520 50  0000 L CNN
F 1 "1u" H 6660 2370 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 6650 2450 60  0001 C CNN
F 3 "" H 6650 2450 60  0000 C CNN
	1    6650 2450
	1    0    0    -1  
$EndComp
$Comp
L ZENERsmall D9
U 1 1 576ED943
P 6900 2450
F 0 "D9" H 6900 2550 50  0000 C CNN
F 1 "ZMY12" H 6900 2350 50  0000 C CNN
F 2 "MyDiodes:MELF_Standard" H 6900 2450 50  0001 C CNN
F 3 "" H 6900 2450 50  0000 C CNN
	1    6900 2450
	0    -1   -1   0   
$EndComp
$Comp
L C_Small C34
U 1 1 576ED949
P 7100 2450
F 0 "C34" H 7110 2520 50  0000 L CNN
F 1 "1u" H 7110 2370 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 7100 2450 60  0001 C CNN
F 3 "" H 7100 2450 60  0000 C CNN
	1    7100 2450
	1    0    0    -1  
$EndComp
$Comp
L SS210 D13
U 1 1 576ED954
P 7000 2200
F 0 "D13" H 6950 2280 50  0000 L CNN
F 1 "SS210" H 6900 2100 50  0000 L CNN
F 2 "MyDiodes:Diode-SMA_Standard" V 7000 2200 60  0001 C CNN
F 3 "" V 7000 2200 60  0000 C CNN
	1    7000 2200
	0    1    1    0   
$EndComp
$Comp
L C_Small C20
U 1 1 576ED95F
P 6050 2300
F 0 "C20" H 6060 2370 50  0000 L CNN
F 1 "1u" H 6060 2220 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 6050 2300 50  0001 C CNN
F 3 "" H 6050 2300 50  0000 C CNN
	1    6050 2300
	0    1    1    0   
$EndComp
$Comp
L C_Small C19
U 1 1 576ED965
P 6050 2100
F 0 "C19" H 6060 2170 50  0000 L CNN
F 1 "100n" H 6060 2020 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 6050 2100 60  0001 C CNN
F 3 "" H 6050 2100 60  0000 C CNN
	1    6050 2100
	0    -1   -1   0   
$EndComp
$Comp
L R_Small R6
U 1 1 576F6C4C
P 6350 5750
F 0 "R6" H 6380 5770 50  0000 L CNN
F 1 "2" H 6380 5710 50  0000 L CNN
F 2 "MyResistors:R_1206" H 6350 5750 60  0001 C CNN
F 3 "" H 6350 5750 60  0000 C CNN
	1    6350 5750
	0    -1   -1   0   
$EndComp
$Comp
L R_Small R5
U 1 1 576F99F1
P 6350 4900
F 0 "R5" H 6380 4920 50  0000 L CNN
F 1 "2" H 6380 4860 50  0000 L CNN
F 2 "MyResistors:R_1206" H 6350 4900 60  0001 C CNN
F 3 "" H 6350 4900 60  0000 C CNN
	1    6350 4900
	0    -1   -1   0   
$EndComp
$Comp
L R_Small R4
U 1 1 576F9AF4
P 6350 3150
F 0 "R4" H 6380 3170 50  0000 L CNN
F 1 "2" H 6380 3110 50  0000 L CNN
F 2 "MyResistors:R_1206" H 6350 3150 60  0001 C CNN
F 3 "" H 6350 3150 60  0000 C CNN
	1    6350 3150
	0    -1   -1   0   
$EndComp
$Comp
L R_Small R3
U 1 1 576F9C1A
P 6350 2100
F 0 "R3" H 6380 2120 50  0000 L CNN
F 1 "2" H 6380 2060 50  0000 L CNN
F 2 "MyResistors:R_1206" H 6350 2100 60  0001 C CNN
F 3 "" H 6350 2100 60  0000 C CNN
	1    6350 2100
	0    -1   -1   0   
$EndComp
$Comp
L C_Small C9
U 1 1 576FF0C9
P 2900 5700
F 0 "C9" H 2910 5770 50  0000 L CNN
F 1 "100n" H 2910 5620 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 2900 5700 60  0001 C CNN
F 3 "" H 2900 5700 60  0000 C CNN
	1    2900 5700
	1    0    0    -1  
$EndComp
$Comp
L C_Small C5
U 1 1 576FF0CF
P 2700 5700
F 0 "C5" H 2710 5770 50  0000 L CNN
F 1 "1u" H 2710 5620 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 2700 5700 60  0001 C CNN
F 3 "" H 2700 5700 60  0000 C CNN
	1    2700 5700
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR023
U 1 1 576FF0D5
P 2900 5950
F 0 "#PWR023" H 2900 5700 50  0001 C CNN
F 1 "GND" H 2900 5800 50  0000 C CNN
F 2 "" H 2900 5950 60  0000 C CNN
F 3 "" H 2900 5950 60  0000 C CNN
	1    2900 5950
	1    0    0    -1  
$EndComp
$Comp
L C_Small C7
U 1 1 576FF9A0
P 2850 4250
F 0 "C7" H 2860 4320 50  0000 L CNN
F 1 "100n" H 2860 4170 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 2850 4250 60  0001 C CNN
F 3 "" H 2850 4250 60  0000 C CNN
	1    2850 4250
	1    0    0    -1  
$EndComp
$Comp
L C_Small C3
U 1 1 576FF9A6
P 2650 4250
F 0 "C3" H 2660 4320 50  0000 L CNN
F 1 "1u" H 2660 4170 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 2650 4250 60  0001 C CNN
F 3 "" H 2650 4250 60  0000 C CNN
	1    2650 4250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR024
U 1 1 576FF9AC
P 2850 4500
F 0 "#PWR024" H 2850 4250 50  0001 C CNN
F 1 "GND" H 2850 4350 50  0000 C CNN
F 2 "" H 2850 4500 60  0000 C CNN
F 3 "" H 2850 4500 60  0000 C CNN
	1    2850 4500
	1    0    0    -1  
$EndComp
$Comp
L C_Small C6
U 1 1 5770093F
P 2800 6750
F 0 "C6" H 2810 6820 50  0000 L CNN
F 1 "100n" H 2810 6670 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 2800 6750 60  0001 C CNN
F 3 "" H 2800 6750 60  0000 C CNN
	1    2800 6750
	1    0    0    -1  
$EndComp
$Comp
L C_Small C2
U 1 1 57700945
P 2600 6750
F 0 "C2" H 2610 6820 50  0000 L CNN
F 1 "1u" H 2610 6670 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 2600 6750 60  0001 C CNN
F 3 "" H 2600 6750 60  0000 C CNN
	1    2600 6750
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR025
U 1 1 5770094B
P 2800 7000
F 0 "#PWR025" H 2800 6750 50  0001 C CNN
F 1 "GND" H 2800 6850 50  0000 C CNN
F 2 "" H 2800 7000 60  0000 C CNN
F 3 "" H 2800 7000 60  0000 C CNN
	1    2800 7000
	1    0    0    -1  
$EndComp
$Comp
L MountingHole M1
U 1 1 5770A99C
P 4750 9700
F 0 "M1" H 4750 9950 60  0000 C CNN
F 1 "MountingHole" H 4750 9450 60  0001 C CNN
F 2 "MyMechanical:MH7x3" H 4750 9700 60  0001 C CNN
F 3 "" H 4750 9700 60  0000 C CNN
	1    4750 9700
	1    0    0    -1  
$EndComp
$Comp
L MountingHole M2
U 1 1 5770AB2D
P 5250 9700
F 0 "M2" H 5250 9950 60  0000 C CNN
F 1 "MountingHole" H 5250 9450 60  0001 C CNN
F 2 "MyMechanical:MH7x3" H 5250 9700 60  0001 C CNN
F 3 "" H 5250 9700 60  0000 C CNN
	1    5250 9700
	1    0    0    -1  
$EndComp
$Comp
L MountingHole M3
U 1 1 5770AC40
P 5750 9700
F 0 "M3" H 5750 9950 60  0000 C CNN
F 1 "MountingHole" H 5750 9450 60  0001 C CNN
F 2 "MyMechanical:MH7x3" H 5750 9700 60  0001 C CNN
F 3 "" H 5750 9700 60  0000 C CNN
	1    5750 9700
	1    0    0    -1  
$EndComp
$Comp
L MountingHole M4
U 1 1 5770AD5E
P 6200 9700
F 0 "M4" H 6200 9950 60  0000 C CNN
F 1 "MountingHole" H 6200 9450 60  0001 C CNN
F 2 "MyMechanical:MH7x3" H 6200 9700 60  0001 C CNN
F 3 "" H 6200 9700 60  0000 C CNN
	1    6200 9700
	1    0    0    -1  
$EndComp
Wire Wire Line
	4650 2700 4350 2700
Wire Wire Line
	4650 2300 4700 2300
Wire Wire Line
	3200 2900 3450 2900
Wire Wire Line
	3400 3000 3450 3000
Wire Wire Line
	3450 2800 3300 2800
Wire Wire Line
	4350 3650 5000 3650
Wire Wire Line
	4550 3750 4350 3750
Wire Wire Line
	3300 3850 3300 4250
Wire Wire Line
	3450 3850 3300 3850
Wire Wire Line
	3200 3950 3450 3950
Connection ~ 3350 3750
Wire Wire Line
	3350 4050 3350 3750
Wire Wire Line
	3450 4050 3350 4050
Wire Wire Line
	2200 3750 3450 3750
Connection ~ 3200 6150
Wire Wire Line
	2600 6900 2800 6900
Wire Wire Line
	2800 6150 2800 6650
Wire Wire Line
	2800 6550 2600 6550
Wire Wire Line
	2600 6550 2600 6650
Connection ~ 2800 6550
Wire Wire Line
	2600 6850 2600 6900
Connection ~ 2800 6900
Wire Wire Line
	2800 6850 2800 7000
Wire Wire Line
	2000 6350 3450 6350
Connection ~ 3200 3650
Wire Wire Line
	2650 4400 2850 4400
Wire Wire Line
	2850 3650 2850 4150
Wire Wire Line
	2850 4050 2650 4050
Wire Wire Line
	2650 4050 2650 4150
Connection ~ 2850 4050
Wire Wire Line
	2650 4350 2650 4400
Connection ~ 2850 4400
Wire Wire Line
	2850 4350 2850 4500
Connection ~ 3200 5100
Wire Wire Line
	2700 5850 2900 5850
Wire Wire Line
	2900 5100 2900 5600
Wire Wire Line
	2900 5500 2700 5500
Wire Wire Line
	2700 5500 2700 5600
Connection ~ 2900 5500
Wire Wire Line
	2700 5800 2700 5850
Connection ~ 2900 5850
Wire Wire Line
	2900 5800 2900 5950
Wire Wire Line
	2700 3350 2900 3350
Connection ~ 3200 2600
Wire Wire Line
	2900 2600 2900 3100
Wire Wire Line
	6450 4900 7250 4900
Wire Wire Line
	6150 4900 6250 4900
Wire Wire Line
	6450 3150 7250 3150
Wire Wire Line
	6150 3150 6250 3150
Wire Wire Line
	6450 2100 7250 2100
Wire Wire Line
	6150 2100 6250 2100
Connection ~ 7100 2600
Connection ~ 6450 2600
Connection ~ 7000 2100
Wire Wire Line
	7250 2100 7250 2300
Connection ~ 6900 2600
Wire Wire Line
	5800 2600 7950 2600
Connection ~ 6550 2100
Connection ~ 6200 2100
Wire Wire Line
	6200 2300 6150 2300
Wire Wire Line
	5800 2300 5950 2300
Wire Wire Line
	5950 2100 5900 2100
Wire Wire Line
	5900 2100 5900 2300
Connection ~ 5900 2300
Wire Wire Line
	6200 2100 6200 2300
Connection ~ 6650 2600
Connection ~ 7000 2300
Wire Wire Line
	7000 2300 7000 2300
Connection ~ 6550 2300
Wire Wire Line
	6550 2300 6550 2300
Wire Wire Line
	6900 2300 6900 2350
Wire Wire Line
	6900 2300 7100 2300
Wire Wire Line
	7100 2300 7100 2350
Wire Wire Line
	7100 2600 7100 2550
Wire Wire Line
	6900 2600 6900 2550
Wire Wire Line
	6450 2300 6450 2350
Wire Wire Line
	6450 2300 6650 2300
Wire Wire Line
	6650 2300 6650 2350
Wire Wire Line
	6650 2600 6650 2550
Wire Wire Line
	6450 2550 6450 2600
Connection ~ 7100 3650
Connection ~ 6450 3650
Connection ~ 7000 3150
Wire Wire Line
	7250 3150 7250 3350
Connection ~ 6900 3650
Wire Wire Line
	5800 3650 7550 3650
Connection ~ 6550 3150
Connection ~ 6200 3150
Wire Wire Line
	6200 3350 6150 3350
Wire Wire Line
	5800 3350 5950 3350
Wire Wire Line
	5950 3150 5900 3150
Wire Wire Line
	5900 3150 5900 3350
Connection ~ 5900 3350
Wire Wire Line
	6200 3150 6200 3350
Connection ~ 6650 3650
Connection ~ 7000 3350
Wire Wire Line
	7000 3350 7000 3350
Connection ~ 6550 3350
Wire Wire Line
	6550 3350 6550 3350
Wire Wire Line
	6900 3350 6900 3400
Wire Wire Line
	6900 3350 7100 3350
Wire Wire Line
	7100 3350 7100 3400
Wire Wire Line
	7100 3650 7100 3600
Wire Wire Line
	6900 3650 6900 3600
Wire Wire Line
	6450 3350 6450 3400
Wire Wire Line
	6450 3350 6650 3350
Wire Wire Line
	6650 3350 6650 3400
Wire Wire Line
	6650 3650 6650 3600
Wire Wire Line
	6450 3600 6450 3650
Connection ~ 7100 5400
Connection ~ 6450 5400
Connection ~ 7000 4900
Wire Wire Line
	7250 4900 7250 5100
Connection ~ 6900 5400
Wire Wire Line
	5800 5400 7950 5400
Connection ~ 6550 4900
Connection ~ 6200 4900
Wire Wire Line
	6200 5100 6150 5100
Wire Wire Line
	5800 5100 5950 5100
Wire Wire Line
	5950 4900 5900 4900
Wire Wire Line
	5900 4900 5900 5100
Connection ~ 5900 5100
Wire Wire Line
	6200 4900 6200 5100
Connection ~ 6650 5400
Connection ~ 7000 5100
Wire Wire Line
	7000 5100 7000 5100
Connection ~ 6550 5100
Wire Wire Line
	6550 5100 6550 5100
Wire Wire Line
	6900 5100 6900 5150
Wire Wire Line
	6900 5100 7100 5100
Wire Wire Line
	7100 5100 7100 5150
Wire Wire Line
	7100 5400 7100 5350
Wire Wire Line
	6900 5400 6900 5350
Wire Wire Line
	6450 5100 6450 5150
Wire Wire Line
	6450 5100 6650 5100
Wire Wire Line
	6650 5100 6650 5150
Wire Wire Line
	6650 5400 6650 5350
Wire Wire Line
	6450 5350 6450 5400
Connection ~ 7100 6250
Connection ~ 6450 6250
Connection ~ 7000 5750
Wire Wire Line
	7250 5750 7250 5950
Connection ~ 6900 6250
Wire Wire Line
	5800 6250 7550 6250
Connection ~ 6550 5750
Connection ~ 6200 5750
Wire Wire Line
	6200 5950 6150 5950
Wire Wire Line
	5800 5950 5950 5950
Wire Wire Line
	6450 5750 7250 5750
Wire Wire Line
	6150 5750 6250 5750
Wire Wire Line
	5950 5750 5900 5750
Wire Wire Line
	5900 5750 5900 5950
Connection ~ 5900 5950
Wire Wire Line
	6200 5750 6200 5950
Connection ~ 6650 6250
Connection ~ 7000 5950
Wire Wire Line
	7000 5950 7000 5950
Connection ~ 6550 5950
Wire Wire Line
	6550 5950 6550 5950
Wire Wire Line
	6900 5950 6900 6000
Wire Wire Line
	6900 5950 7100 5950
Wire Wire Line
	7100 5950 7100 6000
Wire Wire Line
	7100 6250 7100 6200
Wire Wire Line
	6900 6250 6900 6200
Wire Wire Line
	6450 5950 6450 6000
Wire Wire Line
	6450 5950 6650 5950
Wire Wire Line
	6650 5950 6650 6000
Wire Wire Line
	6650 6250 6650 6200
Wire Wire Line
	6450 6200 6450 6250
Wire Wire Line
	4450 5200 4450 5400
Wire Wire Line
	4350 5200 4450 5200
Connection ~ 4650 5100
Wire Wire Line
	4550 6150 4350 6150
Wire Wire Line
	4350 6250 5000 6250
Connection ~ 4550 5950
Wire Wire Line
	4550 5950 4600 5950
Wire Wire Line
	4550 5750 4550 6150
Wire Wire Line
	4600 5750 4550 5750
Wire Wire Line
	4900 5750 4800 5750
Wire Wire Line
	4900 5950 4900 5750
Wire Wire Line
	2800 6150 3450 6150
Wire Wire Line
	3300 6250 3300 6750
Wire Wire Line
	3450 6250 3300 6250
Wire Wire Line
	3200 6550 3450 6550
Wire Wire Line
	3200 6100 3200 6550
Connection ~ 3400 6350
Wire Wire Line
	3400 6450 3400 6350
Wire Wire Line
	3450 6450 3400 6450
Connection ~ 3300 6650
Wire Wire Line
	3450 6650 3300 6650
Wire Wire Line
	4450 5400 5000 5400
Wire Wire Line
	4650 4900 4700 4900
Wire Wire Line
	4650 4900 4650 5100
Wire Wire Line
	4350 5100 4700 5100
Connection ~ 4950 5100
Wire Wire Line
	4950 4900 4900 4900
Wire Wire Line
	4950 5100 4950 4900
Wire Wire Line
	4900 5100 5000 5100
Wire Wire Line
	3300 5200 3300 5700
Wire Wire Line
	3450 5200 3300 5200
Wire Wire Line
	3200 5500 3450 5500
Wire Wire Line
	3200 5000 3200 5500
Wire Wire Line
	2900 5100 3450 5100
Connection ~ 3400 5300
Wire Wire Line
	2100 5300 3450 5300
Wire Wire Line
	3400 5400 3400 5300
Wire Wire Line
	3450 5400 3400 5400
Connection ~ 3300 5600
Wire Wire Line
	3450 5600 3300 5600
Wire Wire Line
	4800 5950 5000 5950
Connection ~ 4900 5950
Connection ~ 4650 2300
Connection ~ 4550 3350
Wire Wire Line
	4550 3350 4600 3350
Wire Wire Line
	4550 3150 4550 3750
Wire Wire Line
	4600 3150 4550 3150
Wire Wire Line
	4900 3150 4800 3150
Wire Wire Line
	4900 3350 4900 3150
Wire Wire Line
	2850 3650 3450 3650
Wire Wire Line
	3200 3600 3200 3950
Connection ~ 3300 4150
Wire Wire Line
	3450 4150 3300 4150
Wire Wire Line
	2200 3100 2200 2700
Wire Wire Line
	5000 2600 4350 2600
Wire Wire Line
	4650 2100 4700 2100
Wire Wire Line
	4650 2100 4650 2700
Connection ~ 4950 2300
Wire Wire Line
	4950 2100 4900 2100
Wire Wire Line
	4950 2300 4950 2100
Wire Wire Line
	4900 2300 5000 2300
Wire Wire Line
	3300 2800 3300 3200
Wire Wire Line
	3200 2500 3200 2900
Wire Wire Line
	2900 2600 3450 2600
Connection ~ 3400 2700
Wire Wire Line
	2200 2700 3450 2700
Wire Wire Line
	3400 3000 3400 2700
Connection ~ 3300 3100
Wire Wire Line
	3450 3100 3300 3100
Wire Wire Line
	2350 10400 2350 10250
Wire Wire Line
	2350 10650 2350 10600
Wire Wire Line
	3050 10400 3050 10250
Wire Wire Line
	3050 10650 3050 10600
Wire Wire Line
	3150 1650 4500 1650
Wire Wire Line
	1750 3400 1650 3400
Wire Wire Line
	1750 2200 1750 3450
Wire Wire Line
	2900 3000 2700 3000
Wire Wire Line
	2700 3000 2700 3100
Connection ~ 2900 3000
Wire Wire Line
	2700 3300 2700 3350
Wire Wire Line
	7550 6100 7550 6450
Connection ~ 2900 3350
Wire Wire Line
	3900 1650 3900 1450
Wire Wire Line
	3600 1650 3600 1450
Connection ~ 3600 1650
Wire Wire Line
	3450 1800 3450 1650
Connection ~ 3450 1650
Wire Wire Line
	3600 1000 3600 1250
Wire Wire Line
	3900 1000 3900 1250
Connection ~ 3600 1000
Connection ~ 3900 1000
Connection ~ 3900 1650
Wire Wire Line
	4200 1000 4200 1250
Connection ~ 4200 1000
Wire Wire Line
	4500 1000 4500 1250
Wire Wire Line
	4500 1650 4500 1450
Wire Wire Line
	4200 1650 4200 1450
Connection ~ 4200 1650
Wire Wire Line
	2350 9500 2050 9500
Wire Wire Line
	2050 9500 2050 9800
Wire Wire Line
	2250 10350 2250 10250
Wire Wire Line
	2350 9350 2350 9500
Wire Wire Line
	3050 9500 2750 9500
Wire Wire Line
	2750 9500 2750 9800
Wire Wire Line
	2950 10350 2950 10250
Wire Wire Line
	3050 9350 3050 9500
Connection ~ 3050 10250
Wire Wire Line
	3350 10250 3350 10050
Wire Wire Line
	2750 10250 2750 10000
Connection ~ 2750 10250
Wire Wire Line
	2050 10000 2050 10250
Connection ~ 2350 10250
Wire Wire Line
	2600 10350 2600 10650
Connection ~ 2600 10350
Connection ~ 2350 9500
Connection ~ 3050 9500
Wire Wire Line
	1750 3200 1650 3200
Connection ~ 1750 3400
Wire Wire Line
	1750 3000 1650 3000
Connection ~ 1750 3200
Wire Wire Line
	1650 2000 1750 2000
Wire Wire Line
	1650 1900 1750 1900
Wire Wire Line
	7550 2450 7550 3100
Connection ~ 7550 2600
Wire Wire Line
	7550 1850 7550 2050
Wire Wire Line
	1750 2600 1650 2600
Connection ~ 1750 3000
Wire Wire Line
	1750 2800 1650 2800
Connection ~ 1750 2800
Wire Wire Line
	4800 3350 5000 3350
Wire Wire Line
	7550 3500 7550 3950
Connection ~ 7550 3650
Wire Wire Line
	7550 5250 7550 5700
Connection ~ 7550 5400
Wire Wire Line
	7550 4700 7550 4850
Connection ~ 7550 6250
Wire Wire Line
	2900 3300 2900 3450
Connection ~ 4900 3350
Wire Wire Line
	2200 3300 2200 3750
Wire Wire Line
	2200 3300 1650 3300
Wire Wire Line
	1650 3100 2200 3100
Wire Wire Line
	2000 6350 2000 2900
Wire Wire Line
	2000 2900 1650 2900
Wire Wire Line
	2100 5300 2100 2700
Wire Wire Line
	2100 2700 1650 2700
Wire Wire Line
	3150 1000 4500 1000
Wire Wire Line
	2250 10350 2950 10350
Wire Wire Line
	2050 10250 3350 10250
Wire Wire Line
	1650 2400 1750 2400
Connection ~ 1750 2600
Wire Wire Line
	1650 2500 1850 2500
Wire Wire Line
	1650 2300 1850 2300
Connection ~ 3450 1000
Wire Wire Line
	1650 2200 1750 2200
Connection ~ 1750 2400
Wire Wire Line
	2400 2100 1650 2100
Wire Wire Line
	3450 800  3450 1000
$EndSCHEMATC
