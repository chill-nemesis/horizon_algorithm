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

#include <queue>
#include "algorithm/concurrent_base_lock.hpp"

namespace HORIZON::ALGORITHM
{
    template<typename T,
             typename Container = std::deque<T>>
    class concurrent_queue : public virtual concurrent_base_lock
    {
    public:
        typedef typename Container::value_type      value_type;
        typedef typename Container::reference       reference;
        typedef typename Container::const_reference const_reference;
        typedef typename Container::size_type       size_type;
        typedef Container                           container_type;

    private:
        std::queue<T, container_type> _container;
        bool                          _closeToken = false;

    public:
        //TODO constructor with allocators?

        ~concurrent_queue()
        { close(); }


        /*!
         * Returns true if the %concurrent_queue is empty.
         */
        [[nodiscard]] bool empty() const
        {
            __HORIZON_CONCURRENT_ACQUIRE_LOCK(lock)

            return _container.empty();
        }

        /*!
         * Returns the number of elements in the %concurrent_queue.
         */
        size_type size() const
        {
            __HORIZON_CONCURRENT_ACQUIRE_LOCK(lock)

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
        {
            //TODO is this valid?
            // push(std::forward<value_type>(item));

            if (!CanModify()) return;

            {
                __HORIZON_CONCURRENT_ACQUIRE_LOCK(lock)
                _container.push(item);
            }

            _containerCV.notify_one();
        }

        void push(value_type&& item)
        {
            if (!CanModify()) return;

            {
                __HORIZON_CONCURRENT_ACQUIRE_LOCK(lock)
                _container.push(std::move(item));
            }

            _containerCV.notify_one();
        }

        // the return value is NOT decltype(auto)
        // because no reference is passed outside (requires freezing, etc.)
        template<typename... Args>
        void emplace(Args&& ... args)
        {
            if (!CanModify()) return;

            __HORIZON_CONCURRENT_ACQUIRE_LOCK(lock)
            _container.emplace(std::forward<Args>(args)...);

            lock.unlock();
            _containerCV.notify_one();
        }

        /*!
         * Tries to remove the first element. If no element is available, this method returns immediatley with the result of false.
         * @return True if an element could be removed.
         */
        bool try_pop(T& item)
        {
            __HORIZON_CONCURRENT_ACQUIRE_LOCK(lock)

            if (!CanModify()) return false;

            item = std::move(_container.front());
            _container.pop();

            return true;
        }


        /*!
         * Removes the first element.
         * The calling thread is blocked until an element can be retrieved or until a user specified timeout is reached.
         * If the timeout is reached, no element is removed and this method returns false.
         *
         * @param maximumWaitTime   The maximum time to wait until the pop operations is aborted.
         * @return                  True if an element could be removed.
         */
        bool pop(std::chrono::nanoseconds const& maximumWaitTime = std::chrono::nanoseconds::max())
        {
            __HORIZON_CONCURRENT_ACQUIRE_LOCK(lock)
            // do not use empty(), because lock is kept!
            if (!WaitForElementsInQueue(lock, maximumWaitTime, [] { return false; }))
                return false;

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
        bool pop(T& item, std::chrono::nanoseconds const& maximumWaitTime = std::chrono::nanoseconds::max())
        {
            __HORIZON_CONCURRENT_ACQUIRE_LOCK(lock)
            if (!WaitForElementsInQueue(lock, maximumWaitTime))
                return false;

            item = std::move(_container.front());
            _container.pop();

            return true;
        }

        bool swap(concurrent_queue& other)
        {
            // todo: throw?
            if (!(CanModify() && other.CanModify())) return false;

            __HORIZON_CONCURRENT_ACQUIRE_LOCK_TARGET(otherLock, other)
            __HORIZON_CONCURRENT_ACQUIRE_LOCK(myLock)

            std::swap(_container, other._container);

            other._containerCV.notify_one();
            _containerCV.notify_one();

            return true;
        }


        void clear()
        {
            __HORIZON_CONCURRENT_ACQUIRE_LOCK(lock)

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
        inline bool WaitForElementsInQueue(LockType& lock, std::chrono::nanoseconds const& waitTime)
        {
            if (is_closed()) return false;

            // do not use empty(), because lock is kept!
            if (!_containerCV.wait_for(lock, waitTime, [this] { return !_container.empty() || _closeToken; }))
                return false; // timeout

            return !_closeToken; //
        }


        [[nodiscard]] inline bool CanModify() const
        { return !(is_closed() || IsLocked()); }

    };
}