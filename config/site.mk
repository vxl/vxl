# File: gmake-site.mk
#
# Put your local site defines in here.  If something has a weird
# pathname that autoconf can't figure out, or you have a local
# set of libraries, put them here.

#------------------------------------------------------------
# Free
ifeq (1,$(USE_FREEVERSION))
 DEFINES += -DFREEVERSION
endif

#------------------------------------------------------------
# INRIA
ifeq (1,$(USE_INRIAVERSION))
 DEFINES += -DINRIAVERSION
endif

#------------------------------------------------------------
# Leuven
ifeq (1,$(USE_LEUVENVERSION))
 DEFINES += -DLEUVENVERSION
 TJWD := /freeware/bin/perl $(IUEROOT)/Scripts/Perl/tjwd

 ifeq ($(OS),SunOS5)
  VTK_EXTRA_LIBS += -lxgl
 endif
 ifeq ($(OS),Linux2)
  OPENGL_LIBS = -lMesaGLU -lMesaGL -lpthread -lglide3
 endif
 USE_IPGENERIC := 1
 ifneq ($(OS),win32)
  ifeq ($(USER),targetjr)
   MKDIR_P := mkdir_ln
   HAVE_GNU_LD := 1
  endif
 endif
endif # USE_LEUVENVERSION

ifeq ($(OS),Linux2)
 VTK_EXTRA_LIBS += -lpthread
endif
ifeq ($(OS),SunOS5)
 VTK_EXTRA_LIBS += -lpthread -lXt
endif

#------------------------------------------------------------
# Oxford
ifeq (1,$(USE_OXFORDVERSION))
 DEFINES += -DOXFORDVERSION

# Packages to add to IUE_USES
 IUE_PERL = perl
 SITE_USES = MultiViewOX Oxford
# TJ_ONLY := 1

# Need gnu ld to make static tjrun
 ifeq ($(OS),Linux2)
  HAVE_GNU_LD := 1
 endif

# Local preference is to have ERROR_CHECKING enabled.
 DEFINES += -DERROR_CHECKING

 ifneq ($(OS),win32)
  LINK_INTO_TMP := 1
 else
  LINK_INTO_TMP :=
 endif

 ifeq ($(PURIFY),1)
  PURIFYHOME := /homes/39/pkg/purify-4.1/purify-4.1-solaris2
  # Assume it was set on the command line
  override PURIFY := $(PURIFYHOME)/purify --always-use-cache-dir --chain-length=12 -best-effort
  PROGSUFFIX := .pure
 endif

endif # USE_OXFORDVERSION

#################################################################
## Group Viewing, and SpatialObject libs, and all that they imply

# MultiView Package
CORE_IULIBS += -lMViewUI -lMViewCompute -lMViewBasics

# Segmentation package
CORE_IULIBS += -lSegmentView -lFitting -lDetection

# Viewing Package
CORE_IULIBS += -lEasyView -lView2D \
          -lImageView \
          -lGenericView3D $(IU3DLIBS) \
          -lGraphTools \
          -lIVAdditions \
          -lPrintingView \
          -lPrinting

# SpatialObjects
CORE_IULIBS += -lCompGeom \
          -lTopology \
          -lGeometry \
          -lDigitalGeometry \
          -lGroups \
          -lFredIO \
          -lDXFIO \
          -lSpatialBasics

# Image package
CORE_IULIBS += -lEasyImage -lImageProcessing -lImageDEX -lImageClasses -ltiff

# Photogrammetry
CORE_IULIBS += -lIUCameras

# Database
CORE_IULIBS += -lDatabaseDEX -lDatabase -lKnowledgeSourceDEX -lKnowledgeSource

# Numerics package
CORE_IULIBS += -lNumerics -lnetlib

# GeneralUtility
CORE_IULIBS += \
          -lBasics \
          -lSolver \
          -lStat \
          -lDEXTableIO -ldex_generic

# MultiView Package
NO_UI_IULIBS += -lMViewCompute -lMViewBasics

# Segmentation package
NO_UI_IULIBS += -lFitting -lDetection

# Viewing Package
NO_UI_IULIBS += -lPrinting

# SpatialObjects
NO_UI_IULIBS += -lCompGeom \
          -lTopology \
          -lGeometry \
          -lDigitalGeometry \
          -lGroups \
          -lFredIO \
          -lDXFIO \
          -lSpatialBasics

# Image package
NO_UI_IULIBS += -lEasyImage -lImageProcessing -lImageDEX -lImageClasses -ltiff

# Photogrammetry
NO_UI_IULIBS += -lIUCameras

# Database
NO_UI_IULIBS += -lDatabaseDEX -lDatabase -lKnowledgeSourceDEX -lKnowledgeSource

# Numerics package
NO_UI_IULIBS += -lNumerics -lnetlib

# GeneralUtility
NO_UI_IULIBS += -lBasics -lSolver -lStat -lDEXTableIO -ldex_generic

# override any TJ_ONLY settings
ifdef USE_LEUVENVERSION
 TJ_ONLY :=
 VXL_ONLY :=
endif
