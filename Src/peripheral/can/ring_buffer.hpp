/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_PERIPHERAL_CAN_RING_BUFFER_HPP_
#define SRC_PERIPHERAL_CAN_RING_BUFFER_HPP_

#include <stddef.h>

template<class ElementType, size_t max_size>
class RingBuffer {
public:
    RingBuffer() : head_(0U), tail_(0U), size_(0U) {
    }

    // Overwrite oldest element when full.
    void push(const ElementType& element) {
        buffer_[tail_] = element;
        tail_ = (tail_ + 1U) % max_size;
        size_++;
        if (size_ > max_size) {
            size_ = max_size;
            head_ = (head_ + 1U) % max_size;
        }
    }

    // Caller should check getSize() before pop().
    ElementType pop() {
        ElementType element = buffer_[head_];
        if (size_ > 0U) {
            head_ = (head_ + 1U) % max_size;
            size_--;
        }
        return element;
    }

    size_t getSize() const {
        return size_;
    }

private:
    ElementType buffer_[max_size]{};
    size_t head_;
    size_t tail_;
    size_t size_;
};

#endif  // SRC_PERIPHERAL_CAN_RING_BUFFER_HPP_
