# oxl/oxp/CMakeLists.txt

ADD_LIBRARY(oxp oxp_sources)

SOURCE_FILES(oxp_sources
 ByteConvertImage.cxx       ByteConvertImage.h
 GXFileVisitor.cxx          GXFileVisitor.h
 ImageSequenceMovieFile.cxx ImageSequenceMovieFile.h
 ImageSequenceName.cxx      ImageSequenceName.h
 Mapping_2d_2d.cxx          Mapping_2d_2d.h
 MedianReduceImage.cxx
 MovieFile.cxx              MovieFile.h
 MovieFileInterface.cxx     MovieFileInterface.h
 RadialLensCorrection.cxx   RadialLensCorrection.h
 SequenceFileName.cxx       SequenceFileName.h
 oxp_parse_seqname.cxx      oxp_parse_seqname.h
 oxp_bunch_of_files.cxx     oxp_bunch_of_files.h
#vsl_conic_as_matrix.cxx    vsl_conic_as_matrix.h
#vsl_conic_as_params.cxx    vsl_conic_as_params.h
#vsl_conic_points.cxx       vsl_conic_points.h
 oxp_yuv_to_rgb.cxx         oxp_yuv_to_rgb.h
 oxp_vidl_moviefile.cxx     oxp_vidl_moviefile.h

 ImageWarp.txx              ImageWarp.h
 MapInverter.h
 POX.h
)

AUX_SOURCE_DIRECTORY(Templates oxp_sources)

INCLUDE(${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindJPEG.cmake)

IF (HAS_JPEG)
  SOURCE_FILES(oxp_sources
   JPEG_Decompressor.cxx    JPEG_Decompressor.h
   SGIMovieFile.cxx         SGIMovieFile.h
   SGIMovieFilePrivates.cxx SGIMovieFilePrivates.h
   SGIMovieFileWrite.cxx    SGIMovieFileWrite.h
  )
ENDIF (HAS_JPEG)

INCLUDE(${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindMPEG.cmake)

IF (HAS_MPEG)
  SOURCE_FILES(oxp_sources
   oxp_mpeg_codec.cxx      oxp_mpeg_codec.h
   oxp_vidl_mpeg_codec.cxx oxp_vidl_mpeg_codec.h
  )
ENDIF (HAS_MPEG)
