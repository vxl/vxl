# oxl/oxp/CMakeLists.txt

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

INCLUDE(${MODULE_PATH}/FindJPEG.cmake)
IF (HAS_JPEG)
  INCLUDE_DIRECTORIES(${JPEG_INCLUDE_PATH})
  SOURCE_FILES(oxp_sources
   JPEG_Decompressor.cxx    JPEG_Decompressor.h
   SGIMovieFile.cxx         SGIMovieFile.h
   SGIMovieFilePrivates.cxx SGIMovieFilePrivates.h
   SGIMovieFileWrite.cxx    SGIMovieFileWrite.h
  )
ENDIF (HAS_JPEG)

INCLUDE(${MODULE_PATH}/FindMPEG.cmake)
IF (HAS_MPEG)
  INCLUDE_DIRECTORIES(${MPEG_INCLUDE_PATH})
  SOURCE_FILES(oxp_sources
   oxp_mpeg_codec.cxx      oxp_mpeg_codec.h
   oxp_vidl_mpeg_codec.cxx oxp_vidl_mpeg_codec.h
  )
ENDIF (HAS_MPEG)


AUX_SOURCE_DIRECTORY(Templates oxp_sources)

ADD_LIBRARY(oxp oxp_sources)
TARGET_LINK_LIBRARIES(oxp vnl_algo vnl vil vul vbl)
IF (HAS_JPEG)
  TARGET_LINK_LIBRARIES(oxp ${JPEG_LIBRARIES})
ENDIF (HAS_JPEG)
IF (HAS_MPEG)
  TARGET_LINK_LIBRARIES(oxp ${MPEG_LIBRARIES})
ENDIF (HAS_MPEG)
