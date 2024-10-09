#ifndef AAD_CONTAINER_TYPES_TO_USE_INCLUDE_H
#define AAD_CONTAINER_TYPES_TO_USE_INCLUDE_H

#include <vector>
#include <array>

namespace aad
{
/**
 * @brief For a given size, the struct specializes in different types
 * @tparam T The type of the content contained
 * @tparam kMaxQueueSize Static size of the container
 */
template <typename T, std::size_t kMaxQueueSize>
struct ContainerTypeToUse
{
    using type = std::array<T, kMaxQueueSize>;
    constexpr void reserveAndFit(type&, std::size_t const size) noexcept {}
};

/**
 * @brief For a 0 size, use a dynamic sized container, as vector
 * @tparam T The type of the content contained
 */
template <typename T>
struct ContainerTypeToUse<T, 0U>
{
    using type = std::vector<T>;
    constexpr void reserveAndFit(type& t, std::size_t const size) noexcept
    {
        t.resize(size);
    }
};

}  // namespace aad

#endif
