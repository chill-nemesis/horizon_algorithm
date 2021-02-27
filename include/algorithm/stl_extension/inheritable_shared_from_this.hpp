//
// @brief   
// @details 
// @author  Steffen Peikert (ch3ll)
// @email   Horizon@ch3ll.com
// @version 1.0.0
// @date    17/06/2020 17:33
// @project Horizon
//


#pragma once

#include <memory>

namespace HORIZON::ALGORITHM::STL_EXTENSION
{
    /*!
     * @brief Allows an inherited class to define the shared_from_this interface.
     * @tparam C
     */
    // TODO: make sure C is a valid type
    template<class C>
    class inheritable_shared_from_this : public std::enable_shared_from_this<inheritable_shared_from_this<C>>
    {
    public:
        inline std::shared_ptr<C> shared_from_this()
        { return std::static_pointer_cast<C>(std::enable_shared_from_this<inheritable_shared_from_this<C>>::shared_from_this()); }

        inline std::shared_ptr<C const> shared_from_this() const
        { return std::static_pointer_cast<C const>(std::enable_shared_from_this<inheritable_shared_from_this<C>>::shared_from_this()); }
    };
}