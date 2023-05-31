//
// @brief
// @details
// @author  Steffen Peikert (ch3ll)
// @email   Horizon@ch3ll.com
// @version 1.0.0
// @date    28/12/2019 21:46
// @project Horizon
//


#pragma once

#include "concurrent_base.hpp"

#include <queue>

namespace HORIZON::ALGORITHM::CONCURRENT
{
	/*!
	 * @ingroup group_algorithm_concurrent
	 *
	 * @brief A concurrent queue.
	 * @tparam T            The element type of the queue.
	 * @tparam Container    The underlying container type.
	 *
	 * @note This implementation is by no means complete and may/will expand upon my need.
	 */
	template<typename T, typename Container = std::deque<T>>
	class concurrent_queue : public concurrent_base
	{
	public:
		/*!
		 * @brief Type of the items stored in the container.
		 */
		using value_type      = typename Container::value_type;
		using reference       = typename Container::reference;
		using const_reference = typename Container::const_reference;
		using size_type       = typename Container::size_type;
		using container_type  = std::queue<value_type, Container>;

	private:
		container_type _container;

	public:
		/*!
		 * @brief Creates a new, empty concurrent queue.
		 */
		concurrent_queue() : concurrent_queue(Container()) { }

		/*!
		 * @brief Creates a concurrent queue from an existing queue.
		 * @param container
		 */
		explicit concurrent_queue(Container const& container) : _container(container) { }

		/*!
		 * @brief Creates a concurrent queue from an existing queue.
		 * @param container
		 */
		explicit concurrent_queue(Container&& container) : _container(std::forward<Container>(container)) { }

		// TODO: other ctors

		/*!
		 * @brief Closes the queue and destroys the concurrent queue object.
		 */
		~concurrent_queue() { close(); }

		using concurrent_base::empty;

		[[nodiscard]] inline bool empty(access_token const& token) const override
		{
			this->CheckForOwnership(token);
			return _container.empty();
		}

		/*!
		 * @returns Returns the number of elements in the %concurrent_queue.
		 */
		[[nodiscard]] inline size_type size() const { return size(std::move(Guard())); }

		/*!
		 * @copydoc size()
		 */
		[[nodiscard]] inline size_type size(access_token const& token) const
		{
			this->CheckForOwnership(token);
			return _container.size();
		}

		/*!
		 * @brief Add data to the end of the queue.
		 * @param item  Data to be added.
		 *
		 * @note Calling this method takes ownership of the container for the duration of the push. If the container is
		 * currently owned by another thread, this method will block until the other thread releases the container.
		 * @sa push(value_type const&, access_token const&)
		 *
		 *  @details This is a typical queue operation.  The function creates an
		 *  element at the end of the %queue and assigns the given data
		 *  to it.  The time complexity of the operation depends on the
		 *  underlying sequence.
		 */
		void push(value_type const& item) noexcept { push(item, std::move(Guard())); }

		/*!
		 * @copydoc push(value_type const&) noexcept
		 */
		void push(value_type&& item) noexcept { push(std::forward<value_type>(item), std::move(Guard())); }

		// we need both variants (first for const&, second for move)

		/*!
		 * @brief Adds an item to the end of the queue.
		 * @details Calling this method requires the caller to provide an ownership token of the corresponding container
		 * object.
		 *
		 * @sa Guard()
		 *
		 * @param item The item to add.
		 * @param token The access token of this queue
		 */
		void push(value_type const& item, access_token const& token) noexcept
		{
			this->CheckForOwnership(token);

			// ThrowIfClosed();

			_container.push(item);

			_containerCV.notify_one();
		}

		/*!
		 * @copydoc push(value_type const&, access_token const& token)
		 */
		void push(value_type&& item, access_token const& token) noexcept
		{
			this->CheckForOwnership(token);

			// ThrowIfClosed();

			_container.push(std::move(item));

			_containerCV.notify_one();
		}

