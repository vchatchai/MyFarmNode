/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_spi_flash.h"
#include "nvs_flash.h"
#include "systeminfo.h"
#include "wifi.h"
#include "esp_log.h"
#include "MQTT.hpp"
#include "Device.hpp"
#include "NSV.hpp"
#include "esp_mac.h"

static const char *TAG = "MAIN";
extern "C"
{
    void app_main();
}

void app_main(void)
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", (unsigned int) esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // string url = Storage::getInstance()->read("");

    gpio_num_t pin_switch01 = GPIO_NUM_23;
    gpio_num_t pin_switch02 = GPIO_NUM_19;
    gpio_num_t pin_switch03 = GPIO_NUM_18;
    gpio_num_t pin_switch04 = GPIO_NUM_5;
    gpio_num_t A1 = GPIO_NUM_16;
    gpio_num_t A2 = GPIO_NUM_17;
    gpio_num_t B1 = GPIO_NUM_4;
    gpio_num_t B2 = GPIO_NUM_0;
 

    initial_drive(B1,B2);
    LOWLOWstatus_drive(B1,B2);

        
    wifi::getInstance()->connect();
    MQTT::getInstance()->connect();

    uint8_t chipid[6];
    
    esp_read_mac(chipid, ESP_MAC_WIFI_STA);
    static char str_chipid[16];
    sprintf(str_chipid, "%x%x%x%x%x%x", chipid[0], chipid[1], chipid[2], chipid[3], chipid[4], chipid[5]);


    Device a("/topic/node/valve03","valve03", TypeDevice::VALVE,str_chipid,pin_switch01, pin_switch02,A1, A2);
    a.mqttPublishTask();
    a.mqttRecieveTask();

    // Device b("/topic/node/valve03","valve03", TypeDevice::VALVE,str_chipid,pin_switch03, pin_switch04,B1, B2);
    // b.mqttPublishTask();
    // b.mqttRecieveTask();


    while(true){
        
        ESP_LOGI(TAG, "Running.."); 
        vTaskDelay(pdMS_TO_TICKS(300000));
    }

}
