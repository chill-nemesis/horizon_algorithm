//
// @brief   
// @details 
// @author  Steffen Peikert (ch3ll)
// @email   Horizon@ch3ll.com
// @version 1.0.0
// @date    28/12/2019 21:46
// @project Horizon
//


#pragma once

#include "concurrent_base.hpp"
#include <queue>

namespace HORIZON::ALGORITHM::CONCURRENT
{
    template<typename T,
             typename Container = std::deque<T>>
    class concurrent_queue : public concurrent_base
    {
    public:
        using value_type = typename Container::value_type;
        using reference = typename Container::reference;
        using const_reference = typename Container::const_reference;
        using size_type = typename Container::size_type;
        using container_type = std::queue<value_type, Container>;

    private:
        container_type _container;

    public:
        //TODO constructor with allocators?

        ~concurrent_queue()
        { close(); }

        using concurrent_base::empty;

        [[nodiscard]] inline bool empty(access_token const& token) const override
        {
            CheckForOwnership;
            return _container.empty();
        }


        /*!
         * Returns the number of elements in the %concurrent_queue.
         */
        [[nodiscard]] inline size_type size() const
        { return size(std::move(Guard())); }

        [[nodiscard]] inline size_type size(access_token const& token) const
        {
            CheckForOwnership;
            return _container.size();
        }

        /*!
         * Add data to the end of the %queue.
         * @param item  Data to be added.
         *
         *  This is a typical %queue operation.  The function creates an
         *  element at the end of the %queue and assigns the given data
         *  to it.  The time complexity of the operation depends on the
         *  underlying sequence.
         */
        void push(value_type const& item) noexcept
        { push(item, std::move(Guard())); }

        void push(value_type&& item) noexcept
        { push(std::forward<value_type>(item), std::move(Guard())); }

        // we need both variants (first for const&, second for move)

        void push(value_type const& item, access_token const& token) noexcept
        {
            CheckForOwnership;

            // ThrowIfClosed();

            _container.push(item);

            _containerCV.notify_one();
        }

        void push(value_type&& item, access_token const& token) noexcept
        {
            CheckForOwnership;

            // ThrowIfClosed();

            _container.push(std::move(item));

            _containerCV.notify_one();
        }

        // the return value is NOT decltype(auto)
        // because no reference is passed outside (requires freezing, etc.)
        template<class... Args>
        void emplace(Args&& ... args) noexcept
        { emplace_helper(std::move(Guard()), std::forward<Args>(args) ...); }


        template<class ... Args>
        reference emplace(access_token const& token, Args&& ... args) noexcept
        { return emplace_helper(token, std::forward<Args>(args) ...); }

        /*!
         * Tries to remove the first element. If no element is available, this method returns immediately with the result of false.
         * @return True if an element could be removed.
         */
        inline bool try_pop(T& item)
        { return try_pop(item, std::move(Guard())); }

        inline bool try_pop(T& item, access_token&& token)
        { return pop(item, std::forward<access_token>(token), time_type::zero()); }


        [[maybe_unused]]
        /*!
         * Removes the first element.
         * The calling thread is blocked until an element can be retrieved or until a user specified timeout is reached.
         * If the timeout is reached, no element is removed and this method returns false.
         *
         * @param maximumWaitTime   The maximum time to wait until the pop operations is aborted.
         * @return                  True if an element could be removed.
         */
        bool pop(time_type const& maximumWaitTime = _maxWaitTime) noexcept
        {
            return pop(std::move(Guard()), maximumWaitTime);
        }

        bool pop(access_token& token, time_type const& maximumWaitTime = _maxWaitTime) noexcept
        {
            CheckForOwnership;

            if (!WaitForElementsInQueue(token, maximumWaitTime)) return false;

            _container.pop();
            return true;
        }

        /*!
         * Removes the first element.
         * The calling thread is blocked until an element can be retrieved or until a user specified timeout is reached.
         * If the timeout is reached, no element is removed and this method returns false.
         *
         * @param item              The removed element.
         * @param maximumWaitTime   The maximum time to wait until the pop operations is aborted.
         * @return                  True if an element could be removed.
         */
        bool pop(T& item, time_type const& maximumWaitTime = _maxWaitTime) noexcept
        {
            return pop(item, std::move(Guard()), maximumWaitTime);
        }

        bool pop(T& item, access_token&& token, time_type const& maximumWaitTime = _maxWaitTime) noexcept
        {
            CheckForOwnership;

            if (!WaitForElementsInQueue(token, maximumWaitTime)) return false;

            item = std::move(_container.front());
            _container.pop();

            return true;
        }

        void swap(concurrent_queue& other) noexcept
        { swap(other, std::move(Guard()), std::move(other.Guard())); }

        void swap(concurrent_queue& other, access_token const& myToken, access_token const& otherToken) noexcept
        {
            assert(myToken.mutex()->native_handle() == _containerAccess.native_handle());
            assert(otherToken.mutex()->native_handle() == other._containerAccess.native_handle());

            // make sure both containers are not closed
            // ThrowIfClosed();
            // other.ThrowIfClosed();

            std::swap(_container, other._container);

            other._containerCV.notify_all();
            _containerCV.notify_all();
        }


        void clear() noexcept
        { clear(std::move(Guard())); }

        void clear(access_token const& token) noexcept
        {
            CheckForOwnership;
            // ThrowIfClosed();

            std::queue<T> empty;
            std::swap(_container, empty);
        }

    private:
        template<class ... Args>
        reference emplace_helper(access_token const& token, Args&& ...args) noexcept
        {
            CheckForOwnership;

            // ThrowIfClosed();

            // emplace value and store result
            reference result = _container.emplace(std::forward<Args>(args)...);

            // notify possible waiting threads and return the result of emplace
            _containerCV.notify_all();
            return result;
        }

        /*!
         * Waits until the container is not empty.
         * Blocks the current thread execution until an element is in queue or the wait timeout is reached.
         * Returns true if an element is in queue.
         */
        inline bool WaitForElementsInQueue(access_token& token, time_type const& waitTime) noexcept
        {
            // assert(waitTime.count() > 0);

            // early checks for either empty or closed.
            // first check if we have elements in queue, then for closed
            // this order allows emptying the queue after it has been closed
            if (!empty(token)) return true;
            if (is_closed()) return false;

            if (!_containerCV.wait_for(token, waitTime, [this, &token] { return !empty(token) || is_closed(); }))
                return false; // timeout

            // ThrowIfClosed();
            return CanModify();
        }
    };
}