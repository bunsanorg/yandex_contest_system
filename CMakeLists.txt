project(yandex_contest_system)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

cmake_minimum_required(VERSION 2.8)

include(BunsanCommon)

# BOOST
bunsan_use_boost(system filesystem serialization)

# LXC
find_package(PkgConfig)
pkg_check_modules(LXC REQUIRED lxc)
link_directories(${LXC_LIBRARY_DIRS})
include_directories(${LXC_INCLUDE_DIRS})
bunsan_use(${LXC_LIBRARIES})

bunsan_use_bunsan(common)
bunsan_use(yandex_contest_common)

bunsan_use(dl)

bunsan_add_shared_library(${PROJECT_NAME}
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
    src/unistd/DynamicLoader.cpp

    src/execution/ResultError.cpp
    src/execution/ErrCall.cpp
    src/execution/AsyncProcess.cpp

    src/Trace.cpp

    src/cgroup/MountPoint.cpp
    src/cgroup/ControlGroup.cpp
    src/cgroup/Freezer.cpp
    src/cgroup/CpuSet.cpp
    src/cgroup/CpuAccounting.cpp
    src/cgroup/Memory.cpp
    src/cgroup/MemorySwap.cpp

    src/lxc/Config.cpp
    src/lxc/lxc.c
    src/lxc/MountConfig.cpp
    src/lxc/RootfsConfig.cpp
    src/lxc/Lxc.cpp
)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME})

bunsan_include_tests()
