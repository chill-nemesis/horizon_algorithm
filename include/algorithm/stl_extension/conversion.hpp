//
// @brief   
// @details 
// @author  Steffen Peikert (ch3ll)
// @email   Horizon@ch3ll.com
// @version 1.0.0
// @date    01/06/2020 21:18
// @project Horizon
//


#pragma once

#include <cstdint>

namespace HORIZON::ALGORITHM::STL_EXTENSION
{
    /*!
     * Converts a hexadecimal character to its byte representation.
     * If the character does not represent a hexadecimal value, its character-value is returned.
     */
    static constexpr std::uint8_t HexCharToByte(char const& hex)
    {
        if ((hex >= 'a') && (hex <= 'f')) return hex - (uint8_t) 'a' + 10;
        if ((hex >= 'A') && (hex <= 'F')) return hex - (uint8_t) 'A' + 10;
        if ((hex >= '0') && (hex <= '9')) return hex - (uint8_t) '0' + 0;

        return hex;
    }

    static constexpr char ToUpper(char const& character)
    {
        if ((character >= 'a') && (character <= 'z')) return character - 'a' + 'A';

        return character;
    }
}
