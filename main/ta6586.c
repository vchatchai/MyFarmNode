#include "ta6586.h"


#include "esp_system.h"
#include "driver/gpio.h"



void initial_drive(gpio_num_t I1, gpio_num_t I2) { 
    // gpio_reset_pin(I1);
    // gpio_reset_pin(I2);
    gpio_set_direction(I1, GPIO_MODE_OUTPUT);
    gpio_set_direction(I2, GPIO_MODE_OUTPUT);
    // gpio_pullup_dis(I1);
    // gpio_pullup_dis(I2);

}


/**
 * input PORT I1, I2
 * output PORT 
 *   5,6 : OPEN
 *   7,8 : OPEN
 * */
void openStatus_drive(gpio_num_t I1, gpio_num_t I2) { 	
    gpio_set_level(I1,0);
    gpio_set_level(I2,0);

}

/**
 * input PORT I1, I2
 * output PORT 
 *   5,6 : L
 *   7,8 : L
 * */
void LOWLOWstatus_drive(gpio_num_t I1, gpio_num_t I2) {
    gpio_set_level(I1,1);
    gpio_set_level(I2,1);

}

/**
 * input PORT I1, I2
 * output PORT 
 *   5,6 : L
 *   7,8 : H
 * */
void LOWHIGHstatus_drive(gpio_num_t I1,gpio_num_t I2) { 
    gpio_set_level(I1,0);
    gpio_set_level(I2,1);

}

/**
 * input PORT I1, I2
 * output PORT 
 *   5,6 : H
 *   7,8 : L
 * */
void HIGHLOWstatus_drive(gpio_num_t I1,gpio_num_t I2) { 
    gpio_set_level(I1,1);
    gpio_set_level(I2,0);

}





