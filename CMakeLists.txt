cmake_minimum_required(VERSION 3.0)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

project(yandex_contest_system)

find_package(BunsanCMake CONFIG REQUIRED)

bunsan_add_library(${PROJECT_NAME}
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

    src/cgroup/detail/StreamableWrapper.cpp

    src/cgroup/ProcPidCgroup.cpp
    src/cgroup/HierarchyInfo.cpp
    src/cgroup/SystemInfo.cpp
    src/cgroup/ControlGroup.cpp
    src/cgroup/SingleControlGroup.cpp
    src/cgroup/MultipleControlGroup.cpp
    src/cgroup/ProcessHierarchyInfo.cpp
    src/cgroup/ProcessInfo.cpp
    src/cgroup/MountPoint.cpp
    src/cgroup/Freezer.cpp
    src/cgroup/CpuSet.cpp
    src/cgroup/CpuAccounting.cpp
    src/cgroup/Memory.cpp
    src/cgroup/MemorySwap.cpp
    src/cgroup/Termination.cpp

    src/cgroup/detail/AttachedControlGroup.cpp
    src/cgroup/detail/CreatedControlGroup.cpp

    src/lxc/Config.cpp
    src/lxc/RootfsConfig.cpp
    src/lxc/Lxc.cpp
    src/lxc/LxcApi.cpp
    src/lxc/MountConfig.cpp
)
bunsan_use_bunsan_package(${PROJECT_NAME} yandex_contest_common yandex_contest_common)
target_link_libraries(${PROJECT_NAME} PRIVATE dl)
bunsan_use_pkgconfig(${PROJECT_NAME} LXC lxc)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME})
bunsan_install_project()

bunsan_include_tests()
