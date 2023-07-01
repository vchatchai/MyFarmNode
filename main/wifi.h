#ifndef WIFI_H
#define WIFI_H

class wifi
{ 
protected:
    static wifi* instance ;
    wifi() { 

    }
public:   

    wifi(wifi &other) = delete;

    void operator=(const wifi &) = delete;
    static wifi* getInstance(); 

    void scan();
    void connect();
    void disconnect();
};

#endif
