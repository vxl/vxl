# This makefile fragment lists all the packages in the IUE/TargetJr

# They are of two main types:
#   Library packages contain sets of libraries useful to many programs
#   Program or application packages are executable programs that demo the use of the libraries

# Libraries are further broken down into
#   IU_PACKAGES:  general Image Understanding stuff
#   GUI_PACKAGES: Various GUIs (Viewing, Parmesan, MotifInterface, ImportVtk)
ifndef IUE_ONLY
ifdef TJ_ONLY
ifdef VXL_ONLY
TJ_ONLY :=
VXL_ONLY :=
endif
NO_IUE := 1
endif
endif

GUI_PACKAGES :=
# make sure we build vcl before v3p as Qv needs vcl.
IU_PACKAGES := TestLib vcl v3p

# Limit compilation to core IUE libraries if IUE_ONLY is set
ifndef IUE_ONLY
# Limit compilation to core TargetJr libraries if TJ_ONLY is set
ifndef TJ_ONLY
IU_PACKAGES += vxl mul tbl rpl oxl gel oul brl
PROGRAMS += conversions
endif # TJ_ONLY
endif # IUE_ONLY

# Limit compilation to core vxl libraries if VXL_ONLY is set
ifndef VXL_ONLY
IU_PACKAGES +=                  \
            Numerics            \
            COOL                \
            GeneralUtility      \
            Dex2

ifndef NO_IUE
IU_PACKAGES += Basics-IUE
endif

ifeq ($(strip $(USE_OBJECTSTORE)),1)
IU_PACKAGES += ObjectStore-IUE
endif

IU_PACKAGES +=                  \
             SpatialObjects     \
             Photogrammetry     \
             Image

ifndef NO_IUE
IU_PACKAGES += CoordSys-IUE     \
             Image-IUE          \
             ImageFeature-IUE   \
             SpatialObject-IUE  \
             SpatialIndex-IUE   \
             Statistics-IUE     \
             Tasks-IUE
endif

# Limit compilation to core IUE libraries if IUE_ONLY is set
ifndef IUE_ONLY
IU_PACKAGES +=                  \
             MultiView          \
             Segmentation       \
             Recognition        \
             Database           \
             Matching

#GUI_PACKAGES += Viewing # InterViews support was removed
GUI_PACKAGES += MotifInterface
GUI_PACKAGES += ImportVtk
endif # IUE_ONLY

GUI_PACKAGES += Parmesan
# Note: have Parmesan at the end, so the manpages of classes with
# identical names are from Parmesan.

endif # VXL_ONLY

PACKAGES := $(IU_PACKAGES) $(GUI_PACKAGES)

# application packages -- these are not part of the core
# General to specific....

ifndef IUE_ONLY
ifndef VXL_ONLY
# tjload must be first as it is needed for Examples
PROGRAMS := tjload              \
            Examples            \
            fxcv                \
            SiteModeling
#           xcv
#           Morse
endif # VXL_ONLY
endif # IUE_ONLY

ifndef NO_IUE
ifndef VXL_ONLY
PROGRAMS += Examples-IUE        \
            Parmesan/Applications
endif
endif

ifeq ($(strip $(USE_OBJECTSTORE)),1)
PROGRAMS += ObjectStore-IUE/Tests
endif

### Site-specific stuff, to be moved to site.mk
# site-specific packages -- these are not part of the core
ifndef IUE_ONLY
ifndef VXL_ONLY
SITE_PACKAGES+=                 \
            Matching            \
            RHUtilities         \
            Carmen              \
            Estimate            \
            SceneReconstruction \
            Oxford              \
            MultiViewOX         \
            IClass              \
            MultiViewLV         \
            f2c                 \
            Reconstruction      \
            SequenceReconstruction

# IUE packages
# site-specific applications
SITE_PROGRAMS+=                 \
            RHStereo            \
            Video               \
            XMS                 \
            XFL                 \
            Impact              \
            Inspection          \
            Radius              \
            IMPACT              \
            IMPROOFS            \
            Viable              \
            Layout              \
            Range               \
            Focus               \
            DDB                 \
            Texture             \
            xor
endif # VXL_ONLY
endif # IUE_ONLY

-include site-packages.mk


#
# fsm@robots : in rules.mk there is a warning issued for packages which
# are referred to in the USES list, but which do not exist in the checkout.
# This is unfair on makefile clients who got their USES list from the
# variables supplied by this file. So we filter out any packages which are
# named in this file but which do not exist in the checkout.
#
# get all package names :
tmp := $(IU_PACKAGES) $(GUI_PACKAGES) $(PROGRAMS) $(SITE_PACKAGES) $(SITE_PROGRAMS)

# look for them in sys or iu :
FOUND_PACKAGES := $(foreach file,$(tmp),$(sys_or_iu_macro))
# strip away the sys/iu paths again :
FOUND_PACKAGES := $(FOUND_PACKAGES:$(IUELOCALROOT)/%=%) $(FOUND_PACKAGES:$(IUEROOT)/%=%)
# sort to make unique :
FOUND_PACKAGES := $(sort $(FOUND_PACKAGES))

# get missing packages :
MISSING_PACKAGES := $(filter-out $(FOUND_PACKAGES),$(tmp))

# retain only those packages which exist :
IU_PACKAGES := $(filter $(FOUND_PACKAGES),$(IU_PACKAGES))
GUI_PACKAGES := $(filter $(FOUND_PACKAGES),$(GUI_PACKAGES))
PROGRAMS := $(filter $(FOUND_PACKAGES),$(PROGRAMS))
SITE_PACKAGES := $(filter $(FOUND_PACKAGES),$(SITE_PACKAGES))
SITE_PROGRAMS := $(filter $(FOUND_PACKAGES),$(SITE_PROGRAMS))
