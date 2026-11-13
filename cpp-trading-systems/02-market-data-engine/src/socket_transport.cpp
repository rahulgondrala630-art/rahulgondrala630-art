#include "socket_transport.hpp"
#include <array>
#include <cerrno>
#include <sys/socket.h>
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
    const auto n=::recvfrom(fd,buffer.data(),buffer.size(),0,nullptr,nullptr);
    if(n<0) { if(errno==EINTR) return read_udp_datagram(fd,max_size,error); error="UDP recvfrom failed"; return std::nullopt; }
    buffer.resize(static_cast<std::size_t>(n));
    return buffer;
}
} // namespace feed
