#include "WebsocketServer.h"

#include <iostream>
#include <thread>
#include <asio/io_service.hpp>
#include <unordered_map>

//The port number the WebSocket server listens on
#define PORT_NUMBER 8080

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

int main(int argc, char* argv[])
{
	//Create the event loop for the main thread, and the WebSocket server
	asio::io_service mainEventLoop;
	WebsocketServer server;
    Hashmap users;
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
            server.broadcastMessage("Hello! Welcome to Cesco's chat, there are currently: " + std::to_string(server.numConnections()) + " active users", Json::Value());
		});
	});
	server.disconnect([&mainEventLoop, &server](ClientConnection conn)
	{
		mainEventLoop.post([conn, &server]()
		{
			std::clog << "Connection closed." << std::endl;
			std::clog << "There are now " << server.numConnections() << " open connections." << std::endl;
            server.broadcastMessage("Someone has left the chat. Current active user count: " + std::to_string(server.numConnections()), Json::Value());
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
            //Todo: Loop through all the connected users and send the messages to them. Change it to broadcast message
            server.broadcastMessage("message", args);

		});
	});
    server.message("login", [&mainEventLoop, &server](ClientConnection conn, const Json::Value& args)
    {
        mainEventLoop.post([conn, args, &server]()
        {
            for (auto key : args.getMemberNames()) {
                std::clog << "\t" << key << ": " << args[key].asString() << std::endl;
            }
            std::clog << args["id"].asString() << std::endl;
            std::clog << args["pw"].asString() << std::endl;
            //todo write functionality for chekiking if the login exsits. Perhaps a hashmap.
            server.broadcastMessage("login", args);
        });
    });

    server.message("register", [&mainEventLoop, &server, &users](ClientConnection conn, const Json::Value& args)
    {
        mainEventLoop.post([conn, args, &server, &users]()
        {
            for (auto key : args.getMemberNames()) {
                std::clog << "\t" << key << ": " << args[key].asString() << std::endl;
            }
            std::clog << args["email"].asString() << std::endl;
            std::clog << args["id"].asString() << std::endl;
            std::clog << args["pw"].asString() << std::endl;
            //todo write functionality for chekiking if the id already exists on the hashmap. And saving the values there.

            if(users.checkIfKeyExist(args["id"].asString())){
                Json::Value newArg;
                newArg.append("Error, this id is taken");
                server.sendMessage(conn, "error", newArg);
                server.broadcastMessage("register", newArg);
            }
            server.broadcastMessage("register", args);
        });
    });
	
	//Start the networking thread
	std::thread serverThread([&server]() {
		server.run(PORT_NUMBER);
	});
	
	//Start a keyboard input thread that reads from stdin
	std::thread inputThread([&server, &mainEventLoop]()
	{
		string input;
		while (1)
		{
			//Read user input from stdin
			std::getline(std::cin, input);
			
			//Broadcast the input to all connected clients (is sent on the network thread)
			Json::Value payload;
			payload["input"] = input;
			server.broadcastMessage("userInput", payload);
			
			//Debug output on the main thread
			mainEventLoop.post([]() {
				std::clog << "User input debug output on the main thread" << std::endl;
			});
		}
	});
	
	//Start the event loop for the main thread
	asio::io_service::work work(mainEventLoop);
	mainEventLoop.run();
	
	return 0;
}
