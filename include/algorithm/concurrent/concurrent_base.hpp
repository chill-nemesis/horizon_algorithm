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
#include <atomic>

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

    protected:
        mutable std::mutex          _containerAccess;
        std::condition_variable_any _containerCV;

    public:
        /*!
         * Locks the container and returns an access token for concurrent calls to the container
         * Blocks if the lock is not available.
         */
        inline access_token TakeOwnership() const noexcept
        {
            // TODO: the access token MUST be the token of the container, how to do?
            return std::move(access_token(_containerAccess));
            // return access_token(_containerAccess);
        }
    };
}