add_library(
  lib_interface
  command_chats.cpp
  command_costs.cpp
  command_edit.cpp
  command_embed.cpp
  command_files.cpp
  command_fine_tune.cpp
  command_models.cpp
  command_run.cpp
  command_short.cpp
  help_messages.cpp
)

target_include_directories(lib_interface PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(lib_interface PRIVATE lib_serialization pthread)
