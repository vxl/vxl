# ./oxl/oxp/CMakeLists.txt

ADD_LIBRARY(oxp oxp_sources)

SOURCE_FILES(oxp_sources
  ByteConvertImage.cxx
  GXFileVisitor.cxx
  ImageSequenceMovieFile.cxx
  ImageSequenceName.cxx
  oxp_vidl_moviefile.cxx
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
  oxp_bunch_of_files.cxx
  vsl_conic_as_matrix.cxx
  vsl_conic_as_params.cxx
  vsl_conic_points.cxx
  oxp_yuv_to_rgb.cxx
)

AUX_SOURCE_DIRECTORY(Templates oxp_sources)

INCLUDE( ${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindMPEG.cmake )

IF( HAS_MPEG )
  SOURCE_FILES(oxp_sources
    oxp_mpeg_codec.cxx
    oxp_vidl_mpeg_codec.cxx
  )
ENDIF( HAS_MPEG )

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
