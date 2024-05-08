# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\TakeFood_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\TakeFood_autogen.dir\\ParseCache.txt"
  "TakeFood_autogen"
  )
endif()
