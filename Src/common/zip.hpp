/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_COMMON_ZIP_HPP_
#define SRC_COMMON_ZIP_HPP_

#include <tuple>
#include <iterator>
#include <utility>

template <typename It1, typename It2>
class zip_iterator {
public:
    using reference = std::tuple<decltype(*std::declval<It1>()), decltype(*std::declval<It2>())>;

    zip_iterator(It1 it1, It2 it2)
        : _it1(it1), _it2(it2) {}

    reference operator*() const {
        return {*_it1, *_it2};
    }

    zip_iterator& operator++() {
        ++_it1;
        ++_it2;
        return *this;
    }

    bool operator!=(const zip_iterator& other) const {
        return _it1 != other._it1 && _it2 != other._it2;
    }

private:
    It1 _it1;
    It2 _it2;
};

template <typename Container1, typename Container2>
class zip_range {
public:
    zip_range(Container1& c1, Container2& c2)
        : _c1(c1), _c2(c2) {}

    auto begin() {
        return zip_iterator<decltype(std::begin(_c1)), decltype(std::begin(_c2))>(std::begin(_c1), std::begin(_c2));
    }

    auto end() {
        return zip_iterator<decltype(std::end(_c1)), decltype(std::end(_c2))>(std::end(_c1), std::end(_c2));
    }

private:
    Container1& _c1;
    Container2& _c2;
};

template <typename Container1, typename Container2>
auto zip(Container1& c1, Container2& c2) {
    return zip_range<Container1, Container2>(c1, c2);
}

#endif  // SRC_COMMON_ZIP_HPP_
