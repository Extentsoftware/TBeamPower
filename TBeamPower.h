#ifndef __TBEAMPOWER
#define __TBEAMPOWER

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
    int sensor_pwr_pin; // gpio that controls power to sensors
    int battery_pin;    // Battery power sensor
    bool hasAXP192;
    int adxsda;
    int adxscl;

 public:
    TBeamPower(int batt_pin = 35, int adx_sda = 21, int adx_scl = 22, int pwr_pin = -1);
    void begin(void);
    void print_status();
    void print_wakeup_reason();
    void flashlight(char code);
    void led_onoff(bool on);
    float get_battery_voltage();
    void deep_sleep(uint64_t timetosleep);
    void power_sensors(bool on);
    void power_peripherals(bool on);
    void power_GPS(bool on);
    void power_LoRa(bool on);
 private:
};


#endif