#include <osp/detail/binary_decoder.h>

using namespace osp::detail;

bool binary_decoder::read(void* buffer, size_type length) {
  if (length > remaining()) {
    return false;
  }

  std::memcpy(buffer, std::to_address(cursor_), length);
  cursor_ += static_cast<difference_type>(length);

  return true;
}

binary_decoder::container_type binary_decoder::slice(size_type length) {
  if (length > remaining()) {
    return {};
  }

  auto slice = std::span<const value_type>(cursor_, length);
  cursor_ += static_cast<difference_type>(length);

  return slice;
}

binary_decoder::size_type binary_decoder::remaining() const {
  return static_cast<size_type>(std::distance(cursor_, data_.cend()));
}

bool binary_decoder::read_string(std::string& value) {
  std::uint8_t tag;
  if (!read(tag)) {
    return false;
  }

  if (tag == 0x0) {
    value.clear();
    return true;
  }

  if (tag != 0xB) {
    return false;
  }

  std::uint64_t size;
  if (!read_uleb128(size)) {
    return false;
  }

  value.resize(size);

  return read(value.data(), size);
}

bool binary_decoder::read_uleb128(std::uint64_t& value) {
  std::size_t shift = 0;

  value = 0;

  while (true) {
    std::uint8_t byte;

    if (!read(byte)) {
      return false;
    }

    value |= static_cast<std::uint64_t>(byte & 0x7F) << shift;
    shift += 7;

    if ((byte & 0x80) == 0) {
      break;
    }
  }

  return shift <= 63;
}
