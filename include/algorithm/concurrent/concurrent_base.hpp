//
// @brief
// @details
// @author  Steffen Peikert (ch3ll)
// @email   Horizon@ch3ll.com
// @version 1.0.0
// @date    04/01/2020 16:58
// @project Horizon
//


#pragma once

#include "algorithm/concurrent/ContainerClosedError.hpp"

#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <preprocessor/unique_lock.hpp>

namespace HORIZON::ALGORITHM::CONCURRENT
{
	/*!
	 * @ingroup group_algorithm_concurrent
	 *
	 * @brief The base class for all concurrent containers.
	 * @details This class adds concurrent access utility and lock functionality. It acts as an interface to prevent
	 * accidental race conditions while working as a standard STL container. As with everything C/C++-related, this
	 * cannot guarantee or prevent malicious behaviour. However, this must be a conscious choice of the implementer to
	 * circumvent the safety-mechanics of the containers.
	 *
	 * You will find that all methods provide two signatures: A default (stl-like) implementation and one requiring an
	 * access_token. Access_tokens are supposed to mark ownership of the container. This is useful if you need to make
	 * multiple calls to the container without interference of other threads. An access_token can be obtained by using
	 * the Guard()-methods of the container. Once a token is out of scope (or manually unlocked) the ownership is
	 * released. Each "default" method signature will acquire a guard token and then call the guarded implementation.
	 */
	class concurrent_base
	{
	public:
		/*!
		 * @brief The access token type used to mark ownership
		 */
		using access_token = std::unique_lock<std::mutex>;
		/*!
		 * @brief Time type used for timeouts.
		 */
		using time_type    = std::chrono::nanoseconds;

	private:
		std::atomic_bool _closeToken = false;

	protected:
		/*!
		 * @brief The container mutex.
		 */
		mutable std::mutex _containerAccess;

		/*!
		 * @brief The container condition variable used to signal changes in the container.
		 */
		std::condition_variable_any _containerCV;

		// see
		// https://stackoverflow.com/questions/27726818/stdcondition-variablewait-for-exits-immediately-when-given-stdchronodura
		// (second answer) for the gcc work-around
		// technically, this is expected behaviour. GCC and windows are just nice to the user and do not create an
		// overflow the standard explicitly warn about this behaviour: "Note that rel_time must be small enough not to
		// overflow when added to std::chrono::steady_clock::now()."
		// (https://en.cppreference.com/w/cpp/thread/condition_variable/wait_for)
		/*!
		 * @brief The default maximum wait time for containers.
		 */
		static constexpr const time_type _maxWaitTime = std::chrono::hours(100);

	public:
		/*
		 * @brief Locks the container and returns an access token for concurrent calls to the container
		 * Blocks if the lock is not available.
		 *
		 * @include <preprocessor/unique_lock.hpp>
		 */
		HORIZON_AddLockGuardsMutable(_containerAccess)

		/*!
		 * @return True if the container is closed (elements cannot be modified).
		 */
		[[nodiscard]] inline bool is_closed() const { return _closeToken; }

		/*!
		 * @brief Closes the container for modification and notifies all waiting threads.
		 */
		void close()
		{
			// I need to lock access to closing while modifications might take place
			auto token = Guard();

			// set the token to true. If it was previously true, do not notify
			if (_closeToken.exchange(true)) { return; }

			// container was previously not closed, notify all waiting threads.
			_containerCV.notify_all();
		}

		/*!
		 * @brief (Re-) opens the queue.
		 */
		inline void open()  // Todo: enable overriding for containers where reopening is not possible?
		{
			_closeToken = false;
		}

		/*!
		 * @return Returns true if the container is empty.
		 */
		inline bool empty() const { return empty(Guard()); }

		/*!
		 * @details Checks if the container is empty.
		 * @param token The access token marking the ownership of the container.
		 * @return Returns true if the container is empty.
		 */
		virtual bool empty(access_token const& token) const = 0;

		/*!
		 * @brief Friend declaration of swap. This requires both containers to be locked.
		 * @relates concurrent_base
		 * @param left      The left container.
		 * @param right     The right container.
		 * @param leftLock  The lock of the left container.
		 * @param rightLock The lock of the right container.
		 *
		 * @note I strongly advice to use the following scheme (in the specialised swap methods for child containers):
		 *
		 * @code{.cpp}
		 *  // safety, do not swap (and more importantly, lock twice) the same object
		 *  if (&left == &right)return;
		 *  auto leftLock  = left.Guard(std::defer_lock);
		 *  auto rightLock = right.Guard(std::defer_lock);
		 *  std::lock(leftLock, rightLock);
		 *
		 *  swap(left, right, leftLock, rightLock);
		 *  @endcode
		 */
		friend void swap(concurrent_base&    left,
						 concurrent_base&    right,
						 access_token const& leftLock,
						 access_token const& rightLock) noexcept
		{
			left.CheckForOwnership(leftLock);
			right.CheckForOwnership(rightLock);

			right._closeToken = left._closeToken.exchange(right._closeToken);
		}

		/*!
		 * @brief Default swap operation for concurrent_base is not supported. Use @sa swap(concurrent_base&,
		 * concurrent_base&, access_token const&, access_token const&) instead.
		 * @relates concurrent_base
		 * @param left
		 * @param right
		 */
		friend void swap(concurrent_base& left, concurrent_base& right) = delete;


	protected:
		/*!
		 * @return True if the container can be modified (close token is not set).
		 */
		[[nodiscard]] inline bool CanModify() const { return !(is_closed()); }

		/*!
		 * @brief Checks if the given token belongs to this class.
		 * @param token The token to check.
		 *
		 * @note This has no effect in no-debug builds.
		 */
		inline void CheckForOwnership(access_token const& token) const
		{
			assert(token.mutex()->native_handle() == _containerAccess.native_handle());
		}
	};
}  // namespace HORIZON::ALGORITHM::CONCURRENT