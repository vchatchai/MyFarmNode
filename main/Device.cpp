#include "Device.hpp"
#include "MQTT.hpp"
#include "SHT3XSensor.hpp"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "cJSON.h"

#include "driver/gpio.h"
#include "ta6586.h"

#define MQTT_AUTO_RELOAD_TIMER_PERIOD pdMS_TO_TICKS(3000)

static const char *TAG = "Device";

void Device::mqttPublishTask()
{

    auto mqttPublishTask = [](void *pvParameters)
    {
        int count = 0;
        Device *d = (Device *)pvParameters;
        string topic = d->topic + "/status"; //"/node/device/test";
        esp_err_t res;

	    res =  gpio_set_pull_mode(d->pinStatusInput, GPIO_PULLUP_ONLY);
	    res =  gpio_set_pull_mode(d->pinStatusOutput, GPIO_PULLUP_ONLY);
        while (true)
        {
            count++;
            d->sensorTicker();

            cJSON *root;
            root = cJSON_CreateObject();

            cJSON_AddStringToObject(root, "NodeID", d->nodeId);
            cJSON_AddNumberToObject(root, "Temperature", d->senserTemperature);
            cJSON_AddNumberToObject(root, "Humidity", d->senserHumidity);
            cJSON_AddStringToObject(root, "Name", d->name);
            cJSON_AddNumberToObject(root, "Switch", d->outputStatus);
            cJSON_AddNumberToObject(root, "Power", d->controlStatus);
            cJSON_AddNumberToObject(root, "Tick", count);
            // cJSON_AddNumberToObject(root, "SensorTick", sensorTick);

            char *out = cJSON_Print(root);

            int msg_id = MQTT::getInstance()->publish(topic, out, 0);
            ESP_LOGI(TAG, "msg id: %d xTaskGetTickCount %d : %d", msg_id, xTaskGetTickCount(), count);

            cJSON_Delete(root);
            free(out);

            vTaskDelay(MQTT_AUTO_RELOAD_TIMER_PERIOD);
        }
    };
    xTaskCreate(
        mqttPublishTask,              /* Function that implements the task. */
        "mqttPublish",                /* Text name for the task. */
        configMINIMAL_STACK_SIZE * 8, /* Stack size in words, not bytes. */
        (void *)this,                 /* Parameter passed into the task. */
        tskIDLE_PRIORITY,             /* Priority at which the task is created. */
        NULL);
}

