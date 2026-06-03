cmake_minimum_required(VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 23)

set(This TypeaheadSearchProfiling)

project(${This})

set(TypeaheadSearchProfilingPath ${CMAKE_CURRENT_SOURCE_DIR})
cmake_path(GET TypeaheadSearchProfilingPath PARENT_PATH TypeaheadSearchRootPath)

include_directories(${TypeaheadSearchRootPath}/include)

include(FetchContent)
FetchContent_Declare(
  slot-map
  GIT_REPOSITORY https://github.com/TheNitesWhoSay/slot-map.git
  GIT_TAG 6bbbd6a
)
FetchContent_MakeAvailable(slot-map)
include_directories(${slot-map_SOURCE_DIR}/include)

find_package(benchmark CONFIG REQUIRED)
get_target_property(BENCHMARK_INCLUDE_DIRS benchmark::benchmark INTERFACE_INCLUDE_DIRECTORIES)
include_directories(${BENCHMARK_INCLUDE_DIRS})

add_compile_definitions(PREFERRED_WORKING_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}")

set(Headers
)
set(Sources
  "main.cpp"
)

add_executable(${This} ${Sources} ${Headers})

find_package(ICU COMPONENTS uc i18n data REQUIRED)

target_link_libraries(${This} PRIVATE benchmark::benchmark benchmark::benchmark_main ICU::uc ICU::i18n ICU::data)