#ifndef MQTT_HPP
#define MQTT_HPP

#include "mqtt_client.h"
#include <string>
#include "esp_log.h"
#include "esp_event.h"
#include "esp_system.h"
#include "freertos/timers.h"
#include "esp_log.h"
#include "freertos/queue.h"
#include<map>

using namespace std;
class MQTT
{
protected:
    static MQTT *instance;
    static string NODE_CONTROL;
    static esp_mqtt_client_handle_t client;
    static map<string,QueueHandle_t> recieveQueue;
    MQTT()
    {
    }
    // static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

public:
    MQTT(MQTT &other) = delete;

    void operator=(const MQTT &) = delete;
    static MQTT *getInstance();
    static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

    int publish(string& topic, char* message, int retain);
    void subscribe(string topic,QueueHandle_t& queue);
    void connect();
};

#endif