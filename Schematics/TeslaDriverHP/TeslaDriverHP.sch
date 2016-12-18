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
LIBS:TeslaDriverHP-cache
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
L IRFP4310ZPbF Q1
U 1 1 557FCE99
P 4300 2500
F 0 "Q1" H 4100 2700 50  0000 L CNN
F 1 "IRFP4310ZPbF" H 3700 2600 50  0000 L CNN
F 2 "MyFootprints:TO-247" H 4500 2425 50  0001 L CIN
F 3 "" H 4300 2500 50  0000 L CNN
	1    4300 2500
	1    0    0    -1  
$EndComp
$Comp
L +24V #PWR01
U 1 1 5581F374
P 4400 1950
F 0 "#PWR01" H 4400 1800 50  0001 C CNN
F 1 "+24V" H 4400 2090 50  0000 C CNN
F 2 "" H 4400 1950 60  0000 C CNN
F 3 "" H 4400 1950 60  0000 C CNN
	1    4400 1950
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 55835B67
P 1550 3450
F 0 "#PWR02" H 1550 3200 50  0001 C CNN
F 1 "GND" H 1550 3300 50  0000 C CNN
F 2 "" H 1550 3450 60  0000 C CNN
F 3 "" H 1550 3450 60  0000 C CNN
	1    1550 3450
	1    0    0    -1  
$EndComp
$Comp
L CP_Small C1
U 1 1 558396E2
P 3850 1250
F 0 "C1" V 3900 1000 50  0000 L CNN
F 1 "1000u/63V" V 3700 1100 50  0000 L CNN
F 2 "MyCapacitors:C_Radial_D16_L25_P7.5" H 3850 1250 60  0001 C CNN
F 3 "" H 3850 1250 60  0000 C CNN
	1    3850 1250
	1    0    0    -1  
$EndComp
$Comp
L CP_Small C2
U 1 1 558396E8
P 4150 1250
F 0 "C2" V 4200 1000 50  0000 L CNN
F 1 "1000u/63V" V 4000 1100 50  0000 L CNN
F 2 "MyCapacitors:C_Radial_D16_L25_P7.5" H 4150 1250 60  0001 C CNN
F 3 "" H 4150 1250 60  0000 C CNN
	1    4150 1250
	1    0    0    -1  
$EndComp
$Comp
L C_Small C3
U 1 1 5583973B
P 4450 1250
F 0 "C3" V 4500 1000 50  0000 L CNN
F 1 "2.2u/63V" V 4350 1100 50  0000 L CNN
F 2 "MyCapacitors:C_Rect_L18_W8_P15" H 4450 1250 60  0001 C CNN
F 3 "" H 4450 1250 60  0000 C CNN
	1    4450 1250
	1    0    0    -1  
$EndComp
$Comp
L C_Small C4
U 1 1 5583998A
P 4750 1250
F 0 "C4" V 4800 1000 50  0000 L CNN
F 1 "2.2u/63V" V 4650 1100 50  0000 L CNN
F 2 "MyCapacitors:C_Rect_L18_W8_P15" H 4750 1250 60  0001 C CNN
F 3 "" H 4750 1250 60  0000 C CNN
	1    4750 1250
	1    0    0    -1  
$EndComp
Text Label 2500 2750 2    47   ~ 0
GatePWM1H
Text Label 2500 3300 2    47   ~ 0
GatePWM1L
$Comp
L DS18B20 U5
U 1 1 558B3E36
P 2200 7250
F 0 "U5" H 2200 7100 60  0000 C CNN
F 1 "DS18B20" H 2175 7400 60  0000 C CNN
F 2 "MyFootprints:TO-92_Inline_Narrow_Oval" H 2200 7250 60  0001 C CNN
F 3 "" H 2200 7250 60  0000 C CNN
	1    2200 7250
	0    1    -1   0   
$EndComp
$Comp
L R_Small R14
U 1 1 558B3FF1
P 1950 7300
F 0 "R14" H 1980 7320 50  0000 L CNN
F 1 "4k7" H 1980 7260 50  0000 L CNN
F 2 "MyResistors:R_0603" H 1950 7300 60  0001 C CNN
F 3 "" H 1950 7300 60  0000 C CNN
	1    1950 7300
	-1   0    0    1   
$EndComp
Text GLabel 2250 6750 1    47   Input ~ 0
TEMP_MOSFET1
$Comp
L DS18B20 U6
U 1 1 558B587C
P 2900 7250
F 0 "U6" H 2900 7100 60  0000 C CNN
F 1 "DS18B20" H 2875 7400 60  0000 C CNN
F 2 "MyFootprints:TO-92_Inline_Narrow_Oval" H 2900 7250 60  0001 C CNN
F 3 "" H 2900 7250 60  0000 C CNN
	1    2900 7250
	0    1    -1   0   
$EndComp
$Comp
L R_Small R15
U 1 1 558B5882
P 2650 7300
F 0 "R15" H 2680 7320 50  0000 L CNN
F 1 "4k7" H 2680 7260 50  0000 L CNN
F 2 "MyResistors:R_0603" H 2650 7300 60  0001 C CNN
F 3 "" H 2650 7300 60  0000 C CNN
	1    2650 7300
	-1   0    0    1   
$EndComp
Text GLabel 2950 6750 1    47   Input ~ 0
TEMP_MOSFET2
$Comp
L +3.3V #PWR03
U 1 1 558B6D8A
P 3250 7450
F 0 "#PWR03" H 3250 7300 50  0001 C CNN
F 1 "+3.3V" H 3250 7590 50  0000 C CNN
F 2 "" H 3250 7450 60  0000 C CNN
F 3 "" H 3250 7450 60  0000 C CNN
	1    3250 7450
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR04
U 1 1 558CA78F
P 2500 8050
F 0 "#PWR04" H 2500 7800 50  0001 C CNN
F 1 "GND" H 2500 7900 50  0000 C CNN
F 2 "" H 2500 8050 60  0000 C CNN
F 3 "" H 2500 8050 60  0000 C CNN
	1    2500 8050
	1    0    0    -1  
