include $(IUELOCALROOT)/config/top-params.mk
VXL := 1

USES = vxl oxl

USE_JPEG = 1
include $(IUEROOT)/vxl/vil/vil_config.mk

LIBRARY = oxp

SOURCES += oxp_parse_seqname.cxx
SOURCES += Mapping_2d_2d.cxx
SOURCES += RadialLensCorrection.cxx
SOURCES += MedianReduceImage.cxx
SOURCES += ByteConvertImage.cxx
SOURCES += JPEG_Decompressor.cxx
SOURCES += ImageSequenceName.cxx
SOURCES += SequenceFileName.cxx
MANPAGE_SOURCES += ImageWarp.txx

SOURCES += GXFileVisitor.cxx

SOURCES += MovieFile.cxx
SOURCES += MovieFileInterface.cxx
ifneq ($(HAS_BROKEN_AS)-$(BUILD),1-noshared)
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

IULIBS += -lvil -lvnl_algo -lvnl -lvbl -lvcl

include $(IUELOCALROOT)/config/top-rules.mk