		// the return value is NOT decltype(auto)
		// because no reference is passed outside (requires freezing, etc.)
		/*!
		 * @brief Constructs an element in place at the end of the queue.
		 * @param args  The arguments to create an element.
		 *
		 * @note The stl-queue returns a reference to the newly created item. However, it is not good practice to pass
		 * references to elements in concurrent containers. Thus, I decided against returning a reference here. The
		 * locked overload of this method however does provide the reference.
		 *
		 * @note Calling this method takes ownership of the container for the duration of the emplace. If the container
		 * is currently owned by another thread, this method will block until the other thread releases the container.
		 *
		 * @sa emplace(access_token const&, Args&& ...)
		 */
		template<class... Args>
		void emplace(Args&&... args) noexcept
		{
			emplace_helper(std::move(Guard()), std::forward<Args>(args)...);
		}

		// NOTE: this is a C++17 feature
		/*!
		 * @brief Constructs an element in place at the end of the queue.
		 * @details Calling this method requires the caller to provide an ownership token of the corresponding container
		 * object.
		 * @tparam Args
		 * @param token The access token of this queue.
		 * @param args  The arguments to create an element.
		 * @return      A reference to the element.
		 *
		 * @warning I decided to keep this signature since it closely models the behaviour of the stl-queue. Accessing
		 * or modifying the reference after releasing ownership is technically possible but will result in undefined
		 * behaviour (and probably race conditions!)s
		 */
		template<class... Args>
		reference emplace(access_token const& token, Args&&... args) noexcept
		{
			return emplace_helper(token, std::forward<Args>(args)...);
		}

		/*!
		 * @brief Tries to remove the first element.
		 * @details Tries to remove the foremost element. If no element is available, this method returns immediately
		 * with the result of false.
		 * @return True if an element could be removed.
		 *
		 * @note Calling this method takes ownership of the container for the duration of the emplace. If the container
		 * is currently owned by another thread, this method will block until the other thread releases the container.
		 *
		 * @sa try_pop(T&, access_token&&)
		 *
		 * @param item If this method returns true this will hold the item removed.
		 * @return True if the item could be removed.
		 */
		inline bool try_pop(T& item) { return try_pop(item, std::move(Guard())); }

		/*!
		 * @brief Tries to remove the first element.
		 * @details Tries to remove the foremost element. If no element is available, this method returns immediately
		 * with the result of false.
		 * @param item If this method returns true this will hold the item removed.
		 * @param token The access token of this queue.
		 * @return  True if the item could be removed.
		 */
		inline bool try_pop(T& item, access_token&& token)
		{
			return pop(item, std::forward<access_token>(token), time_type::zero());
		}

		/*!
		 * @brief Removes the first element.
		 * @details The calling thread is blocked until an element can be retrieved or until a user specified timeout is
		 * reached. If the timeout is reached, no element is removed and this method returns false.
		 *
		 * @param maximumWaitTime   The maximum time to wait until the pop operations is aborted.
		 * @return                  True if an element could be removed.
		 */
		bool pop(time_type const& maximumWaitTime = _maxWaitTime) noexcept
		{
			return pop(std::move(Guard()), maximumWaitTime);
		}

		/*!
		 * @brief Removes the first element.
		 * @details The calling thread is blocked until an element can be retrieved or until a user specified timeout is
		 * reached. If the timeout is reached, no element is removed and this method returns false.
		 *
		 * @param token             The access token of this queue.
		 * @param maximumWaitTime   The maximum time to wait until the pop operation is aborted.
		 * @return                  True if the element could be removed.
		 */
		bool pop(access_token& token, time_type const& maximumWaitTime = _maxWaitTime) noexcept
		{
			this->CheckForOwnership(token);

			if (!WaitForElementsInQueue(token, maximumWaitTime)) { return false; }

			_container.pop();
			return true;
		}

		/*!
		 * @brief Removes the first element.
		 * @details The calling thread is blocked until an element can be retrieved or until a user specified timeout is
		 * reached. If the timeout is reached, no element is removed and this method returns false.
		 *
		 * @param item              The removed element.
		 * @param maximumWaitTime   The maximum time to wait until the pop operations is aborted.
		 * @return                  True if an element could be removed.
		 */
		bool pop(T& item, time_type const& maximumWaitTime = _maxWaitTime) noexcept
		{
			return pop(item, std::move(Guard()), maximumWaitTime);
		}