$EndComp
Text GLabel 1550 2000 2    47   Input ~ 0
TEMP_MOSFET1
Text GLabel 1550 1900 2    47   Input ~ 0
TEMP_MOSFET2
$Comp
L CONN_01X16 P3
U 1 1 55A84FA1
P 1250 2650
F 0 "P3" H 1250 3500 50  0000 C CNN
F 1 "CONTROL" V 1350 2650 50  0000 C CNN
F 2 "MyConnectors:PinHeaderMale_2x08" H 1250 2650 60  0001 C CNN
F 3 "" H 1250 2650 60  0000 C CNN
	1    1250 2650
	-1   0    0    1   
$EndComp
$Comp
L GNDPWR #PWR05
U 1 1 55C2BB8B
P 3700 1700
F 0 "#PWR05" H 3700 1500 50  0001 C CNN
F 1 "GNDPWR" H 3700 1570 50  0000 C CNN
F 2 "" H 3700 1650 60  0000 C CNN
F 3 "" H 3700 1650 60  0000 C CNN
	1    3700 1700
	1    0    0    -1  
$EndComp
$Comp
L GNDPWR #PWR06
U 1 1 55C2C61C
P 4400 5600
F 0 "#PWR06" H 4400 5400 50  0001 C CNN
F 1 "GNDPWR" H 4400 5470 50  0000 C CNN
F 2 "" H 4400 5550 60  0000 C CNN
F 3 "" H 4400 5550 60  0000 C CNN
	1    4400 5600
	1    0    0    -1  
$EndComp
$Comp
L IRFP4310ZPbF Q2
U 1 1 5685F438
P 4300 3000
F 0 "Q2" H 4100 3200 50  0000 L CNN
F 1 "IRFP4310ZPbF" H 3700 3100 50  0000 L CNN
F 2 "MyFootprints:TO-247" H 4500 2925 50  0001 L CIN
F 3 "" H 4300 3000 50  0000 L CNN
	1    4300 3000
	1    0    0    -1  
$EndComp
$Comp
L IRFP4310ZPbF Q3
U 1 1 568615A1
P 4300 4550
F 0 "Q3" H 4100 4750 50  0000 L CNN
F 1 "IRFP4310ZPbF" H 3700 4650 50  0000 L CNN
F 2 "MyFootprints:TO-247" H 4500 4475 50  0001 L CIN
F 3 "" H 4300 4550 50  0000 L CNN
	1    4300 4550
	1    0    0    -1  
$EndComp
$Comp
L +24V #PWR07
U 1 1 568615A7
P 4400 4050
F 0 "#PWR07" H 4400 3900 50  0001 C CNN
F 1 "+24V" H 4400 4190 50  0000 C CNN
F 2 "" H 4400 4050 60  0000 C CNN
F 3 "" H 4400 4050 60  0000 C CNN
	1    4400 4050
	1    0    0    -1  
$EndComp
$Comp
L IRFP4310ZPbF Q4
U 1 1 5686160E
P 4300 5050
F 0 "Q4" H 4100 5250 50  0000 L CNN
F 1 "IRFP4310ZPbF" H 3700 5150 50  0000 L CNN
F 2 "MyFootprints:TO-247" H 4500 4975 50  0001 L CIN
F 3 "" H 4300 5050 50  0000 L CNN
	1    4300 5050
	1    0    0    -1  
$EndComp
$Comp
L GNDPWR #PWR08
U 1 1 56863B45
P 4400 3650
F 0 "#PWR08" H 4400 3450 50  0001 C CNN
F 1 "GNDPWR" H 4400 3520 50  0000 C CNN
F 2 "" H 4400 3600 60  0000 C CNN
F 3 "" H 4400 3600 60  0000 C CNN
	1    4400 3650
	1    0    0    -1  
$EndComp
$Comp
L +12V #PWR09
U 1 1 5688D589
P 1650 2500
F 0 "#PWR09" H 1650 2350 50  0001 C CNN
F 1 "+12V" V 1650 2600 50  0000 L CNN
F 2 "" H 1650 2500 60  0000 C CNN
F 3 "" H 1650 2500 60  0000 C CNN
	1    1650 2500
	0    1    1    0   
$EndComp
$Comp
L +3.3V #PWR010
U 1 1 5688D630
P 1650 2300
F 0 "#PWR010" H 1650 2150 50  0001 C CNN
F 1 "+3.3V" V 1650 2400 50  0000 L CNN
F 2 "" H 1650 2300 60  0000 C CNN
F 3 "" H 1650 2300 60  0000 C CNN
	1    1650 2300
	0    1    1    0   
$EndComp
$Comp
L +24V #PWR011
U 1 1 5688E52A
P 3700 700
F 0 "#PWR011" H 3700 550 50  0001 C CNN
F 1 "+24V" H 3700 840 50  0000 C CNN
F 2 "" H 3700 700 60  0000 C CNN
F 3 "" H 3700 700 60  0000 C CNN
	1    3700 700 
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X01 P1
U 1 1 5689BCD3
P 3200 900
F 0 "P1" H 3200 1000 50  0000 C CNN
F 1 "PWR_IN+" V 3300 900 50  0000 C CNN
F 2 "MyConnectors:PowerPad15_5_via" H 3200 900 50  0001 C CNN
F 3 "" H 3200 900 50  0000 C CNN
	1    3200 900 
	-1   0    0    1   
$EndComp
$Comp
L CONN_01X01 P2
U 1 1 5689BE27
P 3200 1550
F 0 "P2" H 3200 1650 50  0000 C CNN
F 1 "PWR_GND" V 3300 1550 50  0000 C CNN
F 2 "MyConnectors:PowerPad15_5_via" H 3200 1550 50  0001 C CNN
F 3 "" H 3200 1550 50  0000 C CNN
	1    3200 1550
	-1   0    0    1   
