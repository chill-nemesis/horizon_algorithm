//
// @brief   
// @details 
// @author  Steffen Peikert (ch3ll)
// @email   Horizon@ch3ll.com
// @version 1.0.0
// @date    04/01/2020 16:40
// @project Horizon
//


#pragma once

#include "algorithm/concurrent/concurrent_base.hpp"
#include <unordered_map>

namespace HORIZON::ALGORITHM::CONCURRENT
{
    template<typename Key,
             typename T,
             typename Hash = std::hash<Key>,
             typename KeyEqual = std::equal_to<Key>,
             typename Allocator = std::allocator<std::pair<Key const, T>>>
    class concurrent_unorderedmap : public virtual concurrent_base
    {
    public:
        using container_type = std::unordered_map<Key, T, Hash, KeyEqual, Allocator>;
        using key_type = typename container_type::key_type;
        using mapped_type = typename container_type::mapped_type;
        using value_type = typename container_type::value_type;
        using size_type = typename container_type::size_type;
        using node_type = typename container_type::node_type;

        using time_type = std::chrono::nanoseconds;
    private:
        container_type                   _container;
        constexpr static const time_type _maxWaitTime = time_type::max();


    public:
        // iterators
        // class iterator
        // {
        // public:
        //     using self_type = iterator;
        //
        // private:
        //     using iterator_type = typename container_type::iterator;
        //     using reference_type = typename iterator_type::reference;
        //     using pointer_type = typename iterator_type::pointer;
        //
        // private:
        //     iterator_type& _containerIterator;
        //     access_token & _token;
        //
        // public:
        //     iterator(iterator_type& iterator, access_token& token) :
        //             _containerIterator(iterator),
        //             _token(token)
        //     { }
        //
        //     [[nodiscard]] access_token const& Token() const noexcept
        //     { return _token; }
        //
        //     self_type operator++() noexcept
        //     {
        //         _containerIterator.operator++();
        //         return *this;
        //     }
        //
        //     reference_type operator*() const noexcept
        //     { return _containerIterator.operator*(); }
        //
        //     pointer_type operator->() const noexcept
        //     { return _containerIterator.operator->(); }
        // };
        //
        //
        // using iterator_type = iterator;

    public:
        // TODO: constructors

        ~concurrent_unorderedmap()
        { close(); }

        inline size_type size() const
        { return size(std::move(Guard())); }

        inline size_type size(access_token const& token) const
        {
            CheckForOwnership;
            return _container.size();
        }

        using concurrent_base::empty;

        inline bool empty(access_token const& token) const override
        {
            CheckForOwnership;
            return _container.empty();
        }

        inline size_type max_size() const
        { return max_size(std::move(Guard())); }

        inline size_type max_size(access_token const& token) const
        {
            CheckForOwnership;
            return _container.max_size();
        }

        void clear()
        { clear(std::move(Guard())); }

        void clear(access_token const& token)
        {
            CheckForOwnership;
            _container.clear();
        }

        void insert(value_type const& value)
        { insert(value, std::move(Guard())); }

        void insert(value_type const& value, access_token const& token)
        {
            CheckForOwnership;

            ThrowIfClosed();

            _container.insert(value);
            _containerCV.notify_one();
        }

        void insert(value_type&& value)
        { insert(std::forward<value_type>(value), std::move(Guard())); }

        void insert(value_type&& value, access_token const& token)
        {
            CheckForOwnership;

            ThrowIfClosed();

            _container.insert(std::move(value));
            _containerCV.notify_one();
        }

        template<class P>
        void insert(P&& value)
        { insert(std::forward<P>(value), std::move(Guard())); }

        template<class P>
        void insert(P&& value, access_token const& token)
        {
            CheckForOwnership;

            ThrowIfClosed();

            _container.insert(std::forward<P>(value));
            _containerCV.notify_one();
        }

        template<class InputIt>
        void insert(InputIt first, InputIt last)
        { insert(first, last, std::move(Guard())); }

        template<class InputIt>
        void insert(InputIt first, InputIt last, access_token const& token)
        {
            CheckForOwnership;

            ThrowIfClosed();
            _container.insert(first, last);

            _containerCV.notify_all();
        }


        void insert(std::initializer_list<value_type> ilist)
        { insert(ilist, std::move(Guard())); }

        void insert(std::initializer_list<value_type> ilist, access_token const& token)
        {
            CheckForOwnership;

            ThrowIfClosed();

            _container.insert(ilist);
            _containerCV.notify_all();
        }

        void insert(node_type&& nh)
        { insert(std::forward(nh), std::move(nh)); }

