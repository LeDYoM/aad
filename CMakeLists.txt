cmake_minimum_required(VERSION 3.16)
project(
  aad
  VERSION 0.0.0.1
  DESCRIPTION "Almost Always Doing"
  # [HOMEPAGE_URL <url-string>]
  LANGUAGES CXX)

add_executable(aad)

target_sources(aad PRIVATE src/include/th_pool.h
                           src/include/stack_thread_pool.h
                           src/th_pool.cpp)

target_sources(aad PRIVATE src/main.cpp)

set_target_properties(aad PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    POSITION_INDEPENDENT_CODE ON)
