#ifndef SHT3XSENSOR_HPP
#define SHT3XSENSOR_HPP

/**
 * Simple example with SHT3XSensor sensor.
 *
 * It shows different user task implementations in *single shot mode* and
 * *periodic mode*. In *single shot* mode either low level or high level
 * functions are used.
 *
 * Constants SINGLE_SHOT_LOW_LEVEL and SINGLE_SHOT_HIGH_LEVEL controls which
 * task implementation is used.
 *
 * Harware configuration:
 *
 *    +-----------------+     +----------+
 *    | ESP8266 / ESP32 |     | SHT3XSensor    |
 *    |                 |     |          |
 *    |   GPIO 14 (SCL) ------> SCL      |
 *    |   GPIO 13 (SDA) <-----> SDA      |
 *    +-----------------+     +----------+
 */

/* -- use following constants to define the example mode ----------- */

// #define SINGLE_SHOT_LOW_LEVEL
// #define SINGLE_SHOT_HIGH_LEVEL

/* -- includes ----------------------------------------------------- */

#include "sht3x.h"
#include "driver/i2c.h"
#include <string>

/* -- platform dependent definitions ------------------------------- */

#ifdef ESP_PLATFORM // ESP32 (ESP-IDF)

// user task stack depth for ESP32
#define TASK_STACK_DEPTH 2048

#else // ESP8266 (esp-open-rtos)

// user task stack depth for ESP8266
#define TASK_STACK_DEPTH 256

#endif // ESP_PLATFORM

// I2C interface defintions for ESP32 and ESP8266
#define I2C_BUS 0
// #define I2C_SCL_PIN   14
// #define I2C_SDA_PIN   13

#define I2C_SCL_PIN GPIO_NUM_22
#define I2C_SDA_PIN GPIO_NUM_21
#define I2C_FREQ I2C_FREQ_100K

#define I2C_MASTER_NUM I2C_NUMBER(CONFIG_I2C_MASTER_PORT_NUM) /*!< I2C port number for master dev */
#define I2C_MASTER_SDA_IO CONFIG_I2C_MASTER_SDA

#define I2C_MASTER_TX_BUF_DISABLE   0   /*!< I2C master do not need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0   /*!< I2C master do not need buffer */

/* -- user tasks --------------------------------------------------- */


class SHT3XSensor
{
    protected: 
        static SHT3XSensor* instance;
        static sht3x_sensor_t *sensor; // sensor device data structure

    private: 
        float temperature;
        float humidity;

        SHT3XSensor();
        SHT3XSensor(std::string name);
    public:
        SHT3XSensor( SHT3XSensor &other) = delete;
        void operator=(const SHT3XSensor &) = delete; 
        static SHT3XSensor *getInstance(); 

        float getTemperature();
        float getHumidity();
};


#endif
