#
# This file is included from rules.mk *only*
#
# 3rd-party library section.
#
# "3rd-party" means non-IUE libraries that one compiles and installs independently.
#
# The purpose of this section is to detect when makefiles have set USE_blah, where
# blah is a 3rd party package, and to set appropriate variables such as include
# and library paths. Moreover, inconsistencies, like trying to USE_GLUT on a
# system which is not configured for GLUT should be picked up here (see below).

# 3rd Party library parameters which clients might want to refer to should be
# supplied in the closely related file tpp-params.mk


# fsm: This was originally above the start of the 3rd party library section. Is
# there some reason why it must be up here?
#
# Select: USE_INTERVIEWS
# Set to 1 to add InterViews headers and libs to the compile.
ifeq ($(strip $(USE_INTERVIEWS)),1)
  ifneq ($(strip $(HAS_IV)),1)
    err_CANNOT_BUILD_HERE += "USE_INTERVIEWS"
  else
    DEFINES += $(IV_DEFS)
    INCDIRS += $(IV_INCLUDES)
    LIBDIRS += $(IV_LIB)

    DIR_Viewing := $(foreach file,Viewing,$(sys_or_iu_macro_lib))

    # Be specific here --awf
    statics_objs := AskQuit intercept_abort patch_xinter patch_xpainter patch_xwindow
    BUGFIX_OBJECTS += $(statics_objs:%=$(DIR_Viewing)/Statics/$(OBJDIR)/%.$(OBJ_EXT))
    STDLIBS += $(IV_LIBS)
    USE_X11 := 1

# Select: IVRENDER
# Set to 1 to use InterViews rather than OpenGL for 3D rendering.
    ifeq ($(strip $(IVRENDER)),1)
      DEFINES += -DIVRENDER
    endif
  endif
#else
#  DEFINES += -UIV_INSTALLED
endif


# Select: USE_FRESCO
# Set to 1 in order to add Fresco headers and libraries to the compile.
# If HAS_FRESCO has not been set, adds -UHAS_FRESCO to DEFINES
ifeq ($(strip $(USE_FRESCO)),1)
 ifeq "$(HAS_FRESCO)" "1"
  INCDIRS += $(FRESCO_INC_DIR) $(subst $/lib,,$(FRESCO_LIB_DIR))
  # Fresco only uses the config.h on unix
#  ifneq ($(OS),win32)
#     DEFINES += -DHAVE_CONFIG_H
#  endif
  LIBDIRS += $(FRESCO_LIB_DIR)
#  DIR_Parmesan = $(foreach file,Parmesan,$(sys_or_iu_macro_lib))
  STDLIBS += $(FRESCO_LIBS)
  ifeq (1,$(strip $(HAS_X11)))
    IUDXLIBS += -lDXKitTj -lDXKit
    USE_X11 := 1
  endif
 else
  err_CANNOT_BUILD_HERE += "USE_FRESCO"
 endif
endif


# Select: USE_TCL
# tcl library
ifeq  ($(strip $(USE_TCL)),1)
 ifeq ($(strip $(HAS_TCL)),1)
  INCDIRS += $(TCLINCS)
  C_INCDIRS += $(TCLINCS)
  LIBDIRS += $(TCLLIB)
  DEFINES += $(TCLDEFINES)
  STDLIBS += $(TCLLIBS)
# else
#  # Useful as a comment anyway..
#  DEFINES += -UTCL_INSTALLED
 endif
endif


# Select: USE_MSQL
# msql library
ifeq  ($(strip $(USE_MSQL)),1)
  INCDIRS += $(MSQLINCS)
  LIBDIRS += $(MSQLLIB)
  DEFINES += $(MSQLDEF)
  STDLIBS += $(MSQLLIBS)
  USE_SOCKETS := 1
endif

# Select: USE_MYSQL
# mysql library
ifeq  ($(strip $(USE_MYSQL)),1)
  INCDIRS += $(MYSQLINCS)
  LIBDIRS += $(MYSQLLIB)
  DEFINES += $(MYSQLDEF)
  STDLIBS += $(MYSQLLIBS)
  USE_SOCKETS := 1
endif

# Select: USE_PERL5
# perl5 library
ifeq  ($(strip $(USE_PERL5)),1)
  INCDIRS += $(PERL5DIST)
endif

# Select: USE_MOTIFGL
# MOTIFGL libraries
ifeq ($(USE_MOTIFGL),1)
  INCDIRS += $(MOTIFGL_INC_DIR)
  LIBDIRS += $(MOTIFGL_LIB_DIR)
  STDLIBS += $(MOTIFGL_LIBS)
  DEFINES += -DHAS_MOTIFGL
  USE_MOTIF := 1
  USE_OPENGL := 1

  ifndef HAS_MOTIFGL
    err_CANNOT_BUILD_HERE += "USE_MOTIFGL"
  endif
