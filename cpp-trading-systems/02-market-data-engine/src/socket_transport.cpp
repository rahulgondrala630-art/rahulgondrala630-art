#include "socket_transport.hpp"
#include <array>
#include <cerrno>
#include <sys/socket.h>
#include <sys/uio.h>
namespace feed {
namespace {
bool receive_exact(int fd, std::uint8_t* dst, std::size_t count, std::string& error) {
    std::size_t received=0;
    while(received<count) {
        const auto n=::recv(fd,dst+received,count-received,0);
        if(n==0) { error="peer closed the TCP connection"; return false; }
        if(n<0) { if(errno==EINTR) continue; error="TCP recv failed"; return false; }
        received+=static_cast<std::size_t>(n);
    }
    return true;
}
}
std::optional<std::vector<std::uint8_t>> read_tcp_frame(int fd,std::size_t max_size,std::string& error) {
    std::array<std::uint8_t,4> header{};
    if(!receive_exact(fd,header.data(),header.size(),error)) return std::nullopt;
    const std::uint32_t size=(static_cast<std::uint32_t>(header[0])<<24)|
        (static_cast<std::uint32_t>(header[1])<<16)|
        (static_cast<std::uint32_t>(header[2])<<8)|static_cast<std::uint32_t>(header[3]);
    if(size==0||size>max_size) { error="TCP frame length is zero or exceeds configured maximum"; return std::nullopt; }
    std::vector<std::uint8_t> payload(size);
    if(!receive_exact(fd,payload.data(),payload.size(),error)) return std::nullopt;
    return payload;
}
std::optional<std::vector<std::uint8_t>> read_udp_datagram(int fd,std::size_t max_size,std::string& error) {
    if(max_size==0) { error="UDP receive buffer size must be positive"; return std::nullopt; }
    std::vector<std::uint8_t> buffer(max_size);
    iovec iov{buffer.data(),buffer.size()};
    msghdr message{};
    message.msg_iov=&iov;
    message.msg_iovlen=1;
    ssize_t n;
    do { n=::recvmsg(fd,&message,MSG_TRUNC); } while(n<0 && errno==EINTR);
    if(n<0) { error="UDP recvmsg failed"; return std::nullopt; }
    if(static_cast<std::size_t>(n)>max_size || (message.msg_flags&MSG_TRUNC)) {
        error="UDP datagram exceeds configured maximum";
        return std::nullopt;
    }
    buffer.resize(static_cast<std::size_t>(n));
    return buffer;
}
} // namespace feed