$EndComp
$Comp
L CONN_01X01 P5
U 1 1 5689E340
P 7000 4800
F 0 "P5" H 7000 4900 50  0000 C CNN
F 1 "PWR_OUT_L2" V 7100 4800 50  0000 C CNN
F 2 "MyConnectors:PowerPad15_5_via" H 7000 4800 50  0001 C CNN
F 3 "" H 7000 4800 50  0000 C CNN
	1    7000 4800
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X01 P4
U 1 1 5689F756
P 7000 2750
F 0 "P4" H 7000 2850 50  0000 C CNN
F 1 "PWR_OUT_L1" V 7100 2750 50  0000 C CNN
F 2 "MyConnectors:PowerPad15_5_via" H 7000 2750 50  0001 C CNN
F 3 "" H 7000 2750 50  0000 C CNN
	1    7000 2750
	1    0    0    -1  
$EndComp
Text Label 6500 2750 0    60   ~ 0
OUTL1
Text Label 6550 4800 0    60   ~ 0
OUTL2
$Comp
L C_Small C30
U 1 1 5690305E
P 2950 7900
F 0 "C30" H 2960 7970 50  0000 L CNN
F 1 "100n" H 2960 7820 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 2950 7900 60  0001 C CNN
F 3 "" H 2950 7900 60  0000 C CNN
	1    2950 7900
	1    0    0    -1  
$EndComp
$Comp
L C_Small C29
U 1 1 569031F4
P 2250 7900
F 0 "C29" H 2260 7970 50  0000 L CNN
F 1 "100n" H 2260 7820 50  0000 L CNN
F 2 "MyCapacitors:C_0603" H 2250 7900 60  0001 C CNN
F 3 "" H 2250 7900 60  0000 C CNN
	1    2250 7900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR012
U 1 1 569033E5
P 2250 8050
F 0 "#PWR012" H 2250 7800 50  0001 C CNN
F 1 "GND" H 2250 7900 50  0000 C CNN
F 2 "" H 2250 8050 60  0000 C CNN
F 3 "" H 2250 8050 60  0000 C CNN
	1    2250 8050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR013
U 1 1 5690348F
P 2950 8050
F 0 "#PWR013" H 2950 7800 50  0001 C CNN
F 1 "GND" H 2950 7900 50  0000 C CNN
F 2 "" H 2950 8050 60  0000 C CNN
F 3 "" H 2950 8050 60  0000 C CNN
	1    2950 8050
	1    0    0    -1  
$EndComp
$Comp
L +12V #PWR014
U 1 1 576DDEF3
P 2950 2550
F 0 "#PWR014" H 2950 2400 50  0001 C CNN
F 1 "+12V" V 2950 2650 50  0000 L CNN
F 2 "" H 2950 2550 60  0000 C CNN
F 3 "" H 2950 2550 60  0000 C CNN
	1    2950 2550
	1    0    0    -1  
$EndComp
Text Label 2000 2100 2    47   ~ 0
GateEnable
Text Label 2450 5400 2    47   ~ 0
GatePWM2L
Text Label 2500 4800 2    47   ~ 0
GatePWM2H
$Comp
L MountingHole M1
U 1 1 5770A99C
P 4650 7100
F 0 "M1" H 4650 7350 60  0000 C CNN
F 1 "MountingHole" H 4650 6850 60  0001 C CNN
F 2 "MyMechanical:MH7x3" H 4650 7100 60  0001 C CNN
F 3 "" H 4650 7100 60  0000 C CNN
	1    4650 7100
	1    0    0    -1  
$EndComp
$Comp
L MountingHole M2
U 1 1 5770AB2D
P 5150 7100
F 0 "M2" H 5150 7350 60  0000 C CNN
F 1 "MountingHole" H 5150 6850 60  0001 C CNN
F 2 "MyMechanical:MH7x3" H 5150 7100 60  0001 C CNN
F 3 "" H 5150 7100 60  0000 C CNN
	1    5150 7100
	1    0    0    -1  
$EndComp
$Comp
L MountingHole M3
U 1 1 5770AC40
P 5650 7100
F 0 "M3" H 5650 7350 60  0000 C CNN
F 1 "MountingHole" H 5650 6850 60  0001 C CNN
F 2 "MyMechanical:MH7x3" H 5650 7100 60  0001 C CNN
F 3 "" H 5650 7100 60  0000 C CNN
	1    5650 7100
	1    0    0    -1  
$EndComp
$Comp
L MountingHole M4
U 1 1 5770AD5E
P 6100 7100
F 0 "M4" H 6100 7350 60  0000 C CNN
F 1 "MountingHole" H 6100 6850 60  0001 C CNN
F 2 "MyMechanical:MH7x3" H 6100 7100 60  0001 C CNN
F 3 "" H 6100 7100 60  0000 C CNN
	1    6100 7100
	1    0    0    -1  
$EndComp
$Comp
L GateDriver U1
U 1 1 57B90425
P 3400 2750
F 0 "U1" H 3400 2950 60  0000 C CNN
F 1 "GateDriver" H 3400 2450 60  0000 C CNN
F 2 "MyConnectors:GateDriverMain" H 3400 2750 60  0001 C CNN
F 3 "" H 3400 2750 60  0000 C CNN
	1    3400 2750
	1    0    0    -1  
$EndComp
$Comp
L GateDriver U2
U 1 1 57B923A5
P 3400 3350
F 0 "U2" H 3400 3550 60  0000 C CNN
F 1 "GateDriver" H 3400 3050 60  0000 C CNN
F 2 "MyConnectors:GateDriverMain" H 3400 3350 60  0001 C CNN
F 3 "" H 3400 3350 60  0000 C CNN
	1    3400 3350
	1    0    0    -1  
$EndComp
$Comp
L GateDriver U3
U 1 1 57B934AC
P 3400 4800
F 0 "U3" H 3400 5000 60  0000 C CNN
F 1 "GateDriver" H 3400 4500 60  0000 C CNN
F 2 "MyConnectors:GateDriverMain" H 3400 4800 60  0001 C CNN
F 3 "" H 3400 4800 60  0000 C CNN
	1    3400 4800
	1    0    0    -1  
