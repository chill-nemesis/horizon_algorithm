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

#define __HORIZON_CONCURRENT_ACQUIRE_LOCK_TARGET(name, target)  concurrent_base::LockType name(target._containerAccess);
#define __HORIZON_CONCURRENT_ACQUIRE_LOCK(name)                 concurrent_base::LockType name(this->_containerAccess);

namespace HORIZON::ALGORITHM
{
    class concurrent_base
    {
    protected:
        mutable std::mutex          _containerAccess;
        std::condition_variable_any _containerCV;


        typedef typename std::unique_lock<std::mutex> LockType;

    };
}