#!/usr/bin/env bash

g++ -Wall server.cpp -o server -std=c++11 -lczmq -lzmq
g++ -Wall client.cpp -o client -std=c++11 -lczmq -lzmq
g++ -Wall dealer.cpp -o dealer -std=c++11 -lzmq
g++ -Wall router.cpp -o router -std=c++11 -lzmq
g++ -Wall request.cpp -o request -std=c++11 -lzmq
g++ -Wall reply.cpp -o reply -std=c++11 -lzmq