$EndComp
$Comp
L GateDriver U4
U 1 1 57B94EF5
P 3400 5400
F 0 "U4" H 3400 5600 60  0000 C CNN
F 1 "GateDriver" H 3400 5100 60  0000 C CNN
F 2 "MyConnectors:GateDriverMain" H 3400 5400 60  0001 C CNN
F 3 "" H 3400 5400 60  0000 C CNN
	1    3400 5400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR015
U 1 1 57B9891C
P 2850 5800
F 0 "#PWR015" H 2850 5550 50  0001 C CNN
F 1 "GND" H 2850 5650 50  0000 C CNN
F 2 "" H 2850 5800 60  0000 C CNN
F 3 "" H 2850 5800 60  0000 C CNN
	1    2850 5800
	1    0    0    -1  
$EndComp
$Comp
L SK520 D1
U 1 1 57BC78C3
P 4650 2500
F 0 "D1" H 4600 2580 50  0000 L CNN
F 1 "SK520" H 4550 2400 50  0000 L CNN
F 2 "MyDiodes:DO-214AB" V 4650 2500 60  0001 C CNN
F 3 "" V 4650 2500 60  0000 C CNN
	1    4650 2500
	0    1    1    0   
$EndComp
$Comp
L SK520 D2
U 1 1 57BC7B06
P 4650 3000
F 0 "D2" H 4600 3080 50  0000 L CNN
F 1 "SK520" H 4550 2900 50  0000 L CNN
F 2 "MyDiodes:DO-214AB" V 4650 3000 60  0001 C CNN
F 3 "" V 4650 3000 60  0000 C CNN
	1    4650 3000
	0    1    1    0   
$EndComp
$Comp
L SK520 D4
U 1 1 57BC7D17
P 4650 5050
F 0 "D4" H 4600 5130 50  0000 L CNN
F 1 "SK520" H 4550 4950 50  0000 L CNN
F 2 "MyDiodes:DO-214AB" V 4650 5050 60  0001 C CNN
F 3 "" V 4650 5050 60  0000 C CNN
	1    4650 5050
	0    1    1    0   
$EndComp
$Comp
L SK520 D3
U 1 1 57BC8275
P 4650 4550
F 0 "D3" H 4600 4630 50  0000 L CNN
F 1 "SK520" H 4550 4450 50  0000 L CNN
F 2 "MyDiodes:DO-214AB" V 4650 4550 60  0001 C CNN
F 3 "" V 4650 4550 60  0000 C CNN
	1    4650 4550
	0    1    1    0   
$EndComp
$Comp
L R_Small R8
U 1 1 57BC98E5
P 5150 4350
F 0 "R8" H 5000 4300 50  0000 L CNN
F 1 "1" H 5180 4310 50  0000 L CNN
F 2 "MyResistors:R_2512" H 5150 4350 60  0001 C CNN
F 3 "" H 5150 4350 60  0000 C CNN
	1    5150 4350
	-1   0    0    1   
$EndComp
$Comp
L C_Small C17
U 1 1 57BC98EB
P 5150 4650
F 0 "C17" V 5200 4700 50  0000 L CNN
F 1 "3n9/250V" V 5050 4550 50  0000 L CNN
F 2 "MyCapacitors:C_0805" H 5150 4650 60  0001 C CNN
F 3 "" H 5150 4650 60  0000 C CNN
	1    5150 4650
	1    0    0    -1  
$EndComp
$Comp
L R_Small R9
U 1 1 57BC98F5
P 5650 4350
F 0 "R9" H 5500 4300 50  0000 L CNN
F 1 "1" H 5680 4310 50  0000 L CNN
F 2 "MyResistors:R_2512" H 5650 4350 60  0001 C CNN
F 3 "" H 5650 4350 60  0000 C CNN
	1    5650 4350
	-1   0    0    1   
$EndComp
$Comp
L C_Small C19
U 1 1 57BC98FB
P 5650 4650
F 0 "C19" V 5700 4700 50  0000 L CNN
F 1 "3n9/250V" V 5550 4550 50  0000 L CNN
F 2 "MyCapacitors:C_0805" H 5650 4650 60  0001 C CNN
F 3 "" H 5650 4650 60  0000 C CNN
	1    5650 4650
	1    0    0    -1  
$EndComp
$Comp
L R_Small R10
U 1 1 57BC9904
P 6150 4350
F 0 "R10" H 5950 4300 50  0000 L CNN
F 1 "1" H 6180 4310 50  0000 L CNN
F 2 "MyResistors:R_2512" H 6150 4350 60  0001 C CNN
F 3 "" H 6150 4350 60  0000 C CNN
	1    6150 4350
	-1   0    0    1   
$EndComp
$Comp
L C_Small C21
U 1 1 57BC990A
P 6150 4650
F 0 "C21" V 6200 4700 50  0000 L CNN
F 1 "3n9/250V" V 6050 4550 50  0000 L CNN
F 2 "MyCapacitors:C_0805" H 6150 4650 60  0001 C CNN
F 3 "" H 6150 4650 60  0000 C CNN
	1    6150 4650
	1    0    0    -1  
$EndComp
$Comp
L R_Small R13
U 1 1 57BCA6A5
P 6150 5250
F 0 "R13" H 6180 5270 50  0000 L CNN
F 1 "1" H 6180 5210 50  0000 L CNN
F 2 "MyResistors:R_2512" H 6150 5250 60  0001 C CNN
F 3 "" H 6150 5250 60  0000 C CNN
	1    6150 5250
	1    0    0    -1  
$EndComp
$Comp
L C_Small C27
U 1 1 57BCA6AB
P 6150 4950
F 0 "C27" V 6200 4800 50  0000 L CNN
F 1 "3n9/250V" V 6050 4700 50  0000 L CNN
F 2 "MyCapacitors:C_0805" H 6150 4950 60  0001 C CNN
F 3 "" H 6150 4950 60  0000 C CNN
	1    6150 4950
	1    0    0    -1  
