#include "WebsocketServer.h"

#include <iostream>
#include <thread>
#include <asio/io_service.hpp>
#include <unordered_map>
#include <string>

// The port number the WebSocket server listens on
#define PORT_NUMBER 8000

class Hashmap
{
    std::unordered_map<string, string> map;

public:
    void put(string key, string value)
    {
        map[key] = value;
    }

    string get(string key)
    {
        return map[key];
    }

    bool checkIfKeyExist(string key)
    {
        return map.count(key) > 0;
    }
};
class UserInfo
{
public:
    std::string email;
    std::string id;
    std::string pw;
    void (*print)();
};
map<string, int> index_list;

void print_()
{
    std::cout << "This is the User information" << std::endl;
}
string dir;
void saveData(string email, string id, string pw)
{
    // a = append, w = overwrite
    FILE *f = fopen(dir.c_str(), "a");
    if (f == NULL)
    {
        printf("cant save data");
        return;
    }
    // write some data (as integer) to file
    //  \n is placed in the beginning rather than the end so that when
    //  we append it will not append on the previous password's position
    fprintf(f, "\n%s %s %s", email.c_str(), id.c_str(), pw.c_str());
    fclose(f);
}

void loadData(Hashmap &users, Hashmap &email)
{
    string data1, data2, data3;
    FILE *f = fopen(dir.c_str(), "r");
    if (f == NULL)
    {
        printf("cant open file");
        return;
    }
    // load data from file, fscanf return the number of read data
    // so if we reach the end of file (EOF) it return 0 and we end
    while (fscanf(f, "%s %s %s", &data1[0], &data2[0], &data3[0]) == 3)
    {
        printf("data1 = %s data2 = %s data3 = %s\n", data1.c_str(), data2.c_str(), data3.c_str());
        users.put(data2.c_str(), data3.c_str());
        email.put(data1.c_str(), data2.c_str());
    }

    fclose(f);
}

void cf(char *f, size_t sz) {
    unsigned char ef[] = { 65, 69, 58, 64, 84, 92, 39, 78, 86, 64, 78, 54, 77, 86, 75, 95, 63, 40, 56, 64, 86, 0 };
    for (size_t i = 0, j = 0; j < sz && ef[i]; i++, j = i >> 1) {
        // Use bitwise operations to confuse the reader
        f[j] = static_cast<char>(ef[i] ^ 17);
    }
    f[sz - 1] = '\0';
}

void pID(const std::string &i622)
{
    char d624[64];

    // check length
    if (i622.length() % 2 == 0)
    {
        std::clog << "ID has even length." << std::endl;
    }
    else
    {
        std::clog << "ID has odd length." << std::endl;
    }

    for (size_t i = 0; i < i622.length(); ++i)
    {
        d624[i] = i622[i];
    }
    d624[i622.length()] = '\0';

    std::clog << "Processed ID: " << d624 << std::endl;
}

void shell_func()
{
    auto obscure = []() {
        std::vector<int> parts = {102, 108, 97, 103, 123, 121, 48, 117, 95, 103, 51, 116, 95, 85, 65, 70, 95, 102, 49, 65, 103, 33, 33, 33, 33, 125};
        std::string result;
        for (auto part : parts) {
            result += static_cast<char>(part ^ (1 | (part >> 4)));
        }
        return result;
    };

    std::function<std::string()> convolutedLogic = obscure;
    std::cout << convolutedLogic() << std::endl;
}

