FROM ubuntu:22.04

RUN apt-get update && apt-get install -y g++ && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN g++ backend/server_linux.cpp -o server -std=c++11 -pthread

EXPOSE 5000

CMD ["./server"]
