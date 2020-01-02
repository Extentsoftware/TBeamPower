
#include "TBeamPower.h"

#define S_to_uS_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */

TBeamPower::TBeamPower(int batt_pin, int adx_sda, int adx_scl, int pwr_pin)
{
    sensor_pwr_pin = pwr_pin;
    battery_pin = batt_pin;
    adxsda=adx_sda;
    adxscl=adx_scl;
}

TBeamPower::TBeamPower(int adx_sda, int adx_scl, int pwr_pin)
{
    sensor_pwr_pin = pwr_pin;
    battery_pin = TBP_NO_PIN;
    adxsda=adx_sda;
    adxscl=adx_scl;
}

void TBeamPower::begin(void)
{
    Wire.begin(adxsda, adxscl);  
    hasAXP192 = !axp.begin(Wire, AXP192_SLAVE_ADDRESS);
    if (hasAXP192)
    {
        Serial.println("AXP192 Begin PASS");
        axp.setDCDC1Voltage(3300);
        led_onoff(false);
    }
    else
    {
        Serial.println("AXP192 Begin FAIL");
        return;
    }
}

void TBeamPower::print_wakeup_reason()
{
    esp_sleep_wakeup_cause_t wakeup_reason;

    wakeup_reason = esp_sleep_get_wakeup_cause();

    switch (wakeup_reason)
    {
    case 2:
        Serial.println("Wakeup caused by external signal using RTC_IO");
        break;
    case 3:
        Serial.println("Wakeup caused by external signal using RTC_CNTL");
        break;
    case 4:
        Serial.println("Wakeup caused by timer");
        break;
    case 5:
        Serial.println("Wakeup caused by touchpad");
        break;
    case 6:
        Serial.println("Wakeup caused by ULP program");
        break;
    default:
        Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
        break;
    }
}

void TBeamPower::led_onoff(bool on)
{
    if (on)
        axp.setChgLEDMode(AXP20X_LED_LOW_LEVEL);
    else
        axp.setChgLEDMode(AXP20X_LED_OFF);
}

void TBeamPower::flashlight(char code)
{
    led_onoff(false);
    for (char i = 0; i < 8; i++)
    {
        led_onoff((code & 1) == 1);
        delay(150);
        code = code >> 1;
        led_onoff(false);
    }
}

void TBeamPower::print_status()
{
}

float TBeamPower::get_battery_voltage()
{
    if (hasAXP192)
    {
        Serial.printf("Battery Percent: %d\n", axp.getBattPercentage());
        Serial.printf("DCDC1:           %f\n", axp.getDCDC1Voltage() / 1000.0);
        Serial.printf("DCDC2:           %f\n", axp.getDCDC2Voltage() / 1000.0);
        Serial.printf("DCDC3:           %f\n", axp.getDCDC3Voltage() / 1000.0);
        Serial.printf("ChargeCurrent:   %f\n", axp.getSettingChargeCurrent());
        Serial.printf("IPSOUTVoltage:   %f\n", axp.getSysIPSOUTVoltage() / 1000.0);
        Serial.printf("Temp:            %f\n", axp.getTemp());
        Serial.printf("TSTemp:          %f\n", axp.getTSTemp());
        Serial.printf("VbusCurrent:     %f\n", axp.getVbusCurrent());
        Serial.printf("VbusVoltage:     %f\n", axp.getVbusVoltage() / 1000.0);
        Serial.printf("BattInpower:     %f\n", axp.getBattInpower());
        Serial.printf("BattDischgCur:   %f\n", axp.getBattDischargeCurrent());
        Serial.printf("BattChargeCur:   %f\n", axp.getBattChargeCurrent());
        Serial.printf("LDO2Voltage:     %d\n", axp.getLDO2Voltage());
        Serial.printf("LDO3Voltage:     %d\n", axp.getLDO3Voltage());

        if (axp.isBatteryConnect())
            return axp.getBattVoltage() / 1000.0;
        else
            return axp.getSysIPSOUTVoltage() / 1000.0;        
    }
    else if(battery_pin == TBP_NO_PIN){
        return -1;
    }else{
        // we've set 10-bit ADC resolution 2^10=1024 and voltage divider makes it half of maximum readable value (which is 3.3V)
        // set battery measurement pin
        adcAttachPin(battery_pin);
        adcStart(battery_pin);
        analogReadResolution(10); // Default of 12 is not very linear. Recommended to use 10 or 11 depending on needed resolution.
        return analogRead(battery_pin) * 2.0 * (3.3 / 1024.0);
    }
}

void TBeamPower::power_sensors(bool on)
{
    if(sensor_pwr_pin==TBP_NO_PIN){
        return;
    }else if (on){
        pinMode(sensor_pwr_pin, OUTPUT);    // power enable for the sensors
        digitalWrite(sensor_pwr_pin, HIGH); // turn off power to the sensor bus
    }
    else
        digitalWrite(sensor_pwr_pin, LOW); // turn off power to the sensor bus
}

void TBeamPower::power_peripherals(bool on)
{
    axp.setPowerOutPut(AXP192_DCDC1, on ? AXP202_ON : AXP202_OFF);
    axp.setPowerOutPut(AXP192_DCDC2, on ? AXP202_ON : AXP202_OFF);
    axp.setPowerOutPut(AXP192_LDO2, on ? AXP202_ON : AXP202_OFF);
    axp.setPowerOutPut(AXP192_LDO3, on ? AXP202_ON : AXP202_OFF);
}

void TBeamPower::power_GPS(bool on)
{
    axp.setPowerOutPut(AXP192_LDO3, on ? AXP202_ON : AXP202_OFF); // GPS Power
}

void TBeamPower::power_LoRa(bool on)
{
    axp.setPowerOutPut(AXP192_LDO2, on ? AXP202_ON : AXP202_OFF); // LoRa Power
}

void TBeamPower::deep_sleep(uint64_t timetosleep)
{
    power_sensors(false);
    power_peripherals(false);

    // turn Off RTC
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_OFF);

    // turnOffWifi
    esp_wifi_stop();
    esp_wifi_deinit();

    // turnOffBluetooth
    esp_bluedroid_disable();
    esp_bluedroid_deinit();

    esp_bt_controller_disable();
    esp_bt_controller_deinit();

    esp_err_t result;
    do
    {
        uint64_t us = timetosleep * S_to_uS_FACTOR;
        result = esp_sleep_enable_timer_wakeup(us);
        if (result == ESP_ERR_INVALID_ARG)
        {
            if (timetosleep > 60)
                timetosleep = timetosleep - 60;
            else if (timetosleep == 10)
                return; // avoid infinite loop
            else
                timetosleep = 10;
        }
    } while (result == ESP_ERR_INVALID_ARG);

    led_onoff(false);

    esp_deep_sleep_start();
}
