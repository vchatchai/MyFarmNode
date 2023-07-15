#include "MQTT.hpp"

#include "mqtt_client.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_system.h"
#include "mqtt_client.h"
#include "freertos/timers.h"

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "cJSON.h"

static const char *TAG = "MQTT";
// static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
MQTT *MQTT::instance = nullptr;
// esp_mqtt_client_handle_t *MQTT::client;
// auto xCreatedEventGroup = xEventGroupCreate();
map<string, QueueHandle_t> MQTT::recieveQueue;
esp_mqtt_client_handle_t MQTT::client;
MQTT *MQTT::getInstance()
{
    if (instance)
    {
        instance = new MQTT();
    }
    return instance;
}

esp_mqtt_client_config_t getConfig(esp_mqtt_client_config_t mqtt_cfg) {
    mqtt_cfg.broker.address.uri = CONFIG_BROKER_URL; 
    return mqtt_cfg;

}

void MQTT::connect()
{
    ESP_LOGI(TAG, "MQTT Connect");
    // const esp_mqtt_client_config_t mqtt_cfg = {
    //     .broker.address.uri = CONFIG_BROKER_URL,
    // };
        esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg = getConfig(mqtt_cfg);

    // mqtt_cfg.broker.address.uri = CONFIG_BROKER_URL;
   
    // esp_mqtt_event_handle_t
#if CONFIG_BROKER_URL_FROM_STDIN
    char line[128];

    if (strcmp(mqtt_cfg.uri, "FROM_STDIN") == 0)
    {
        int count = 0;
        printf("Please enter url of mqtt broker\n");
        while (count < 128)
        {
            int c = fgetc(stdin);
            if (c == '\n')
            {
                line[count] = '\0';
                break;
            }
            else if (c > 0 && c < 127)
            {
                line[count] = c;
                ++count;
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        mqtt_cfg.uri = line;
        printf("Broker url: %s\n", line);
    }
    else
    {
        ESP_LOGE(TAG, "Configuration mismatch: wrong broker url");
        abort();
    }
#endif /* CONFIG_BROKER_URL_FROM_STDIN */
    // xSemaphoreTake(xMutex, portMAX_DELAY);

    client = esp_mqtt_client_init(&mqtt_cfg);

    // esp_mqtt_client_register_event(client, -1, this->mqtt_event_handler, client);
    esp_mqtt_client_register_event(client, (esp_mqtt_event_id_t)ESP_EVENT_ANY_ID, mqtt_event_handler, client);

    esp_mqtt_client_start(client);
}

uint32_t value = 0;

// static esp_err_t MQTT::mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
// {
//     // static char *NODE_NAME = "valve01";
// // static char *NODE_STATUS =  "/topic/node/valve01/status";
// // static string *NODE_CONTROL = "/topic/node/valve01/control";
//     // esp_mqtt_client_handle_t client = event->client;
//     int msg_id = -1;
//     // your_context_t *context = event->context;

//     // const TickType_t xBlockTime = pdMS_TO_TICKS(200);
//     switch (event->event_id)
//     {
//     case MQTT_EVENT_CONNECTED:
//         // ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
//         // msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "data_3", 0, 1, 0);
//         // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

//         // msg_id = esp_mqtt_client_subscribe(client, NODE_CONTROL, 0);
//         ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

//         // msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
//         // ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

//         // msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
//         // ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
//         break;
//     case MQTT_EVENT_DISCONNECTED:
//         ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
//         break;

//     case MQTT_EVENT_SUBSCRIBED:
//         ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
//         // msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);

//         // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

//         break;
//     case MQTT_EVENT_UNSUBSCRIBED:
//         ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
//         break;
//     case MQTT_EVENT_PUBLISHED:
//         ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
//         break;
//     case MQTT_EVENT_DATA:
//         ESP_LOGI(TAG, "MQTT_EVENT_DATA %s\r\n",event->data);
//         // printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
//         // printf("DATA=%.*s\r\n", event->data_len, event->data);
//         // value++;
//         // xQueueSend(xQueue, &event->data, xBlockTime);
//         break;
//     case MQTT_EVENT_ERROR:
//         ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
//         break;
//     default:
//         ESP_LOGI(TAG, "Other event id:%d", event->event_id);
//         break;
//     }
//     return ESP_OK;
// }
const TickType_t xBlockTime = pdMS_TO_TICKS(200); 
void MQTT::mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{

    // ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    // mqtt_event_handler_cb(event_data);
    // static char *NODE_NAME = "valve01";
    // static char *NODE_STATUS =  "/topic/node/valve01/status";
    // static string *NODE_CONTROL = "/topic/node/valve01/control";
    // esp_mqtt_client_handle_t client = event->client;
    // int msg_id = -1;
    // your_context_t *context = event->context;
    esp_mqtt_client_handle_t client = (esp_mqtt_client_handle_t)handler_args;
    // const TickType_t xBlockTime = pdMS_TO_TICKS(200);
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    switch (event->event_id)
    {
    case MQTT_EVENT_BEFORE_CONNECT:

        break;
    case MQTT_EVENT_CONNECTED:
        // ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        // msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "data_3", 0, 1, 0);
        // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
      

        for (const auto &[key, xQueue] : MQTT::recieveQueue)
        {
            ESP_LOGI(TAG, "subscribe TOPIC: %s", key.c_str());


            int msg_id = esp_mqtt_client_subscribe(client, key.c_str(), 1); 
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        };



        // msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
        // ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        // msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
        // ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        // msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);

        // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
    {
        // ESP_LOGI(TAG, "MQTT_EVENT_DATA TOPIC: %s \r\n", event->topic);
        // ESP_LOGI(TAG, "MQTT_EVENT_DATA DATA: %s\r\n", event->data);
        // std::string(event->data, event->data_len);
        // auto dataString = std::string(event->data, event->data_len);
        // std::string topicString = std::string(event->topic, event->topic_len);
        // char* value = event->topic;

        ESP_LOGI(TAG, "MQTT TOPIC=%.*s\r\n", event->topic_len, event->topic);
        ESP_LOGI(TAG, "MQTT_EVENT_DATA=%.*s\r\n", event->data_len, event->data);
        // ESP_LOGD(TAG, "Data for topic %s:", topicString.data());
        // ESP_LOG_BUFFER_HEXDUMP(TAG, dataString.data(), dataString.length(), ESP_LOG_VERBOSE);
        std::string topicString(event->topic_len, *event->topic);
        std::string dataString(event->data_len, *event->data);

        // printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        // printf("DATA=%.*s\r\n", event->data_len, event->data);

        // std::format("TETS");
        char buffer[event->topic_len];
        sprintf(buffer, "%.*s", event->topic_len, event->topic);

        // auto queue = MQTT::recieveQueue[event->topic];
        // xQueueSend( MQTT::recieveQueue[event->topic], &event->data, xBlockTime);

        ESP_LOGI(TAG, "MQTT----TOPIC=%s\r\n", buffer);
        for (const auto &[key, xQueue] : MQTT::recieveQueue)
        {
            // ESP_LOGI(TAG, "compare subscribe TOPIC: %s : %s", key.c_str(), topicString.c_str());
            if (key == buffer)
            {
                char bufferData[event->data_len];
                sprintf(bufferData, "%.*s", event->data_len, event->data);
                printf("Equal %s \r\n", bufferData);
                cJSON *root = cJSON_Parse(bufferData);


                xQueueSend(xQueue, &root, xBlockTime);
            }else { 

                printf("not Equal \r\n");
            }
        };
        // value++;
    }
    break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    case MQTT_EVENT_DELETED:
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

// static char *NODE_STATUS =  "/device/node/valve01/status";
int MQTT::publish(string &topic, char *message, int retain)
{
    // esp_mqtt_client_handle_t client = client;
    int msg_id = esp_mqtt_client_publish(client, topic.c_str(), message, 0, 0, 0);
    // int msg_id = esp_mqtt_client_publish(client, NODE_STATUS, NODE_STATUS, 0, 0, 0);
    return msg_id;
}

void MQTT::subscribe(string topic, QueueHandle_t& queue)
{

    // xQueueSend(queue, "Test subscribe", xBlockTime);
    int msg_id = esp_mqtt_client_subscribe(client, topic.c_str(), 1);
    this->recieveQueue.insert(pair<string, QueueHandle_t>(topic, queue));
    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
}