$EndComp
$Comp
L R_Small R12
U 1 1 57BCA6B5
P 5650 5250
F 0 "R12" H 5680 5270 50  0000 L CNN
F 1 "1" H 5680 5210 50  0000 L CNN
F 2 "MyResistors:R_2512" H 5650 5250 60  0001 C CNN
F 3 "" H 5650 5250 60  0000 C CNN
	1    5650 5250
	1    0    0    -1  
$EndComp
$Comp
L C_Small C25
U 1 1 57BCA6BB
P 5650 4950
F 0 "C25" V 5700 4800 50  0000 L CNN
F 1 "3n9/250V" V 5550 4700 50  0000 L CNN
F 2 "MyCapacitors:C_0805" H 5650 4950 60  0001 C CNN
F 3 "" H 5650 4950 60  0000 C CNN
	1    5650 4950
	1    0    0    -1  
$EndComp
$Comp
L R_Small R11
U 1 1 57BCA6C4
P 5150 5250
F 0 "R11" H 5180 5270 50  0000 L CNN
F 1 "1" H 5180 5210 50  0000 L CNN
F 2 "MyResistors:R_2512" H 5150 5250 60  0001 C CNN
F 3 "" H 5150 5250 60  0000 C CNN
	1    5150 5250
	1    0    0    -1  
$EndComp
$Comp
L C_Small C23
U 1 1 57BCA6CA
P 5150 4950
F 0 "C23" V 5200 4800 50  0000 L CNN
F 1 "3n9/250V" V 5050 4700 50  0000 L CNN
F 2 "MyCapacitors:C_0805" H 5150 4950 60  0001 C CNN
F 3 "" H 5150 4950 60  0000 C CNN
	1    5150 4950
	1    0    0    -1  
$EndComp
Wire Wire Line
	1450 3300 2550 3300
Wire Wire Line
	2000 3100 2000 2750
Wire Wire Line
	2250 7800 2250 7650
Wire Wire Line
	2250 8050 2250 8000
Wire Wire Line
	2950 7650 2950 7800
Wire Wire Line
	2950 8050 2950 8000
Wire Wire Line
	3400 1550 4750 1550
Wire Wire Line
	1550 3400 1450 3400
Wire Wire Line
	1550 2200 1550 3450
Wire Wire Line
	4400 5250 4400 5600
Wire Wire Line
	4150 1550 4150 1350
Wire Wire Line
	3850 1550 3850 1350
Connection ~ 3850 1550
Wire Wire Line
	3700 1700 3700 1550
Connection ~ 3700 1550
Wire Wire Line
	3850 900  3850 1150
Wire Wire Line
	4150 900  4150 1150
Connection ~ 3850 900 
Connection ~ 4150 900 
Connection ~ 4150 1550
Wire Wire Line
	4450 900  4450 1150
Connection ~ 4450 900 
Wire Wire Line
	4750 900  4750 1150
Wire Wire Line
	4750 1550 4750 1350
Wire Wire Line
	4450 1550 4450 1350
Connection ~ 4450 1550
Wire Wire Line
	2250 6900 1950 6900
Wire Wire Line
	1950 6900 1950 7200
Wire Wire Line
	2150 7750 2150 7650
Wire Wire Line
	2250 6750 2250 6900
Wire Wire Line
	2950 6900 2650 6900
Wire Wire Line
	2650 6900 2650 7200
Wire Wire Line
	2850 7750 2850 7650
Wire Wire Line
	2950 6750 2950 6900
Connection ~ 2950 7650
Wire Wire Line
	3250 7650 3250 7450
Wire Wire Line
	2650 7650 2650 7400
Connection ~ 2650 7650
Wire Wire Line
	1950 7400 1950 7650
Connection ~ 2250 7650
Wire Wire Line
	2500 7750 2500 8050
Connection ~ 2500 7750
Connection ~ 2250 6900
Connection ~ 2950 6900
Wire Wire Line
	1550 3200 1450 3200
Connection ~ 1550 3400
Wire Wire Line
	1550 3000 1450 3000
Connection ~ 1550 3200
Wire Wire Line
	1450 2000 1550 2000
Wire Wire Line
	1450 1900 1550 1900
Wire Wire Line
	4400 2700 4400 2800
Connection ~ 4400 2750
Wire Wire Line
	4400 1950 4400 2300
Wire Wire Line
	1550 2600 1450 2600
Connection ~ 1550 3000
Wire Wire Line
	1550 2800 1450 2800
Connection ~ 1550 2800
Wire Wire Line
	4400 3200 4400 3650
Connection ~ 4400 3350
Wire Wire Line
	4400 4750 4400 4850
Connection ~ 4400 4800
Wire Wire Line
	4400 4050 4400 4350
Connection ~ 4400 5400
Wire Wire Line
	1450 3100 2000 3100
Wire Wire Line
	1800 5400 1800 2900
Wire Wire Line
	1800 2900 1450 2900
Wire Wire Line
	1900 4800 1900 2700
Wire Wire Line
	1900 2700 1450 2700
Wire Wire Line
	3400 900  4750 900 
Wire Wire Line
	2150 7750 2850 7750
Wire Wire Line
	1950 7650 3250 7650
Wire Wire Line
	1450 2400 1550 2400
Connection ~ 1550 2600
Wire Wire Line
	1450 2500 1650 2500
Wire Wire Line
	1450 2300 1650 2300
Connection ~ 3700 900 
Wire Wire Line
	1450 2200 1550 2200
Connection ~ 1550 2400
Wire Wire Line
	1450 2100 2750 2100
Wire Wire Line
	3700 700  3700 900 
Wire Wire Line
	3000 2650 2950 2650
Wire Wire Line
	2950 2550 2950 5300
Wire Wire Line
	2000 2750 3000 2750
Wire Wire Line
	4100 2550 4000 2550
Wire Wire Line
	4000 2550 4000 2650
Wire Wire Line
	4000 2650 3800 2650
