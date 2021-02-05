#include <string>
#include <iostream>

#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include <ctime>
#include <errno.h>
#include <string.h>

#include <zmq.h>

int main(int argc, char *argv[])
{
    time_t t;
    srand((unsigned) time(&t));

    void* context = zmq_ctx_new();
    void* client = zmq_socket(context, ZMQ_DEALER);
    zmq_setsockopt(client, ZMQ_ROUTING_ID, "client", sizeof("client"));
    int rc = zmq_connect(client, "tcp://127.0.0.1:5555");
    assert(rc == 0);


    long i = 0;
    while (i++ < 10) {
        zmq_send(client, "hello", 6, 0);
        zmq_msg_t msg;
        zmq_msg_init(&msg);
        int rc = zmq_msg_recv(&msg, client, 0);
        //std::cout << strerror(errno) << std::endl;
        //if (errno == ENOTSOCK) {
        //    std::cout << "ENOTSOCK" << std::endl;
        //    return -1;
        //}
        std::cout << "Recv msg is: " << (char*)zmq_msg_data(&msg) << std::endl;
    }

    zmq_close(client);
    zmq_ctx_destroy(context);
    return 0;
}
