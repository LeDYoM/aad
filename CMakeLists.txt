cmake_minimum_required(VERSION 3.16)
project(
  aad
  VERSION 0.0.0.1
  DESCRIPTION "Almost Always Doing"
  # [HOMEPAGE_URL <url-string>]
  LANGUAGES CXX)

add_executable(aad)

target_sources(aad PRIVATE src/include/container_type.h
                           src/include/stack_thread_pool.h
                           )

target_sources(aad PRIVATE src/main.cpp)

set_target_properties(aad PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    POSITION_INDEPENDENT_CODE ON)
