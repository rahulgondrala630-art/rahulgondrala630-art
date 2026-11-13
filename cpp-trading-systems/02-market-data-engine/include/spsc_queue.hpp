#pragma once
#include <array>
#include <atomic>
#include <cstddef>
#include <type_traits>
#include <utility>
namespace feed {
// Bounded single-producer/single-consumer ring. One slot is reserved to distinguish full from empty.
template <typename T, std::size_t Capacity>
class SpscQueue {
    static_assert(Capacity >= 2, "SPSC queue capacity must be at least two");
    static_assert(std::is_default_constructible_v<T>, "T must be default constructible");
public:
    bool try_push(const T& value) {
        const auto head=head_.load(std::memory_order_relaxed);
        const auto next=increment(head);
        if(next==tail_.load(std::memory_order_acquire)) return false;
        buffer_[head]=value;
        head_.store(next,std::memory_order_release);
        return true;
    }
    bool try_pop(T& value) {
        const auto tail=tail_.load(std::memory_order_relaxed);
        if(tail==head_.load(std::memory_order_acquire)) return false;
        value=std::move(buffer_[tail]);
        tail_.store(increment(tail),std::memory_order_release);
        return true;
    }
private:
    static constexpr std::size_t increment(std::size_t i) { return (i+1)%Capacity; }
    alignas(64) std::array<T,Capacity> buffer_{};
    alignas(64) std::atomic<std::size_t> head_{0};
    alignas(64) std::atomic<std::size_t> tail_{0};
};
} // namespace feed
