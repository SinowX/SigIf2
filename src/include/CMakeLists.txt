set (LIBTHRD thrd.cc)
set (LIBINSMGR instruction_manager.cc)


add_library(thrd STATIC ${LIBTHRD})
set_target_properties(thrd PROPERTIES OUTPUT_NAME "thrd")

add_library(insmgr STATIC ${LIBINSMGR})
set_target_properties(insmgr PROPERTIES OUTPUT_NAME "insmgr")
