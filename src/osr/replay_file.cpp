#include <lzma.h>
#include <osp/detail/binary_decoder.h>
#include <osp/detail/string_utilities.h>
#include <osp/osr/replay_file.h>

#include <fstream>
#include <ranges>

using namespace osp::osr;

namespace {

constexpr std::size_t lzma_buffer_size = 1024;

std::string decompress_lzma(std::span<const char> compressed_data) {
  std::string result;

  lzma_stream stream = LZMA_STREAM_INIT;
  lzma_ret status = LZMA_OK;

  stream.next_in = reinterpret_cast<const uint8_t*>(compressed_data.data());
  stream.avail_in = compressed_data.size();

  if (lzma_auto_decoder(&stream, UINT64_MAX, LZMA_RUN) != LZMA_OK) {
    return {};
  }

  auto buffer = std::make_unique<std::uint8_t[]>(lzma_buffer_size);

  do {
    stream.next_out = buffer.get();
    stream.avail_out = lzma_buffer_size;

    status = lzma_code(&stream, LZMA_FINISH);

    if (stream.avail_out != lzma_buffer_size) {
      result.append(reinterpret_cast<const char*>(buffer.get()), lzma_buffer_size - stream.avail_out);
    }
  } while (status == LZMA_OK);

  if (status != LZMA_STREAM_END) {
    return {};
  }

  return result;
}

replay_frames decode_frames(std::span<const char> compressed_data) {
  const auto decompressed = decompress_lzma(compressed_data);
  if (decompressed.empty()) {
    return {};
  }

  replay_frames result;

  std::int32_t elapsed_time = 0;
  for (const auto& frame : decompressed | std::views::split(',')) {
    const auto sv = std::string_view(frame.data(), frame.size());
    const auto split = osp::detail::split<std::int32_t, float, float, std::int32_t>(sv, '|');

    if (!split.has_value()) [[unlikely]] {
      continue;
    }

    const auto [delta, x, y, keys] = split.value();

    if (delta != -12345) [[likely]] {
      elapsed_time += delta;
    }

    result.emplace_back(delta, elapsed_time, osp::vector2f(x, y), static_cast<osp::key_state>(keys));
  }

  return result;
}

lifebar_graph decode_lifebar(std::string_view lifebar) {
  lifebar_graph result;

  for (const auto& frame : lifebar | std::views::split(',')) {
    const auto sv = std::string_view(frame.data(), frame.size());
    const auto split = osp::detail::split<std::int32_t, float>(sv, '|');

    if (!split.has_value()) [[unlikely]] {
      continue;
    }

    const auto [t, x] = split.value();

    result.emplace_back(t, x);
  }

  return result;
}

bool decode_common_fields(osp::detail::binary_decoder& decoder, replay_file* result) {
  bool status = true;

  status &= decoder.read(result->mode);
  status &= decoder.read(result->version);
  status &= decoder.read_string(result->beatmap_hash);
  status &= decoder.read_string(result->player_name);
  status &= decoder.read_string(result->replay_hash);
  status &= decoder.read(result->count_300s);
  status &= decoder.read(result->count_100s);
  status &= decoder.read(result->count_50s);
  status &= decoder.read(result->count_gekis);
  status &= decoder.read(result->count_katus);
  status &= decoder.read(result->count_misses);
  status &= decoder.read(result->score);
  status &= decoder.read(result->max_combo);
  status &= decoder.read(result->perfect);
  status &= decoder.read(result->mods);

  return status;
}

bool decode_complex_fields(osp::detail::binary_decoder& decoder, replay_file* result) {
  /**
   * Parse the life bar graph.
   */

  {
    std::string life_bar;
    if (!decoder.read_string(life_bar)) {
      return false;
    }
    result->lifebar = decode_lifebar(life_bar);
  }

  /**
   * Don't forget the timestamp in the mwiddle. >///<
   */

  if (!decoder.read((result->timestamp))) {
    return false;
  }

  /**
   * Parse replay frames.
   */

  {
    std::int32_t replay_length;
    if (!decoder.read(replay_length)) {
      return false;
    }

    const auto slice = decoder.slice(replay_length);
    if (slice.empty()) {
      return false;
    }

    result->frames = decode_frames(slice);

    /**
     * Handle sentinel values.
     */

    if (!result->frames.empty() && result->version >= version_has_rng) {
      const auto& back = result->frames.back();

      /**
       * On replays set on version 20130319 or later, the 32-bit integer RNG seed used for the score will be encoded
       * into an additional replay frame at the end of the LZMA stream, under the format -12345|0|0|seed.
       */

      if (back.delta == -12345 && back.position.x == 0 && back.position.y == 0) {
        result->rng_seed = static_cast<std::int32_t>(back.keys);
        result->frames.pop_back();
      }
    }
  }

  return true;
}

bool decode_optional_fields(osp::detail::binary_decoder& decoder, replay_file* result) {
  /**
   * Handle the online score ID field across various iterations of the replay format.
   */

  if (result->version >= version_has_long_id) {
    std::int64_t value;
    if (!decoder.read(value)) {
      return false;
    }
    result->online_score_id = value;
  } else if (result->version >= version_first_osz2) {
    std::int32_t value;
    if (!decoder.read(value)) {
      return false;
    }
    result->online_score_id = static_cast<std::int64_t>(value);
  }

  /**
   * Mod-specific data.
   */

  if ((result->mods & osp::osu_mods::target) == osp::osu_mods::target) {
    double value;
    if (!decoder.read(value)) {
      return false;
    }
    result->target_practice_accuracy = value;
  }

  return true;
}

}  // namespace

std::unique_ptr<replay_file> osp::osr::from_memory(std::span<const char> data) {
  auto result = std::make_unique<replay_file>();
  {
    detail::binary_decoder decoder(data);

    if (!decode_common_fields(decoder, result.get())) {
      return nullptr;
    }

    if (!decode_complex_fields(decoder, result.get())) {
      return nullptr;
    }

    if (!decode_optional_fields(decoder, result.get())) {
      return nullptr;
    }
  }
  return result;
}

std::unique_ptr<replay_file> osp::osr::from_file(const std::filesystem::path& path) {
  std::ifstream file(path, std::ios::binary);

  if (!file.is_open()) {
    return nullptr;
  }

  std::vector<char> data(std::filesystem::file_size(path));

  file.read(data.data(), static_cast<std::streamsize>(data.size()));

  return from_memory(data);
}
