#-*-makefile-*--------------------------------------------------------
#
#  Definitions of variables that may be useful within makefiles.
#  This file is included from params.mk about halfway through.
#
#  Users may use these variables to define USES, etc. prior to
#  including top-params.mk or top.mk as long as they define the
#  variable with = or += and not :=.  NOTE that USES and IULIBS must be
#  defined prior to including top-params.mk or top.mk in order 
#  for the package name computation to work correctly when
#  packages are not at the top level (e.g., Tasks-IUE/EdgeDetection-IUE).
#----------------------------------------------------------------------

#======================================================================
#  TJr-Specific Settings
#======================================================================

ifndef TJ_CORE_PACKAGES
TJ_CORE_PACKAGES = \
        Recognition \
	Segmentation \
	MultiView \
	Photogrammetry \
	Database \
	Image \
	SpatialObjects \
	GeneralUtility \
	COOL \
	Numerics
endif

# Variable: IUE_USES
# List of all core library packages and those defined in SITE_USES and USER_USES
IUE_USES = $(IUE_CORE_PACKAGES) $(USER_USES) $(SITE_USES) $(TJ_CORE_PACKAGES)


#======================================================================
#  IUE-Specific Settings
#======================================================================

# For backward compatibility... IUE_ONLY is the new name
ifneq ($(strip $(IUE-CORE)),)
  IUE_ONLY := 1
endif

IUE_CORE_PACKAGES = \
	GeneralUtility COOL Numerics Image Dex2 \
	Basics-IUE CoordSys-IUE Image-IUE SpatialObject-IUE \
	ImageFeature-IUE SpatialIndex-IUE Statistics-IUE

IUE_LIBS = \
	-lIUE-IF-Curve -lIUE-IF-Edgel -lIUE-IF-ImageFeature \
	-lIUE-IF-Point -lIUE-IF-Region -lIUE-IF-Topology \
	-lIUE-SO-FunctionalSurface \
	-lIUE-SO-PlanarPatch -lIUE-SO-Plane \
	-lIUE-SO-Topology -lIUE-SO-Volume \
	-lIUE-SO-Spline -lIUE-SO-Surface \
	-lIUE-SO-SampledCurve -lIUE-SO-Conic -lIUE-SO-Line -lIUE-SO-Curve \
	-lIUE-SO-Point -lIUE-SO-Rectangle \
	-lIUE-SO-Sample \
	-lIUE-SO-Utility-3d \
	-lIUE-SO-AxisAlignedBox -lIUE-SO-Base -lIUE-SO-CellLattice \
	-lIUE-CS-ColorSystem -lIUE-CS-ColorTransform \
	-lIUE-CS-Functional-Transform -lIUE-CS-GeographicSystem \
	-lIUE-CS-GeographicTransform \
	-lIUE-CS-GeometricSystem \
	-lIUE-SpatialIndex \
	-lIUE-CS-NonHomogeneousTransform \
	-lIUE-CS-CartesianSystem \
	-lIUE-CS-HomogeneousTransform \
	-lIUE-CS-GeometricTransform -lIUE-CS-Base \
	-lIUE-SO-BinaryOps -lIUE-SO-AxisAlignedBox -lIUE-SO-PlanarPatch \
	-lIUE-SO-Plane -lIUE-SO-Line -lIUE-SO-Point -lIUE-SO-Rectangle -lIUE-SO-Base \
	-lIUE-Image -lIUE-Stat -lIUE-Math-Basics \
	-lIUE-Container -lIUE-Root -lIUE-Unit \
	-lIaf -lTraits -lDex2 \
	$(NITF) -lImageClasses -ltiff -ljpeg -lBasics \
	-lNumerics -lIUE-MathDex -liue-math-extensions

# NOTE: the 7 duplicate libraries following -lIUE-SO-BinaryOps are for linkers
# (notably the one on SGI) that expect to find symbols to be resolved from a
# static library (libIUE-SO-BinaryOps.a) in shared libs *after* the static one.

