
#include "driver/i2c.h"
#include "SHT3XSensor.hpp"

SHT3XSensor *SHT3XSensor::instance = nullptr;
sht3x_sensor_t *SHT3XSensor::sensor = nullptr;

SHT3XSensor::SHT3XSensor()
{
    humidity = 0.0f;
    temperature = 0.0f;

}

SHT3XSensor::SHT3XSensor(std::string name)
{

    // ESP_ERROR_CHECK(i2c_master_init());

    int i2c_master_port = 0;
    i2c_config_t conf ;
    // = {
    //     .mode = I2C_MODE_MASTER,
    //     .sda_io_num = I2C_SDA_PIN, // select GPIO specific to your project
    //     .sda_pullup_en = GPIO_PULLUP_ENABLE,
    //     .scl_io_num = I2C_SCL_PIN, // select GPIO specific to your project
    //     .scl_pullup_en = GPIO_PULLUP_ENABLE,
    //     // .master.clk_speed = I2C_FREQ, // select frequency specific to your project
    //     // .clk_flags = 0,          /*!< Optional, you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here. */
    // };
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_SDA_PIN; // select GPIO specific to your project
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_SCL_PIN; // select GPIO specific to your project
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_FREQ;
    

    esp_err_t err = i2c_param_config(i2c_master_port, &conf);
    ESP_ERROR_CHECK(err);

    // Init I2C bus interfaces at which SHT3x sensors are connected
    // (different busses are possible).
    i2c_init(I2C_BUS, I2C_SCL_PIN, I2C_SDA_PIN, I2C_FREQ);

    // Create the sensors, multiple sensors are possible.
    if ((this->sensor = sht3x_init_sensor(I2C_BUS, SHT3x_ADDR_1)))
    {

        /*
         * User task that fetches latest measurement results of sensor every 2
         * seconds. It starts the SHT3x in periodic mode with 1 measurements per
         * second (*sht3x_periodic_1mps*).
         */
        auto sensorTask = [](void *pvParameters)
        {
            float temperature;
            float humidity;

            // Start periodic measurements with 1 measurement per second.
            sht3x_start_measurement(sensor, sht3x_periodic_1mps, sht3x_high);

            // Wait until first measurement is ready (constant time of at least 30 ms
            // or the duration returned from *sht3x_get_measurement_duration*).
            vTaskDelay(sht3x_get_measurement_duration(sht3x_high));

            TickType_t last_wakeup = xTaskGetTickCount();

            while (1)
            {
                // Get the values and do something with them.
                if (sht3x_get_results(sensor, &temperature, &humidity))
                    printf("%.3f SHT3x Sensor: %.2f °C, %.2f %%\n",
                           (double)sdk_system_get_time() * 1e-3, temperature, humidity);

                // Wait until 2 seconds (cycle time) are over.
                vTaskDelayUntil(&last_wakeup, 2000 / portTICK_PERIOD_MS);
            }
        };

        // Create a user task that uses the sensors.
        xTaskCreate(sensorTask, "sensorTask", TASK_STACK_DEPTH, NULL, 2, 0);
    }
    else
        printf("Could not initialize SHT3x sensor\n");

    // That's it.

    
}

SHT3XSensor *SHT3XSensor::getInstance()
{
    if (instance)
    {
        instance = new SHT3XSensor();
    }

    return instance;
}

float SHT3XSensor::getTemperature()
{
    return temperature;
}

float SHT3XSensor::getHumidity()
{
    return humidity;
}
