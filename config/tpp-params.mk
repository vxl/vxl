#
# This file is included from params.mk *only*
#
# Purpose: Supply 3rd party library parameter for use in client makefiles.
#
# Here we respond to values from config.mk.in and set HAS_blah := 1 for each
# 3rd party package "blah" installed. Subsequent USE_blah statements should
# query HAS_blah to determine whether or not "blah" is installed, so make sure
# you get it right here.
#
# Note that this mirrors the 3rd party library section in tpp-rules.mk, in fact
# most of this can be simplified away by directly autoconfing for HAS_blah, and
# blah_* There are exceptions, though, e.g. the TIFF and JPEG libraries where
# the TargetJr ones are used if no native libraries are given.

# The autoconf output variable is called no_x
ifeq "" "$(no_x)"
 HAS_X11 := 1
else
 HAS_X11 :=
endif


# Check Fresco setup
ifeq "$(HAS_FRESCO)" "1"
 FRESCO_LIBS :=  -lfigures -lwidgets -lfdisplay -lbase -llayouts -ldefs -locs -ljpeg -lpng -lz # -ltiff -limages -ltext -lpthread
 ifneq ($(HAS_X11),1)
  FRESCO_LIBS += -lGDI32 -lUSER32 -lwsock32
 endif
#else
# # Comment on nonexistence of fresco
# DEFINES += -UHAS_FRESCO
endif


# Check Interviews setup
ifneq "" "$(IVINSTALLED_DIR)"
 HAS_IV := 1
 IV_INCLUDES := $(IVINSTALLED_DIR)$/include$/IV-2_6 $(IVINSTALLED_DIR)$/include
 ifeq "" "$(IVINSTALLED_LIBS)"
  IVINSTALLED_LIBS := $(IVINSTALLED_DIR)$/lib$/$(OBJDIR)
 endif
 IV_LIB      := $(IVINSTALLED_LIBS)
 ## fsm@robots : we can't have -lIVAdditions here because -lIVAdditions must
 ## go in the list of archives to all_extract when linking a static tjload-iv.
 ## If it doesn't then -lIVAdditions ends up after -lTJ_COOL on the link line
 ## which causes undefined references in libIVAdditions.a
 #IV_LIBS     := -lIVAdditions -lUnidraw -lIV
 IV_LIBS     := -lUnidraw -lIV
 IV_DEFS     := -DIV_INSTALLED -Div2_6_compatible -D_XDisplay=XDisplay
endif


# This one is a bit different, it uses the makefiles of omniorb to extract
# the compiler flags used by omniorb, so we can use the same flags
# and compile omni stuff ourselves
ifneq "$(OMNIORB)" ""
HAS_OMNIORB := 1
OMNI_DIR := $(dir $(OMNIORB)/)
OMNI_platform := $(shell  $(MAKE) -I$(configdir) -f$(configdir)/omni.mk OMNI_DIR=$(OMNI_DIR) echovar-platform )
OMNI_INC_DIR := $(OMNI_DIR)include
OMNI_LIB_DIR := $(OMNI_DIR)lib/$(OMNI_platform)
OMNI_BIN_DIR := $(OMNI_DIR)bin/$(OMNI_platform)
OMNI_IDL     := $(OMNI_BIN_DIR)/omniidl2
OMNI_DFLAGS := $(shell $(MAKE) -I$(configdir) -f$(configdir)/omni.mk OMNI_DIR=$(OMNI_DIR) echovar-IMPORT_CPPFLAGS)
OMNI_CXXFLAGS := $(shell $(MAKE) -I$(configdir) -f$(configdir)/omni.mk OMNI_DIR=$(OMNI_DIR) echovar-CXXOPTIONS)
OMNI_CXXFLAGS += $(shell $(MAKE) -I$(configdir) -f$(configdir)/omni.mk OMNI_DIR=$(OMNI_DIR) echovar-CXXDEBUGFLAGS)
OMNI_LIBS := -lomniORB2 -lomniDynamic2 -lomnithread -lpthread -lposix4 -lsocket -lnsl -ltcpwrapGK
endif


ifneq "$(ACE)" ""
HAS_ACE := 1
ACE_DIR := $(dir $(ACE)/)
ACE_LIB_DIR := $(ACE_DIR)/ace
ACE_INC_DIR := $(ACE_DIR)
ACE_LIBS := -lACE
endif

NETLIB_INC_DIR := $(IUEROOT)/v3p/netlib

