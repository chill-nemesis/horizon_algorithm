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

#pragma error "Unsupported file included (concurrent_unorderedmap)"

#include "algorithm/concurrent_base_access.hpp"
#include <unordered_map>

namespace HORIZON::ALGORITHM::CONCURRENT
{
    template<typename Key,
             typename T,
             typename Hash = std::hash<Key>,
             typename KeyEqual = std::equal_to<Key>,
             typename Allocator = std::allocator<std::pair<Key const, T>>>
    class concurrent_unorderedmap : public virtual concurrent_base_access<concurrent_unorderedmap<Key, Hash, KeyEqual, Allocator>, T, Key>
    {
    public:
        typedef typename std::unordered_map<Key, T, Hash, KeyEqual, Allocator> container_type;
        typedef typename container_type::key_type                              key_type;
        typedef typename container_type::mapped_type                           mapped_type;
        typedef typename container_type::value_type                            value_type;
        typedef typename container_type::size_type                             size_type;


    private:
        container_type _container;

    public:
        // TODO: constructors

        // TODO: destructors

        inline size_type size() const
        {
            __HORIZON_CONCURRENT_ACQUIRE_LOCK(lock)
            return _container.size();
        }

        [[nodiscard]] inline bool empty() const
        {
            __HORIZON_CONCURRENT_ACQUIRE_LOCK(lock)
            return _container.empty();
        }

        inline size_type max_size() const
        {
            __HORIZON_CONCURRENT_ACQUIRE_LOCK(lock)
            return _container.max_size();
        }


        void clear()
        {

        }

    protected:
        T& access(Key const& id) override
        {
            // these methods can only be called if the container is already locked, so do not acquire the lock
            return _container[id];
        }

        T const& access(Key const& id) const override
        {
            return _container[id];
        }

    };
}