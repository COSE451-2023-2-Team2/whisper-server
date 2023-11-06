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