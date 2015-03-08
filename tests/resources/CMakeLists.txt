project(${PROJECT_NAME}_tests_resources)

add_executable(dummy_process dummy_process.c)
bunsan_use_threads(dummy_process)
