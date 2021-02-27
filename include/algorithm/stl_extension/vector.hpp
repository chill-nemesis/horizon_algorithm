//
// @brief   
// @details 
// @author  Steffen Peikert (ch3ll)
// @email   Horizon@ch3ll.com
// @version 1.0.0
// @date    28/03/2020 17:14
// @project Horizon
//


#pragma once

#include <vector>
#include <algorithm>

namespace HORIZON::ALGORITHM::STL_EXTENSION
{
    /*!
     * @ingroup group_algorithm_stl
     *
     * @brief Returns true if an item is present in a container.
     *
     * @param container The container to search.
     * @param element   The element to find.
     * @return          True if the element is present in the container.
     */
    template<class Container,
             typename El = typename Container::value_type>
    inline bool HasItem(Container const& container, El const& element)
    { return std::find(container.begin(), container.end(), element) != container.end(); }

    /*!
     * @ingroup group_algorithm_stl
     * @brief Finds an item in a container.
     *
     * @details Returns true if an item is present in a container. If true, the iterator will point to the item.
     * If the item is not found, iterator points to container.end() and false is returned.
     *
     * @param container The container to search.
     * @param element   The element to find.
     * @param iterator  A container iterator. After calling this method, the iterator will either point to the element or the end of the container.
     * @return          True if the element is present.
     */
    template<class Container,
             typename El = typename Container::value_type,
             typename It = typename Container::iterator>
    inline bool HasItem(Container const& container, El const& element, It& iterator) // TODO: const iterator?
    {
        iterator = std::find(container.begin(), container.end(), element);
        return iterator != container.end();
    }

    /*!
     * @ingroup group_algorithm_stl
     * @brief Removes an item from a container.
     *
     * @details Erases an item from a container if the item is present.
     * Returns true on a successful removal.
     *
     * @param container
     * @param element
     * @return
     */
    template<class Container,
             typename El = typename Container::value_type,
             typename It = typename Container::iterator>
    inline bool EraseItem(Container& container, El const& element)
    {
        It iterator;

        // find the position of the element. This requires an equals operation defined on the element
        if (!IsItemInVector(container, element, iterator)) return false;

        // iterator points to the element
        // this modifies the iterator
        container.erase(iterator);
        return true;
    }
}