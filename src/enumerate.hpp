#include <cstddef>
#include <utility>

/*
Implementation of enumerate.
The function takes a container (array/vector/etc) and returns an Enumerator
object. Enumerator implements the standard iterator pattern, implementing
begin() and end() functions, which return Enumerator::Iterator. Dereferencing
Enumerator::Iterator returns EnumeratedIteratorResult, which has custom
implementation of structured bindings.

References:
    Structured binding:
        https://jguegant.github.io/blogs/tech/structured-bindings.html
    Custom iterators:
        https://internalpointers.com/post/writing-custom-iterators-modern-cpp
*/

template <typename IteratorType, typename ValueType>
struct EnumeratedIteratorResult {
    using index_type = size_t;
    using value_type = ValueType;
    using ref_type = ValueType&;
    index_type index;   // NOLINT(misc-non-private-member-variables-in-classes)
    IteratorType value; // NOLINT(misc-non-private-member-variables-in-classes)

    EnumeratedIteratorResult(index_type index, IteratorType value)
        : index(index), value(value) {}
};

// The following template specializations of tuple_size and tuple_element and
// `get` are required for structured bindings to work.
template <typename IteratorType, typename ValueType>
struct std::tuple_size<EnumeratedIteratorResult<IteratorType, ValueType>> {
    static constexpr size_t value = 2;
};

template <typename IteratorType, typename ValueType>
struct std::tuple_element<0,
                          EnumeratedIteratorResult<IteratorType, ValueType>> {
    using type = const typename EnumeratedIteratorResult<IteratorType,
                                                         ValueType>::index_type;
};
template <typename IteratorType, typename ValueType>
struct std::tuple_element<1,
                          EnumeratedIteratorResult<IteratorType, ValueType>&> {
    using type =
        typename EnumeratedIteratorResult<IteratorType, ValueType>::ref_type;
};
template <typename IteratorType, typename ValueType>
struct std::tuple_element<
    1, const EnumeratedIteratorResult<IteratorType, ValueType>&> {
    using type = const typename EnumeratedIteratorResult<IteratorType,
                                                         ValueType>::ref_type;
};
template <typename IteratorType, typename ValueType>
struct std::tuple_element<1,
                          EnumeratedIteratorResult<IteratorType, ValueType>> {
    using type =
        typename EnumeratedIteratorResult<IteratorType, ValueType>::value_type;
};
template <typename IteratorType, typename ValueType>
struct std::tuple_element<
    1, const EnumeratedIteratorResult<IteratorType, ValueType>> {
    using type = const typename EnumeratedIteratorResult<IteratorType,
                                                         ValueType>::value_type;
};

template <size_t Index, typename IteratorType, typename ValueType>
auto& get(EnumeratedIteratorResult<IteratorType, ValueType>& enumeratedValue)
{
    if constexpr (Index == 0)
    {
        return enumeratedValue.index;
    }
    else if constexpr (Index == 1)
    {
        return *enumeratedValue.value;
    }
}

template <size_t Index, typename IteratorType, typename ValueType>
const auto&
get(const EnumeratedIteratorResult<IteratorType, ValueType>& enumeratedValue)
{
    if constexpr (Index == 0)
    {
        return enumeratedValue.index;
    }
    else if constexpr (Index == 1)
    {
        return *enumeratedValue.value;
    }
}

template <typename Container, typename ContainerValue,
          typename ContainerIterator>
class Enumerator {
  public:
    explicit Enumerator(Container& container) : _container(container) {}

    class Iterator {
      public:
        Iterator(ContainerIterator containerIt, size_t index)
            : _containerIt(containerIt), _index(index),
              _enumeratedValue(index, containerIt) {}

        Iterator& operator++()
        {
            ++_containerIt;
            ++_index;
            _enumeratedValue.index = _index;
            _enumeratedValue.value = _containerIt;
            return *this;
        }

        Iterator operator++(int)
        {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const Iterator& other) const
        {
            return _containerIt == other._containerIt;
        }

        bool operator!=(const Iterator& other) const { return !(*this == other); }

        auto& operator*() { return _enumeratedValue; }

      private:
        EnumeratedIteratorResult<ContainerIterator, ContainerValue>
            _enumeratedValue{};
        ContainerIterator _containerIt;
        size_t _index;
    };

    Iterator begin() { return Iterator(_container.begin(), 0); }

    Iterator end() { return Iterator(_container.end(), _container.size()); }

  private:
    Container& _container;
};

/*
 * @brief Enumerate a container, similar to pythons enumerate and
 * std::ranges::views::enumerate (C++23:
 * https://en.cppreference.com/w/cpp/ranges/enumerate_view)
 *
 * Example usage:
 * `for (auto& [index, value] : enumerate(arr))`
 *
 * `const auto&` is also supported. Copying using just `auto` is not supported.
 */
template <typename Container>
Enumerator<Container, typename Container::value_type,
           typename Container::iterator>
enumerate(Container& container)
{
    return Enumerator<Container, typename Container::value_type,
                      typename Container::iterator>(container);
}

/*
 * @brief Enumerate a constant container, similar to pythons enumerate and
 * std::ranges::views::enumerate (C++23:
 * https://en.cppreference.com/w/cpp/ranges/enumerate_view)
 *
 * Example usage:
 * `for (const auto& [index, value] : MCUCommon::Tools::enumerate(arr))`
 *
 * Copying using just `auto` is not supported.
 */
template <typename Container>
Enumerator<const Container, const typename Container::value_type,
           typename Container::const_iterator>
enumerate(const Container& container)
{
    return Enumerator<const Container, const typename Container::value_type,
                      typename Container::const_iterator>(container);
}
