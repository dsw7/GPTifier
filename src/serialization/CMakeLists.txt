add_library(
  lib_serialization
  chat_completions.cpp
  costs.cpp
  embeddings.cpp
  files.cpp
  fine_tuning.cpp
  models.cpp
  response_to_json.cpp
) 

target_include_directories(lib_serialization PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(lib_serialization PRIVATE lib_networking)