void Device::mqttRecieveTask()
{
    auto mqttRecieveTask = [](void *pvParameters)
    {
        Device *d = (Device *)pvParameters;
        QueueHandle_t xQueue = d->xQueue;

        MQTT::getInstance()->subscribe(d->topic + "/control", xQueue);
        // gpio_set_direction(d->pinControlOutput1, GPIO_MODE_OUTPUT);
        // gpio_set_direction(d->pinControlOutput2, GPIO_MODE_OUTPUT);
        initial_drive(d->pinControlOutput1,d->pinControlOutput2);
        LOWLOWstatus_drive(d->pinControlOutput1,d->pinControlOutput2); 
        // gpio_set_level(d->pinControlOutput1, 0);
        // gpio_set_level(d->pinControlOutput2, 0);
        // openStatus_drive(d->pinControlOutput1,d->pinControlOutput2);
        while (true)
        { 
            cJSON *root;
            BaseType_t result = xQueueReceive(xQueue, &root, portMAX_DELAY);
            if (result == pdTRUE)
            {
 
                
                if (root == NULL)
                {
                    const char *error_ptr = cJSON_GetErrorPtr();
                    if (error_ptr != NULL)
                    {
                        ESP_LOGE(TAG, "%s Error before: %s", d->name,  error_ptr);
                    }
                    else
                    {
                        ESP_LOGE(TAG, "Error before");
                    }
                }
                else
                {
                    cJSON* power = cJSON_GetObjectItem(root, "Power");
                     if (power != NULL)
                    {
                        d->controlStatus = power->valueint;

                        if (d->controlStatus > 0)
                        {

                            ESP_LOGI(TAG, "%s ON", d->name);
                            if (d->type == TypeDevice::PUMP)
                            {
                            ESP_LOGI(TAG, "PUMP ON");
                                // gpio_set_level(d->pinControlOutput1, 1);
                                // gpio_set_level(d->pinControlOutput2, 1);

                                LOWHIGHstatus_drive(d->pinControlOutput1,d->pinControlOutput2);
                                
                            }
                            else
                            {

                            ESP_LOGI(TAG, "VALVE ON");
                                // gpio_set_level(d->pinControlOutput1, 0);
                                // gpio_set_level(d->pinControlOutput2, 1);
                                LOWHIGHstatus_drive(d->pinControlOutput1,d->pinControlOutput2);
                            }
                        }
                        else if (d->controlStatus == 0)
                        {
                            ESP_LOGI(TAG, "%s OFF", d->name);
                            if (d->type == TypeDevice::PUMP)
                            {

                            ESP_LOGI(TAG, "PUMP OFF");
                                // gpio_set_level(d->pinControlOutput1, 0);
                                // gpio_set_level(d->pinControlOutput2, 1);
                                LOWLOWstatus_drive(d->pinControlOutput1,d->pinControlOutput2);
                            }
                            else
                            {

                            ESP_LOGI(TAG, "VALVE OFF");
                                // gpio_set_level(d->pinControlOutput1, 1);
                                // gpio_set_level(d->pinControlOutput2, 0);
                                HIGHLOWstatus_drive(d->pinControlOutput1,d->pinControlOutput2);
                            }
                        }
                    }
                }

                cJSON_Delete(root);
            }
        }
    };

    xTaskCreate(
        mqttRecieveTask,              /* Function that implements the task. */
        "mqttRecieve",                /* Text name for the task. */
        configMINIMAL_STACK_SIZE * 8, /* Stack size in words, not bytes. */
        (void *)this,                 /* Parameter passed into the task. */
        tskIDLE_PRIORITY,             /* Priority at which the task is created. */
        NULL);                        /* Used to pass out the created task's handle. */
}

void Device::sensorTicker()
{
    inputStatus = !gpio_get_level(pinStatusInput);
    outputStatus = !gpio_get_level(pinStatusOutput);

    // senserHumidity = SHT3XSensor::getInstance()->getHumidity();
    // senserTemperature = SHT3XSensor::getInstance()->getTemperature();


    // int switchSensor03 = gpio_get_level(pin_switch03);
    // int switchSensor04 = gpio_get_level(pin_switch04);
    // // ESP_LOGI(TAG, "switchSensor PIN: %d %d %%\n", msg_id,pin_switch, switchSensor);
    ESP_LOGI(TAG, "%s inputStatus PIN: %d %d  ", name,  pinStatusInput, inputStatus);
    ESP_LOGI(TAG, "%s outputStatus PIN: %d %d ", name,  pinStatusOutput, outputStatus);
    // printf("switchSensor PIN: %d %d  \n", pin_switch03, switchSensor03);
    // printf("switchSensor PIN: %d %d  \n", pin_switch04, switchSensor04);
}

Device::Device(string topic, char *name, TypeDevice type, char *nodeId,gpio_num_t pinStatusInput, gpio_num_t pinStatusOutput, gpio_num_t pinControlOutput1, gpio_num_t pinControlOutput2)
{
    this->topic = topic;
    this->name = name;
    this->nodeId = nodeId;
    this->pinStatusInput = pinStatusInput;
    this->pinStatusOutput = pinStatusOutput;
    this->pinControlOutput1 = pinControlOutput1;
    this->pinControlOutput2 = pinControlOutput2;

    // ESP_LOGI(TAG, "Type: %s  ", type);
    this->type = type;
    // this->type = type;
    this->xQueue = xQueueCreate(5, sizeof(esp_mqtt_event_handle_t));
    // Device *d = this;
}
