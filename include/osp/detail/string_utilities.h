#pragma once

#include <charconv>
#include <concepts>
#include <optional>
#include <ranges>
#include <string_view>
#include <tuple>
#include <vector>

namespace osp::detail {

/**
 * @brief A concept representing a string-encoded value.
 * @tparam T Type of the value.
 */
template <typename T>
concept string_component = std::integral<T> || std::floating_point<T>;

/**
 * @brief A small wrapper around std::from_chars that returns a std::optional.
 * @tparam T Type of the value to parse.
 * @param str String to parse.
 * @return The parsed value if successful, otherwise std::nullopt.
 */
template <string_component T>
std::optional<T> from_chars(std::string_view str) {
  T value;
  if (const auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value); ec != std::errc()) {
    return std::nullopt;
  }
  return value;
}

/**
 * @brief Split a string into a tuple of components.
 * @tparam Ts Types of the components.
 * @param str String to split.
 * @param delimiter Character to split on.
 * @return A tuple of the decoded components if successful, otherwise std::nullopt.
 */
template <typename... Ts>
std::optional<std::tuple<Ts...>> split(std::string_view str, char delimiter) {
  const auto sv = str | std::views::split(delimiter) | std::ranges::to<std::vector<std::string>>();

  if (sv.size() != sizeof...(Ts)) {
    return std::nullopt;
  }

  std::tuple<Ts...> result;

  const auto reduce = [&]<std::size_t... Is>(std::index_sequence<Is...>) {
    ((std::get<Is>(result) = from_chars<Ts>(sv[Is]).value_or(Ts{})), ...);
  };

  reduce(std::index_sequence_for<Ts...>{});

  return result;
}

}  // namespace osp::detail
