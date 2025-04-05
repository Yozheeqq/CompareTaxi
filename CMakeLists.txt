set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

cmake_minimum_required(VERSION 3.12...3.31)
project(taxi_compare CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(DownloadUserver)

find_package(userver COMPONENTS core kafka postgresql QUIET)
if(NOT userver_FOUND)
  # Tries TRY_DIR first, falls back to downloading userver from GitHub using CPM.
  download_userver(TRY_DIR third_party/userver)
endif()

userver_setup_environment()

set(ONNXRUNTIME_ROOT "${CMAKE_SOURCE_DIR}/onnxruntime")
include_directories(${ONNXRUNTIME_ROOT}/include)
link_directories(${ONNXRUNTIME_ROOT}/lib)

# Common sources
add_library(${PROJECT_NAME}_objs OBJECT
    http_server/handlers.cpp
    http_server/handlers.h
    lib/common_types.cpp
    lib/common_types.h
    lib/kafka_helpers.h
    lib/kafka_helpers.cpp
    lib/model.cpp
    lib/model.h
    nodes/profile_updater.cpp
    nodes/profile_updater.h
    nodes/ml_updater.h
    nodes/ml_updater.cpp
)
target_include_directories(${PROJECT_NAME}_objs PUBLIC ${CMAKE_SOURCE_DIR}/lib)
target_link_libraries(${PROJECT_NAME}_objs PUBLIC
  userver::core
  userver::kafka
  userver::postgresql
  userver-ydb
  onnxruntime
)

# The Service
add_executable(http_server http_server/main.cpp)
target_link_libraries(http_server PRIVATE ${PROJECT_NAME}_objs)

# The Nodes Service
add_executable(node nodes/main.cpp)
target_link_libraries(node PRIVATE ${PROJECT_NAME}_objs)

# Install
include(GNUInstallDirs)

if(DEFINED ENV{PREFIX})
  message(STATUS "Set install prefix: $ENV{PREFIX}")
  file(TO_CMAKE_PATH "$ENV{PREFIX}" PREFIX_PATH)
  set(CMAKE_INSTALL_PREFIX ${PREFIX_PATH})
endif()

file(GLOB CONFIGS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.yaml ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.json)

install(TARGETS http_server DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${PROJECT_NAME})
install(TARGETS node DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${PROJECT_NAME})
install(FILES ${CONFIGS_FILES} DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME} COMPONENT ${PROJECT_NAME})
