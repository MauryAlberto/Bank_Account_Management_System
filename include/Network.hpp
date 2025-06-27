#pragma once

#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>

constexpr int PORT = 6000;
constexpr int BUFFER_SIZE = 4096;
constexpr const char* SERVER_IP = "127.0.0.1";

std::string recieveMessage(int socketFD);
bool sendMessage(int socketFD, const std::string& message);