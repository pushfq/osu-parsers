cmake_minimum_required(VERSION 3.25.0)

project(osu-parsers)

find_package(LibLZMA REQUIRED)

add_library(
  ${PROJECT_NAME}
  STATIC
  src/detail/binary_decoder.cpp
  src/osr/replay_file.cpp
)

add_library(
  ${PROJECT_NAME}::${PROJECT_NAME}
  ALIAS
  ${PROJECT_NAME}
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
  include
)

target_compile_features(
  ${PROJECT_NAME}
  PRIVATE
  cxx_std_23
)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE
  LibLZMA::LibLZMA
)