ifeq ($(strip $(USE_RENDERING)),1)

  ifeq (1,$(HAS_OPENGL))
     IUE_LOCAL_IU3DLIBS += -lIUEOpenGLView3D 
     USE_OPENGL := 1
  else
     ifneq ($(strip $(OPENGL)),)
       IUE_LOCAL_IU3DLIBS += -lIUEOpenGLView3D 
       USE_OPENGL := 1
     endif
  endif
  ifeq (1,$(HAS_XGL))
     IUE_LOCAL_IU3DLIBS += -lIUEXglView3D
     USE_XGL := 1
  else
     ifneq ($(strip $(XGL)),)
        IUE_LOCAL_IU3DLIBS += -lIUEXglView3D
        USE_XGL := 1
     endif
  endif

  IUE_VISTOOL_USES = \
	$(IUE_CORE_PACKAGES) \
	Parmesan SpatialObjects \
	Photogrammetry \
	Tasks-IUE/ChangeDetection-AAI \
	Tasks-IUE/PointDetection-AAI \
	Tasks-IUE/RegionsOfInterest-AAI \
	Tasks-IUE/SolarFeatureDetection-AAI \
	Tasks-IUE/CurveFormation-IUE \
	Tasks-IUE/EdgeDetection-IUE \
	Tasks-IUE/Fitting-IUE \
	Tasks-IUE/PointDetection-IUE \
	Tasks-IUE/RegionFormation-IUE \
	Tasks-IUE/CurveFormation-Brunel \
	Tasks-IUE/CurveFormation-CSU \
	Tasks-IUE/CurveFormation-Stanford \
	Tasks-IUE/ScaleSpace-UCL

  IUE_VISTOOL_LIBS += -lp-lib -lIUEExamples \
	-lCurveFormation-IUE -lEdgeDetection-IUE -lFitting-IUE \
	-lPointDetection-IUE -lRegionFormation-IUE -lChangeDetection-AAI \
	-lPointDetection-AAI -lRegionsOfInterest-AAI \
	-lSolarFeatureDetection-AAI -lCurveFormation-Brunel \
	-lCurveFormation-CSU  -lCurveFormation-Stanford \
	-lUCL-Image -lUCL-Diffusion \
	$(IUE_LOCAL_IU3DLIBS) \
	-lIUEGenericView3D -lIUEImageView -lDXKitIUE \
	$(IUE_LIBS) \
	-lFView2D -lStackManager -lFresh -lScrollList -lPrinting \
	-lDXKitTj -lDXKit -lMiniMenu -lPlaneManager -lPlanes -lGU -lDXKit \
        -lImageClasses $(NITF) -ltiff -lImageProcessing \
	-lIUCamerasDEX -lIUCameras -lDXFIO -lFredIO -lSpatialObjectDEX \
	-lCompGeom -lGroups -lTopology -lDigitalGeometry -lGeometry \
	-lSpatialBasics -lSolver -lStat -lBasics -lTJCOOL \
	-lDEXTableIO -ldex_generic \
	-lDex2 -lTraits -lNumerics -lnetlib

  IUE_VISTOOL_LIBS += -lfdisplay

  ifeq ($(strip $(OS)),freebsd)
  else	
    ifeq ($(strip $(OS)),Linux2)
      IUE_VISTOOL_LIBS += -ldl
    else
      ifeq ($(strip $(OS)),win32)
      else
        IUE_VISTOOL_LIBS += -lw
      endif
    endif
  endif

#END USE_RENDERING
endif


#======================================================================
#  IUE ObjectStore-Specific Settings
#======================================================================

# If user has ObjectStore-IUE checked out, they probably want to use it
# rather than the one in IUEROOT
os_params_dir := $(firstword $(wildcard $(IUELOCALROOT)/ObjectStore-IUE $(IUEROOT)/ObjectStore-IUE))

# add ObjectStore libraries if using object store
ifeq ($(strip $(USE_OBJECTSTORE)),1)
  IUE_CORE_PACKAGES += ObjectStore-IUE
  IUE_USES += ObjectStore-IUE
  IUE_LIBS += -lIUE-ObjectStore
  DEFINES += -DIUE_OBJECTSTORE -DIUEi_NO_STL -DNO_DUMMY_IMPLEMENTATION

  include ${os_params_dir}/os-params.mk
  ifneq (,$(findstring GeneralUtility,$(USES)))
    ifeq (,$(findstring ObjectStore-IUE,$(USES)))
	USES += ObjectStore-IUE
    endif
    ifneq (,$(findstring -lTraits,$(IULIBS)))
	IULIBS += -lIUE-ObjectStore
    endif
    ifeq (,$(findstring -lIUE-Containers,$(IULIBS)))
	IULIBS += -lIUE-Containers
    endif
  endif
  IULIBS +=  ${OSTORELIBS}
endif
