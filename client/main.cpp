#include "../include/Network.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main(){
    struct sockaddr_in server_addr;
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock == -1){
        std::perror("socket");
        exit(EXIT_FAILURE);
    }

    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family= AF_INET;
    server_addr.sin_port = htons(PORT);
    
    if(inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0){
        std::cerr << "Invalid address.\n";
        exit(EXIT_FAILURE);
    }
    
    if(connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        std::perror("connect");
        exit(EXIT_FAILURE);
    }

    json req;
    req["action"] = "CREATE";
    req["accountNumber"] = 1001;
    req["holderName"] = "John Doe";
    req["balance"] = 1000.00;
    req["accountType"] = "SAVINGS";
    req["interestRate"] = 0.02;

    sendMessage(sock, req.dump());

    std::string response = recieveMessage(sock);
    std::cout << response;

    close(sock);
    return 0;
}