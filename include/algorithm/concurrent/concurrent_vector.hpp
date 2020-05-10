//
// @brief   
// @details 
// @author  Steffen Peikert (ch3ll)
// @email   Horizon@ch3ll.com
// @version 1.0.0
// @date    01/01/2020 13:58
// @project Horizon
//


#pragma once

#include "concurrent_base.hpp"
#include <vector>
#include <algorithm>

namespace HORIZON::ALGORITHM::CONCURRENT
{
    /*!
     * A concurrent vector.
     *
     * Note: this implementation is by no means complete and gets extended upon need!
     */
    template<typename T,
             typename Alloc = std::allocator<T>>
    class concurrent_vector : public virtual concurrent_base
    {
    public:
        using value_type = T;
        using allocator_type = Alloc;
        using container_type = std::vector<value_type, allocator_type>;
        using size_type = typename container_type::size_type;

        using reference = value_type&;
        using const_reference = value_type const&;
        using access_type = size_type;

        using iterator = typename container_type::iterator;
        using const_iterator = typename container_type::const_iterator;
        using reverse_iterator = typename container_type::reverse_iterator;
        using const_reverse_iterator = typename container_type::const_reverse_iterator;


    private:
        container_type _container;
    public:
        //    TODO construcotrs


        /*!
         * Gets the capacity of the container.
         */
        [[nodiscard]] inline size_type capacity() const
        { return capacity(std::move(Guard())); }

        [[nodiscard]] inline size_type capacity(access_token const& token) const
        {
            CheckForOwnership;
            return _container.capacity();
        }

        /*!
         * Gets the size of the container.
         */
        [[nodiscard]] inline size_type size() const
        { return size(std::move(Guard())); }

        [[nodiscard]] inline size_type size(access_token const& token) const
        {
            CheckForOwnership;
            return _container.size();
        }

        /*!
         * True if the container is empty.
         */
        [[nodiscard]] inline bool empty() const
        { return empty(std::move(Guard())); }

        [[nodiscard]] inline bool empty(access_token const& token) const
        {
            CheckForOwnership;
            return _container.empty();
        }


        /*!
         * Resizes the container.
         */
        void resize(size_type size)
        { resize(size, std::move(Guard())); }

        void resize(size_type size, access_token const& token) const
        {
            CheckForOwnership;
            _container.resize(size);
        }

        /*!
         * Resizes the container and initialises with a default element.
         */
        void resize(size_type size, value_type const& def)
        { resize(size, def, std::move(Guard())); }

        void resize(size_type size, value_type const& def, access_token const& token)
        {
            CheckForOwnership;
            _container.resize(size, def);
        }

        /*!
         * Reserves space in the container wihtout initialising.
         */
        void reserve(size_type size)
        { reserve(size, std::move(Guard())); }

        void reserve(size_type size, access_token const& token)
        {
            CheckForOwnership;
            _container.reserve(size);
        }

        /*!
         * Places the given element at the end of the vector.
         */
        void push_back(value_type const& item)
        { push_back(item, std::move(Guard())); }

        void push_back(value_type const& item, access_token const& token)
        {
            CheckForOwnership;

            _container.push_back(item);
        }

        void push_back(value_type&& item)
        { push_back(std::move(item), std::move(Guard())); }

        void push_back(value_type&& item, access_token const& token)
        {
            CheckForOwnership;
            _container.push_back(std::move(item));
        }


        /*!
         * Swaps the first element with the second element.
         */
        void swap(access_type const& first, access_type const& second)
        { swap(first, second, std::move(Guard())); }

        void swap(access_type const& first, access_type const& second, access_token const& token)
        {
            CheckForOwnership;
            std::iter_swap(begin() + first, begin() + second);
        }

        /*!
         * Swaps the given element with the last element.
         * @param id The id of the element.
         */
        void swap(access_type const& id)
        { swap(id, std::move(Guard())); }


        void swap(access_type const& id, access_token const& token)
        {
            CheckForOwnership;

            // TODO: this might block if an error is thrown...
            std::iter_swap(id, end(token) - 1);
        }


        /*!
         * Gets an iterator to the beginning of the container.
         */
        iterator begin(access_token const& token) noexcept
        {
            CheckForOwnership;
            return _container.begin();
        }


        /*!
         * Gets a reverse iterator to the beginning of the container.
         */
        reverse_iterator rbegin(access_token const& token) noexcept
        {
            CheckForOwnership;
            return _container.rbegin();
        }

        /*!
         * Gets an iterator to the end of the container.
         *
         * This is unsafe per default.
         */
        iterator end(access_token const& token) noexcept
        {
            CheckForOwnership;
            return _container.end();
        }

        /*!
         * Gets a reverse iterator to the end of the container.
         *
         * This is unsafe per default.
         */
        reverse_iterator rend(access_token const& token) noexcept
        {
            CheckForOwnership;
            return _container.rend();
        }


        /*!
         * Gets an iterator to the beginning of the container.
         */
        const_iterator begin(access_token const& token) const noexcept
        {
            CheckForOwnership;
            return _container.begin();
        }

        /*!
         * Gets a reversed iterator to the beginning of the container.
         */
        const_reverse_iterator rbegin(access_token const& token) const noexcept
        {
            CheckForOwnership;
            return _container.rbegin();
        }

        /*!
         * Gets an iterator to the end of the container.
         */
        const_iterator end(access_token const& token) const noexcept
        {
            CheckForOwnership;
            return _container.end();
        }

        /*!
         * Gets a reverse iterator to the end of the container.
         */
        const_reverse_iterator rend(access_token const& token) const noexcept
        {
            CheckForOwnership;
            return _container.rend();
        }


        void clear() noexcept
        { clear(std::move(Guard())); }

        void clear(access_token const& token) noexcept
        {
            CheckForOwnership;

            _container.clear();
        }
    };
}