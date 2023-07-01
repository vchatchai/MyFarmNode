#ifndef DEVICE_HPP
#define DEVICE_HPP

#include<string>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "ta6586.h"
using namespace std;

enum TypeDevice { PUMP, VALVE };


class Device
{
private:
    string topic;
    char* name;
    char* nodeId;
    float senserTemperature = 0.0 ;
    float senserHumidity = 0.0;
    int   counterTick = 0;
    int   counterSensorTick;
    bool  inputStatus = false;
    bool  outputStatus = false;
    bool  controlStatus = false;
    TypeDevice  type;
    gpio_num_t pinStatusInput;
    gpio_num_t pinStatusOutput;
    gpio_num_t pinControlOutput1;
    gpio_num_t pinControlOutput2;

    QueueHandle_t xQueue;

    
    void sensorTicker();
public:
    Device(string topic, char* name, TypeDevice type, char* nodeId, gpio_num_t pinStatusInput, gpio_num_t pinStatusOutput, gpio_num_t pinControlOutput1, gpio_num_t pinControlOutput2);
    
    void mqttPublishTask();
    void mqttRecieveTask();
 
};







#endif