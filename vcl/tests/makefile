USES = vxl

TESTS += test_vcl_compiler.cxx
TESTS += test_vcl_long_double.cxx
TESTS += test_vcl_rel_ops.cxx
TESTS += test_vcl_iterator.cxx
TESTS += test_vcl_iostream.cxx
TESTS += test_vcl_fstream.cxx
TESTS += test_vcl_map.cxx
TESTS += test_vcl_multimap.cxx
TESTS += test_vcl_vector.cxx
TESTS += test_vcl_list.cxx
TESTS += test_vcl_string.cxx
TESTS += test_vcl_set.cxx
TESTS += test_vcl_deque.cxx
TESTS += test_vcl_complex.cxx
TESTS += test_vcl_algorithm.cxx
TESTS += test_vcl_new.cxx
TESTS += test_vcl_cmath.cxx
TESTS += $(wildcard test_vcl_rtti.cxx)

IULIBS = -lvcl 

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################

STRIP = echo # There are problems running stripped versions of test_vcl_iterator.exe and test_vcl_rel_ops.exe on SGI
