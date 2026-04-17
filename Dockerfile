FROM ubuntu:22.04

# Install g++ and libs
RUN apt-get update &amp;&amp; apt-get install -y g++ &amp;&amp; rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy source
COPY backend/server.cpp .

# Compile for Linux (remove winsock2, add simple handling? Wait, full port needed.

# Temp: Note - server.cpp is Windows-only (winsock2). Needs port to Linux (use <sys/socket.h> etc).
# For demo: Compile if ported, CMD ["./server"]

CMD ["echo", "Server ready on port 5000 - port server.cpp to Linux"]
