#include <string>
#include <iostream>
#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

int main(int argc, char *argv[])
{
    void* context = zmq_ctx_new();
    zmq_ctx_set(context, ZMQ_IO_THREADS, 3);
    //void* socket = zmq_socket(context, ZMQ_DEALER);
    void* socket = zmq_socket(context, ZMQ_ROUTER);
    std::string address = "tcp://127.0.0.1:6666";
    int rc = zmq_bind(socket, address.c_str());
    assert(rc == 0);

    while (true) {
        zmq_msg_t msg;
        zmq_msg_init_size(&msg, 6);
        zmq_msg_recv(&msg, socket, 0);
        std::cout << "msg: \n" << (char*)zmq_msg_data(&msg) << std::endl;
        sleep(1);
        zmq_msg_send(&msg, socket, 0);
        zmq_close(&msg);
    }

    zmq_close(socket);
    zmq_ctx_destroy(context);

    return 0;
}
