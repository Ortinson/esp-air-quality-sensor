#ifndef BATTERY_LEVEL_CALCULATOR_H_
#define BATTERY_LEVEL_CALCULATOR_H_

class BatteryLevelCalculator {
public:
    BatteryLevelCalculator();
    float GetInputVoltage(unsigned int adc);
    float GetBatteryVoltage(unsigned int adc);
    unsigned int GetBatteryPercentage(unsigned int adc);

private:
    static const float _vmax;
    static const float _vmin;
    static const float _vmax_adc;
    static const float _r1;
    static const float _r2;

    static const unsigned int _adc_precision;

};

#endif // BATTERY_LEVEL_CALCULATOR_H_