
VXL := 1
USES = vxl

TESTS += vbl_test_arg.cxx
TESTS += vbl_test_sparse_array_2d.cxx
TESTS += vbl_test_sprintf.cxx
TESTS += vbl_test_smart_ptr.cxx
#TESTS += vbl_test_basic_relation.cxx   FIXME

IULIBS := -lvbl -lvcl

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################

# each makefile MUST have include $(IUEROOT)/config/something in order for the script generate_class_table to work
# so please NEVER replace this by something like "all:\t make -f some_makefile.mk"