endif

# MOTIF
# Select: USE_MOTIF
ifeq ($(strip $(USE_MOTIF)),1)
  INCDIRS += $(MOTIF_INC_DIR)
  LIBDIRS += $(MOTIF_LIB_DIR)
  DEFINES += $(MOTIF_DEFS)
  STDLIBS += $(MOTIF_LIBS)
  C_INCDIRS += $(MOTIF_INC_DIR)
  USE_X11 := 1

  ifndef HAS_MOTIF
    err_CANNOT_BUILD_HERE += "USE_MOTIF"
  endif
endif

# Select: USE_VTK
# Set to 1 to include the VTK headers and libraries
ifeq ($(strip $(USE_VTK)),1)
   INCDIRS += $(VTK_INC_DIR)
   LIBDIRS += $(VTK_LIB_DIR)
   DEFINES += $(VTK_DEFINES)
   STDLIBS += $(VTK_LIBS)
   ifeq ($(strip $(HAS_X11)),1)
      USE_X11 := 1
   endif
endif

# Select: USE_TIFF
# Set to 1 to use the TIFF library
ifeq ($(strip $(USE_TIFF)),1)
  INCDIRS += $(TIFF_INC_DIR)
  LIBDIRS += $(TIFF_LIB_DIR)
  DEFINES += -DHAS_TIFF
  STDLIBS += -ltiff #$(TIFF_LIBS)
  USE_JPEG := 1
  ifndef HAS_TIFF
    err_CANNOT_BUILD_HERE += "USE_TIFF"
  endif
endif

# Select: USE_JPEG
# Set to 1 to use the JPEG library
ifeq ($(strip $(USE_JPEG)),1)
  INCDIRS += $(JPEG_INC_DIR)
  LIBDIRS += $(JPEG_LIB_DIR)
  DEFINES += -DHAS_JPEG
  STDLIBS += -ljpeg #$(JPEG_LIBS)
  ifndef HAS_JPEG
    err_CANNOT_BUILD_HERE += "USE_JPEG"
  endif
endif

# Select: USE_MPEG
# Set to 1 to use the MPEG library
ifeq ($(strip $(USE_MPEG)),1)
  INCDIRS += $(MPEG_INC_DIR)
  LIBDIRS += $(MPEG_LIB_DIR)
  DEFINES += -DHAS_MPEG
  STDLIBS += $(MPEG_LIBS)
  ifndef HAS_MPEG
    err_CANNOT_BUILD_HERE += "USE_MPEG"
  endif
endif

# Select: USE_MPEG2
# Set to 1 to use the MPEG2 library
ifeq ($(strip $(USE_MPEG2)),1)
  INCDIRS += $(MPEG2_INC_DIR)
  LIBDIRS += $(MPEG2_LIB_DIR)
  DEFINES += -DHAS_MPEG2
  STDLIBS += $(MPEG2_LIBS)
  ifndef HAS_MPEG2
    err_CANNOT_BUILD_HERE += "USE_MPEG2"
  endif
endif

# Select: USE_NETLIB
# Set to 1 to use the NETLIB library
ifeq ($(strip $(USE_NETLIB)),1)
  INCDIRS += $(NETLIB_INC_DIR)
  LIBDIRS += $(NETLIB_LIB_DIR)
# was this ever used? -- fsm
#  DEFINES += -DHAS_NETLIB
  STDLIBS += $(NETLIB_LIBS)
  ifndef HAS_NETLIB
    err_CANNOT_BUILD_HERE += "USE_NETLIB"
  endif
endif

# Select: USE_PNG
# PNG image format libraries
ifeq ($(strip $(USE_PNG)),1)
  INCDIRS += $(PNG_INC_DIR)
  LIBDIRS += $(PNG_LIB_DIR)
  DEFINES += -DHAS_PNG
  STDLIBS += $(PNG_LIBS)
  USE_ZLIB := 1
  ifndef HAS_PNG
    err_CANNOT_BUILD_HERE += "USE_PNG"
  endif
endif


# Select: USE_ZLIB
# zlib compression library
ifeq ($(strip $(USE_ZLIB)),1)
  INCDIRS += $(ZLIB_INC_DIR)
  LIBDIRS += $(ZLIB_LIB_DIR)
  DEFINES += -DHAS_ZLIB
  STDLIBS += $(ZLIB_LIBS)
  ifndef HAS_ZLIB
    err_CANNOT_BUILD_HERE += "USE_ZLIB"
  endif
endif


# Select: USE_GLUT
# Set to 1 to use the GLUT (OpenGL Utility Toolkit) library.
ifeq ($(strip $(USE_GLUT)),1)
  INCDIRS += $(GLUT_INC_DIR)
  LIBDIRS += $(GLUT_LIB_DIR)
  STDLIBS += $(GLUT_LIBS)
  DEFINES += -DHAS_GLUT
  USE_OPENGL := 1
  ifndef HAS_GLUT
    err_CANNOT_BUILD_HERE += "USE_GLUT"
  endif
