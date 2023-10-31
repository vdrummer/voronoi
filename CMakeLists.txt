cmake_minimum_required(VERSION 3.18)
project(voronoi)

include_directories(include)

add_executable(voronoi src/voronoi.c)
target_sources(voronoi PRIVATE
  src/hsl.c
)
target_link_libraries(voronoi PRIVATE m)
