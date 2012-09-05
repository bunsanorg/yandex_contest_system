project(yandex_contest_system)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

cmake_minimum_required(VERSION 2.8)

include(YandexContestCommon)

# BOOST
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost COMPONENTS system filesystem serialization unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
list(APPEND libraries ${Boost_LIBRARIES})

# LXC
find_package(PkgConfig)
pkg_check_modules(LXC REQUIRED lxc)
link_directories(${LXC_LIBRARY_DIRS})
include_directories(${LXC_INCLUDE_DIRS})
list(APPEND libraries ${LXC_LIBRARIES})

# yandex_contest_common
list(APPEND libraries yandex_contest_common)

# Local
include_directories(include)

# Targets

add_library(${PROJECT_NAME} SHARED
    src/unistd/access/Id.cpp
    src/unistd/access/Operations.cpp

    src/unistd/Pipe.cpp
    src/unistd/Descriptor.cpp
    src/unistd/ResourceUsage.cpp
    src/unistd/CharStarStar.cpp
    src/unistd/Exec.cpp
    src/unistd/ProcessResult.cpp
    src/unistd/FileStatus.cpp
    src/unistd/ExecImpl.cpp
    src/unistd/Fstab.cpp
    src/unistd/MountEntry.cpp
    src/unistd/MountEntryAlias.cpp
    src/unistd/Operations.cpp

    src/execution/ResultError.cpp
    src/execution/ErrCall.cpp
    src/execution/AsyncProcess.cpp

    src/cgroup/MountPoint.cpp
    src/cgroup/ControlGroup.cpp
    src/cgroup/Freezer.cpp
    src/cgroup/CpuSet.cpp
    src/cgroup/CpuAccounting.cpp
    src/cgroup/Memory.cpp
    src/cgroup/MemorySwap.cpp

    src/lxc/Config.cpp
    src/lxc/LXC.c
    src/lxc/MountConfig.cpp
    src/lxc/LXC.cpp
)
target_link_libraries(${PROJECT_NAME} ${libraries})

# installation
install(DIRECTORY include DESTINATION .)
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

yandex_contest_tests()
