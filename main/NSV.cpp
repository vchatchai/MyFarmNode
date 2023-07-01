#include "NSV.hpp"

#include "esp_system.h"
#include "nvs.h"
#include <cstring>

#include "esp_log.h"



using namespace std;

static const char *TAG = "NVS";

Storage *Storage::getInstance()
{

    if (instance)
    {

        instance = new Storage();
    }

    return instance;
}

std::string Storage::read(std::string key)
{
    nvs_handle_t read_handle;

    char uri_buffer[254] = {};
    size_t uri_length = 254;

    nvs_open("xasin", NVS_READONLY, &read_handle);
    auto ret = nvs_get_str(read_handle, "mqtt_uri", uri_buffer, &uri_length);
    nvs_close(read_handle);

    if (ret != ESP_OK)
        return "";

    ESP_LOGI(TAG, "Starting from NVS with URI %s", uri_buffer);
    return uri_buffer;
}

void Storage::write(char* key, char* value)
{
    if (strlen(value) > 500)
        return;

    nvs_handle_t write_handle;

    nvs_open("xasin", NVS_READWRITE, &write_handle);
    nvs_set_str(write_handle, key, value);

    nvs_commit(write_handle);
    nvs_close(write_handle);
}
