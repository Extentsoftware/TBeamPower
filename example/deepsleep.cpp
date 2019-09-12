// Pin map http://tinymicros.com/wiki/TTGO_T-Beam
// https://github.com/Xinyuan-LilyGO/TTGO-T-Beam
//
// pio device list
// pio run --target upload
// pio run --target uploadfs
// pio device monitor -p COM14 -b 115200

#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>         // https://github.com/sandeepmistry/arduino-LoRa/blob/master/API.md
#include "deepsleep.h"
#include "TBeamPower.h"

TBeamPower power(BUSPWR, BATTERY_PIN,PWRSDA,PWRSCL);

void setupSerial() { 
  Serial.println();
  power.print_wakeup_reason();
}

void setup() {

  Serial.begin(115200);
  while (!Serial);
  Serial.println("Deep Sleep Test");

  power.begin();

  float currentVoltage = power.get_battery_voltage();
}

void loop() {
  power.led_onoff(true);
  delay(500);
  power.led_onoff(false);

  power.power_sensors(false);
  power.power_peripherals(false);

  Serial.println("Awake");
  delay(5000);

  Serial.println("LoRa");
  power.power_LoRa(true);
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);
  LoRa.idle();
  delay(5000);

  Serial.println("GPS");
  power.power_GPS(true);
  Serial1.begin(GPSBAUD, SERIAL_8N1, GPSRX, GPSTX);

  Serial.println("Sleeping");
  power.deep_sleep(5);
}
