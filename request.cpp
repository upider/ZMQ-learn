#include <string>
#include <iostream>
#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

int main(int argc, char *argv[])
{
    void* context = zmq_ctx_new();
    void* socket = zmq_socket(context, ZMQ_REQ);
    std::string address = "tcp://127.0.0.1:";
    address.append(argv[1]);
    int rc = zmq_connect(socket, address.c_str());
    assert(rc == 0);

    long i = 0;
    while (i++ < 10) {
        zmq_send(socket, "hello", 6, 0);
        zmq_msg_t msg;
        zmq_msg_init_size(&msg, 6);
        zmq_msg_recv(&msg, socket, 0);
        std::cout << "no." << i << "msg: \n" << (char*)zmq_msg_data(&msg) << std::endl;
        zmq_msg_close(&msg);
        std::cout << "send next message?";
        std::cin.get();
    }

    zmq_close(socket);
    zmq_ctx_destroy(context);

    return 0;
}
