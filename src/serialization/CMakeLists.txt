add_library(
  _serialization
  chat_completions.cpp
  costs.cpp
  embeddings.cpp
  files.cpp
  fine_tuning.cpp
  models.cpp
  response_to_json.cpp
) 

target_include_directories(_serialization PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(_serialization PRIVATE _networking)
