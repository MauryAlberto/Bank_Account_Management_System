#include "../include/Network.hpp"
#include "../include/Bank.hpp"
#include <iostream>
#include <cstdlib>
#include <thread>
#include <netinet/in.h>
#include <unistd.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void handleClient(int clientSocket){
    while(true){
        std::string request = recieveMessage(clientSocket);
        
        if(request.empty()){
            break;
        }

        json reqJson;
        json response;

        try{
            reqJson = json::parse(request);
        }catch(const json::parse_error& e){
            response["status"] = "error";
            response["message"] = "Invalid JSON format.";
            sendMessage(clientSocket, response.dump());
            continue;
        }

        std::string action = reqJson["action"];

        if(action == "CREATE"){
            Bank::getInstance().createAccountFromJson(reqJson);
            response["status"] = "success";
            response["message"] = "Account created!";
        }else if(action == "DELETE"){
            int accNum = reqJson["accountNumber"];
            Bank::getInstance().closeAccount(accNum);
            response["status"] = "success";
            response["message"] = "Account deleted!";
        }else if(action == "MODIFY"){
            Bank::getInstance().modifyAccount(reqJson);
            response["status"] = "success";
            response["message"] = "Account modified!";
        }else if(action == "DEPOSIT"){
            
        }else if(action == "WITHDRAW"){

        }else if(action == "APPLY_INTEREST"){

        }else if(action == "DISPLAY_ONE"){

        }else if(action == "DISPLAY_ALL"){

        }else if(action == "DELETE_ALL"){

        }else if(action == "EXPORT_JSON"){

        }else{
            response["status"] = "failed";
            response["message"] = "Invalid action!"; 
        }

        sendMessage(clientSocket, response.dump());
    }

    close(clientSocket);
    std::cout << "[Server] Client disconnected.\n";
}

int main(){

    int server_fd, client_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(server_fd == -1){
        std::perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == -1){
        std::perror("bind");
        exit(EXIT_FAILURE);
    }

    if(listen(server_fd, 10) == -1){
        std::perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "[Server] listening on port " << PORT << "...\n";

    while(true){
        client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        std::cout << "[Server] New client connected.\n";
        std::thread clientThread(handleClient, client_fd);
        clientThread.detach();
    }

    return 0;
}