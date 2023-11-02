//
// @brief
// @details
// @author  Steffen Peikert (ch3ll)
// @email   Horizon@ch3ll.com
// @version 1.0.0
// @date    05/05/2020 15:46
// @project Horizon
//


#pragma once

#include <stdexcept>

namespace HORIZON::ALGORITHM::CONCURRENT
{
	class ContainerClosedError : public std::logic_error
	{
	public:
		ContainerClosedError() : std::logic_error("Container is closed and cannot be modified!") { }
	};
}  // namespace HORIZON::ALGORITHM::CONCURRENT
