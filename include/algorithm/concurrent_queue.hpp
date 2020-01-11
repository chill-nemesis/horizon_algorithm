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

#include "algorithm/concurrent_base.hpp"
#include <cassert>
#include <queue>

namespace HORIZON::ALGORITHM
{
    template<typename T,
             typename Container = std::deque<T>>
    class concurrent_queue : public virtual concurrent_base
    {
    public:
        using value_type = typename Container::value_type;
        using reference = typename Container::reference;
        using const_reference = typename Container::const_reference;
        using size_type = typename Container::size_type;
        using container_type = std::queue<value_type, Container>;

        using time_type = std::chrono::nanoseconds;
    private:
        container_type _container;
        bool           _closeToken = false;

    public:
        //TODO constructor with allocators?

        ~concurrent_queue()
        { close(); }


        /*!
         * Returns true if the %concurrent_queue is empty.
         */
        [[nodiscard]] inline bool empty() const
        { return empty(TakeOwnership()); }

        [[nodiscard]] inline bool empty(access_token&& token) const
        {
            CheckForOwnership;
            return _container.empty();
        }


        /*!
         * Returns the number of elements in the %concurrent_queue.
         */
        [[nodiscard]] inline size_type size() const
        { return size(TakeOwnership()); }

        [[nodiscard]] inline size_type size(access_token&& token) const
        {
            CheckForOwnership;
            return _container.size();
        }

        /*!
         * True if the queue is open (elements can be accessed).
         */
        [[nodiscard]] inline bool is_closed() const
        { return _closeToken; }

        // TODO: maybe freeze the queue?
        // reference front();
        // const_reference front() const;
        // reference back();
        // const_reference back() const;

        /*!
         * Add data to the end of the %queue.
         * @param item  Data to be added.
         *
         *  This is a typical %queue operation.  The function creates an
         *  element at the end of the %queue and assigns the given data
         *  to it.  The time complexity of the operation depends on the
         *  underlying sequence.
         */
        void push(value_type const& item)
        { push(item, TakeOwnership()); }

        void push(value_type&& item)
        { push(std::forward<value_type>(item), TakeOwnership()); }

        void push(value_type&& item, access_token&& token)
        {
            CheckForOwnership;

            if (!CanModify()) return;

            _container.push(std::move(item));

            _containerCV.notify_one();
        }

        // the return value is NOT decltype(auto)
        // because no reference is passed outside (requires freezing, etc.)
        template<typename... Args>
        void emplace(Args&& ... args)
        { emplace(TakeOwnership(), std::forward<Args>(args) ...); }


        template<typename ... Args>
        void emplace(access_token&& token, Args&& ... args)
        {
            CheckForOwnership;

            if (!CanModify()) return;

            _container.emplace(std::forward<Args>(args)...);

            _containerCV.notify_all();
        }

        // TODO: implement me
        /*!
         * Tries to remove the first element. If no element is available, this method returns immediatley with the result of false.
         * @return True if an element could be removed.
         */
        // bool try_pop(T& item)
        // {
        //     __HORIZON_CONCURRENT_ACQUIRE_LOCK(lock)
        //
        //     if (!CanModify()) return false;
        //
        //     item = std::move(_container.front());
        //     _container.pop();
        //
        //     return true;
        // }


        /*!
         * Removes the first element.
         * The calling thread is blocked until an element can be retrieved or until a user specified timeout is reached.
         * If the timeout is reached, no element is removed and this method returns false.
         *
         * @param maximumWaitTime   The maximum time to wait until the pop operations is aborted.
         * @return                  True if an element could be removed.
         */
        bool pop(time_type const& maximumWaitTime = time_type::max())
        { return pop(TakeOwnership(), maximumWaitTime); }

        bool pop(access_token&& token, time_type const& maximumWaitTime = time_type::max())
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
        bool pop(T& item, time_type const& maximumWaitTime = time_type::max())
        { return pop(item, TakeOwnership(), maximumWaitTime); }

        bool pop(T& item, access_token&& token, time_type maximumWaitTime = time_type::max())
        {
            CheckForOwnership;

            if (!WaitForElementsInQueue(token, maximumWaitTime)) return false;

            item = std::move(_container.front());
            _container.pop();

            return true;
        }

        bool swap(concurrent_queue& other)
        { return swap(other, TakeOwnership(), other.TakeOwnership()); }

        bool swap(concurrent_queue& other, access_token&& myToken, access_token&& otherToken)
        {
            assert(myToken.mutex()->native_handle() == _containerAccess.native_handle());
            assert(otherToken.mutex()->native_handle() == other._containerAccess.native_handle());

            // todo: throw?
            if (!(CanModify() && other.CanModify())) return false;

            std::swap(_container, other._container);

            other._containerCV.notify_one();
            _containerCV.notify_one();

            return true;
        }


        void clear()
        { clear(TakeOwnership()); }

        void clear(access_token&& token)
        {
            CheckForOwnership;

            std::queue<T> empty;
            std::swap(_container, empty);
        }

        /*!
         * Closes the queue and notifies all waiting threads.
         */
        void close()
        {
            //TODO: atomic?
            if (_closeToken) return;
            _closeToken = true;

            _containerCV.notify_all();
        }

        /*!
         * (Re-) opens the queue.
         */
        void open()
        {
            //TODO: atomic?
            if (!_closeToken) return;

            _closeToken = false;
        }

    private:
        /*!
         * Waits until the container is not empty.
         * Blocks the current thread execution until an element is in queue or the wait timeout is reached.
         * Returns true if an element is in queue.
         */
        inline bool WaitForElementsInQueue(access_token&& token, time_type const& waitTime)
        {
            if (is_closed()) return false;

            if (!_containerCV.wait_for(token, waitTime, [this, token] { return empty(token) || _closeToken; }))
                return false; // timeout

            return !_closeToken;
        }


        [[nodiscard]] inline bool CanModify() const
        { return !(is_closed()); }

    };
}