Wire Wire Line
	3800 2750 6800 2750
Wire Wire Line
	3800 3250 4000 3250
Wire Wire Line
	4000 3250 4000 3050
Wire Wire Line
	4000 3050 4100 3050
Wire Wire Line
	3800 3350 6150 3350
Wire Wire Line
	1900 4800 3000 4800
Wire Wire Line
	3800 4700 4000 4700
Wire Wire Line
	4000 4700 4000 4600
Wire Wire Line
	4000 4600 4100 4600
Wire Wire Line
	3800 4800 6800 4800
Wire Wire Line
	3000 5400 1800 5400
Wire Wire Line
	3800 5300 3950 5300
Wire Wire Line
	3950 5300 3950 5100
Wire Wire Line
	3950 5100 4100 5100
Wire Wire Line
	3800 5400 6150 5400
Wire Wire Line
	2950 3250 3000 3250
Connection ~ 2950 2650
Wire Wire Line
	2950 4700 3000 4700
Connection ~ 2950 3250
Wire Wire Line
	2950 5300 3000 5300
Connection ~ 2950 4700
Wire Wire Line
	3000 2950 2850 2950
Wire Wire Line
	2850 2950 2850 5800
Wire Wire Line
	2850 3550 3000 3550
Wire Wire Line
	2850 5000 3000 5000
Connection ~ 2850 3550
Connection ~ 2850 5000
Wire Wire Line
	3000 5600 2850 5600
Connection ~ 2850 5600
Connection ~ 4400 2150
Wire Wire Line
	5150 4450 5150 4550
Wire Wire Line
	5150 4200 5150 4250
Connection ~ 5150 4200
Wire Wire Line
	5650 4450 5650 4550
Wire Wire Line
	5650 4200 5650 4250
Connection ~ 5650 4200
Wire Wire Line
	6150 4450 6150 4550
Wire Wire Line
	6150 4200 6150 4250
Wire Wire Line
	6150 5050 6150 5150
Wire Wire Line
	6150 5400 6150 5350
Wire Wire Line
	5650 5050 5650 5150
Wire Wire Line
	5650 5400 5650 5350
Connection ~ 5650 5400
Wire Wire Line
	5150 5050 5150 5150
Wire Wire Line
	5150 5400 5150 5350
Connection ~ 5150 5400
Wire Wire Line
	4400 2150 6150 2150
Wire Wire Line
	4650 2150 4650 2400
Connection ~ 4650 2150
Wire Wire Line
	4650 2600 4650 2900
Connection ~ 4650 2750
Wire Wire Line
	4650 3350 4650 3100
Connection ~ 4650 3350
Wire Wire Line
	4400 4200 6150 4200
Connection ~ 4400 4200
Wire Wire Line
	5150 4750 5150 4850
Connection ~ 5150 4800
Wire Wire Line
	5650 4750 5650 4850
Connection ~ 5650 4800
Wire Wire Line
	6150 4750 6150 4850
Connection ~ 6150 4800
Wire Wire Line
	4650 5150 4650 5400
Connection ~ 4650 5400
Wire Wire Line
	4650 4650 4650 4950
Connection ~ 4650 4800
Wire Wire Line
	4650 4450 4650 4200
Connection ~ 4650 4200
Wire Wire Line
	5150 4500 6400 4500
Connection ~ 5650 4500
Connection ~ 5150 4500
Connection ~ 6150 4500
Wire Wire Line
	5150 5100 6400 5100
Connection ~ 5650 5100
Connection ~ 5150 5100
Connection ~ 6150 5100
$Comp
L C_Small C18
U 1 1 57BD4382
P 5400 4650
F 0 "C18" V 5450 4700 50  0000 L CNN
F 1 "3n9/250V" V 5300 4550 50  0000 L CNN
F 2 "MyCapacitors:C_0805" H 5400 4650 60  0001 C CNN
F 3 "" H 5400 4650 60  0000 C CNN
	1    5400 4650
	1    0    0    -1  
$EndComp
$Comp
L C_Small C24
U 1 1 57BD443E
P 5400 4950
F 0 "C24" V 5450 4800 50  0000 L CNN
F 1 "3n9/250V" V 5300 4700 50  0000 L CNN
F 2 "MyCapacitors:C_0805" H 5400 4950 60  0001 C CNN
F 3 "" H 5400 4950 60  0000 C CNN
	1    5400 4950
	1    0    0    -1  
$EndComp
$Comp
L C_Small C20
U 1 1 57BD464B
P 5900 4650
F 0 "C20" V 5950 4700 50  0000 L CNN
F 1 "3n9/250V" V 5800 4550 50  0000 L CNN
F 2 "MyCapacitors:C_0805" H 5900 4650 60  0001 C CNN
F 3 "" H 5900 4650 60  0000 C CNN
	1    5900 4650
	1    0    0    -1  
$EndComp
$Comp
L C_Small C26
U 1 1 57BD470D
P 5900 4950
F 0 "C26" V 5950 4800 50  0000 L CNN
F 1 "3n9/250V" V 5800 4700 50  0000 L CNN
F 2 "MyCapacitors:C_0805" H 5900 4950 60  0001 C CNN
F 3 "" H 5900 4950 60  0000 C CNN
	1    5900 4950
	1    0    0    -1  
$EndComp
$Comp
L C_Small C22
U 1 1 57BD5180
P 6400 4650
F 0 "C22" V 6450 4700 50  0000 L CNN
F 1 "3n9/250V" V 6300 4550 50  0000 L CNN
F 2 "MyCapacitors:C_0805" H 6400 4650 60  0001 C CNN
F 3 "" H 6400 4650 60  0000 C CNN
	1    6400 4650
	1    0    0    -1  
$EndComp
Wire Wire Line
	5400 5100 5400 5050
Connection ~ 5400 5100
Wire Wire Line
	5400 4750 5400 4850
Connection ~ 5400 4800
Wire Wire Line
	5400 4550 5400 4500
Connection ~ 5400 4500
Wire Wire Line
	5900 4500 5900 4550
