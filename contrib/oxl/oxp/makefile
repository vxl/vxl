USES = vxl oxl
include $(IUEROOT)/vxl/vil/config.mk
TRY_JPEG := 1
TRY_MPEG := 1

LIBRARY = oxp

IMMEDIATE_LIBS += -lvil -lvnl_algo -lvnl -lvbl

#######################################
include $(IUEROOT)/config/top-params.mk
#######################################

SOURCES += oxp_parse_seqname.cxx
SOURCES += Mapping_2d_2d.cxx
SOURCES += RadialLensCorrection.cxx
SOURCES += MedianReduceImage.cxx
SOURCES += ByteConvertImage.cxx
ifeq ($(HAS_JPEG),1)
SOURCES += JPEG_Decompressor.cxx
endif
SOURCES += ImageSequenceName.cxx
SOURCES += SequenceFileName.cxx
MANPAGE_SOURCES += ImageWarp.txx

SOURCES += GXFileVisitor.cxx

SOURCES += MovieFile.cxx
SOURCES += MovieFileInterface.cxx
ifeq ("$(HAS_JPEG)-$(HAS_BROKEN_AS)","1-")
SOURCES += SGIMovieFile.cxx
SOURCES += SGIMovieFilePrivates.cxx
SOURCES += SGIMovieFileWrite.cxx
else
TEMPLATE_SOURCES := $(subst Templates/vector+SGIMV_Variables-.cxx,,$(TEMPLATE_SOURCES))
endif
SOURCES += ImageSequenceMovieFile.cxx

SOURCES += vsl_conic_as_params.cxx
SOURCES += vsl_conic_as_matrix.cxx
SOURCES += vsl_conic_points.cxx

SOURCES += oxp_bunch_of_files.cxx
SOURCES += oxp_vidl_moviefile.cxx

ifeq ($(HAS_MPEG),1)
SOURCES += oxp_mpeg_codec.cxx
SOURCES += oxp_vidl_mpeg_codec.cxx
endif

SOURCES += oxp_yuv_to_rgb.cxx
MANPAGE_SOURCES += MapInverter.h
MANPAGE_SOURCES += POX.h

######################################
include $(IUEROOT)/config/top-rules.mk
######################################
