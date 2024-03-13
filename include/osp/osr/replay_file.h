#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <vector>

#include "../osu_enums.h"
#include "../vector.h"

namespace osp::osr {

/**
 * @brief Version identifier for osz2 replays.
 */
constexpr std::int32_t version_first_osz2 = 20121008;

/**
 * @brief Version identifier for replays with ids longer than 32 bits.
 */
constexpr std::int32_t version_has_long_id = 20140721;

/**
 * @brief Version identifier for replays with RNG seeds.
 */
constexpr std::int32_t version_has_rng = 20130319;

struct lifebar_component {
  /**
   * @brief Time in milliseconds.
   */
  std::int32_t time;

  /**
   * @brief Percentage of the lifebar at this point.
   */
  float percent;
};

struct replay_frame {
  /**
   * @brief Time since the previous frame in milliseconds.
   */
  std::int32_t delta;

  /**
   * @brief Frame start time in milliseconds.
   */
  std::int32_t time;

  /**
   * @brief Cursor position.
   */
  vector2f position;

  /**
   * @brief Button state.
   */
  key_state keys;
};

using lifebar_graph = std::vector<lifebar_component>;
using replay_frames = std::vector<replay_frame>;

struct replay_file {
  /**
   * @brief Game mode (0 = osu!, 1 = Taiko, 2 = CtB, 3 = osu!mania).
   */
  osu_modes mode;

  /**
   * @brief Version of the game when the replay was created (ex. 20131216).
   */
  std::int32_t version;

  /**
   * @brief osu! beatmap MD5 hash.
   */
  std::string beatmap_hash;

  /**
   * @brief The name of the player.
   */
  std::string player_name;

  /**
   * @brief  osu! replay MD5 hash (includes certain properties of the replay).
   */
  std::string replay_hash;

  /**
   * @brief Number of 300s.
   */
  std::int16_t count_300s;

  /**
   * @brief Number of 100s in standard, 150s in Taiko, 100s in CTB, 100s in mania.
   */
  union {
    std::int16_t count_100s;
    std::int16_t count_150s;
  };

  /**
   * @brief Number of 50s in standard, small fruit in CTB, 50s in mania.
   */
  union {
    std::int16_t count_50s;
    std::int16_t count_small_fruits;
  };

  /**
   * @brief Number of Gekis in standard, Max 300s in mania.
   */
  union {
    std::int16_t count_gekis;
    std::int16_t max_300s;
  };

  /**
   * @brief Number of Katus in standard, 200s in mania.
   */
  union {
    std::int16_t count_katus;
    std::int16_t count_200s;
  };

  /**
   * @brief Number of misses.
   */
  std::int16_t count_misses;

  /**
   * @brief Total score displayed on the score report.
   */
  std::int32_t score;

  /**
   * @brief Greatest combo displayed on the score report
   */
  std::int16_t max_combo;

  /**
   * @brief Perfect/FC.(true = no misses and no slider breaks and no early finished sliders).
   */
  bool perfect;

  /**
   * @brief Bitflag representing the active mods.
   */
  osu_mods mods;

  /**
   * @brief Parsed lifebar graph.
   */
  lifebar_graph lifebar;

  /**
   * @brief Timestamp in windows ticks.
   */
  std::int64_t timestamp;

  /**
   * @brief Parsed replay frames.
   */
  replay_frames frames;

  /**
   * @brief RNG seed. Present if the version is version_has_rng or higher.
   */
  std::optional<std::int32_t> rng_seed;

  /**
   * @brief Online Score ID. Present if the version is version_first_osz2 or higher.
   */
  std::optional<std::int64_t> online_score_id;

  /**
   * @brief Additional mod information. Only present if Target Practice is enabled.
   */
  std::optional<double> target_practice_accuracy;
};

/**
 * @brief Decode a replay from memory.
 * @param data Replay data.
 * @return A pointer to the decoded replay if successful.
 */
std::unique_ptr<replay_file> from_memory(std::span<const char> data);

/**
 * @brief Decode a replay from a file.
 * @param path Input file.
 * @return A pointer to the decoded replay if successful.
 */
std::unique_ptr<replay_file> from_file(const std::filesystem::path& path);

}  // namespace osp::osr
