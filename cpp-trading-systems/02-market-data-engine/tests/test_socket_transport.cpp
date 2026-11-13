#include "socket_transport.hpp"
#include <array>
#include <cassert>
#include <cstdint>
#include <sys/socket.h>
#include <unistd.h>
int main() {
    int sockets[2]{};
    assert(::socketpair(AF_UNIX,SOCK_STREAM,0,sockets)==0);
    const std::array<std::uint8_t,7> frame{0,0,0,3,0xaa,0xbb,0xcc};
    assert(::send(sockets[0],frame.data(),frame.size(),0)==static_cast<ssize_t>(frame.size()));
    std::string error;
    auto payload=feed::read_tcp_frame(sockets[1],64,error);
    assert(payload && payload->size()==3);
    assert((*payload)[0]==0xaa && (*payload)[2]==0xcc);
    ::close(sockets[0]); ::close(sockets[1]);
}
