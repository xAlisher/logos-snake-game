# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/high_score_plugin_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/high_score_plugin_autogen.dir/ParseCache.txt"
  "high_score_plugin_autogen"
  )
endif()
