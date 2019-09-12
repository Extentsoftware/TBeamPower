# TTGO T-Beam Deep Sleep Power Test

## TTGO Product Upgrade V07->V8

Lilygo released V8 of the TTGO T-Beam board in August 2019. This rework included several changes mainly targetted at reducing power consumption by using integrating the AXP192 power controller IC. A list of the changes is as follows:

1. User button Pin GPIO39 pin is replaced with GPIO38
2. Replace the charging IC (TP5400) with the power management AXP192
3. GPS TX (12), RX (34) pin replacement
4. Power on switch removed and replaced with push button
5. Reduced sleep current
6. GPS battery replacement

The AXP193 controls the board's power domains and has 4 linear voltage regulators (LDO) and 3 Buck DC-DC converters.

- DCDC1 powers OLED if installed
- DCDC2 not connected
- DCDC3 now controls the 3.3v to the board and to header 2 Pin7 & pin11 
- LDO2 provides LoRa power
- LDO3 provides GPS power
- AXP Charge Led replaces Led on GPIO

## Schematic

https://github.com/Xinyuan-LilyGO/TTGO-T-Beam

## Power Consumption

Experiments indicate that

| Mode  |  Pwer (mA) |
|---|---|
| Deep sleep  |  5.3 |
| GPS and LoRa powered off  | 42mA  |
| GPS and LoRa powered on  | 44mA  |
| Blue LED charge light | +20mA  |


