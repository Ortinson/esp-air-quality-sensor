#include "battery_level_calculator.h"

const float _vmax = 4.2;
const float BatteryLevelCalculator::_vmin = 3;
const float BatteryLevelCalculator::_vmax_adc = 1;
const float BatteryLevelCalculator::_r1 = 100000;
const float BatteryLevelCalculator::_r2 = 25000;
const unsigned int BatteryLevelCalculator::_adc_precision = 2^10;

BatteryLevelCalculator::BatteryLevelCalculator(){
    
}

float BatteryLevelCalculator::GetInputVoltage(unsigned int adc){
    return adc * _vmax_adc / _adc_precision;
}

float BatteryLevelCalculator::GetBatteryVoltage(unsigned int adc){
    float V = this->GetInputVoltage(adc);
    return V * (_r1 + _r2) /_r2;
}

unsigned int BatteryLevelCalculator::GetBatteryPercentage(unsigned int adc){
    // y = (x-3) * 100 / (4.2-3)
    float V = this->GetBatteryVoltage(adc);
    float percentage = (V - _vmin) * 100 / (_vmax - _vmin);
    return static_cast<unsigned int>(percentage);
}