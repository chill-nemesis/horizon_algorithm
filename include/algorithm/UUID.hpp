//
// @brief   
// @details 
// @author  Steffen Peikert (ch3ll)
// @email   Horizon@ch3ll.com
// @version 1.0.0
// @date    30/05/2020 18:51
// @project Horizon
//


#pragma once

#include <cstdint>
#include <array>
#include <utility>
#include <cassert>

#include "algorithm/stl_extension/conversion.hpp"

namespace HORIZON::ALGORITHM
{
    struct UUID
    {
    private:
        static constexpr const size_t UUID_LENGTH     = 8 + 1 +
                                                        4 + 1 +
                                                        4 + 1 +
                                                        4 + 1 +
                                                        12;
        static constexpr const size_t UUID_LENGTH_RAW = UUID_LENGTH - 4;

        std::uint8_t _uuid[UUID_LENGTH_RAW]{ };
        char         _uuidString[UUID_LENGTH]{ };

    public:
        static UUID const Nil;

    public:
        template<size_t N = UUID_LENGTH>
        constexpr explicit UUID(char const (& uuid)[N])
        {
            // TODO: check length?
            // assert(N == UUID_LENGTH);
            // assert(uuid[8] == uuid[8 + 1 + 4] == uuid[8 + 1 + 4 + 1 + 4] == uuid[8 + 1 + 4 + 1 + 4 + 1 + 4] == '-');

            for (auto i = 0, k = 0; i < N; ++i)
            {
                _uuidString[i] = STL_EXTENSION::ToUpper(uuid[i]);

                // skip - characters during hex conversion
                if (uuid[i] == '-') continue;

                // convert to number
                _uuid[k++] = STL_EXTENSION::HexCharToByte(uuid[i]);
            }
        }

        [[nodiscard]] constexpr bool Equals(UUID const& other) const noexcept
        {
            for (auto i = 0; i < UUID_LENGTH_RAW; ++i) { if (_uuid[i] != other._uuid[i]) return false; }
            return true;
        }

        constexpr bool operator==(UUID const& other) const noexcept
        { return Equals(other); }

        explicit operator std::string() const noexcept
        { return std::string(_uuidString); }
    };

    // TODO: make that a constexpr
    inline UUID const UUID::Nil{ "00000000-0000-0000-0000-000000000000" };
}