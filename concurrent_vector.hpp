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

// #pragma error "Unsupported file included (concurrent_vector)"

#include "algorithm/concurrent_base_lock.hpp"
#include <vector>
#include <algorithm>

namespace HORIZON::ALGORITHM
{
    /*!
     * A concurrent vector.
     *
     * Note: this implementation is by no means complete and gets extended upon need!
     */
    template<typename T,
             typename Alloc = std::allocator<T>>
    class concurrent_vector : public virtual concurrent_base_lock
    {
    public:
        using value_type = T;
        using allocator_type = Alloc;
        using container_type = std::vector<value_type, allocator_type>;
        using size_type = typename container_type::size_type;

        using reference = value_type&;
        using const_reference =  value_type const&;
        using access_type = size_type;

        using iterator = typename container_type::iterator;
        using const_iterator = typename container_type::const_iterator;
        using reverse_iterator = typename container_type::reverse_iterator;
        using const_reverse_iterator = typename container_type::const_reverse_iterator;


    private:
        container_type _container;
    public:
        //    TODO construcotrs


        [[nodiscard]] inline size_type capacity() const
        {
            __HORIZON_CONCURRENT_ACQUIRE_LOCK(lock)
            return this->_container.capacity();
        }

        [[nodiscard]] inline size_type size() const
        {
            __HORIZON_CONCURRENT_ACQUIRE_LOCK(lock)
            return _container.size();
        }

        [[nodiscard]] inline bool empty() const
        {
            __HORIZON_CONCURRENT_ACQUIRE_LOCK(lock)
            return _container.empty();
        }


        void resize(size_type size)
        {
            __HORIZON_CONCURRENT_ACQUIRE_LOCK(lock)
            this->_container.resize(size);
        }

        void resize(size_type size, value_type const& def)
        {
            __HORIZON_CONCURRENT_ACQUIRE_LOCK(lock)
            this->_container.resize(size, def);
        }

        void reserve(size_type size)
        {
            __HORIZON_CONCURRENT_ACQUIRE_LOCK(lock)
            this->_container.reserve(size);
        }

        void push_back(value_type const& item)
        {
            __HORIZON_CONCURRENT_ACQUIRE_LOCK(lock)

            this->_container.push_back(item);
        }

        void push_back(value_type&& item)
        {
            __HORIZON_CONCURRENT_ACQUIRE_LOCK(lock)
            this->_container.push_back(std::move(item));
        }


        /*!
         * Swaps the given element with the last element.
         * @param id The id of the element.
         */
        void swap(access_type const& first, access_type const& second)
        {
            // need to call freeze because of iterators
            // TODO: this might block if there is an error thrown...


            this->freeze();
            std::iter_swap(begin() + first, begin() + second);
            this->unfreeze();
        }

        void swap(access_type const& id)
        {
            // need to call freeze because of iterators
            // TODO: this might block if there is an error thrown...
            this->freeze();
            std::iter_swap(id, end() - 1);
            this->unfreeze();
        }


        /*!
         * Gets an iterator to the beginning of the container.
         */
        iterator begin_unsafe() noexcept
        {
            return _container.begin();
        }


        /*!
         * Gets a reverse iterator to the beginning of the container.
         */
        reverse_iterator rbegin_unsafe() noexcept
        {
            return _container.rbegin();
        }

        /*!
         * Gets an iterator to the end of the container.
         *
         * This is unsafe per default.
         */
        iterator end_unsafe() noexcept
        {
            return _container.end();
        }

        /*!
         * Gets a reverse iterator to the end of the container.
         *
         * This is unsafe per default.
         */
        reverse_iterator rend_unsafe() noexcept
        {
            return _container.rend();
        }


        /*!
         * Gets an iterator to the beginning of the container.
         */
        const_iterator begin() const noexcept
        {
            CheckForLocked();
            return _container.begin();
        }

        /*!
         * Gets a reversed iterator to the beginning of the container.
         */
        const_reverse_iterator rbegin() const noexcept
        {
            CheckForLocked();
            return _container.rbegin();
        }

        /*!
         * Gets an iterator to the end of the container.
         */
        const_iterator end() const noexcept
        {
            CheckForLocked();
            return _container.end();
        }

        /*!
         * Gets a reverse iterator to the end of the container.
         */
        const_reverse_iterator rend() const noexcept
        {
            CheckForLocked();
            return _container.rend();
        }


        void clear() noexcept
        {
            __HORIZON_CONCURRENT_ACQUIRE_LOCK(lock)

            _container.clear();
        }

    };
}