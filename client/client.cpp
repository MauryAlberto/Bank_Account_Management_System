#include "../include/Network.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void displayStatusMessage(const json& msg) {
    try {
        // Get the values (will throw if fields don't exist)
        std::string status = msg.at("status");
        std::string message = msg.at("message");
        
        // Print concatenated result
        std::cout << status << message << std::endl;
    }
    catch (const json::exception& e) {
        std::cerr << "Error parsing message: " << e.what();
    }
}

void displayResponse(const json& response) {
    try {
        // Check if response contains accounts array
        if (!response.contains("accounts") || !response["accounts"].is_array()) {
            displayStatusMessage(response);
            return;
        }

        const auto& accounts = response["accounts"];
        
        // Display header
        std::cout << "\n===== ACCOUNTS =====\n";
        std::cout << std::left 
                  << std::setw(10) << "Type"
                  << std::setw(10) << "Number"
                  << std::setw(25) << "Holder"
                  << std::setw(15) << "Balance"
                  << std::setw(15) << "Rate/Limit"
                  << "\n" << std::string(75, '-') << "\n";

        // Display each account
        for (const auto& acc : accounts) {
            try {
                std::string type = acc.value("accountType", "UNKNOWN");
                int number = acc.value("accountNumber", 0);
                std::string holder = acc.value("holderName", "N/A");
                double balance = acc.value("balance", 0.0);

                // Format rate/limit with 2 decimal places
                std::ostringstream rateLimitStream;
                rateLimitStream << std::fixed << std::setprecision(2);
                
                if (type == "SAVINGS") {
                    double rate = acc.value("interestRate", 0.0);
                    rateLimitStream << (rate * 100) << "%";
                } else if (type == "CHECKING") {
                    double limit = acc.value("overdraftLimit", 0.0);
                    rateLimitStream << "$" << limit;
                } else {
                    rateLimitStream << "N/A";
                }

                std::cout << std::left
                          << std::setw(10) << type
                          << std::setw(10) << number
                          << std::setw(25) << holder
                          << std::setw(15) << std::fixed << std::setprecision(2) << balance
                          << std::setw(15) << rateLimitStream.str()
                          << "\n";

            } catch (const std::exception& e) {
                std::cerr << "Error displaying account: " << e.what() << "\n";
                continue;
            }
        }
        std::cout << std::string(75, '=') << "\n";

    } catch (const std::exception& e) {
        std::cerr << "\nFatal error displaying accounts: " << e.what() << "\n";
    }
}

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

    std::vector<json> testCommands = {
    { {"action", "CREATE"}, {"accountType", "SAVINGS"}, {"accountNumber", "101"}, {"holderName", "Alice"}, {"balance", "1000"}, {"interestRate", "0.03"} },
    { {"action", "CREATE"}, {"accountType", "CHECKING"}, {"accountNumber", "102"}, {"holderName", "Bob"}, {"balance", "500"}, {"overdraftLimit", "200"} },
    { {"action", "DISPLAY_ALL"} },
    { {"action", "DEPOSIT"}, {"accountNumber", "101"}, {"amount", "200"} },
    { {"action", "WITHDRAW"}, {"accountNumber", "102"}, {"amount", "100"} },
    { {"action", "TRANSFER"}, {"accountNumber1", "101"}, {"accountNumber2", "102"}, {"amount", "100"}},
    { {"action", "DISPLAY_ONE"}, {"accountNumber", "101"} },
    { {"action", "APPLY_INTEREST_ONE"}, {"accountNumber", "101"} },
    { {"action", "APPLY_INTEREST_ALL"} },
    { {"action", "MODIFY"}, {"accountNumber", "102"}, {"holderName", "Robert"}, {"balance", "600"}, {"overdraftLimit", "300"} },
    { {"action", "DELETE"}, {"accountNumber", "101"} },
    { {"action", "EXPORT_JSON"} },
    { {"action", "DELETE"}, {"accountNumber", "102"} }/* ,
    { {"action", "DELETE_ALL"} } ,    // DELETE_ALL works
    { {"action", "EXIT"} } */   // EXIT works
    };

    size_t testIndex = 0;
    char test_yes_no;
    std::cout << "Would you like to run a test? (y) or (n): ";
    std::cin >> test_yes_no;
    std::cin.ignore();

    while(true){
        json request;

        if(testIndex < testCommands.size() && test_yes_no == 'y' || test_yes_no == 'Y'){
            request = testCommands[testIndex++];
        }else{
            std::string line, action, accountType;
            std::vector<std::string> items;
            
            std::cout << SERVER_IP << ":" << PORT << "> ";
            std::getline(std::cin, line);
            std::stringstream ss(line);
            std::string item;

            while(ss >> item){
                items.push_back(item);
            }

            if(items.size() > 6){
                std::cerr << "Error: Too many arguments passed\n";
                continue;
            }

            if(items.empty()) {
                std::cerr << "Error: No command entered\n";
                continue;
            }

            action = items[0];
            std::transform(action.begin(), action.end(), action.begin(), ::toupper);

            if(action != "APPLY_INTEREST_ALL" && action != "DISPLAY_ALL" && action != "DELETE_ALL" && action != "EXPORT_JSON" && action != "EXIT") {
                if(items.size() < 2) {
                    std::cerr << "Error: Missing required arguments\n";
                    continue;
                }
                std::transform(items[1].begin(), items[1].end(), items[1].begin(), ::toupper);
            }

            if(action == "CREATE" && items.size() == 6){
                /* 
                    formatting:
                    CREATE ACCOUNT_TYPE accountNumber holderName balance rate/limit
                */
                request["action"] = action;
                request["accountType"] = items[1];
                request["accountNumber"] = items[2];
                request["holderName"] = items[3];
                request["balance"] = items[4];
                if(items[1] == "SAVINGS"){
                    request["interestRate"] = items[5];
                }else if(items[1] == "CHECKING"){
                    request["overdraftLimit"] = items[5];
                }else{
                    std::cerr << "Unkown account type\n";
                    continue;
                }
            }else if(action == "DELETE"){
                /* 
                    formatting:
                    DELETE accountNumber
                */
                request["action"] = action;
                request["accountNumber"] = items[1];
            }else if(action == "MODIFY"){
                /* 
                    formatting:
                    MODIFY accountNumber holderName balance rate/limit
                */
                request["action"] = action;
                request["accountNumber"] = items[1];
                request["holderName"] = items[2];
                request["balance"] = items[3];

                if(accountType == "SAVINGS"){
                    request["interestRate"] = items[4];
                }else if(accountType == "CHECKING"){
                    request["overdraft"] = items[4];
                }else{
                    std::cerr << "Unkown account type\n";
                    continue;
                }
            }else if(action == "DEPOSIT"){
                /* 
                    foramtting:
                    DEPOSIT accountNumber amount
                */
            request["action"] = action;
            request["accountNumber"] = items[1];
            request["amount"] = items[2];
            }else if(action == "WITHDRAW"){
                /* 
                    foramtting:
                    WITHDRAW accountNumber amount
                */
            request["action"] = action;
            request["accountNumber"] = items[1];
            request["amount"] = items[2];
            }else if(action == "TRANSFER"){
                /* 
                    formatting:
                    TRANSFER From_Account# To_Account# amount
                */
                request["action"] = action;
                request["accountNumber1"] = items[1];
                request["accountNumber2"] = items[2];
                request["amount"] = items[3];
            }else if(action == "APPLY_INTEREST_ONE"){
                /* 
                    formatting:
                    APPLY_INTEREST_ONE accountNumber
                */
                request["action"] = action;
                request["accountNumber"] = items[1];
            }else if(action == "APPLY_INTEREST_ALL"){
                /* 
                    formatting:
                    APPLY_INTEREST_ALL
                */
                request["action"] = action;
            }else if(action == "DISPLAY_ONE"){
                /* 
                    formatting:
                    DISPLAY_ONE accountNumber
                */
            request["action"] = action;
            request["accountNumber"] = items[1];
            }else if(action == "DISPLAY_ALL"){
                /* 
                    formatting:
                    DISPLAY_ALL
                */
                request["action"] = action;
                sendMessage(sock, request.dump());
                std::string rawResponse = recieveMessage(sock);
                
                if(rawResponse.empty()){
                    std::cerr << "[ERROR] Empty response from server\n";
                    continue;
                }
                
                try{
                    json response = json::parse(rawResponse);
                    displayResponse(response);
                }catch(const json::exception& e) {
                    std::cerr << "JSON parse error: " << e.what() << "\n";
                    std::cerr << "Raw response was:\n" << rawResponse << "\n";
                }
                continue;  // Skip the duplicate receive at bottom
            }else if(action == "DELETE_ALL"){
                /* 
                    formatting:
                    DELETE_ALL
                */
            request["action"] = action;
            }else if(action == "EXPORT_JSON"){
                /* 
                    formatting:
                    EXPORT_JSON
                */
                request["action"] = action;
            }else if(action == "EXIT"){

                /*
                    formatting:
                    EXIT 
                */
                request["action"] = action;
                sendMessage(sock, request.dump());

                break;
            }else{
                std::cerr << "Error: Invalid action!\n";
                continue;
            }
        }

        sendMessage(sock, request.dump());

        try{
            json response = json::parse(recieveMessage(sock));
            displayResponse(response);
        }catch(const json::exception& e){
            std::cerr << "Invalid server response: " << e.what() << "\n";
        }
    }

    close(sock);
    return 0;
}