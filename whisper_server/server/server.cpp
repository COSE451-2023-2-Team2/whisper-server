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

class UserInfo {
public:
    std::string email;
    std::string id;
    std::string pw;
    virtual void print(){
        std::cout << "User information: " << email << id << pw << std::endl;
    }
};
map<string,int> index_list;

string dir;
void saveData(string email, string id, string pw) {
    //a = append, w = overwrite
    FILE* f = fopen(dir.c_str(), "a");
    if(f == NULL) {
        printf("cant save data");
        return;
    }
    //write some data (as integer) to file
    // \n is placed in the beginning rather than the end so that when
    // we append it will not append on the previous password's position
    fprintf(f, "\n%s %s %s", email.c_str(), id.c_str(), pw.c_str());
    fclose(f);
}

void loadData(Hashmap &users, Hashmap &email) {
    string data1, data2, data3;
    FILE* f = fopen(dir.c_str(), "r");
    if(f == NULL) {
        printf("cant open file");
        return;
    }
    //load data from file, fscanf return the number of read data
    //so if we reach the end of file (EOF) it return 0 and we end
    while(fscanf(f, "%s %s %s", &data1[0], &data2[0], &data3[0]) == 3) {
        printf("data1 = %s data2 = %s data3 = %s\n", data1.c_str(), data2.c_str(), data3.c_str());
        users.put(data2.c_str(), data3.c_str());
        email.put(data1.c_str(), data2.c_str());
    }

    fclose(f);

}