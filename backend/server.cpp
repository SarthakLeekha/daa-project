#include <winsock2.h>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int editDistance(string s1, string s2) {
    int m = s1.length();
    int n = s2.length();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));
    
    for (int i = 0; i <= m; i++) {
        dp[i][0] = i;
    }
    for (int j = 0; j <= n; j++) {
        dp[0][j] = j;
    }
    
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (s1[i - 1] == s2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                dp[i][j] = 1 + min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
            }
        }
    }
    return dp[m][n];
}

int main() {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    sockaddr_in serverAddr, clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    char buffer[4096];
    
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed\n";
        return 1;
    }
    
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Socket creation failed, WSAErr=" << WSAGetLastError() << "\n";
        WSACleanup();
        return 1;
    }
    
    int reuse = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse));
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
   
    int portsToTry[] = {5000, 8080};
    bool bound = false;
    for (int p : portsToTry) {
        serverAddr.sin_port = htons(p);
        if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == 0) {
            cout << "Bound to port " << p << "\n";
            bound = true;
            break;
        } else {
            cerr << "Bind to port " << p << " failed, WSAErr=" << WSAGetLastError() << "\n";
        }
    }
    if (!bound) {
        cerr << "Bind failed on all tried ports\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    
    listen(serverSocket, SOMAXCONN);
    cout << "Server started on http://127.0.0.1:5000\n";
    
    while (true) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == INVALID_SOCKET) {
            continue;
        }
        
        int received = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (received <= 0) {
            closesocket(clientSocket);
            continue;
        }
        if (received >= (int)sizeof(buffer)) received = sizeof(buffer) - 1;
        buffer[received] = '\0';
        string request(buffer);
        cout << "Received request:\n" << request << '\n';
        
        stringstream ss(request);
        string method, path, dummy;
        ss >> method >> path;
        cout << "Parsed method='" << method << "' path='" << path << "'\n";
        
       
        auto makeResponse = [&](const string &status, const string &body){
            string resp = status + "\r\n";
            resp += "Content-Type: application/json\r\n";
            resp += "Access-Control-Allow-Origin: *\r\n";
            resp += "Access-Control-Allow-Methods: POST, OPTIONS, GET\r\n";
            resp += "Access-Control-Allow-Headers: Content-Type, Accept\r\n";
            resp += "Connection: close\r\n\r\n";
            resp += body;
            return resp;
        };
        
        if (method == "OPTIONS" && path == "/calculate") {
            string response = makeResponse("HTTP/1.1 200 OK", "");
            send(clientSocket, response.c_str(), response.length(), 0);
        } else if (method == "POST" && path == "/calculate") {
            size_t bodyStart = request.find("\r\n\r\n");
            if (bodyStart != string::npos) {
                string body = request.substr(bodyStart + 4);
                cout << "Body: " << body << '\n';
                
                // Parse JSON body
                size_t s1Start = body.find("\"s1\":\"");
                size_t s2Start = body.find("\"s2\":\"");
                
                if (s1Start != string::npos && s2Start != string::npos) {
                    s1Start += 6; 
                    size_t s1End = body.find("\"", s1Start);
                    s2Start += 6; 
                    size_t s2End = body.find("\"", s2Start);
                    
                    if (s1End != string::npos && s2End != string::npos) {
                        string s1 = body.substr(s1Start, s1End - s1Start);
                        string s2 = body.substr(s2Start, s2End - s2Start);
                        
                        int distance = editDistance(s1, s2);
                        cout << "Edit distance between \"" << s1 << "\" and \"" << s2 << "\" = " << distance << "\n";
                        
                        string response = makeResponse("HTTP/1.1 200 OK", "{\"distance\":" + to_string(distance) + "}");
                        send(clientSocket, response.c_str(), response.length(), 0);
                    } else {
                        string response = makeResponse("HTTP/1.1 400 Bad Request", "{\"error\":\"Invalid JSON format\"}");
                        send(clientSocket, response.c_str(), response.length(), 0);
                    }
                } else {
                    string response = makeResponse("HTTP/1.1 400 Bad Request", "{\"error\":\"Missing s1 or s2\"}");
                    send(clientSocket, response.c_str(), response.length(), 0);
                }
            } else {
                cout << "No body found in request\n";
                string response = makeResponse("HTTP/1.1 400 Bad Request", "{\"error\":\"No body found\"}");
                send(clientSocket, response.c_str(), response.length(), 0);
            }
        } else if (method == "GET" && (path == "/" || path == "/index.html")) {
            string response = makeResponse("HTTP/1.1 200 OK", "{\"message\":\"Edit Distance Calculator API\",\"endpoints\":[\"/calculate (POST)\"]}");
            send(clientSocket, response.c_str(), response.length(), 0);
        } else {
            cout << "Unhandled path: " << method << " " << path << '\n';
            string response = makeResponse("HTTP/1.1 404 Not Found", "{\"error\":\"Not found\"}");
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        
        closesocket(clientSocket);
    }
    
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}

