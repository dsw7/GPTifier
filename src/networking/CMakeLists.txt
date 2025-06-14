add_library(
  _networking
  api_openai_admin.cpp
  api_openai_user.cpp
  curl_base.cpp
) 

target_include_directories(_networking PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(_networking PRIVATE curl)
