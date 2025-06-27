#include "../include/Network.hpp"

bool sendMessage(int socketFD, const std::string& message){
    return send(socketFD, message.c_str(), message.size(), 0) >= 0;
}

std::string recieveMessage(int socketFD){
    char buffer[BUFFER_SIZE] = {0};
    int bytesRead = read(socketFD, buffer, BUFFER_SIZE);
    return bytesRead > 0 ? std::string(buffer, bytesRead) : "";
}