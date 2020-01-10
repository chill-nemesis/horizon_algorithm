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

#include "algorithm/concurrent_base.hpp"
#include <atomic>

namespace HORIZON::ALGORITHM
{
    /*!
     * TODO: is this really useful? In theory, anyone could lock, store a reference to an element and unlock, thus
     *      circumventing the lock mechanic...
     *
     */

    class concurrent_base_lock : public virtual concurrent_base
    {
    private:
        bool               _isLocked = false;
        mutable std::mutex _lockAccess;

    public:
        /*!
         * Locks the container.
         * Blocks if the lock is not available.
         */
        void lock()
        {
            _containerAccess.lock();
            _isLocked = true;
        }

        /*!
         * Unlocks the container.
         */
        void unlock()
        {
            LockType lock(_lockAccess);

            if (_isLocked) return;

            _containerAccess.unlock();
            _isLocked = false;
        }

        /*!
         * Tries to lock the container.
         * @return True if the container was locked successfully, otherwise false.
         */
        bool try_lock()
        {
            LockType lock(_lockAccess);

            if (_containerAccess.try_lock())
            {
                _isLocked = true;
                return true;
            }

            return false;
        }


        /*!
         * Returns true if the container is locked.
         */
        inline bool IsLocked() const
        {
            LockType lock(_lockAccess);
            return _isLocked;
        }

    protected:
        /*!
         * Checks if the container is frozen.
         * If the container is not safe, this method throws a runtime error.
         *
         * NOTE: If the library is compiled with HORIZON_NO_FROZEN_CONTAINER flag enabled, this method does nothing.
         *       Methods relying on this for save access become unsafe!
         */
        inline void CheckForLocked() const
        {
            #ifndef HORIZON_NO_FROZEN_CONTAINER
            if (!IsLocked()) throw std::runtime_error("Container is not locked!. To disable this error, compile HORIZON with HORIZON_NO_FROZEN_CONTAINER!");
            #else
            #warning "Potential unsafe container access!"
            #endif
        }
    };
}