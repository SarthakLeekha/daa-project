#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <fstream>

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

string readFile(const string & filename) {
    ifstream file(filename);
    if (!file.is_open()) return "";
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

string makeResponse(const string & status, const string & contentType, const string & body) {
    string resp = status + "\r\n";
    resp += "Content-Type: " + contentType + "\r\n";
    resp += "Access-Control-Allow-Origin: *\r\n";
    resp += "Access-Control-Allow-Methods: POST, OPTIONS, GET\r\n";
    resp += "Access-Control-Allow-Headers: Content-Type, Accept\r\n";
    resp += "Connection: close\r\n\r\n";
    resp += body;
    return resp;
}

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        cerr << "Socket creation failed" << endl;
        return 1;
    }

    int reuse = 1;
setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(10000);  // Render port $PORT or 10000

bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr))
        cerr << "Bind failed" << endl;
        close(serverSocket);
        return 1;
    }

    listen(serverSocket, SOMAXCONN);
    cout << "Linux Server listening on port 10000" << endl;

    while (true) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket == -1) continue;

        char buffer[4096];
        int received = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (received <= 0) {
            close(clientSocket);
            continue;
        }
        buffer[received] = '\0';
        string request(buffer);

        stringstream ss(request);
        string method, path, dummy;
        ss >> method >> path;

if (method == "OPTIONS" && path == "/calculate") {
            string response = makeResponse("HTTP/1.1 200 OK", "application/json", "");
            send(clientSocket, response.c_str(), response.length(), 0);
            close(clientSocket);
            continue;
        } else if (method == "POST" &amp;&amp; path == "/calculate") {
            size_t bodyStart = request.find("\r\n\r\n");
            if (bodyStart != string::npos) {
                string body = request.substr(bodyStart + 4);

                size_t s1Start = body.find("\"s1\":\"") + 6;
                size_t s2Start = body.find("\"s2\":\"") + 6;
                
                size_t s1End = body.find("\"", s1Start);
                size_t s2End = body.find("\"", s2Start);

if (s1End != string::npos && s2End != string::npos) {
                    string s1 = body.substr(s1Start, s1End - s1Start);
                    string s2 = body.substr(s2Start, s2End - s2Start);
                    
                    int distance = editDistance(s1, s2);
                    
                    string responseBody = "{\"distance\":" + to_string(distance) + "}";
                    string response = makeResponse("HTTP/1.1 200 OK", "application/json", responseBody);
                    send(clientSocket, response.c_str(), response.length(), 0);
                } else {
                    string responseBody = "{\"error\":\"Invalid JSON\"}";
                    string response = makeResponse("HTTP/1.1 400 Bad Request", "application/json", responseBody);
                    send(clientSocket, response.c_str(), response.length(), 0);
                }
            }
            close(clientSocket);
            continue;
        } else if (method == "GET") {
            string body;
            string cType = "application/json";
            if (path == "/" || path == "/index.html") {
                body = readFile("frontend/index.html");
                cType = "text/html";
            } else if (path == "/style.css") {
                body = readFile("frontend/style.css");
                cType = "text/css";
            } else if (path == "/script.js") {
                body = readFile("frontend/script.js");
                cType = "application/javascript";
            } else {
                body = "{\"error\":\"Not found\"}";
                cType = "application/json";
            }
            
            string header = "HTTP/1.1 200 OK\r\nContent-Type: " + cType + "\r\nConnection: close\r\n\r\n";
            string response = header + body;
            send(clientSocket, response.c_str(), response.length(), 0);
        } else {
            string response = "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\nNot found";
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        
        close(clientSocket);
    }

    close(serverSocket);
    return 0;
}