# if no TIFF_LIB_DIR supplied, use the one in v3p.
ifeq ($(strip $(HAS_TIFF))-$(strip $(TIFF_LIB_DIR)),1-)
  TIFF_INC_DIR := $(IUEROOT)/v3p/tiff
  ifndef CENTRAL_LIBDIR
    TIFF_LIB_DIR := $(IUEROOT_libbase)/v3p/$(tj_libdir)
  endif
  TIFF_LIBS := -ltiff
endif

# if no JPEG_LIB_DIR supplied, use the one in v3p.
ifeq ($(strip $(HAS_JPEG))-$(strip $(JPEG_LIB_DIR)),1-)
  JPEG_INC_DIR := $(IUEROOT)/v3p/jpeg
  ifndef CENTRAL_LIBDIR
    JPEG_LIB_DIR := $(IUEROOT_libbase)/v3p/$(tj_libdir)
  endif
  JPEG_LIBS := -ljpeg
endif

# if no PNG_LIB_DIR supplied, use the one in v3p.
ifeq ($(strip $(HAS_PNG))-$(strip $(PNG_LIB_DIR)),1-)
  PNG_INC_DIR := $(IUEROOT)/v3p/png
  ifndef CENTRAL_LIBDIR
    PNG_LIB_DIR := $(IUEROOT_libbase)/v3p/$(tj_libdir)
  endif
  PNG_LIBS := -ljpeg
endif

ifeq "$(HAS_ZLIB)" "1"
  ZLIB_LIBS := -lz
endif

# PNG image library: some makefiles use the value of PNG_LIBS (in ptic, Image/iulibs.mk).
ifeq "$(HAS_PNG)" "1"
  PNG_LIBS := -lpng $(ZLIB_LIBS)
endif

# if no MPEG2_LIB_DIR supplied, use the one in v3p.
ifeq ($(strip $(HAS_MPEG2))-$(strip $(MPEG2_LIB_DIR)),1-)
  MPEG2_INC_DIR := $(IUEROOT)/v3p/mpeg2/include
  ifndef CENTRAL_LIBDIR
    MPEG2_LIB_DIR := $(IUEROOT_libbase)/v3p/$(tj_libdir)
  endif
  MPEG2_LIBS := -lmpeg2
endif

# NETLIB: if none supplied, use the one in v3p
ifneq ($(strip $(HAS_NETLIB)),1)
  HAS_NETLIB := 1
  HAS_NATIVE_NETLIB :=
  ifndef CENTRAL_LIBDIR
    NETLIB_LIB_DIR := $(IUEROOT_libbase)/v3p/$(tj_libdir)
  else
    NETLIB_LIB_DIR :=
  endif
  NETLIB_LIBS := -lnetlib
else
  HAS_NATIVE_NETLIB := 1
  NETLIB_LIBS := -lnetlib
endif


# GLUT
ifeq "$(HAS_GLUT)" "1"
 ifeq (win32,$(OS))
  GLUT_LIBS  := -lglut32 $(GLUT_XILIB)
 else
  GLUT_LIBS  := -lglut $(GLUT_XILIB)
 endif
endif

# FLTK
ifeq "$(HAS_FLTK)" "1"
 ifeq (win32,$(OS))
  FLTK_LIBS  := -lfltk32 # perhaps. who knows?
 else
  FLTK_LIBS  := -lfltk
 endif
endif


# Add MOTIF
ifneq "$(HAS_MOTIF)" ""
# MOTIF_HOMES   := $(dir $(MOTIF)/)
# MOTIF_LIBDIR  := $(MOTIFLIBS)
# MOTIF_INCDIR  := $(MOTIFINCS)
 ifdef HAS_INVENTOR
  MOTIF_LIBS     := -lInventorXt -lInventor -lXm -lXt
  MOTIF_DEFS     := -DINVENTOR
 else
  MOTIF_LIBS   := -lXm -lXt
  MOTIF_DEFS   :=
 endif
endif

# Add MOTIF GL (will need opengl and motif)
ifneq "$(HAS_MOTIFGL)" ""
# MOTIFGL_HOMES := $(dir $(MOTIFGL)/)
# MOTIFGL_LIB_DIR := $(MOTIFGLLIBS)
# MOTIFGL_INC_DIR := $(MOTIFGLINCS)
 ifeq (1,$(OPENGL_IS_MESA))
  MOTIFGL_LIBS   := -lMesaGLwM
 else
  MOTIFGL_LIBS   := -lGLw
 endif
endif


