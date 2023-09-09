cmake_minimum_required(VERSION 3.10)

project(SpyOpt CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(yaml-cpp REQUIRED)

include_directories(
    include
    ${YAML_CPP_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME}
    src/agent.cpp
    src/spy_opt.cpp
    src/config_parser.cpp
)

target_link_libraries(${PROJECT_NAME}
  ${catkin_LIBRARIES}
  ${YAML_CPP_LIBRARIES}
)

add_executable(spyopt
  src/main.cpp
)

target_link_libraries(spyopt
  ${PROJECT_NAME}
)