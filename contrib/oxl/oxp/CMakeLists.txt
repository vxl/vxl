# ./oxl/oxp/CMakeLists.txt

ADD_LIBRARY(oxp oxp_sources)

SOURCE_FILES(oxp_sources
  ByteConvertImage.cxx
  GXFileVisitor.cxx
  ImageSequenceMovieFile.cxx
  ImageSequenceName.cxx
  JPEG_Decompressor.cxx
  Mapping_2d_2d.cxx
  MedianReduceImage.cxx
  MovieFile.cxx
  MovieFileInterface.cxx
  RadialLensCorrection.cxx
  SGIMovieFile.cxx
  SGIMovieFilePrivates.cxx
  SGIMovieFileWrite.cxx
  SequenceFileName.cxx
  oxp_parse_seqname.cxx
  vsl_conic_as_matrix.cxx
  vsl_conic_as_params.cxx
  vsl_conic_points.cxx
)

AUX_SOURCE_DIRECTORY(Templates oxp_sources)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/oxl/oxp/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

IF(JPEG)
  # do nothing
ELSE(JPEG)
  INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/v3p/jpeg)
ENDIF(JPEG)
