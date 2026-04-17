# DAA Project - Edit Distance Calculator

C++ Windows HTTP server (port 5000) + static frontend for Levenshtein (edit) distance calculation.

## Local Setup
1. Compile backend: `g++ backend/server.cpp -o backend/server.exe -lws2_32`
2. Run: `backend/server.exe`
3. Open `frontend/index.html` in browser

## Render Deploy
### Frontend (Static Site)
- Language: Static Site
- Build Command: (empty)
- Publish Directory: frontend

### Backend (Docker - Linux port needed)
Current server Windows-only. Port to Linux sockets for Docker:

Dockerfile:
```
FROM ubuntu:22.04
RUN apt-get update &amp;&amp; apt-get install -y g++
WORKDIR /app
COPY . .
# Compile Linux server.cpp (after port)
CMD ["./server"]
```

Port server.cpp: winsock2.h → sys/socket.h, WS* → standard sockets.

## API
POST /calculate
```json
{"s1":"hello","s2":"hill"}
```
Response: {"distance":3}

Repo: https://github.com/SarthakLeekha/daa-project

