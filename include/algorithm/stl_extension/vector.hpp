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
     * Gets the iterator to an item in a vector or vector.end() if the item could not be found.
     *
     * @param vector    The vector to search.
     * @param element   The element to find.
     * @return          An iterator to the element or vector.end().
     */
    template<class Vec,
             typename El = typename Vec::value_type,
             typename It = typename Vec::iterator>
    inline It FindInVector(Vec const& vector, El const& element)
    { return std::find(vector.begin(), vector.end(), element); }


    /*!
     * Returns true if an item is present in the vector.
     *
     * @param vector
     * @param element
     * @return
     */
    template<class Vec,
             typename El = typename Vec::value_type>
    inline bool IsItemInVector(Vec const& vector, El const& element)
    { return FindInVector(vector, element) != vector.end(); }

    /*!
     * Returns true if an item is present in the vector. The iterator will point to the item.
     * If the item is not found, iterator points to vector.end() and false is returned.
     *
     * @param vector
     * @param element
     * @param iterator
     * @return
     */
    template<class Vec,
             typename El = typename Vec::value_type,
             typename It = typename Vec::iterator>
    inline bool IsItemInVector(Vec const& vector, El const& element, It& iterator)
    {
        iterator = FindInVector(vector, element);
        return iterator != vector.end();
    }

    /*!
     * Erases an item from a vector, if the item is present.
     * Returns true on a successful removal.
     *
     * @param vector
     * @param element
     * @return
     */
    template<class Vec,
             typename El = typename Vec::value_type,
             typename It = typename Vec::iterator>
    inline bool EraseItemFromVector(Vec& vector, El const& element)
    {
        It iterator;
        if (IsItemInVector(vector, element, iterator))
            vector.erase(element);

        return iterator != vector.end();
    }

}