        void insert(node_type&& nh, access_token const& token)
        {
            CheckForOwnership;

            ThrowIfClosed();

            _container.insert(std::forward(nh));
            _containerCV.notify_all();
        }

        template<class M>
        bool insert_or_assign(key_type const& k, M&& value)
        { return _container.insert_or_assign(k, std::forward(value), std::move(Guard())).second; }

        template<class M>
        bool insert_or_assign(key_type const& k, M&& value, access_token const& token)
        {
            CheckForOwnership;

            ThrowIfClosed();

            return _container.insert_or_assign(k, std::forward(value)).second;
        }

        template<class... Args>
        bool emplace(Args&& ...args)
        { return emplace_helper(std::move(Guard()), std::forward<Args>(args)...); }

        template<class... Args>
        bool emplace(access_token const& token, Args&& ...args)
        { return emplace_helper(token, std::forward<Args>(args)...); }


        void swap(concurrent_unorderedmap& other)
        { swap(other, std::move(Guard()), std::move(other.Guard())); }

        void swap(concurrent_unorderedmap& other, access_token const& myToken, access_token const& otherToken)
        {
            assert(myToken.mutex()->native_handle() == _containerAccess.native_handle());
            assert(otherToken.mutex()->native_handle() == other._containerAccess.native_handle());

            // make sure both containers are not closed
            ThrowIfClosed();
            other.ThrowIfClosed();

            std::swap(_container, other._container);

            other._containerCV.notify_all();
            _containerCV.notify_all();
        }

        size_type erase(key_type const& key)
        { return erase(key, std::move(Guard())); }

        size_type erase(key_type const& key, access_token const& token)
        {
            CheckForOwnership;
            ThrowIfClosed();

            return _container.erase(key);
        }


        // require external locking
        /*!
         * Returns a reference to the mapped value of the element with key.
         * If no such element exists, an exception of type std::out_of_range is thrown.
         */
        T& at(key_type const& key, access_token const& token)
        {
            CheckForOwnership;

            // reference version, this introduces race conditions!
            // TODO: lock the container
            return _container.at(key);
        }

        /*!
         * Replacement for bracket operator.
         * Returns the reference to the value that is mapped to a key, performing an insertion if such key does not already exist.
         */
        T& operator()(key_type const& key, access_token const& token)
        {
            CheckForOwnership;

            // TODO: lock the container
            return _container[key];
        }

        T& operator()(key_type&& key, access_token const& token)
        {
            CheckForOwnership;

            // TODO: lock the container
            return _container[std::forward<key_type>(key)];
        }

        size_type count(key_type const& key) const
        { return count(key, std::move(Guard())); }

        size_type count(key_type const& key, access_token const& token) const
        {
            CheckForOwnership;

            return _container.count(key);
        }


        bool find(key_type const& key, T& item, access_token const& token)
        {
            CheckForOwnership;

            auto it = _container.find(key);
            if (it == _container.end()) return false;

            item = it->second;
            return true;
        }

        T& get_first(access_token const& token)
        {
            CheckForOwnership;

            auto it = _container.begin();
            if (it == _container.end()) throw std::out_of_range("No first element.");

            return it->second;
        }

        T& pop_first()
        { return pop_first(std::move(Guard())); }

        T& pop_first(access_token const& token)
        {
            CheckForOwnership;

            auto it = _container.begin();
            if (it == _container.end()) throw std::out_of_range("No first element.");

            // remove item
            auto item = it->second;
            _container.erase(it);

            return item;
        }

        bool pop_key(key_type const& key, T& item)
        { return pop_key(key, item, std::move(Guard())); }

        bool pop_key(key_type const& key, T& item, access_token const& token)
        {
            CheckForOwnership;

            // find the item
            if (find(key, item, token))
            {
                // and remove it
                erase(key, token);
                return true;
            }
            // item does not exist
            return false;
        }

        // iterator_type find(key_type const& key)
        // {
        //     auto token = Guard();
        //     auto it    = _container.find(key);
        //
        //     return iterator_type(it, token);
        // }
        //
        // iterator_type begin()
        // {
        //     auto token = Guard();
        //     return begin(token);
        // }
        //
        // iterator_type begin(access_token& token)
        // { return iterator_type(_container.begin(), token); }
        //
        // iterator_type end()
        // { return end(std::move(Guard())); }
        //
        // iterator_type end(access_token& token)
        // { return iterator_type(_container.end(), token); }


    private:
        template<class ... Args>
        bool emplace_helper(access_token const& token, Args&& ...args)
        {
            CheckForOwnership;
            ThrowIfClosed();

            auto result = _container.emplace(std::forward<Args>(args)...).second;
            _containerCV.notify_all();
            return result;
        }

    };
}