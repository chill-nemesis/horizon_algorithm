//
// @brief
// @details
// @author  Steffen Peikert (ch3ll)
// @email   Horizon@ch3ll.com
// @version 1.0.0
// @date    01/01/2020 13:58
// @project Horizon
//


#pragma once

#include "concurrent_base.hpp"

#include <algorithm>
#include <vector>

namespace HORIZON::ALGORITHM::CONCURRENT
{
	/*!
	 * @ingroup group_algorithm_concurrent
	 *
	 * @brief A concurrent vector.
	 * @copydetails concurrent_base
	 *
	 * @tparam T        The element type.
	 * @tparam Alloc    The underlying allocator.
	 *
	 * @note This implementation is by no means complete and gets extended upon need!
	 */
	template<typename T, typename Alloc = std::allocator<T>>
	class concurrent_vector : public virtual concurrent_base
	{
	public:
		using value_type     = T;
		using allocator_type = Alloc;
		using container_type = std::vector<value_type, allocator_type>;
		using size_type      = typename container_type::size_type;

		using reference       = value_type&;
		using const_reference = value_type const&;
		using access_type     = size_type;

		using iterator               = typename container_type::iterator;
		using const_iterator         = typename container_type::const_iterator;
		using reverse_iterator       = typename container_type::reverse_iterator;
		using const_reverse_iterator = typename container_type::const_reverse_iterator;


	private:
		container_type _container;

	public:
		// TODO construcotrs


		/*!
		 * @return Gets the capacity of the container.
		 */
		[[nodiscard]] inline size_type capacity() const { return capacity(std::move(Guard())); }

		/*!
		 * @brief Gets the capacity of the container if it is owned by the calling thread.
		 * @warning The calling thread must provide the corresponding access token!
		 * @param token The access token.
		 * @return  The capacity of the container.
		 */
		[[nodiscard]] inline size_type capacity(access_token const& token) const
		{
			CheckForOwnership(token);
			return _container.capacity();
		}

		/*!
		 * @return Gets the size of the container.
		 */
		[[nodiscard]] inline size_type size() const { return size(std::move(Guard())); }

		/*!
		 * @brief Gets the size of hte container if it is owned by the calling thread.
		 * @warning The calling thread must provide the corresponding access token!
		 * @param token The access token.
		 * @return The size of the container.
		 */
		[[nodiscard]] inline size_type size(access_token const& token) const
		{
			CheckForOwnership(token);
			return _container.size();
		}

		using concurrent_base::empty;

		[[nodiscard]] inline bool empty(access_token const& token) const override
		{
			CheckForOwnership(token);
			return _container.empty();
		}

		/*!
		 * @brief Resizes the container.
		 * @param size The new size of the container (in elements).
		 */
		void resize(size_type size) { resize(size, std::move(Guard())); }

		/*!
		 * @brief Resizes the container if it is owned by the calling thread.
		 * @warning The calling thread must provide the corresponding access token!
		 * @param size  The new size of the container.
		 * @param token The access token.
		 */
		void resize(size_type size, access_token const& token) const
		{
			CheckForOwnership(token);
			_container.resize(size);
		}

		/*!
		 * @brief Resizes the container and initialises with a default element.
		 * @param size The new size of the container.
		 * @param def The default value of the container.
		 */
		void resize(size_type size, value_type const& def) { resize(size, def, std::move(Guard())); }

		/*!
		 * @brief Resizes the container and initialises with a default element if it is owned by the calling thread.
		 * @param size The new size of the container.
		 * @param def The default value of the container.
		 * @param token The access token.
		 */
		void resize(size_type size, value_type const& def, access_token const& token)
		{
			CheckForOwnership(token);
			_container.resize(size, def);
		}

		/*!
		 * @brief Reserves space in the container without initialising.
		 * @param size The new capacity of the container.
		 */
		void reserve(size_type size) { reserve(size, std::move(Guard())); }

		/*!
		 * @brief Reserves space in the container without initialising.
		 * @param size The new capacity of the container.
		 * @param token The access token.
		 */
		void reserve(size_type size, access_token const& token)
		{
			CheckForOwnership(token);
			_container.reserve(size);
		}

		/*!
		 * @brief Places the given element at the end of the vector.
		 * @param item The element to add.
		 */
		void push_back(value_type const& item) { push_back(item, std::move(Guard())); }

		/*!
		 * @brief Places the given element at the end of the vector if the vector is owned by the calling thread.
		 * @param item  The item to add.
		 * @param token The access token.
		 */
		void push_back(value_type const& item, access_token const& token)
		{
			CheckForOwnership(token);

			_container.push_back(item);
		}

		/*!
		 * @brief Inplace creates the given element at the end of the vector.
		 * @param item  The item to add.
		 */
		void push_back(value_type&& item) { push_back(std::forward<value_type>(item), std::move(Guard())); }

		/*!
		 * @brief Inplace creates the given element at the end of the vector if the container is owned by the calling
		 * thread.
		 * @param item  The item to add.
		 * @param token The access token.
		 */
		void push_back(value_type&& item, access_token const& token)
		{
			CheckForOwnership(token);
			_container.push_back(std::forward<value_type>(item));
		}

