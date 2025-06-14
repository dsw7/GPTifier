add_library(
  lib_networking
  api_openai_admin.cpp
  api_openai_user.cpp
  curl_base.cpp
) 

target_include_directories(lib_networking PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(lib_networking PRIVATE curl)
