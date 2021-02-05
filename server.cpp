#include <string>
#include <iostream>
#include <thread>
#include <atomic>

#include <cassert>
#include <cstring>
#include <stdio.h>
#include <unistd.h>

#include <zmq.h>

std::atomic_bool running{true};
int recvTimeOut = 500;

void serverWorker(void* ctx) {
    //void* socket = zmq_socket(ctx, ZMQ_DEALER);
    //int rc = zmq_connect(socket, "inproc://backend");
    //assert(rc == 0);
    //zmq_setsockopt(socket, ZMQ_RCVTIMEO, &recvTimeOut, sizeof(recvTimeOut));
    std::cout << "======启动server worker======" << std::endl;
    while(running) {
        //recv id
        //zmq_msg_t id;
        //zmq_msg_init(&id);
        //zmq_msg_recv(&id, socket, 0);
        //std::string connID((char*)zmq_msg_data(&id));
        //std::cout << "thread id is: " << std::this_thread::get_id() << " "
        //          << "Recv msg from: " << connID << std::endl;

        //recv data
        //zmq_msg_t msg;
        //zmq_msg_init(&msg);
        //zmq_msg_recv(&msg, socket, 0);
        //std::cout << "Recv msg is: " << (char*)zmq_msg_data(&msg) << std::endl;
        //
        //zmq_send(socket, connID.c_str(), connID.size() + 1, ZMQ_SNDMORE);
        //zmq_msg_send(&msg, socket, 0);
        //zmq_close(&msg);
    }
    //zmq_close(socket);
}

void* frontend;
void* backend;

void stop(void* ctx) {
    sleep(3);
    running = false;
    zmq_close(frontend);
    zmq_close(backend);
    zmq_ctx_term(ctx);
    //zmq_ctx_destroy(ctx);
}

int main(int argc, char *argv[])
{
    void* context = zmq_ctx_new();
    //zmq_ctx_set(context, ZMQ_IO_THREADS, 3);
    frontend = zmq_socket(context, ZMQ_ROUTER);
    backend = zmq_socket(context, ZMQ_DEALER);
    zmq_setsockopt(frontend, ZMQ_RCVTIMEO, &recvTimeOut, sizeof(recvTimeOut));
    zmq_setsockopt(backend, ZMQ_RCVTIMEO, &recvTimeOut, sizeof(recvTimeOut));
    //int linger = 0;
    //zmq_setsockopt(frontend, ZMQ_LINGER, &linger, sizeof(linger));
    //zmq_setsockopt(backend, ZMQ_LINGER, &linger, sizeof(linger));

    int rc = zmq_bind(frontend, "tcp://127.0.0.1:5555");
    assert(rc == 0);
    rc = zmq_bind(backend, "inproc://backend");
    assert(rc == 0);

    void* th0 = zmq_threadstart(serverWorker, context);
    //void* th1 = zmq_threadstart(serverWorker, context);
    void* th2 = zmq_threadstart(stop, context);

    zmq_proxy(frontend, backend, NULL);
    std::cout << "proxy stopped" << std::endl;

    //zmq_ctx_term(context);
    zmq_threadclose(th0);
    //zmq_threadclose(th1);
    zmq_threadclose(th2);

    return 0;
}
