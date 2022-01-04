#ifndef __TBEAMPOWER
#define __TBEAMPOWER

#define TBP_NO_PIN -1

#include "Arduino.h"
#include <axp20x.h>
#include <esp_wifi.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_bt.h>
#include <driver/rtc_io.h>

class TBeamPower
{
protected:
    AXP20X_Class axp;
    int _sensor_pwr_pin; // gpio that controls power to sensors
    int _battery_pin;    // Battery power sensor
    int _led_pin;
    bool _hasAXP192;
    int _adxsda;
    int _adxscl;

 public:
    TBeamPower(int adx_sda = 21, int adx_scl = 22, int sensor_pwr_pin = TBP_NO_PIN, int batt_pin = TBP_NO_PIN, int led_pin = TBP_NO_PIN);
    void begin(void);
    bool hasAXP();
    void print_status();
    void print_wakeup_reason();
    void flashlight(char code);
    void led_onoff(bool on);
    float get_battery_voltage();
    float get_supply_voltage();
    void deep_sleep(uint64_t timetosleep);
    void power_sensors(bool on);
    void power_peripherals(bool on);
    void power_GPS(bool on);
    void power_LoRa(bool on);
    void shutdown();
 private:
};


#endif