/*
 * Part of esp-open-rtos
 * Copyright (C) 2016 Jonathan Hartsuiker (https://github.com/jsuiker)
 * BSD Licensed as described in the file LICENSE
 *
 */



// #include <stdint.h>
// #include <stdbool.h>
#include "driver/gpio.h"



#ifndef __TA6586__
#define __TA6586__


#ifdef __cplusplus
extern "C" {
#endif



void initial_drive(gpio_num_t I1, gpio_num_t I2);


/**
 * input PORT I1, I2
 * output PORT 
 *   5,6 : OPEN
 *   7,8 : OPEN
 * */
void openStatus_drive(gpio_num_t I1, gpio_num_t I2) ;

/**
 * input PORT I1, I2
 * output PORT 
 *   5,6 : L
 *   7,8 : L
 * */
void LOWLOWstatus_drive(gpio_num_t I1, gpio_num_t I2);

/**
 * input PORT I1, I2
 * output PORT 
 *   5,6 : L
 *   7,8 : H
 * */
void LOWHIGHstatus_drive(gpio_num_t I1,gpio_num_t I2);

/**
 * input PORT I1, I2
 * output PORT 
 *   5,6 : H
 *   7,8 : L
 * */
void HIGHLOWstatus_drive(gpio_num_t I1,gpio_num_t I2) ;



#ifdef __cplusplus
}
#endif

#endif  // __DHT_H__
