#OS_DEFINES := -DSVR4 -DSYSV
MAKE_ARLIB = rm -f $@ && ar qcv $@
RPATHLIBDIRS = $(strip $(LIBDIRS))
ifeq ($(BUILD),noshared)
RPATHLIBDIRS = $(strip $(filter-out %.noshared,$(LIBDIRS)))
endif
rpath = $(RPATHLIBDIRS:%=-R%)
lib_dl := -ldl

MAKEDEPEND := $(C++) -M
MAKE_SHLIB := $(C++) 
pic := -Kpic
PIC := -KPIC
wall :=  
debug = -g

CC := cc
#C++ := KCC -DUSE_KCC_PCH --pch --pch_dir /var/tmp/pch
C++ := KCC

LINK_INTO_TMP := 1

ifndef TMPDIR
 TMPDIR := /tmp
endif

SolarisArchitecture := 1


#optimize := -O
optimize := 

ifeq "$(HAS_GLUT)" "1"
GLUT_XILIB	:= -lXi
endif