Connection ~ 5900 4500
Wire Wire Line
	5900 4750 5900 4850
Connection ~ 5900 4800
Wire Wire Line
	6400 4750 6400 4850
Connection ~ 6400 4800
Wire Wire Line
	6400 4500 6400 4550
Wire Wire Line
	5900 5050 5900 5100
Connection ~ 5900 5100
$Comp
L C_Small C28
U 1 1 57BD5E6E
P 6400 4950
F 0 "C28" V 6450 4800 50  0000 L CNN
F 1 "3n9/250V" V 6300 4700 50  0000 L CNN
F 2 "MyCapacitors:C_0805" H 6400 4950 60  0001 C CNN
F 3 "" H 6400 4950 60  0000 C CNN
	1    6400 4950
	1    0    0    -1  
$EndComp
Wire Wire Line
	6400 5100 6400 5050
$Comp
L R_Small R2
U 1 1 57BD6624
P 5150 2300
F 0 "R2" H 5000 2250 50  0000 L CNN
F 1 "1" H 5180 2260 50  0000 L CNN
F 2 "MyResistors:R_2512" H 5150 2300 60  0001 C CNN
F 3 "" H 5150 2300 60  0000 C CNN
	1    5150 2300
	-1   0    0    1   
$EndComp
$Comp
L C_Small C5
U 1 1 57BD662A
P 5150 2600
F 0 "C5" V 5200 2650 50  0000 L CNN
F 1 "3n9/250V" V 5050 2500 50  0000 L CNN
F 2 "MyCapacitors:C_0805" H 5150 2600 60  0001 C CNN
F 3 "" H 5150 2600 60  0000 C CNN
	1    5150 2600
	1    0    0    -1  
$EndComp
$Comp
L R_Small R3
U 1 1 57BD6630
P 5650 2300
F 0 "R3" H 5500 2250 50  0000 L CNN
F 1 "1" H 5680 2260 50  0000 L CNN
F 2 "MyResistors:R_2512" H 5650 2300 60  0001 C CNN
F 3 "" H 5650 2300 60  0000 C CNN
	1    5650 2300
	-1   0    0    1   
$EndComp
$Comp
L C_Small C7
U 1 1 57BD6636
P 5650 2600
F 0 "C7" V 5700 2650 50  0000 L CNN
F 1 "3n9/250V" V 5550 2500 50  0000 L CNN
F 2 "MyCapacitors:C_0805" H 5650 2600 60  0001 C CNN
F 3 "" H 5650 2600 60  0000 C CNN
	1    5650 2600
	1    0    0    -1  
$EndComp
$Comp
L R_Small R4
U 1 1 57BD663C
P 6150 2300
F 0 "R4" H 6000 2250 50  0000 L CNN
F 1 "1" H 6180 2260 50  0000 L CNN
F 2 "MyResistors:R_2512" H 6150 2300 60  0001 C CNN
F 3 "" H 6150 2300 60  0000 C CNN
	1    6150 2300
	-1   0    0    1   
$EndComp
$Comp
L C_Small C9
U 1 1 57BD6642
P 6150 2600
F 0 "C9" V 6200 2650 50  0000 L CNN
F 1 "3n9/250V" V 6050 2500 50  0000 L CNN
F 2 "MyCapacitors:C_0805" H 6150 2600 60  0001 C CNN
F 3 "" H 6150 2600 60  0000 C CNN
	1    6150 2600
	1    0    0    -1  
$EndComp
$Comp
L R_Small R7
U 1 1 57BD6648
P 6150 3200
F 0 "R7" H 6180 3220 50  0000 L CNN
F 1 "1" H 6180 3160 50  0000 L CNN
F 2 "MyResistors:R_2512" H 6150 3200 60  0001 C CNN
F 3 "" H 6150 3200 60  0000 C CNN
	1    6150 3200
	1    0    0    -1  
$EndComp
$Comp
L C_Small C15
U 1 1 57BD664E
P 6150 2900
F 0 "C15" V 6200 2750 50  0000 L CNN
F 1 "3n9/250V" V 6050 2650 50  0000 L CNN
F 2 "MyCapacitors:C_0805" H 6150 2900 60  0001 C CNN
F 3 "" H 6150 2900 60  0000 C CNN
	1    6150 2900
	1    0    0    -1  
$EndComp
$Comp
L R_Small R6
U 1 1 57BD6654
P 5650 3200
F 0 "R6" H 5680 3220 50  0000 L CNN
F 1 "1" H 5680 3160 50  0000 L CNN
F 2 "MyResistors:R_2512" H 5650 3200 60  0001 C CNN
F 3 "" H 5650 3200 60  0000 C CNN
	1    5650 3200
	1    0    0    -1  
$EndComp
$Comp
L C_Small C13
U 1 1 57BD665A
P 5650 2900
F 0 "C13" V 5700 2750 50  0000 L CNN
F 1 "3n9/250V" V 5550 2650 50  0000 L CNN
F 2 "MyCapacitors:C_0805" H 5650 2900 60  0001 C CNN
F 3 "" H 5650 2900 60  0000 C CNN
	1    5650 2900
	1    0    0    -1  
$EndComp
$Comp
L R_Small R5
U 1 1 57BD6660
P 5150 3200
F 0 "R5" H 5180 3220 50  0000 L CNN
F 1 "1" H 5180 3160 50  0000 L CNN
F 2 "MyResistors:R_2512" H 5150 3200 60  0001 C CNN
F 3 "" H 5150 3200 60  0000 C CNN
	1    5150 3200
	1    0    0    -1  
$EndComp
$Comp
L C_Small C11
U 1 1 57BD6666
P 5150 2900
F 0 "C11" V 5200 2750 50  0000 L CNN
F 1 "3n9/250V" V 5050 2650 50  0000 L CNN
F 2 "MyCapacitors:C_0805" H 5150 2900 60  0001 C CNN
F 3 "" H 5150 2900 60  0000 C CNN
	1    5150 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 2400 5150 2500
