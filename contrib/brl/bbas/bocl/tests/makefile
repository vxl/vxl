USES += contrib/brl contrib/brl/bbas core
include ../config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
SOURCES += bocl_global_memory_bandwidth_manager.cxx
SOURCES += bocl_command_queue_mgr.cxx

TESTS += test_global_io_bandwidth.cxx
TESTS += test_command_queue.cxx
TESTS += test_kernel.cxx

MINI_PROG_SOURCES += test_include.cxx
MINI_PROG_SOURCES += test_template_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
