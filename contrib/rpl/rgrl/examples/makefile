USES += contrib/rpl core
IULIBS += -lrgrl_examples
include ../config.mk
include $(IUEROOT)/core/testlib/config.mk

LIBRARY := rgrl_examples
SOURCES += test_util.cxx

MINI_PROG_SOURCES += estimate_normals_3d.cxx

TESTS += registration1.cxx
PARAMS_registration1 := IMG0002.pgm.txt
TESTS += registration2.cxx
PARAMS_registration2 := IMG0002.pgm.txt IMG0003.pgm.txt
TESTS += registration3.cxx
PARAMS_registration3 := IMG0002.pgm.txt IMG0002_low.pgm.txt IMG0003.pgm.txt IMG0003_low.pgm.txt
TESTS += registration_simple_shapes.cxx
TESTS += registration_simple_shapes_outliers.cxx
TESTS += registration_blackbox.cxx
TESTS += registration_landmark.cxx
PARAMS_registration_landmark := IMG0004.pgm.landmarks.txt IMG0002.pgm.landmarks.txt mask.png
TESTS += registration_retina_no_view.cxx
PARAMS_registration_retina_no_view := IMG0002.pgm.txt IMG0002.pgm.landmarks.invariant.txt IMG0004.pgm.txt IMG0004.landmarks.invariant.pgm.txt
TESTS += registration_retina.cxx
PARAMS_registration_retina_no_view := IMG0002.pgm.txt IMG0002.pgm.landmarks.invariant.txt IMG0004.pgm.txt IMG0004.landmarks.invariant.pgm.txt
TESTS += registration_range_data.cxx
PARAMS_registration_range_data := bun000.txt.out bun045.txt.out
TESTS += registration_range_data_blackbox.cxx
PARAMS_registration_range_data_blackbox := dragonStandRight_0_normals.txt

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
