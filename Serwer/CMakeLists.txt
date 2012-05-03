project(TPServer)
cmake_minimum_required(VERSION 2.6)

find_package(Boost COMPONENTS program_options filesystem system thread REQUIRED)

# where to look for cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_HOME_DIRECTORY}/cmake_modules )

set(CMAKE_BUILD_TYPE Debug)

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -Wclobbered -Wempty-body -Wignored-qualifiers -Wmissing-field-initializers -Wsign-compare -Wtype-limits -Wuninitialized")
endif()

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -Wextra -Wno-logical-op-parentheses")
endif()

set(TPServer_SRCS 
    main.cpp
    Dictionary.cpp
    Serwer.cpp
    stdafx.cpp
    StopWatch.cpp
    PlayerDrivers.cpp
    Pos.cpp
    Actions.cpp
)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${Boost_INCLUDE_DIRS})

add_executable(TPServer ${TPServer_SRCS})

if(UNIX)

else(UNIX)
  if(WIN32)

  endif(WIN32)
endif(UNIX)

target_link_libraries(TPServer ${Boost_LIBRARIES})
