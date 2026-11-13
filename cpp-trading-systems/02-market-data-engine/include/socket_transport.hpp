#pragma once
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
namespace feed {
// POSIX/Linux helpers. The caller owns, configures, and closes the socket descriptor.
// TCP frames use a four-byte unsigned big-endian payload length prefix.
std::optional<std::vector<std::uint8_t>> read_tcp_frame(int socket_fd, std::size_t max_frame_size, std::string& error);
std::optional<std::vector<std::uint8_t>> read_udp_datagram(int socket_fd, std::size_t max_datagram_size, std::string& error);
} // namespace feed