endif

# Select: USE_GMP
# Set to 1 to use the GMP (GNU Multiple Precision) library.
ifeq ($(strip $(USE_GMP)),1)
  INCDIRS += $(GMP_INC_DIR)
  LIBDIRS += $(GMP_LIB_DIR)
  STDLIBS += -lgmp
  DEFINES += -DHAS_GMP
  ifndef HAS_GMP
    err_CANNOT_BUILD_HERE += "USE_GMP"
  endif
endif

# QGL
# Select: USE_QGL
ifeq ($(strip $(USE_QGL)),1)
 USE_QT := 1
 USE_OPENGL := 1
 DEFINES += -DHAS_QGL
 STDLIBS += $(QGL_LIBS)
endif

# QT
# Select: USE_QT
ifeq ($(strip $(USE_QT)),1)
 INCDIRS += $(QT_INC_DIR)
 LIBDIRS += $(QT_LIB_DIR)
 DEFINES += -DHAS_QT
 ifeq ($(OS)-$(COMPILER),IRIX6-CC-n32)
  DEFINES += -DQ_BROKEN_TEMPLATE_SPECIALIZATION
 endif
 STDLIBS += $(QT_LIBS)
endif


# backwards compat
ifeq ($(strip $(USE_GTKGL)),1)
  USE_GTKGLAREA := 1
xall::
	@echo "replace USE_GTKGL with USE_GTKGLAREA"
endif

# Select: USE_GTKGLAREA
# GTKGLAREA (another OpenGL Utility Toolkit) library
ifeq ($(strip $(USE_GTKGLAREA)),1)
  INCDIRS += $(GTKGLAREA_INC_DIR)
  LIBDIRS += $(GTKGLAREA_LIB_DIR)
  STDLIBS += $(GTKGLAREA_LIBS)
  DEFINES += -DHAS_GTKGLAREA
  USE_GTK := 1
  ifndef HAS_GTKGLAREA
    err_CANNOT_BUILD_HERE += "USE_GTKGLAREA"
  endif
endif

# Select: USE_GTK
# GTK library
ifeq ($(strip $(USE_GTK)),1)
  INCDIRS += $(GTK_INC_DIRS)
  LIBDIRS += $(GTK_LIB_DIRS)
  STDLIBS += $(GTK_LIBS)
  DEFINES += -DHAS_GTK
  ifndef HAS_GTK
    err_CANNOT_BUILD_HERE += "USE_GTK"
  endif
endif

# Select: USE_FLTK
# FLTK (yet another OpenGL Utility Toolkit) libraries
ifeq ($(USE_FLTK),1)
  INCDIRS += $(FLTK_INC_DIR)
  LIBDIRS += $(FLTK_LIB_DIR)
  DEFINES += -DHAS_FLTK
  STDLIBS += $(FLTK_LIBS)
  USE_OPENGL := 1
  ifndef HAS_FLTK
    err_CANNOT_BUILD_HERE += "USE_FLTK"
  endif
endif


# Qt libraries
ifeq ($(strip $(USE_QT)),1)
  QT_MOC = $(QT_MOC_DIR)/moc

  # rule to make moc files
moc_%.cc: %.h
	$(QT_MOC) $(@:moc_%.cc=%.h) -o $@

  # Libraries for link closure (needed by Win32 and Template Instantiator)
  INCDIRS += $(QT_INC_DIR)
  LIBDIRS += $(QT_LIB_DIR)
  DEFINES +=
  STDLIBS += -lqt
  USE_X11 := 1
endif

# Select: USE_OPENGL
# Open GL libraries
ifeq ($(strip $(USE_OPENGL)),1)
  INCDIRS += $(OPENGL_INC_DIR)
  LIBDIRS += $(OPENGL_LIB_DIR)
  DEFINES += $(OPENGLDEF)
  STDLIBS += $(OPENGL_LIBS)
  USE_X11 := 1 # some of vgui needs this
endif

ifeq ($(strip $(OPENGL_IS_MESA)),1)
  DEFINES += -DVGUI_MESA=1
else
  DEFINES += -DVGUI_MESA=0
endif

# XGL libraries
# Select: USE_XGL
# XGL libraries
ifeq ($(strip $(USE_XGL)),1)
  INCDIRS += $(XGLINCS)
  LIBDIRS += $(XGLLIB)
  ifeq ($(words $(wildcard $(XGL)/lib/libxgl.a)),0)
    STDLIBS += $(XGL_LIBS)
  else
    STDLIBS += $(link-static) $(XGL_LIBS) $(link-dynamic)
  endif
  DEFINES += $(XGLDEF)
