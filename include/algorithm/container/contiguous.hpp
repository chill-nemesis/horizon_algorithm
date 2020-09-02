//
// @brief   
// @details 
// @author  Steffen Peikert (ch3ll)
// @email   Horizon@ch3ll.com
// @version 1.0.0
// @date    14/07/2020 15:46
// @project Horizon
//


#pragma once

#include <vector>
#include <algorithm>
#include <exception>

namespace HORIZON::ALGORITHM::CONTAINER
{
    template<class T,
             class Identifier = size_t>
    class contiguous_unordered_set
    {
    public:
        using Node = std::pair<T, Identifier const>;

        using reference = Node&;
        using const_reference = Node const&;

    private:
        using Container = std::vector<Node>;
        using iterator = typename Container::iterator;

        Container  _data;
        Identifier _lastIdentifier;

    public:
        contiguous_unordered_set() noexcept:
                _data(),
                _lastIdentifier()
        { }

        explicit contiguous_unordered_set(size_t const& N) noexcept:
                contiguous_unordered_set()
        { _data.reserve(N); }

        contiguous_unordered_set(size_t const& count, T const& value)
        {
            //     TODO:
        }

        explicit contiguous_unordered_set(std::initializer_list<T> init)
        {
            //    TODO
        }

        contiguous_unordered_set(contiguous_unordered_set const& other) noexcept = default;

        contiguous_unordered_set(contiguous_unordered_set&& other) noexcept = default;

        [[nodiscard]] inline size_t empty() const noexcept
        { return _data.empty(); }

        inline size_t size() const noexcept
        { return _data.size(); }

        inline size_t max_size() const noexcept
        { return _data.max_size(); }

        inline size_t capacity() const noexcept
        { return _data.capacity(); }

        void shrink_to_fit() noexcept
        { _data.shrink_to_fit(); }

        void reserve(size_t size) noexcept
        { _data.reserve(size); }


        inline constexpr reference operator[](Identifier pos)
        { return at(pos); }

        inline constexpr const_reference operator[](Identifier pos) const
        { return at(pos); }

        inline constexpr reference at(Identifier pos)
        {
            iterator it;
            if (!GetIterator(pos, it)) throw std::out_of_range("Identifier is not valid!");

            return *it;
        }

        inline constexpr const_reference at(Identifier pos) const
        { return at(pos); }

        //    TODO: data?



        void clear() noexcept
        {
            _data.clear();
            _lastIdentifier.exchange(Identifier());
        }

        Identifier add(T const& value)
        {
            auto currentID = _lastIdentifier++;

            _data.emplace_back({ value, currentID });
            return currentID;
        }

        Identifier add(T&& value)
        {
            auto currentID = _lastIdentifier++;

            _data.emplace_back({ value, currentID });
            return currentID;
        }

        constexpr void swap(contiguous_unordered_set& other) noexcept
        {
            using std::swap;
            swap(_data, other._data);
            swap(_lastIdentifier, other._lastIdentifier);
        }

        std::pair<bool, Node> remove(Identifier id)
        {
            // check if the element does exist
            iterator it;
            if (!GetIterator(id, it)) return { false, Node() };

            // swap element for deletion to end
            std::iter_swap(it, _data.end() - 1);

            // get element from end of container
            Node element = std::move(_data.back());
            // remove element from container
            _data.pop_back();

            return { true, element };
        }

    private:
        inline bool GetIterator(Identifier const& id, iterator& it) const noexcept
        {
            it = std::find_if(_data.begin(), _data.end(), [&id](Node const& node) { return node.second == id; });
            return it == _data.end();
        }
    };

    template<class T,
             class Identifier>
    constexpr void swap(contiguous_unordered_set<T, Identifier>& lhs,
                        contiguous_unordered_set<T, Identifier>& rhs) noexcept
    { lhs.swap(rhs); }
}