ifneq "$(strip $(VTKHOME))" ""
HAS_VTK := 1
VTK_DIR := $(dir $(VTKHOME)/)
VTK_LIB_DIR := $(VTKLIBPATH) $(VTKLIBPATH)/common $(VTKLIBPATH)/graphics $(VTKLIBPATH)/imaging
VTK_INC_DIR := $(VTK_DIR)common $(VTK_DIR)graphics $(VTK_DIR)imaging
VTK_LIBS := -lVTKGraphics -lVTKImaging -lVTKCommon $(VTK_EXTRA_LIBS)
VTK_DEFINES := -DWITH_VTK
endif

ifeq "$(HAS_QT)" "1"
 QT_LIBS := -lqt
 MOC := $(QT_MOC_DIR)/moc
endif

ifeq "$(HAS_QGL)" "1"
 QGL_LIBS := -lqt # -lXmu
endif

# Add GTK
ifneq "$(GTKCONFIGLIBS)" ""
 HAS_GTK        := 1
 GTK_INC_DIRS   := $(patsubst -I%,%,$(filter -I%,$(GTKCONFIGCFLAGS)))
 GTK_LIB_DIRS   := $(patsubst -L%,%,$(filter -L%,$(GTKCONFIGLIBS)))
 GTK_LIBS       := $(filter -l%,$(GTKCONFIGLIBS))
endif

# backwards compat
HAS_GTKGL := $(HAS_GTKGLAREA)

# Add GtkGlArea (will need opengl and gtk)
ifeq "$(HAS_GTKGLAREA)" "1"
  GTKGLAREA_LIBS     := -lgtkgl
endif

## What is the purpose of this? HAS_OPENGL should be set
## by configure. If this is needed, it means there is
## some failure of the build system somewhere and it
## should be fixed properly. I'll remove it unless
## someone adds an explanation.
## fsm@robots.ox.ac.
#ifdef OPENGL_LIB_DIR
#HAS_OPENGL:=1
#endif
#ifdef OPENGL_INC_DIR
#HAS_OPENGL:=1
#endif

# we can have the libs but not need an include on windows!
ifeq (1,$(HAS_OPENGL))
 OPENGLDEF     := -DOPENGLRENDER
 ifndef OPENGL_LIBS
  ifeq (win32,$(OS))
   OPENGL_LIBS := -lGlu32 -lGlaux -lOpengl32
  else
# capes@robots : these library names are no longer used in modern versions
# of Mesa. Update your Mesa, rename the libraries or add symbolic links if
# you have a problem.
# PVr: just check for the existence of libGLU.so (or .sl) and decide then.
   ifeq (1,$(OPENGL_IS_MESA))
    ifeq (,$(wildcard $(OPENGL_LIB_DIR)/libGLU.s[lo]))
     OPENGL_LIBS     := -lMesaGLU -lMesaGL
    else
     OPENGL_LIBS     := -lGLU -lGL
    endif
   else
    OPENGL_LIBS     := -lGLU -lGL
   endif
  endif
 endif
endif


# XGL
ifneq "$(XGL)"  ""
HAS_XGL  := 1
XGLHOMES := $(dir $(XGL)/)
XGLLIB   := $(XGLHOMES)lib
XGLINCS  := $(XGLHOMES)include
XGLDEF   := -DXGLRENDER
XGL_LIBS := -lxgl
endif

# TCL
ifneq "$(TCL)"  ""
HAS_TCL  := 1
TCLHOMES := $(dir $(TCL)/)
TCLLIB   := $(TCLHOMES)lib
TCLINCS  := $(TCLHOMES)include
TCLDEFINES := -DTCL_INSTALLED
TCLLIBS := -ltcl8.0
endif

# MSQLHOME
ifneq "$(MSQL)"  ""
HAS_MSQL       := 1
HAS_PERSISTENT := 1
MSQLHOMES      := $(dir $(MSQL)/)
MSQLLIB        := $(MSQLHOMES)lib
MSQLINCS       := $(MSQLHOMES)include
MSQLLIBS       := -lmsql
MSQLDEF        := -DMSQL
endif

# MYSQL
ifneq "$(MYSQL)"  ""
HAS_MYSQL      := 1
HAS_PERSISTENT := 1
MYSQLHOMES     := $(dir $(MYSQL)/)
MYSQLLIB       := $(MYSQLHOMES)lib
MYSQLINCS      := $(MYSQLHOMES)include
MYSQLLIBS      := -lmysqlclient
MYSQLDEF       := -DMYSQL=MYSQL
endif

# XERCES
ifeq "$(HAS_XERCES)" "1"
  XERCESLIB   := $(XERCESHOME)/lib
  XERCESINCS  := $(XERCESHOME)/include
  XERCES_LIBS := -lxerces-c1_4
endif

