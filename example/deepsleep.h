#ifndef __DEEPSLEEP__
#define __DEEPSLEEP__

#define BATTERY_PIN  35   // battery level measurement pin, here is the voltage divider connected
#define BUSPWR        4   // GPIO04 -- sensor bus power control
#define PWRSDA       21
#define PWRSCL       22

#define SCK           5   // GPIO5  -- SX1278's SCK
#define MISO         19   // GPIO19 -- SX1278's MISnO
#define MOSI         27   // GPIO27 -- SX1278's MOSI
#define SS           18   // GPIO18 -- SX1278's CS
#define RST          14   // GPIO14 -- SX1278's RESET
#define DI0          26   // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define GPSRX        34   
#define GPSTX        12   
#define GPSBAUD      9600

#endif