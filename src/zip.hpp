#ifndef ZIP_H
#define ZIP_H

// https://gist.github.com/andesyv/5d4734568fd4a843cb5bbe0407ee2cec

#include <iterator>
#include <tuple>
#include <utility>

template <class ... Ts>
class Zipper {
public:
    std::tuple<Ts ...> containers;

    template <typename ... Args>
    Zipper(Args&& ... args) : containers{std::forward<Args>(args)...} {}

    template <std::random_access_iterator ... Its>
    class ZipIterator {
    public:
        std::tuple<Its ...> its;

        ZipIterator(Its&& ... args) : its{std::forward<Its>(args)...} {}

        auto operator*() {
            return std::apply([](auto&& ... it){ return std::tie(*it ...); }, its);
        }

        auto operator++() {
            std::apply([](auto& ... it){ ((++it), ...); }, its);
            return *this;
        }
        
        auto operator<=>(const ZipIterator& rhs) const = default; // lol
    };

    auto begin() const {
        return std::apply([](auto&& ... args){ return ZipIterator{std::begin(args)... }; }, containers);
    }

    auto end() const {
        return std::apply([](auto&& ... args){ return ZipIterator{std::end(args)... }; }, containers);
    }
};

template <typename ... T>
auto zip(T&& ... t) {
    return Zipper<T...>{std::forward<T>(t)...};
}

#endif // ZIP_H