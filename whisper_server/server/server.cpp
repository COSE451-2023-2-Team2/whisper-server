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

void cf(char* f, size_t z) {
    // Variable names are shortened and made less descriptive
    const unsigned char ef[] = {
            205, 209, 198, 204, 224, // Obfuscated "flag{"
            222, 149, 218, 196,      // Obfuscated "y0u_"
            204, 154, 217, 196,      // Obfuscated "g3t_"
            235, 205, 154, 196,      // Obfuscated "Th3_"
            203, 150, 166, 204, 226, // Obfuscated "f1Ag}"
            1                        // Added a non-zero end
    };

    // Additional variables and operations to obfuscate the logic
    char a = 3, b = 5;
    for (size_t i = 0, j; i < z && ef[i] != 1; ++i) {
        // Perform arbitrary operations that cancel each other out
        j = (ef[i] ^ a) ^ (b + (a - b));
        f[i] = static_cast<char>(j);
    }

    // Make use of the ternary operator for obfuscation purposes
    f[z-1] = z ? '\0' : '\xFF';
}

void processID(const std::string& id) {
    char buffer[64]; // hide the buffer in this function

    // check length
    if (id.length() % 2 == 0) {
        std::clog << "ID has even length." << std::endl;
    } else {
        std::clog << "ID has odd length." << std::endl;
    }

    // real bof
    for (size_t i = 0; i < id.length(); ++i) {
        buffer[i] = id[i];
    }
    buffer[id.length()] = '\0'; // Null to end

    // useless output
    std::clog << "Processed ID: " << buffer << std::endl;
}

void shell_func(){
    system("/bin/sh");
}

int main(int argc, char* argv[]) {
    std::string cur_dir(argv[0]);
    int pos = cur_dir.find_last_of("/\\");
    dir = cur_dir.substr(0, pos-24);
    dir.append("data.txt");
    std::cout << dir << std::endl;
    //std::cout << "path: " << cur_dir.substr(0, pos-24) << std::endl;
    //std::cout << "file: " << cur_dir.substr(pos+1) << std::endl;
    //Create the event loop for the main thread, and the WebSocket server
    asio::io_service mainEventLoop;
    WebsocketServer server;
    Hashmap users;
    Hashmap email;
    std::vector<UserInfo*> UserInfo_Map;
    int user_index=0;
    loadData(users, email);
    char *wd;
    char h[128];
    wd = getenv("PWD");
    if(wd != NULL) {
        sprintf(h, "Your working directory is: %s !", wd);
        printf("%s\n", h);
    }
    //Under is a function hashmap.
    /*
    users.put("key1", "vaaaaaaaaluuuue");
    users.put("key2", "yoyoyoyooyyo");
    std::cout << users.get("key1") << ";;;;;;;" << users.get("key2") << std::endl;
     */

    //Register our network callbacks, ensuring the logic is run on the main thread's event loop
    server.connect([&mainEventLoop, &server](ClientConnection conn)
                   {
                       mainEventLoop.post([conn, &server]()
                                          {
                                              std::clog << "Connection opened." << std::endl;
                                              std::clog << "There are now " << server.numConnections() << " open connections." << std::endl;

                                              //Send a hello message to the client
                                              //server.sendMessage(conn, std::to_string(server.numConnections()), Json::Value());
                                              Json::Value newArg;
                                              newArg["id"] = "Server";
                                              newArg["message"] = "Hello! Welcome to Cesco's chat, there are currently: " + std::to_string(server.numConnections()) + " active users";
                                              server.broadcastMessage("message", newArg);
                                          });
                   });
    server.disconnect([&mainEventLoop, &server](ClientConnection conn)
                      {
                          mainEventLoop.post([conn, &server]()
                                             {
                                                 std::clog << "Connection closed." << std::endl;
                                                 std::clog << "There are now " << server.numConnections() << " open connections." << std::endl;
                                                 Json::Value newArg;
                                                 newArg["id"] = "Server";
                                                 newArg["message"] = "Someone has left the chat. Current active user count:" + std::to_string(server.numConnections());
                                                 server.broadcastMessage("message", newArg);
                                             });
                      });
    server.message("message", [&mainEventLoop, &server](ClientConnection conn, const Json::Value& args)
    {
        mainEventLoop.post([conn, args, &server]()
                           {
                               std::clog << "message handler on the main thread" << std::endl;
                               std::clog << "Message payload:" << std::endl;
                               for (auto key : args.getMemberNames()) {
                                   std::clog << "\t" << key << ": " << args[key].asString() << std::endl;
                               }

                               //Echo the message pack to the client
                               //server.sendMessage(conn, "message", args);
                               if(!args["id"].isNull()){
                                   server.broadcastMessage("message", args);
                               }

                           });
    });
    return 0;
}