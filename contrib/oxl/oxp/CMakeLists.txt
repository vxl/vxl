#
# ./oxl/oxp/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#

ADD_LIBRARY(oxp oxp_sources)

SOURCE_FILES(oxp_sources
ByteConvertImage        RadialLensCorrection
GXFileVisitor           SGIMovieFile
ImageSequenceMovieFile  SGIMovieFilePrivates
ImageSequenceName       SGIMovieFileWrite
JPEG_Decompressor       SequenceFileName
Mapping_2d_2d           oxp_parse_seqname
MedianReduceImage       vsl_conic_as_matrix
MovieFile               vsl_conic_as_params
MovieFileInterface      vsl_conic_points
)

IF(JPEG)
  # do nothing
ELSE(JPEG)
  INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/v3p/jpeg)
ENDIF(JPEG)

AUX_SOURCE_DIRECTORY(Templates oxp_sources)