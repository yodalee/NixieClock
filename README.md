Nixie Clock
-----------

All data about my nixie tube implementation:

## The Nixie Clock

![Nixie Clock](fig/v1result.jpg)

Some demo video:  
[Test Tube](https://www.youtube.com/watch?v=yUsdp3tUh6w)  
[Change WorldLine](https://www.youtube.com/watch?v=egiKm6L4Y-A)  

## Repository Structure
├── fig  
├── hardware  
│   ├── BOMmanual\_v1.0.1.csv : BOM file manually written  
│   ├── BOM\_v1.0.1.tsv : BOM file export from EasyEDA  
│   ├── layout  
│   │   ├── gerber\_v1.01 : v1.01 Gerber file  
│   │   └── gerber\_v1.02 : v1.02 Gerber file  
│   └── schematic : Schematic for [Altium](https://www.altium.com) and [EasyEDA](https://easyeda.com)  
└── software  
&emsp;&emsp;└── v1.01  
&emsp;&emsp;&emsp;&emsp;├── DS1309 : Set time into DS1309  
&emsp;&emsp;&emsp;&emsp;├── HC238test : Test 74HC238 works fine.  
&emsp;&emsp;&emsp;&emsp;├── HC4514test : Test 74HC4514 works fine.  
&emsp;&emsp;&emsp;&emsp;└── nixie : Source code that right now runs on my Nixie Clock  

## CAUTION

* The BOM table may have some difference that I did not notice, please check before you order.
* v1.02 only draw on easyEDA, it does not get fabricated and tested. It might have design problem and cannot work normally. Use it carefully.