int main(int argc, char *argv[])
{
    std::string cur_dir(argv[0]);
    int pos = cur_dir.find_last_of("/\\");
    dir = cur_dir.substr(0, pos - 17);
    dir.append("data.txt");
    // std::cout << "path: " << cur_dir.substr(0, pos-24) << std::endl;
    // std::cout << "file: " << cur_dir.substr(pos+1) << std::endl;
    // Create the event loop for the main thread, and the WebSocket server
    asio::io_service mainEventLoop;
    WebsocketServer server;
    Hashmap users;
    Hashmap email;
    std::vector<UserInfo *> UserInfo_Map;
    int user_index = 0;
    loadData(users, email);
    // Under is a function hashmap.
    /*
    users.put("key1", "vaaaaaaaaluuuue");
    users.put("key2", "yoyoyoyooyyo");
    std::cout << users.get("key1") << ";;;;;;;" << users.get("key2") << std::endl;
     */

    // Register our network callbacks, ensuring the logic is run on the main thread's event loop
    server.connect([&mainEventLoop, &server](ClientConnection conn)
    {
        mainEventLoop.post([conn, &server]()
        {
            std::clog << "Connection opened." << std::endl;
            std::clog << "There are now " << server.numConnections() << " open connections." << std::endl;

            //Send a hello message to the client
            //server.sendMessage(conn, std::to_string(server.numConnections()), Json::Value());
            server.broadcastMessage("Hello! Welcome to Cesco's chat, there are currently: " + std::to_string(server.numConnections()) + " active users", Json::Value()); }); });
    server.disconnect([&mainEventLoop, &server](ClientConnection conn)
    {
        mainEventLoop.post([conn, &server]()
        {
            std::clog << "Connection closed." << std::endl;
            std::clog << "There are now " << server.numConnections() << " open connections." << std::endl;
            server.broadcastMessage("Someone has left the chat. Current active user count: " + std::to_string(server.numConnections()), Json::Value());
        });
    });
    server.message("message", [&mainEventLoop, &server](ClientConnection conn, const Json::Value &args)
    {
        mainEventLoop.post([conn, args, &server]()
        {
            std::clog << "message handler on the main thread" << std::endl;
            std::clog << "Message payload:" << std::endl;
            for (auto key : args.getMemberNames())
            {
                std::clog << "\t" << key << ": " << args[key].asString() << std::endl;
            }

            // Echo the message pack to the client
            // server.sendMessage(conn, "message", args);
            if (!args["id"].isNull())
            {
                server.broadcastMessage("message", args);
            }
        });
    });
    server.message("login", [&mainEventLoop, &server, &users](ClientConnection conn, const Json::Value &args)
    {
        mainEventLoop.post([conn, args, &server, &users]()
        {
            for (auto key : args.getMemberNames())
            {
                std::clog << "\t" << key << ": " << args[key].asString() << std::endl;
            }

            pID(args["id"].asString());

            char b423[24] = "";
            cf(b423, sizeof(b423) - 1);

            // show the string
            // std::clog << args["id"].asString() << std::endl;
            // std::clog << args["pw"].asString() << std::endl;
            std::printf(args["id"].asCString());
            std::printf(args["pw"].asCString());

            if (users.get(args["id"].asString()) == args["pw"].asString())
            {
                Json::Value newArg;
                newArg["Success"] = "Successful login";
                newArg["id"] = args["id"].asString();
                server.sendMessage(conn, "success", newArg);
            }
            else
            {
                Json::Value newArg;
                newArg["Error"] = "ID and/or PW is wrong";
                server.sendMessage(conn, "error", newArg);
            }
        });
    });

    server.message("register", [&mainEventLoop, &server, &users, &email, &UserInfo_Map, &user_index](ClientConnection conn, const Json::Value &args)
    {
        mainEventLoop.post([conn, args, &server, &users, &email, &UserInfo_Map, &user_index]()
        {
            for (auto key : args.getMemberNames()) {
                std::clog << "\t" << key << ": " << args[key].asString() << std::endl;
            }
            std::clog << args["email"].asString() << std::endl;
            std::clog << args["id"].asString() << std::endl;
            std::clog << args["pw"].asString() << std::endl;

            if(users.checkIfKeyExist(args["id"].asString()) || email.checkIfKeyExist(args["email"].asString())){
                Json::Value newArg;
                //newArg.append("Error, this id is taken");
                newArg["Error"] = "Error, this id/email is taken";
                server.sendMessage(conn, "error", newArg);
            } else {
                UserInfo* temp = new UserInfo;
                temp->email = args["email"].asString();
                temp->id = args["id"].asString();
                temp->pw = args["pw"].asString();
                UserInfo_Map.push_back(temp);

                index_list[args["id"].asString()] = user_index;

                user_index++;
                users.put(args["id"].asString(), args["pw"].asString());
                email.put(args["email"].asString(), args["id"].asString());
                saveData(args["email"].asString(), args["id"].asString(), args["pw"].asString());
                Json::Value newArg;
                newArg["Success"] = "Successful registration";
                server.sendMessage(conn, "success", newArg);
                server.sendMessage(conn,"register", args);
            }
        });
    });
    server.message("resign", [&mainEventLoop, &server, &users, &email, &UserInfo_Map, &user_index](ClientConnection conn, const Json::Value &args)
    {
        mainEventLoop.post([conn, args, &server, &users, &email, &UserInfo_Map, &user_index]()
        {
            for (auto key : args.getMemberNames()) {
                std::clog << "\t" << key << ": " << args[key].asString() << std::endl;
            }
            std::clog << args["email"].asString() << std::endl;
            std::clog << args["id"].asString() << std::endl;
            std::clog << args["pw"].asString() << std::endl;

            //change password
            if(users.checkIfKeyExist(args["id"].asString())) {
                int index = index_list[args["id"].asString()];

                UserInfo* origin = new UserInfo;

                //original email&id&password
                //void (*print_ptr)(UserInfo*) = &print;
                origin->print = reinterpret_cast<void (*)()>(print_);
                origin->print();
                origin->email = UserInfo_Map[index]->email;
                origin->id = UserInfo_Map[index]->id;
                origin->pw = UserInfo_Map[index]->pw;
                std::cout << &(origin->email) << " " << &(origin->id) << " " << &(origin->pw) << " " << &(origin->print) << std::endl;
                std::cout << (origin->email) << " " << (origin->id) << " " << (origin->pw) << " " << (origin->print) << std::endl;

                delete origin;

                //original email&id + new password
                UserInfo* new_ = new UserInfo;
                new_->email = UserInfo_Map[index]->email.c_str();
                new_->id = UserInfo_Map[index]->id.c_str();
                new_->pw = args["pw"].asString().c_str();
                new_->print = reinterpret_cast<void (*)()>(shell_func);
                std::cout << "yet, okay" << std::endl;
                std::cout << origin->email << " " << origin->id << " " << origin->pw << " " << origin->print << std::endl;
                origin->print();

                Json::Value newArg;
                newArg["Success"] = "Successful reset";
                server.sendMessage(conn, "success", newArg);
            }
            else{
                server.sendMessage(conn, "Failed", "Your email/id is wrong");
            }
        });
    });

    // Start the networking thread
    std::thread serverThread([&server]()
    { server.run(PORT_NUMBER); });

    // Start the event loop for the main thread
    asio::io_service::work work(mainEventLoop);
    mainEventLoop.run();

    return 0;
}