//
// @brief   
// @details 
// @author  Steffen Peikert (ch3ll)
// @email   Horizon@ch3ll.com
// @version 1.0.0
// @date    13/07/2020 10:18
// @project Horizon
//


#pragma once

#include "algorithm/stl_extension/vector.hpp"

#include <functional>
#include <vector>
#include <atomic>

namespace HORIZON::ALGORITHM::STL_EXTENSION
{
    template<class Signature>
    class CallbackManager;

    template<class Result,
             class... Args>
    class CallbackManager<Result(Args...)>
    {
    public:
        using CallbackType = std::function<Result(Args...)>;

        struct Callback
        {
        private:
            CallbackType _callback;
            size_t const _identifier;

        public:
            Callback(size_t identifier, CallbackType&& callback) :
                    _identifier(identifier),
                    _callback(callback)
            { }

            ~Callback()
            { Unregister(); }

            // TODO: what if Result is void?
            // Result operator()(Args... args)
            // { return _callback(args...); }

            void operator()(Args&& ... args)
            { _callback(std::forward<Args>(args)...); }

            bool Unregister()
            { return false; }

            bool operator==(Callback const& other) noexcept
            { return _identifier == other._identifier; }
        };

    private:
        std::vector <Callback> _callbacks;
        std::atomic_size_t     _lastIdentifier = 0;

    public:
        Callback Register(CallbackType&& callback)
        {
            // determine the id of the callback
            // this is race cond. free, since lastIdentifier is an atomic
            auto id = _lastIdentifier++;

            // create callback object for unregistration
            Callback newCallback(id, std::move(callback));

            // store callback and return handle
            _callbacks.push_back(newCallback);
            return std::move(newCallback);
        }

        [[maybe_unused]] bool Unregister(Callback const& handle) noexcept
        {
            // this is called multiple times for each handle getting deleted...
            return EraseItemFromVector(_callbacks, handle);
        }

        // TODO: what if the function have return values?
        void operator()(Args&& ... args) noexcept
        {
            for (auto& cbf : _callbacks)
                // copy arguments for each invocation and execute
                cbf(args...);
        }
    };
}