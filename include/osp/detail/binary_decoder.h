#pragma once

#include <cstdlib>
#include <span>
#include <string>

namespace osp::detail {

struct binary_decoder {
  using value_type = char;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using container_type = std::span<const value_type>;

  /**
   * @brief Construct a decoder from a view into a byte buffer.
   * @param data Byte buffer.
   */
  explicit binary_decoder(container_type data) : data_(data), cursor_(data_.cbegin()) {}

  binary_decoder(const binary_decoder&) = delete;
  binary_decoder& operator=(const binary_decoder&) = delete;
  binary_decoder(binary_decoder&&) = default;
  binary_decoder& operator=(binary_decoder&&) = default;

  /**
   * @brief Decode N bytes from the buffer.
   * @param buffer Output storage.
   * @param length Number of bytes to decode.
   * @return True if N < remaining bytes.
   */
  bool read(void* buffer, size_type length);

  /**
   * @brief Decode N bytes from the buffer.
   * @tparam T Type of the value to decode. Must satisfy std::is_trivially_copyable.
   * @param buffer Output storage.
   * @param length Number of bytes to decode.
   * @return True if N < remaining bytes.
   */
  template <typename T>
    requires std::is_trivially_copyable_v<T>
  bool read(T& value) {
    return read(&value, sizeof(T));
  }

  /**
   * @brief Decode a string from the buffer.
   * @param value Output storage.
   * @return True if the string was successfully decoded.
   */
  bool read_string(std::string& value);

  /**
   * @brief Decode a ULEB128-encoded integer.
   * @param value Output storage.
   * @return True if the integer was successfully decoded.
   */
  bool read_uleb128(std::uint64_t& value);

  /**
   * @brief Retrieve a slice of the buffer.
   * @param length Number of bytes to slice.
   * @return A span of the buffer if N < remaining bytes. Otherwise, an empty span is returned.
   */
  container_type slice(size_type length);

  /**
   * @brief Retrieve the remaining bytes in the buffer.
   * @return Number of remaining bytes.
   */
  size_type remaining() const;

 private:
  container_type data_;
  container_type::const_iterator cursor_;
};

}  // namespace osp::detail
