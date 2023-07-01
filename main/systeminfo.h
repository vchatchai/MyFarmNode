#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include "esp_system.h"

class Systeminfo
{
private:
    esp_chip_info_t chip_info;

public:
    Systeminfo();
    void print();
};

#endif