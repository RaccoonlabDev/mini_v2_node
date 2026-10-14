/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_COMMON_DATAPOOL_HPP_
#define SRC_COMMON_DATAPOOL_HPP_

#include <array>
#include <cstdint>
#include <type_traits>

/**
 * @brief The datapool keeps the latest sample of each topic, so a module that produces data does
 * not depend on the protocol frontends that put it on a bus. See docs/datapool.md.
 *
 * Not thread safe: publish and read only from the main loop, never from an interrupt.
 */
namespace datapool {

/**
 * @brief Declare a topic as a tag type: struct MyTopic : datapool::Topic<MyMessage, 2> {};
 * Every tag gets its own storage, even if two topics share a message type.
 */
template <typename MessageT, uint8_t INSTANCES = 1>
struct Topic {
    // A message is plain data, see the message rules in docs/datapool.md
    static_assert(std::is_aggregate_v<MessageT>, "A message is a plain struct of fields");
    static_assert(std::is_standard_layout_v<MessageT>, "A message is a plain struct of fields");
    static_assert(std::is_trivially_copyable_v<MessageT>, "A sample is copied as is");
    static_assert(INSTANCES > 0, "A topic needs at least one instance");

    using Message = MessageT;
    static constexpr uint8_t INSTANCES_AMOUNT{INSTANCES};
};

namespace detail {

template <typename TopicT>
struct Slot {
    typename TopicT::Message message{};
    uint32_t generation{0};  // 0 means that nothing was published yet
};

// An inline variable has a single definition, so all translation units share the same slots.
template <typename TopicT>
inline std::array<Slot<TopicT>, TopicT::INSTANCES_AMOUNT> slots{};

}  // namespace detail

template <typename TopicT>
class Publisher {
public:
    using Message = typename TopicT::Message;

    explicit constexpr Publisher(uint8_t instance = 0) : _instance(instance) {}

    /**
     * @brief Overwrite the sample of this instance.
     * @return false if the instance is out of range, the sample is dropped then
     */
    bool publish(const Message& message) {
        if (_instance >= TopicT::INSTANCES_AMOUNT) {
            return false;
        }

        auto& slot = detail::slots<TopicT>[_instance];
        slot.message = message;
        slot.generation++;
        if (slot.generation == 0) {
            slot.generation = 1;
        }

        return true;
    }

private:
    uint8_t _instance;
};

template <typename TopicT>
class Subscriber {
public:
    using Message = typename TopicT::Message;

    explicit constexpr Subscriber(uint8_t instance = 0) : _instance(instance) {}

    /**
     * @return true if a sample was published since the last copy() of this subscriber
     */
    bool updated() const {
        return generation() != _last_generation;
    }

    /**
     * @brief Copy the latest sample, new or not. A subscriber that reads slower than the
     * publisher writes gets only the latest sample.
     * @return false if nothing was published yet or the instance is out of range
     */
    bool copy(Message* message) {
        const auto current = generation();
        if (current == 0) {
            return false;
        }

        *message = detail::slots<TopicT>[_instance].message;
        _last_generation = current;
        return true;
    }

    /**
     * @brief Copy the sample only if it is new.
     */
    bool update(Message* message) {
        return updated() && copy(message);
    }

private:
    uint32_t generation() const {
        if (_instance >= TopicT::INSTANCES_AMOUNT) {
            return 0;
        }

        return detail::slots<TopicT>[_instance].generation;
    }

    uint8_t _instance;
    uint32_t _last_generation{0};
};

}  // namespace datapool

#endif  // SRC_COMMON_DATAPOOL_HPP_
