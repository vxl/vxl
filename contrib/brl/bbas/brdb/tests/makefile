USES += contrib/brl contrib/brl/bbas core
include ../config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx

SOURCES += sample_database.cxx
TESTS += test_database.cxx
TESTS += test_query.cxx
TESTS += test_relation.cxx
TESTS += test_tuple.cxx
TESTS += test_value.cxx

MINI_PROG_SOURCES += test_include.cxx
MINI_PROG_SOURCES += test_template_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
