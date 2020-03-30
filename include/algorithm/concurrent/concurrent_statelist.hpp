//
// @brief   
// @details 
// @author  Steffen Peikert (ch3ll)
// @email   Horizon@ch3ll.com
// @version 1.0.0
// @date    04/01/2020 14:08
// @project Horizon
//


#pragma once

#pragma error "Unsupported file included (concurrent_statelist)"
// #include <vector>
// #include <mutex>
// #include <condition_variable>
//
// namespace HORIZON::ALGORITHM::CONCURRENT
// {
//     template<typename T,
//              typename S,
//              typename Allocator = std::allocator<T>>
//     class concurrent_statelist
//     {
//     private:
//         struct ElementNode;
//         struct StateNode;
//
//
//         struct ElementNode
//         {
//         private:
//             T _item;
//             ElementNode* _next;
//             ElementNode* _previous;
//             StateNode  * _parent;
//
//         public:
//
//         };
//
//         struct StateNode
//         {
//         public:
//             using const_reference = const S&;
//
//         private:
//             ElementNode* _start;
//             ElementNode* _end;
//
//             const S _state;
//
//         public:
//             explicit StateNode(S const& state) :
//                     _state(s),
//                     _start(nullptr),
//                     _end(nullptr)
//             { }
//
//             inline const_reference State() const
//             { return _state; }
//
//
//         };
//
//
//     private:
//         std::vector<ElementNode, Allocator> _container;
//         // using vector, because usually there are not many states (assuming 2 or 3), so memory alignment is way better
//         // for finding than a hash table (unordered_map)
//         std::vector<StateNode>              _states;
//
//
//
//
//     public:
//         void
//     };
// }