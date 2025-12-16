/**
 * @file INA226.h
 * @brief INA226 Power Monitor Library for ESP32
 * 
 * Driver library for Texas Instruments INA226 power monitor IC.
 * Measures bus voltage, shunt voltage, current, and power.
 */

#ifndef INA226_H
#define INA226_H

#include <Arduino.h>
#include <Wire.h>

// INA226 Register Addresses
#define INA226_REG_CONFIG           0x00
#define INA226_REG_SHUNT_VOLTAGE    0x01
#define INA226_REG_BUS_VOLTAGE      0x02
#define INA226_REG_POWER            0x03
#define INA226_REG_CURRENT          0x04
#define INA226_REG_CALIBRATION      0x05
#define INA226_REG_MASK_ENABLE      0x06
#define INA226_REG_ALERT_LIMIT      0x07
#define INA226_REG_MANUFACTURER_ID  0xFE
#define INA226_REG_DIE_ID           0xFF

// INA226 Configuration Bits
// Averaging Mode
#define INA226_AVG_1                0x0000
#define INA226_AVG_4                0x0200
#define INA226_AVG_16               0x0400
#define INA226_AVG_64               0x0600
#define INA226_AVG_128              0x0800
#define INA226_AVG_256              0x0A00
#define INA226_AVG_512              0x0C00
#define INA226_AVG_1024             0x0E00

// Bus Voltage Conversion Time
#define INA226_VBUS_140US           0x0000
#define INA226_VBUS_204US           0x0040
#define INA226_VBUS_332US           0x0080
#define INA226_VBUS_588US           0x00C0
#define INA226_VBUS_1100US          0x0100
#define INA226_VBUS_2116US          0x0140
#define INA226_VBUS_4156US          0x0180
#define INA226_VBUS_8244US          0x01C0

// Shunt Voltage Conversion Time
#define INA226_VSHUNT_140US         0x0000
#define INA226_VSHUNT_204US         0x0008
#define INA226_VSHUNT_332US         0x0010
#define INA226_VSHUNT_588US         0x0018
#define INA226_VSHUNT_1100US        0x0020
#define INA226_VSHUNT_2116US        0x0028
#define INA226_VSHUNT_4156US        0x0030
#define INA226_VSHUNT_8244US        0x0038

// Operating Mode
#define INA226_MODE_POWER_DOWN      0x0000
#define INA226_MODE_SHUNT_TRIG      0x0001
#define INA226_MODE_BUS_TRIG        0x0002
#define INA226_MODE_SHUNT_BUS_TRIG  0x0003
#define INA226_MODE_POWER_DOWN2     0x0004
#define INA226_MODE_SHUNT_CONT      0x0005
#define INA226_MODE_BUS_CONT        0x0006
#define INA226_MODE_SHUNT_BUS_CONT  0x0007

// Default Configuration
#define INA226_DEFAULT_CONFIG       (INA226_AVG_16 | INA226_VBUS_1100US | INA226_VSHUNT_1100US | INA226_MODE_SHUNT_BUS_CONT)

/**
 * @class INA226
 * @brief Class for interfacing with INA226 power monitor
 */
class INA226 {
public:
    /**
     * @brief Constructor
     * @param address I2C address of the INA226 (default 0x40)
     */
    INA226(uint8_t address = 0x40);
    
    /**
     * @brief Initialize the INA226
     * @param wire Pointer to TwoWire instance
     * @return true if initialization successful, false otherwise
     */
    bool begin(TwoWire *wire = &Wire);
    
    /**
     * @brief Check if the INA226 is connected
     * @return true if connected, false otherwise
     */
    bool isConnected();
    
    /**
     * @brief Reset the INA226 to default settings
     */
    void reset();
    
    /**
     * @brief Configure the INA226
     * @param config Configuration value (see INA226 configuration bits)
     */
    void setConfig(uint16_t config);
    
    /**
     * @brief Get current configuration
     * @return Current configuration value
     */
    uint16_t getConfig();
    
    /**
     * @brief Set the calibration value for current/power measurements
     * @param shuntResistor Shunt resistor value in Ohms
     * @param maxCurrent Maximum expected current in Amps
     */
    void calibrate(float shuntResistor, float maxCurrent);
    
    /**
     * @brief Get shunt voltage
     * @return Shunt voltage in millivolts
     */
    float getShuntVoltage();
    
    /**
     * @brief Get bus voltage
     * @return Bus voltage in Volts
     */
    float getBusVoltage();
    
    /**
     * @brief Get current
     * @return Current in Amps
     */
    float getCurrent();
    
    /**
     * @brief Get power
     * @return Power in Watts
     */
    float getPower();
    
    /**
     * @brief Get manufacturer ID
     * @return Manufacturer ID (should be 0x5449 for TI)
     */
    uint16_t getManufacturerID();
    
    /**
     * @brief Get die ID
     * @return Die ID (should be 0x2260 for INA226)
     */
    uint16_t getDieID();
    
    /**
     * @brief Set averaging mode
     * @param avg Averaging mode (INA226_AVG_x)
     */
    void setAveraging(uint16_t avg);
    
    /**
     * @brief Set bus voltage conversion time
     * @param time Conversion time (INA226_VBUS_xxxUS)
     */
    void setBusVoltageConversionTime(uint16_t time);
    
    /**
     * @brief Set shunt voltage conversion time
     * @param time Conversion time (INA226_VSHUNT_xxxUS)
     */
    void setShuntVoltageConversionTime(uint16_t time);
    
    /**
     * @brief Set operating mode
     * @param mode Operating mode (INA226_MODE_x)
     */
    void setMode(uint16_t mode);
    
    /**
     * @brief Read all values at once (more efficient)
     * @param voltage Pointer to store bus voltage (V)
     * @param current Pointer to store current (A)
     * @param power Pointer to store power (W)
     * @return true if read successful
     */
    bool readAll(float *voltage, float *current, float *power);
    
private:
    uint8_t _address;
    TwoWire *_wire;
    float _currentLSB;
    float _powerLSB;
    float _shuntResistor;
    bool _initialized;
    
    /**
     * @brief Write a 16-bit value to a register
     * @param reg Register address
     * @param value Value to write
     */
    void writeRegister(uint8_t reg, uint16_t value);
    
    /**
     * @brief Read a 16-bit value from a register
     * @param reg Register address
     * @return Register value
     */
    uint16_t readRegister(uint8_t reg);
};

#endif // INA226_H
