add_executable(solver main.cc)

target_include_directories(solver
  PUBLIC astar_ddd
  PUBLIC compress
  PUBLIC external_astar
  PUBLIC hash_functions
  PUBLIC utils)

add_subdirectory(compress)
add_subdirectory(utils)

add_library(utils SHARED utils.cc)
add_library(fatal SHARED fatal.cc)
add_library(tiles SHARED tiles.cc)

target_link_libraries(solver
  PRIVATE named_fstream
  PRIVATE wall_timer
  PRIVATE pointer_table
  PRIVATE tiles
  PRIVATE fatal
  PRIVATE utils)
