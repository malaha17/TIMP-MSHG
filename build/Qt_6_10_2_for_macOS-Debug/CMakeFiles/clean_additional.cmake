# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "client/CMakeFiles/saper_client_autogen.dir/AutogenUsed.txt"
  "client/CMakeFiles/saper_client_autogen.dir/ParseCache.txt"
  "client/saper_client_autogen"
  "common/CMakeFiles/common_autogen.dir/AutogenUsed.txt"
  "common/CMakeFiles/common_autogen.dir/ParseCache.txt"
  "common/common_autogen"
  "core/CMakeFiles/core_autogen.dir/AutogenUsed.txt"
  "core/CMakeFiles/core_autogen.dir/ParseCache.txt"
  "core/core_autogen"
  "server/CMakeFiles/saper_server_autogen.dir/AutogenUsed.txt"
  "server/CMakeFiles/saper_server_autogen.dir/ParseCache.txt"
  "server/saper_server_autogen"
  )
endif()
