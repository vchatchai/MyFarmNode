#ifndef NSV_HPP
#define NSV_HPP

#include<string>



class Storage 
{
    protected: 
        static Storage* instance;
        Storage(){

        }
    private: 
        

    public:
        Storage( Storage &other) = delete;
        void operator=(const Storage &) = delete; 
        static Storage *getInstance();

        std::string read(std::string key);
        void write(char* key, char* value);
};


#endif