		/*!
		 * @brief Swaps two elements of the container.
		 * @param first The index of the first element.
		 * @param second THe index of the second element.
		 *
		 * @throws out_of_range If either first or second is out of bounds.
		 */
		void SwapElements(access_type const& first, access_type const& second)
		{
			SwapElements(first, second, std::move(Guard()));
		}

		/*!
		 * @brief Swaps two elements of the container if the container is owned by the calling thread.
		 * @param first The index of the first element.
		 * @param second The index of the second element.
		 * @param token The access token.
		 *
		 * @throws out_of_range If either first or second is out of bounds.
		 */
		void SwapElements(access_type const& first, access_type const& second, access_token const& token)
		{
			// do not swap the same element
			if (first == second) { return; }

			// <0 is (currently) impossible since access_type is unsigned
			if (first >= size(token)) { throw std::out_of_range("Parameter \"first\" is out of bounds."); }
			if (second >= size(token)) { throw std::out_of_range("Parameter \"second\" is out of bounds."); }

			CheckForOwnership(token);
			std::iter_swap(begin() + first, begin() + second);
		}

		/*!
		 * Swaps the given element with the last element.
		 * @param index The index of the element.
		 *
		 * @throws out_of_range If @p index is out of range.
		 */
		void SwapElements(access_type const& index) { swap(index, std::move(Guard())); }

		/*!
		 * Swaps the given element with the last element if the container is owned by the calling thread.
		 * @param index The index of the element.
		 * @param token The access token.
		 *
		 * @throws out_of_range If @p index is out of range.
		 */
		void SwapElements(access_type const& index, access_token const& token)
		{
			CheckForOwnership(token);

			// TODO: this might block if an error is thrown...
			std::iter_swap(index, end(token) - 1);
		}

		/*!
		 * @brief Gets an iterator to the beginning of the container.
		 *
		 * @details Iterators are THE problem for concurrent containers. There is no guarantee that the container is
		 * owned by the thread requesting the iterator, thus resulting in race conditions. I decided to expose the
		 * underlying container iterator, but only in "locked" methods. If you need to use iterators (and lets be
		 * honest, they are pretty neat), make sure you follow this principle:
		 * @code{.cpp}
		 * {
		 *      auto token = container.Guard();
		 *      auto it = container.begin(token);
		 *      ...
		 * }
		 * @endcode
		 * This ensures that the container is locked until the iterator is out of scope (curly-brackets). It is not as
		 * nice as just using the standard begin, and prevents the "default" usage of containers in stl algorithms but
		 * emphasises the need for ownership. If I feel the need for it, I'll implement a custom iterator that locks the
		 * container until it is destroyed.
		 *
		 * @param token The access token.
		 * @warning This is inherently unsafe!
		 */
		iterator begin(access_token const& token) noexcept
		{
			CheckForOwnership(token);
			return _container.begin();
		}

		// copydetails apparently copies param and warning
		/*!
		 * @brief Gets a reverse iterator to the beginning of the container.
		 * @copydetails begin()
		 */
		reverse_iterator rbegin(access_token const& token) noexcept
		{
			CheckForOwnership(token);
			return _container.rbegin();
		}

		/*!
		 * @brief Gets an iterator to the end of the container.
		 * @copydetails begin()
		 */
		iterator end(access_token const& token) noexcept
		{
			CheckForOwnership(token);
			return _container.end();
		}

		/*!
		 * @brief Gets a reverse iterator to the end of the container.
		 * @copydetails begin()
		 */
		reverse_iterator rend(access_token const& token) noexcept
		{
			CheckForOwnership(token);
			return _container.rend();
		}

		/*!
		 * @brief Gets an iterator to the beginning of the container.
		 * @copydetails begin()
		 */
		const_iterator begin(access_token const& token) const noexcept
		{
			CheckForOwnership(token);
			return _container.begin();
		}

		/*!
		 * @brief Gets a reversed iterator to the beginning of the container.
		 * @copydetails begin()
		 */
		const_reverse_iterator rbegin(access_token const& token) const noexcept
		{
			CheckForOwnership(token);
			return _container.rbegin();
		}

		/*!
		 * @brief Gets an iterator to the end of the container.
		 * @copydetails begin()
		 */
		const_iterator end(access_token const& token) const noexcept
		{
			CheckForOwnership(token);
			return _container.end();
		}

		/*!
		 * @brief Gets a reverse iterator to the end of the container.
		 * @copydetails begin()
		 */
		const_reverse_iterator rend(access_token const& token) const noexcept
		{
			CheckForOwnership(token);
			return _container.rend();
		}

		/*!
		 * @brief Clears the container.
		 *
		 * @note Calling this method takes ownership of the container for the duration of the clear. If the container is
		 * currently owned by another thread, this method will block until the other thread releases the container.
		 *
		 * @sa clear(access_token const&)
		 */
		void clear() noexcept { clear(std::move(Guard())); }

		/*!
		 * @brief Clears the container.
		 * @details Calling this method requires the caller to provide an ownership token of the corresponding queue
		 * object.
		 * @param token The access token of this container.
		 */
		void clear(access_token const& token) noexcept
		{
			CheckForOwnership(token);

			_container.clear();
		}
	};
}  // namespace HORIZON::ALGORITHM::CONCURRENT