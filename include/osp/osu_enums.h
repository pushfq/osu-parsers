#pragma once

#include <cstdint>
#include <utility>

namespace osp {

enum struct osu_modes : std::uint8_t {
  osu = 0,
  taiko = 1,
  catch_the_beat = 2,
  mania = 3,
};

enum struct osu_mods : std::uint32_t {
  no_fail = 1 << 0,
  easy = 1 << 1,
  no_video = 1 << 2,
  hidden = 1 << 3,
  hard_rock = 1 << 4,
  sudden_death = 1 << 5,
  double_time = 1 << 6,
  relax = 1 << 7,
  half_time = 1 << 8,
  nightcore = 1 << 9,
  flashlight = 1 << 10,
  autoplay = 1 << 11,
  spun_out = 1 << 12,
  relax2 = 1 << 13,
  perfect = 1 << 14,
  key4 = 1 << 15,
  key5 = 1 << 16,
  key6 = 1 << 17,
  key7 = 1 << 18,
  key8 = 1 << 19,
  fade_in = 1 << 20,
  random = 1 << 21,
  cinema = 1 << 22,
  target = 1 << 23,
  key9 = 1 << 24,
  key_coop = 1 << 25,
  key1 = 1 << 26,
  key3 = 1 << 27,
  key2 = 1 << 28,
  score_v2 = 1 << 29,
  mirror = 1 << 30,
};

enum struct key_state : std::uint32_t {
  none = 0,
  m1 = 1 << 0,
  m2 = 1 << 1,
  k1 = 1 << 2,
  k2 = 1 << 3,
  smoke = 1 << 4,
};

constexpr osu_mods operator|(osu_mods lhs, osu_mods rhs) {
  return static_cast<osu_mods>(std::to_underlying(lhs) | std::to_underlying(rhs));
}

constexpr osu_mods operator&(osu_mods lhs, osu_mods rhs) {
  return static_cast<osu_mods>(std::to_underlying(lhs) & std::to_underlying(rhs));
}

constexpr osu_mods operator^(osu_mods lhs, osu_mods rhs) {
  return static_cast<osu_mods>(std::to_underlying(lhs) ^ std::to_underlying(rhs));
}

constexpr osu_mods operator~(osu_mods mods) {
  return static_cast<osu_mods>(~std::to_underlying(mods));
}

constexpr osu_mods& operator|=(osu_mods& lhs, osu_mods rhs) {
  return lhs = lhs | rhs;
}

constexpr osu_mods& operator&=(osu_mods& lhs, osu_mods rhs) {
  return lhs = lhs & rhs;
}

constexpr osu_mods& operator^=(osu_mods& lhs, osu_mods rhs) {
  return lhs = lhs ^ rhs;
}

}  // namespace osp
