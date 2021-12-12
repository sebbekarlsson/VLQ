cmake_minimum_required(VERSION 3.20)
add_compile_options(-g -Wall)

project(VLQ)

file(GLOB PUBLIC_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB VLQ_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)


add_executable(VLQ_e ${VLQ_sources})


list(FILTER VLQ_sources EXCLUDE REGEX "main.c")

add_library(VLQ SHARED ${VLQ_sources})
add_library(VLQ_static STATIC ${VLQ_sources})

target_compile_options(VLQ PUBLIC -fPIC)
target_compile_options(VLQ_static PUBLIC -fPIC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories(
  VLQ
    PUBLIC
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      ${CMAKE_CURRENT_BINARY_DIR}/include
      ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_include_directories(
  VLQ_static
    PUBLIC
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      ${CMAKE_CURRENT_BINARY_DIR}/include
      ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_include_directories(
  VLQ_e
    PUBLIC
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      ${CMAKE_CURRENT_BINARY_DIR}/include
      ${CMAKE_CURRENT_SOURCE_DIR}/include)


include(FetchContent)
FetchContent_Declare(
  b64_static
  GIT_REPOSITORY https://github.com/sebbekarlsson/b64.git
  )

FetchContent_MakeAvailable(b64_static)

target_link_libraries(VLQ PRIVATE m b64_static)
target_link_libraries(VLQ_e PRIVATE m b64_static)
target_link_libraries(VLQ_static PRIVATE m b64_static)
