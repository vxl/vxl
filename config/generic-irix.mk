MAKE_ARLIB_CMD := ar qcv $@ 
CXX_HDRDIR=
RANLIB=:
X11_INC_DIR=
X11_LIB_DIR=
#LORDER=lorder

lib_sockets :=

ifeq ($(OS),IRIX5)
lib_dl := -ldl
lib_c := -lsun -lmalloc -lc
else
lib_dl := 
lib_c := -lc -lmalloc
endif

soname_opt = -Wl,-soname,$(shlibname) # this option must be passed to the linker

SGIArchitecture := 1

## IRIX linker is incredibly slow over NFS.
LINK_INTO_TMP := 1

OS_DEFINES += -DSVR4 -DSYSV

OS_COMPILE.S = as $(source_for_object) $(extraflags_for_object) $(CFLAGS) -c $(OUTPUT_OPTION)

IUE_LD_ALLEXTRACT = -Wl,-all
IUE_LD_ALLEXTRACT_OFF = -Wl,-notall

ld_whole_archive_on = $(IUE_LD_ALLEXTRACT)
ld_whole_archive_off = $(IUE_LD_ALLEXTRACT_OFF)

STRIP := echo # There are problems running stripped versions of the executables on SGI
