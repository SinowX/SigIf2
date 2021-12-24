set (LIBTHRD thrd.cc)
set (LIBINSMGR instruction_manager.cc)


add_library(thrd STATIC ${LIBTHRD})
set_target_properties(thrd PROPERTIES OUTPUT_NAME "thrd")

add_library(insmgr STATIC ${LIBINSMGR})
set_target_properties(insmgr PROPERTIES OUTPUT_NAME "insmgr")


add_executable(main main.cc)
target_link_libraries(main thrd insmgr)

add_subdirectory(${PROJECT_SOURCE_DIR}/src/include)
