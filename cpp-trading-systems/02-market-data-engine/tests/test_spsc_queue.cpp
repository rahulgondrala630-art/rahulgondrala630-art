#include "spsc_queue.hpp"
#include <cassert>
int main() {
    feed::SpscQueue<int,4> q;
    int value=0;
    assert(!q.try_pop(value));
    assert(q.try_push(10)); assert(q.try_push(20)); assert(q.try_push(30));
    assert(!q.try_push(40));
    assert(q.try_pop(value)&&value==10);
    assert(q.try_push(40));
    assert(q.try_pop(value)&&value==20);
    assert(q.try_pop(value)&&value==30);
    assert(q.try_pop(value)&&value==40);
    assert(!q.try_pop(value));
}
