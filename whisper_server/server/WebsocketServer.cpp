#include "WebsocketServer.h"

#include <algorithm>
#include <functional>
#include <iostream>

//The name of the special JSON field that holds the message type for messages
#define MESSAGE_FIELD "MessageType"

Json::Value WebsocketServer::parseJson(const string& json)
{
    Json::Value root;
    Json::Reader reader;
    reader.parse(json, root);
    return root;
}

string WebsocketServer::stringifyJson(const Json::Value& val)
{
    //When we transmit JSON data, we omit all whitespace
    Json::StreamWriterBuilder wbuilder;
    wbuilder["commentStyle"] = "None";
    wbuilder["indentation"] = "";

    return Json::writeString(wbuilder, val);
}

WebsocketServer::WebsocketServer()
{
    //Wire up our event handlers
    this->endpoint.set_open_handler(std::bind(&WebsocketServer::onOpen, this, std::placeholders::_1));
    this->endpoint.set_close_handler(std::bind(&WebsocketServer::onClose, this, std::placeholders::_1));
    this->endpoint.set_message_handler(std::bind(&WebsocketServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));

    //Initialise the Asio library, using our own event loop object
    this->endpoint.init_asio(&(this->eventLoop));
}
