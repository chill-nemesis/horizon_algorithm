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
	 * @ingroup group_algorithm_stl
	 *
	 * @details Converts a hexadecimal character to its byte representation.
	 * If the character does not represent a hexadecimal value, its character-value is returned.
	 *
	 * @param hex The character to convert.
	 * @returns The byte representation of the hex character.
	 */
	static constexpr std::uint8_t HexCharToByte(char const& hex)
	{
		if ((hex >= 'a') && (hex <= 'f')) { return (uint8_t)(hex - 'a' + 10); }
		if ((hex >= 'A') && (hex <= 'F')) { return (uint8_t)(hex - 'A' + 10); }
		if ((hex >= '0') && (hex <= '9')) { return (uint8_t)(hex - '0' + 0); }

		return hex;
	}

	/*!
	 * @ingroup group_algorithm_stl
	 *
	 * @brief Converts a character to upper case.
	 * @details If the character is not in [a-zA-Z], the returned character is not modified.
	 * @param character The character to convert.
	 * @return The character converted to upper case or the unmodified character if it was not in range [a-zA-Z].
	 */
	static constexpr char ToUpper(char const& character)
	{
		if ((character >= 'a') && (character <= 'z')) { return character - 'a' + 'A'; }

		return character;
	}
}  // namespace HORIZON::ALGORITHM::STL_EXTENSION