		/*!
		 * @copydoc pop(T&, time_type const&)
		 * @param token  The access token of this queue.
		 */
		bool pop(T& item, access_token&& token, time_type const& maximumWaitTime = _maxWaitTime) noexcept
		{
			this->CheckForOwnership(token);

			if (!WaitForElementsInQueue(token, maximumWaitTime)) { return false; }

			item = std::move(_container.front());
			_container.pop();

			return true;
		}

		/*!
		 * @brief Swaps two concurrent_queues.
		 * @param left      The left container.
		 * @param right     The right container.
		 * @param leftLock  The access token of the left queue.
		 * @param rightLock The access token of the right queue.
		 */
		friend void swap(concurrent_queue&   left,
						 concurrent_queue&   right,
						 access_token const& leftLock,
						 access_token const& rightLock) noexcept
		{
			swap(static_cast<concurrent_base&>(left), static_cast<concurrent_base&>(right), leftLock, rightLock);

			assert(leftLock.mutex()->native_handle() == left._containerAccess.native_handle());
			assert(rightLock.mutex()->native_handle() == right._containerAccess.native_handle());

			// make sure both containers are not closed
			// ThrowIfClosed();
			// other.ThrowIfClosed();

			std::swap(left._container, right._container);

			left._containerCV.notify_all();
			right._containerCV.notify_all();
		}

		/*!
		 * @brief Swaps two concurrent_queues.
		 *
		 * @note Calling this method takes ownership of both containers for the duration of the swap. If a container is
		 * currently owned by another thread, this method will block until the other thread releases the container. Both
		 * containers are locked simultaneously via defer_lock.
		 *
		 * @param left  The left container.
		 * @param right The right container.
		 *
		 * @sa swap(concurrent_queue&, concurrent_queue&, access_token const&, access_token const&)
		 */
		friend void swap(concurrent_queue& left, concurrent_queue& right) noexcept
		{
			// safety, do not swap (and more importantly, lock twice) the same object
			if (&left == &right) { return; }

			auto leftLock  = left.Guard(std::defer_lock);
			auto rightLock = right.Guard(std::defer_lock);

			std::lock(leftLock, rightLock);

			swap(left, right, leftLock, rightLock);
		}

		/*!
		 * @brief Clears the queue.
		 *
		 * @note Calling this method takes ownership of the container for the duration of the clear. If the container is
		 * currently owned by another thread, this method will block until the other thread releases the container.
		 *
		 * @sa clear(access_token const&)
		 */
		void clear() noexcept { clear(std::move(Guard())); }

		/*!
		 * @brief Clears the queue.
		 * @details Calling this method requires the caller to provide an ownership token of the corresponding queue
		 * object.
		 * @param token The access token of this queue.
		 */
		void clear(access_token const& token) noexcept
		{
			this->CheckForOwnership(token);
			// ThrowIfClosed();

			std::queue<T> empty;
			std::swap(_container, empty);
		}

	private:
		template<class... Args>
		reference emplace_helper(access_token const& token, Args&&... args) noexcept
		{
			this->CheckForOwnership(token);

			// ThrowIfClosed();

			// emplace value and store result
			reference result = _container.emplace(std::forward<Args>(args)...);

			// notify possible waiting threads and return the result of emplace
			_containerCV.notify_all();
			return result;
		}

		/*!
		 * Waits until the container is not empty.
		 * Blocks the current thread execution until an element is in queue or the wait timeout is reached.
		 * Returns true if an element is in queue.
		 */
		inline bool WaitForElementsInQueue(access_token& token, time_type const& waitTime) noexcept
		{
			// assert(waitTime.count() > 0);

			// early checks for either empty or closed.
			// first check if we have elements in queue, then for closed
			// this order allows emptying the queue after it has been closed
			if (!empty(token)) { return true; }
			if (is_closed()) { return false; }

			if (!_containerCV.wait_for(token, waitTime, [this, &token] { return !empty(token) || is_closed(); }))
			{
				return false;  // timeout
			}

			// ThrowIfClosed();
			return CanModify();
		}
	};
}  // namespace HORIZON::ALGORITHM::CONCURRENT