#include "WebsocketServer.h"

#include <iostream>
#include <thread>
#include <asio/io_service.hpp>
#include <unordered_map>
#include <string>

//The port number the WebSocket server listens on
#define PORT_NUMBER 8000

class Hashmap {
    std::unordered_map<string, string> map;

public:
    void put(string key, string value) {
        map[key] = value;
    }

    string get(string key) {
        return map[key];
    }

    bool checkIfKeyExist(string key){
        return map.count(key)>0;
    }
};