endif

# Select: USE_SGI_OPENGL
# SGI OpenGL Software Development Kit
ifeq ($(strip $(USE_SGI_OPENGL)),1)
  INCDIRS += $(SGI_OPENGLINCS)
  LIBDIRS += $(SGI_OPENGLLIBS)
  STDLIBS += -lglu -lopengl
  err_MUST_BUILD_ON_WINDOWS := 1
endif

# Wing libraries
# Select: USE_WING
# Microsoft WinG Software Development Kit
ifeq ($(strip $(USE_WING)),1)
  INCDIRS += $(WINGINCS)
  LIBDIRS += $(WINGLIBS)
  STDLIBS += -lWing32
  err_MUST_BUILD_ON_WINDOWS := 1
endif

# Select: USE_WINAPI
# Microsoft Windows API
ifeq ($(strip $(USE_WINAPI)),1)
#  INCDIRS += $(WINAPIINCS)
  LIBDIRS += $(WINAPILIBS)
  STDLIBS += Gdi32.lib User32.lib Comdlg32.lib Comctl32.lib Ws2_32.lib Advapi32.lib kernel32.lib
  err_MUST_BUILD_ON_WINDOWS := 1
endif

# Select: USE_MFC
# Microsoft Foundation Class API
ifeq ($(strip $(USE_MFC)),1)
  INCDIRS += $(MFCINCS)
  LIBDIRS += $(MFCLIBS)
  err_MUST_BUILD_ON_WINDOWS := 1
endif

# Select: USE_OLE
# Microsoft Foundation Class API
ifeq ($(strip $(USE_OLE)),1)
  #INCDIRS += $(OLEINCS)
  LIBDIRS += $(OLELIBS)
  STDLIBS += Ole32.lib Uuid.lib
  err_MUST_BUILD_ON_WINDOWS := 1
endif

# Select: USE_ATL
# Microsoft ATL API
ifeq ($(strip $(USE_ATL)),1)
  INCDIRS += $(ATLINCS)
  LIBDIRS += $(ATLLIBS)
  err_MUST_BUILD_ON_WINDOWS := 1
endif


# MATLAB
# Select: USE_MATLAB
ifeq ($(strip $(USE_MATLAB)),1)
  ifeq ($(strip $(HAS_MATLAB)),1)
    INCDIRS += $(MATLAB_INC_DIR)
    LIBDIRS += $(MATLAB_LIB_DIR)
    DEFINES += -DHAS_MATLAB
    STDLIBS += -lmat -leng -lmx -lmi -lut
  else
    err_CANNOT_BUILD_HERE += "USE_MATLAB"
  endif
endif


# SDL
# Select: USE_SDL
ifeq ($(strip $(USE_SDL)),1)
  ifeq ($(strip $(HAS_SDL)),1)
    INCDIRS += $(SDL_INC_DIR)
    LIBDIRS += $(SDL_LIB_DIR)
    DEFINES += -DHAS_SDL
    STDLIBS += -lSDL -lpthread
  else
    err_CANNOT_BUILD_HERE += "USE_SDL"
  endif
endif

# HERMES
# Select: USE_HERMES
ifeq ($(strip $(USE_HERMES)),1)
  ifeq ($(strip $(HAS_HERMES)),1)
    INCDIRS += $(HERMES_INC_DIR)
    LIBDIRS += $(HERMES_LIB_DIR)
    DEFINES += -DHAS_HERMES
    STDLIBS += -lHermes
  else
    err_CANNOT_BUILD_HERE += "USE_HERMES"
  endif
endif


# ACE
# Select: USE_ACE
# Set to 1 to include the ACE headers and libACE
ifeq ($(strip $(USE_ACE)),1)
   INCDIRS += $(ACE_INC_DIR)
   LIBDIRS += $(ACE_LIB_DIR)
   DEFINES += -DWITH_ACE -DHAS_ACE
   STDLIBS += $(ACE_LIBS)
endif

# OMNIORB
# Select: USE_OMNIORB
# Set to 1 to include the OMNIORB headers and the omniorb libraries
ifeq ($(strip $(USE_OMNIORB)),1)
   INCDIRS += $(OMNI_INC_DIR)
   LIBDIRS += $(OMNI_LIB_DIR)
   DEFINES += -DWITH_OMNIORB $(OMNI_DFLAGS) $(OMNI_CXXFLAGS)
   STDLIBS += $(OMNI_LIBS)
endif

# Select: USE_XERCES
# XERCES library
ifeq  ($(strip $(USE_XERCES)),1)
 ifeq ($(strip $(HAS_XERCES)),1)
  INCDIRS += $(XERCESINCS)
  C_INCDIRS += $(XERCESINCS)
  LIBDIRS += $(XERCESLIB)
  STDLIBS += $(XERCES_LIBS)
 endif
endif

