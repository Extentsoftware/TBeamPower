
#include "TBeamPower.h"

#define S_to_uS_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */

TBeamPower::TBeamPower(int adx_sda, int adx_scl, int pwr_pin, int batt_pin, int led_pin)
{
    _sensor_pwr_pin = pwr_pin;
    _battery_pin = batt_pin;
    _adxsda=adx_sda;
    _adxscl=adx_scl;
    _led_pin = led_pin;
}

void TBeamPower::begin(void)
{
    if (_adxsda==TBP_NO_PIN)
    {
        _hasAXP192 = false;    
    }
    else
    {
        Wire.begin(_adxsda, _adxscl);  
        _hasAXP192 = !axp.begin(Wire, AXP192_SLAVE_ADDRESS);
        if (_hasAXP192)
        {
            Serial.println("AXP192 Begin PASS");
            
            axp.setAdcSamplingRate(AXP_ADC_SAMPLING_RATE_25HZ);
            axp.adc1Enable(AXP202_VBUS_VOL_ADC1, true);
            axp.adc1Enable(AXP202_VBUS_CUR_ADC1, true);
            axp.setDCDC1Voltage(3300);            
            led_onoff(false);
        }
        else
        {
            Serial.println("AXP192 Begin FAIL");
            return;
        }
    }    
}

bool TBeamPower::hasAXP(void){
    return _hasAXP192;
}
void TBeamPower::shutdown(void){
    if (_hasAXP192)
    {
        axp.shutdown();
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
    if (_hasAXP192)
    {
        if (on)
            axp.setChgLEDMode(AXP20X_LED_LOW_LEVEL);
        else
            axp.setChgLEDMode(AXP20X_LED_OFF);
    }
    if (_led_pin != TBP_NO_PIN)
    {
          pinMode(_led_pin, OUTPUT);    // power enable for the sensors
          digitalWrite(_led_pin, on?HIGH:LOW); // turn off power to the sensor bus
    }
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
    if (_hasAXP192)
    {
        Serial.printf("Input  Voltages:\n");
        Serial.printf("      Vbus:     %.2fv  %.2f mA\n", axp.getVbusVoltage() / 1000.0, axp.getVbusCurrent());
        Serial.printf("      Batt:     %.2fv  %.2f mA\n", axp.getBattVoltage() / 1000.0, axp.getBattDischargeCurrent());
        Serial.printf("Output Voltages:\n");
        Serial.printf("         DCDC1: %.2fv\n", axp.getDCDC1Voltage() / 1000.0);
        Serial.printf("         DCDC2: %.2fv\n", axp.getDCDC2Voltage() / 1000.0);
        Serial.printf("         DCDC3: %.2fv\n", axp.getDCDC3Voltage() / 1000.0);
        Serial.printf("          LDO2: %.2fv\n", axp.getLDO2Voltage()  / 1000.0);
        Serial.printf("          LDO3: %.2fv\n", axp.getLDO3Voltage()  / 1000.0);
        Serial.printf("ChargeCurrent:  %.2fA\n", axp.getSettingChargeCurrent()/1000.0);
        Serial.printf("IPSOUTVoltage:  %.2fv\n", axp.getSysIPSOUTVoltage() / 1000.0);
        Serial.printf("Temp:           %.2f°C\n", axp.getTemp());
        Serial.printf("TSTemp:         %.2f\n", axp.getTSTemp());
        delay(100);
        Serial.printf("Battery:\n");
        delay(100);
        Serial.printf("         Voltage:   %.2fv\n", axp.getBattVoltage() / 1000.0);
        delay(100);
        //Serial.printf("         Inpower:   %.2f\n",  axp.getBattInpower() /1000.0);
        Serial.printf("         ChargeCur: %.2f\n",  axp.getBattChargeCurrent()/1000.0);      
        delay(100);
        Serial.printf("         Connected: %s\n",    axp.isBatteryConnect()?"true":"false");
        delay(100);
        Serial.printf("         ChargEN :  %s\n",    axp.isChargeingEnable()?"true":"false");
        delay(100);
        Serial.printf("         Percent:   %d\n",    axp.getBattPercentage());
        delay(100);
        Serial.printf("         Charging:  %s\n",    axp.isChargeing()?"true":"false");
        delay(100);
    }
}

float TBeamPower::get_battery_voltage()
{
    if (_hasAXP192)
    {
        if (axp.isBatteryConnect())
            return axp.getBattVoltage() / 1000.0;
        else
            return 0.0;        
    }
    else if(_battery_pin == TBP_NO_PIN){
        return -1;
    }else{
        // we've set 10-bit ADC resolution 2^10=1024 and voltage divider makes it half of maximum readable value (which is 3.3V)
        // set battery measurement pin
        adcAttachPin(_battery_pin);
        //adcStart(battery_pin);
        analogReadResolution(10); // Default of 12 is not very linear. Recommended to use 10 or 11 depending on needed resolution.
        return analogRead(_battery_pin) * 2.0 * (3.3 / 1024.0);
    }
}

float TBeamPower::get_supply_voltage()
{
    if (_hasAXP192)
    {
        float v = axp.getVbusVoltage() / 1000.0;
        if (v>0)
            return v;
    }

    return get_battery_voltage();
}

void TBeamPower::power_sensors(bool on)
{
    if(_sensor_pwr_pin==TBP_NO_PIN){
        return;
    }else if (on){
        pinMode(_sensor_pwr_pin, OUTPUT);    // power enable for the sensors
        digitalWrite(_sensor_pwr_pin, HIGH); // turn off power to the sensor bus
    }
    else
        digitalWrite(_sensor_pwr_pin, LOW); // turn off power to the sensor bus
}

void TBeamPower::power_peripherals(bool on)
{
    if (_hasAXP192)
    {
        axp.setPowerOutPut(AXP192_DCDC1, on ? AXP202_ON : AXP202_OFF);
        axp.setPowerOutPut(AXP192_DCDC2, on ? AXP202_ON : AXP202_OFF);
        axp.setPowerOutPut(AXP192_LDO2, on ? AXP202_ON : AXP202_OFF);
        axp.setPowerOutPut(AXP192_LDO3, on ? AXP202_ON : AXP202_OFF);
    }
}

void TBeamPower::power_GPS(bool on)
{
    if (_hasAXP192)
    {
        axp.setPowerOutPut(AXP192_LDO3, on ? AXP202_ON : AXP202_OFF); // GPS Power
    }
}

void TBeamPower::power_LoRa(bool on)
{
    if (_hasAXP192)
    {
        axp.setPowerOutPut(AXP192_LDO2, on ? AXP202_ON : AXP202_OFF); // LoRa Power
    }
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
