//
// @brief   
// @details 
// @author  Steffen Peikert (ch3ll)
// @email   Horizon@ch3ll.com
// @version 1.0.0
// @date    04/01/2020 16:58
// @project Horizon
//


#pragma once

#include <mutex>
#include <condition_variable>
#include <preprocessor/unique_lock.hpp>
#include <cassert>

#include "algorithm/concurrent/ContainerClosedError.hpp"

namespace HORIZON::ALGORITHM::CONCURRENT
{
    #if NDEBUG
    #define CheckForOwnership
    #else
    #define CheckForOwnership assert(token.mutex()->native_handle() == _containerAccess.native_handle())
    #endif

    /*!
     * TODO: is this really useful? In theory, anyone could lock, store a reference to an element and unlock, thus
     *      circumventing the lock mechanic...
     *
     */
    class concurrent_base
    {
    public:
        using access_token = std::unique_lock<std::mutex>;
        using time_type = std::chrono::nanoseconds;

    protected:
        mutable std::mutex               _containerAccess;
        std::condition_variable_any      _containerCV;
        constexpr static const time_type _maxWaitTime = time_type::max();

    private:
        std::atomic_bool _closeToken = false;

    public:
        /*!
         * Locks the container and returns an access token for concurrent calls to the container
         * Blocks if the lock is not available.
         */
        AddLockGuardsMutable(_containerAccess)


        /*!
         * True if the container is closed (elements cannot be modified).
         */
        [[nodiscard]] inline bool is_closed() const
        { return _closeToken; }

        /*!
         * Closes the container for modification and notifies all waiting threads.
         */
        void close()
        {
            //I need to lock access to closing while modifications might take place
            auto token = Guard();

            // set the token to true. If it was previously true, do not notify
            if (_closeToken.exchange(true)) return;

            // container was not closed, notify all waiting threads.
            _containerCV.notify_all();
        }

        /*!
         * (Re-) opens the queue.
         */
        inline void open() //Todo: enable overriding for containers where reopening is not possible?
        { _closeToken = false; }


        /*!
         * Returns true if the container is empty.
         */
        inline bool empty() const
        { return empty(Guard()); }

        /*!
         * Returns true if the container is empty.
         */
        virtual bool empty(access_token const& token) const = 0;


    protected:
        /*!
         * True if the container can be modified (close token is not set).
         */
        [[nodiscard]] inline bool CanModify() const
        { return !(is_closed()); }

        // /*!
        //  * Checks if the container is closed.
        //  * @throws ContainerClosedError if the container is closed.
        //  */
        // void ThrowIfClosed() const noexcept(false)
        // {
        //     if (!CanModify())
        //         throw ContainerClosedError();
        // }
    };
}