Wire Wire Line
	5150 2150 5150 2200
Connection ~ 5150 2150
Wire Wire Line
	5650 2400 5650 2500
Wire Wire Line
	5650 2150 5650 2200
Connection ~ 5650 2150
Wire Wire Line
	6150 2400 6150 2500
Wire Wire Line
	6150 2150 6150 2200
Wire Wire Line
	6150 3000 6150 3100
Wire Wire Line
	6150 3350 6150 3300
Wire Wire Line
	5650 3000 5650 3100
Wire Wire Line
	5650 3350 5650 3300
Connection ~ 5650 3350
Wire Wire Line
	5150 3000 5150 3100
Wire Wire Line
	5150 3350 5150 3300
Connection ~ 5150 3350
Wire Wire Line
	5150 2700 5150 2800
Connection ~ 5150 2750
Wire Wire Line
	5650 2700 5650 2800
Connection ~ 5650 2750
Wire Wire Line
	6150 2700 6150 2800
Connection ~ 6150 2750
Wire Wire Line
	5150 2450 6400 2450
Connection ~ 5650 2450
Connection ~ 5150 2450
Connection ~ 6150 2450
Wire Wire Line
	5150 3050 6400 3050
Connection ~ 5650 3050
Connection ~ 5150 3050
Connection ~ 6150 3050
$Comp
L C_Small C6
U 1 1 57BD66A8
P 5400 2600
F 0 "C6" V 5450 2650 50  0000 L CNN
F 1 "3n9/250V" V 5300 2500 50  0000 L CNN
F 2 "MyCapacitors:C_0805" H 5400 2600 60  0001 C CNN
F 3 "" H 5400 2600 60  0000 C CNN
	1    5400 2600
	1    0    0    -1  
$EndComp
$Comp
L C_Small C12
U 1 1 57BD66AE
P 5400 2900
F 0 "C12" V 5450 2750 50  0000 L CNN
F 1 "3n9/250V" V 5300 2650 50  0000 L CNN
F 2 "MyCapacitors:C_0805" H 5400 2900 60  0001 C CNN
F 3 "" H 5400 2900 60  0000 C CNN
	1    5400 2900
	1    0    0    -1  
$EndComp
$Comp
L C_Small C8
U 1 1 57BD66B4
P 5900 2600
F 0 "C8" V 5950 2650 50  0000 L CNN
F 1 "3n9/250V" V 5800 2500 50  0000 L CNN
F 2 "MyCapacitors:C_0805" H 5900 2600 60  0001 C CNN
F 3 "" H 5900 2600 60  0000 C CNN
	1    5900 2600
	1    0    0    -1  
$EndComp
$Comp
L C_Small C14
U 1 1 57BD66BA
P 5900 2900
F 0 "C14" V 5950 2750 50  0000 L CNN
F 1 "3n9/250V" V 5800 2650 50  0000 L CNN
F 2 "MyCapacitors:C_0805" H 5900 2900 60  0001 C CNN
F 3 "" H 5900 2900 60  0000 C CNN
	1    5900 2900
	1    0    0    -1  
$EndComp
$Comp
L C_Small C10
U 1 1 57BD66C0
P 6400 2600
F 0 "C10" V 6450 2650 50  0000 L CNN
F 1 "3n9/250V" V 6300 2500 50  0000 L CNN
F 2 "MyCapacitors:C_0805" H 6400 2600 60  0001 C CNN
F 3 "" H 6400 2600 60  0000 C CNN
	1    6400 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	5400 3050 5400 3000
Connection ~ 5400 3050
Wire Wire Line
	5400 2700 5400 2800
Connection ~ 5400 2750
Wire Wire Line
	5400 2500 5400 2450
Connection ~ 5400 2450
Wire Wire Line
	5900 2450 5900 2500
Connection ~ 5900 2450
Wire Wire Line
	5900 2700 5900 2800
Connection ~ 5900 2750
Wire Wire Line
	6400 2700 6400 2800
Connection ~ 6400 2750
Wire Wire Line
	6400 2450 6400 2500
Wire Wire Line
	5900 3000 5900 3050
Connection ~ 5900 3050
$Comp
L C_Small C16
U 1 1 57BD66D8
P 6400 2900
F 0 "C16" V 6450 2750 50  0000 L CNN
F 1 "3n9/250V" V 6300 2650 50  0000 L CNN
F 2 "MyCapacitors:C_0805" H 6400 2900 60  0001 C CNN
F 3 "" H 6400 2900 60  0000 C CNN
	1    6400 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	6400 3050 6400 3000
Wire Wire Line
	2550 3300 2550 3350
Wire Wire Line
	2550 3350 3000 3350
Wire Wire Line
	2750 2000 2750 5500
Wire Wire Line
	2750 5500 3000 5500
Wire Wire Line
	3000 4900 2750 4900
Connection ~ 2750 4900
Wire Wire Line
	3000 3450 2750 3450
Connection ~ 2750 3450
Wire Wire Line
	3000 2850 2750 2850
Connection ~ 2750 2850
$Comp
L R_Small R1
U 1 1 57BE81E8
P 2750 1900
F 0 "R1" H 2780 1920 50  0000 L CNN
F 1 "4k7" H 2780 1860 50  0000 L CNN
F 2 "MyResistors:R_0603" H 2750 1900 60  0001 C CNN
F 3 "" H 2750 1900 60  0000 C CNN
	1    2750 1900
	-1   0    0    1   
$EndComp
$Comp
L +3.3V #PWR016
U 1 1 57BE8D9A
P 2750 1700
F 0 "#PWR016" H 2750 1550 50  0001 C CNN
F 1 "+3.3V" V 2750 1800 50  0000 L CNN
F 2 "" H 2750 1700 60  0000 C CNN
F 3 "" H 2750 1700 60  0000 C CNN
	1    2750 1700
	1    0    0    -1  
$EndComp
Connection ~ 2750 2100
Wire Wire Line
	2750 1800 2750 1700
$EndSCHEMATC