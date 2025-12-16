/**
 * @file INA226.cpp
 * @brief Implementation of INA226 Power Monitor Library
 */

#include "INA226.h"

INA226::INA226(uint8_t address) {
    _address = address;
    _wire = nullptr;
    _currentLSB = 0;
    _powerLSB = 0;
    _shuntResistor = 0.1;
    _initialized = false;
}

bool INA226::begin(TwoWire *wire) {
    _wire = wire;
    
    // Check if device is connected
    if (!isConnected()) {
        return false;
    }
    
    // Verify manufacturer and die ID
    uint16_t mfgId = getManufacturerID();
    uint16_t dieId = getDieID();
    
    if (mfgId != 0x5449) {  // Texas Instruments
        Serial.printf("INA226 Warning: Unexpected Manufacturer ID: 0x%04X\n", mfgId);
    }
    
    // Reset device
    reset();
    delay(10);
    
    // Set default configuration
    setConfig(INA226_DEFAULT_CONFIG);
    
    _initialized = true;
    return true;
}

bool INA226::isConnected() {
    _wire->beginTransmission(_address);
    return (_wire->endTransmission() == 0);
}

void INA226::reset() {
    // Set reset bit (bit 15) in configuration register
    writeRegister(INA226_REG_CONFIG, 0x8000);
    delay(1);
}

void INA226::setConfig(uint16_t config) {
    writeRegister(INA226_REG_CONFIG, config);
}

uint16_t INA226::getConfig() {
    return readRegister(INA226_REG_CONFIG);
}

void INA226::calibrate(float shuntResistor, float maxCurrent) {
    _shuntResistor = shuntResistor;
    
    // Calculate Current_LSB = Max_Current / 2^15
    _currentLSB = maxCurrent / 32768.0;
    
    // Calculate Power_LSB = 25 * Current_LSB
    _powerLSB = 25.0 * _currentLSB;
    
    // Calculate Calibration = 0.00512 / (Current_LSB * R_shunt)
    float cal = 0.00512 / (_currentLSB * _shuntResistor);
    uint16_t calValue = (uint16_t)cal;
    
    // Write calibration register
    writeRegister(INA226_REG_CALIBRATION, calValue);
    
    Serial.printf("INA226 Calibrated: CurrentLSB=%.6f A, PowerLSB=%.6f W, Cal=%d\n", 
                  _currentLSB, _powerLSB, calValue);
}

float INA226::getShuntVoltage() {
    int16_t value = (int16_t)readRegister(INA226_REG_SHUNT_VOLTAGE);
    // LSB = 2.5 µV
    return value * 0.0025;  // Return in millivolts
}

float INA226::getBusVoltage() {
    uint16_t value = readRegister(INA226_REG_BUS_VOLTAGE);
    // LSB = 1.25 mV
    return value * 0.00125;  // Return in Volts
}

float INA226::getCurrent() {
    if (_currentLSB == 0) {
        // Not calibrated, calculate from shunt voltage
        float shuntVoltage = getShuntVoltage();  // in mV
        return (shuntVoltage / 1000.0) / _shuntResistor;  // I = V/R
    }
    
    int16_t value = (int16_t)readRegister(INA226_REG_CURRENT);
    return value * _currentLSB;  // Return in Amps
}

float INA226::getPower() {
    if (_powerLSB == 0) {
        // Not calibrated, calculate from voltage and current
        return getBusVoltage() * getCurrent();
    }
    
    uint16_t value = readRegister(INA226_REG_POWER);
    return value * _powerLSB;  // Return in Watts
}

uint16_t INA226::getManufacturerID() {
    return readRegister(INA226_REG_MANUFACTURER_ID);
}

uint16_t INA226::getDieID() {
    return readRegister(INA226_REG_DIE_ID);
}

void INA226::setAveraging(uint16_t avg) {
    uint16_t config = getConfig();
    config &= ~0x0E00;  // Clear AVG bits
    config |= (avg & 0x0E00);
    setConfig(config);
}

void INA226::setBusVoltageConversionTime(uint16_t time) {
    uint16_t config = getConfig();
    config &= ~0x01C0;  // Clear VBUS CT bits
    config |= (time & 0x01C0);
    setConfig(config);
}

void INA226::setShuntVoltageConversionTime(uint16_t time) {
    uint16_t config = getConfig();
    config &= ~0x0038;  // Clear VSHUNT CT bits
    config |= (time & 0x0038);
    setConfig(config);
}

void INA226::setMode(uint16_t mode) {
    uint16_t config = getConfig();
    config &= ~0x0007;  // Clear mode bits
    config |= (mode & 0x0007);
    setConfig(config);
}

bool INA226::readAll(float *voltage, float *current, float *power) {
    if (!_initialized) {
        return false;
    }
    
    // Read bus voltage
    uint16_t vBus = readRegister(INA226_REG_BUS_VOLTAGE);
    *voltage = vBus * 0.00125;
    
    if (_currentLSB == 0) {
        // Not calibrated - calculate from shunt voltage
        int16_t vShunt = (int16_t)readRegister(INA226_REG_SHUNT_VOLTAGE);
        float shuntVoltage_mV = vShunt * 0.0025;
        *current = (shuntVoltage_mV / 1000.0) / _shuntResistor;
        *power = (*voltage) * (*current);
    } else {
        // Calibrated - read from registers
        int16_t cur = (int16_t)readRegister(INA226_REG_CURRENT);
        *current = cur * _currentLSB;
        
        uint16_t pwr = readRegister(INA226_REG_POWER);
        *power = pwr * _powerLSB;
    }
    
    // Ensure non-negative values (measurement noise can cause small negatives)
    if (*current < 0) *current = 0;
    if (*power < 0) *power = 0;
    
    return true;
}

void INA226::writeRegister(uint8_t reg, uint16_t value) {
    _wire->beginTransmission(_address);
    _wire->write(reg);
    _wire->write((value >> 8) & 0xFF);  // MSB first
    _wire->write(value & 0xFF);          // LSB
    _wire->endTransmission();
}

uint16_t INA226::readRegister(uint8_t reg) {
    _wire->beginTransmission(_address);
    _wire->write(reg);
    _wire->endTransmission(false);  // Repeated start
    
    _wire->requestFrom(_address, (uint8_t)2);
    
    uint16_t value = 0;
    if (_wire->available() == 2) {
        value = _wire->read() << 8;  // MSB first
        value |= _wire->read();       // LSB
    }
    
    return value;
}
