/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include <gtest/gtest.h>
#include <cstdint>
#include <limits>
#include "datapool.hpp"

namespace {

struct Sample {
    int32_t value;
};

// The storage is static and outlives a test, so every test declares its own topics.

TEST(Datapool, NothingToReadBeforeFirstPublish) {
    struct T : datapool::Topic<Sample> {};
    datapool::Subscriber<T> sub;
    Sample sample{7};

    EXPECT_FALSE(sub.updated());
    EXPECT_FALSE(sub.copy(&sample));
    EXPECT_FALSE(sub.update(&sample));
    EXPECT_EQ(sample.value, 7);
}

TEST(Datapool, SubscriberSeesEachSampleOnce) {
    struct T : datapool::Topic<Sample> {};
    datapool::Publisher<T> pub;
    datapool::Subscriber<T> sub;
    Sample sample{};

    ASSERT_TRUE(pub.publish({1}));
    EXPECT_TRUE(sub.updated());
    EXPECT_TRUE(sub.update(&sample));
    EXPECT_EQ(sample.value, 1);

    EXPECT_FALSE(sub.updated());
    EXPECT_FALSE(sub.update(&sample));

    sample.value = 0;
    EXPECT_TRUE(sub.copy(&sample));
    EXPECT_EQ(sample.value, 1);
}

TEST(Datapool, SlowSubscriberGetsOnlyTheLatestSample) {
    struct T : datapool::Topic<Sample> {};
    datapool::Publisher<T> pub;
    datapool::Subscriber<T> sub;
    Sample sample{};

    pub.publish({1});
    pub.publish({2});
    pub.publish({3});

    EXPECT_TRUE(sub.update(&sample));
    EXPECT_EQ(sample.value, 3);
    EXPECT_FALSE(sub.updated());
}

TEST(Datapool, SubscribersTrackTheirOwnProgress) {
    struct T : datapool::Topic<Sample> {};
    datapool::Publisher<T> pub;
    datapool::Subscriber<T> first;
    datapool::Subscriber<T> second;
    Sample sample{};

    pub.publish({1});
    first.update(&sample);

    EXPECT_FALSE(first.updated());
    EXPECT_TRUE(second.updated());
}

TEST(Datapool, LateSubscriberGetsTheCurrentSample) {
    struct T : datapool::Topic<Sample> {};
    datapool::Publisher<T> pub;
    pub.publish({5});

    datapool::Subscriber<T> sub;
    Sample sample{};
    EXPECT_TRUE(sub.update(&sample));
    EXPECT_EQ(sample.value, 5);
}

TEST(Datapool, InstancesAreIndependent) {
    struct T : datapool::Topic<Sample, 2> {};
    datapool::Publisher<T> pub1(1);
    datapool::Subscriber<T> sub0(0);
    datapool::Subscriber<T> sub1(1);
    Sample sample{};

    pub1.publish({11});

    EXPECT_FALSE(sub0.updated());
    EXPECT_TRUE(sub1.update(&sample));
    EXPECT_EQ(sample.value, 11);
}

TEST(Datapool, OutOfRangeInstanceIsRejected) {
    struct T : datapool::Topic<Sample, 2> {};
    datapool::Publisher<T> pub(2);
    datapool::Subscriber<T> sub(2);
    Sample sample{};

    EXPECT_FALSE(pub.publish({1}));
    EXPECT_FALSE(sub.updated());
    EXPECT_FALSE(sub.copy(&sample));
}

TEST(Datapool, TopicsWithTheSameMessageDoNotShareStorage) {
    struct A : datapool::Topic<Sample> {};
    struct B : datapool::Topic<Sample> {};
    datapool::Publisher<A> pub_a;
    datapool::Subscriber<B> sub_b;

    pub_a.publish({1});

    EXPECT_FALSE(sub_b.updated());
}

TEST(Datapool, GenerationWrapAroundIsNotMistakenForNoData) {
    struct T : datapool::Topic<Sample> {};
    datapool::Publisher<T> pub;
    datapool::Subscriber<T> sub;
    Sample sample{};

    datapool::detail::slots<T>[0].generation = std::numeric_limits<uint32_t>::max();
    sub.copy(&sample);

    pub.publish({2});

    EXPECT_TRUE(sub.update(&sample));
    EXPECT_EQ(sample.value, 2);
}

}  // namespace
