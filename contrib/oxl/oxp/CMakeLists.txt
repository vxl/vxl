# oxl/oxp/CMakeLists.txt

SET(oxp_sources
 ByteConvertImage.cxx       ByteConvertImage.h
 GXFileVisitor.cxx          GXFileVisitor.h
 ImageSequenceMovieFile.cxx ImageSequenceMovieFile.h
 ImageSequenceName.cxx      ImageSequenceName.h
 Mapping_2d_2d.cxx          Mapping_2d_2d.h
 MedianReduceImage.cxx      MedianReduceImage.h
 MovieFile.cxx              MovieFile.h
 MovieFileInterface.cxx     MovieFileInterface.h
 RadialLensCorrection.cxx   RadialLensCorrection.h
 SequenceFileName.cxx       SequenceFileName.h
 oxp_parse_seqname.cxx      oxp_parse_seqname.h
 oxp_bunch_of_files.cxx     oxp_bunch_of_files.h
 oxp_yuv_to_rgb.cxx         oxp_yuv_to_rgb.h
 oxp_vidl_moviefile.cxx     oxp_vidl_moviefile.h
 oxp_vob_frame_index.cxx    oxp_vob_frame_index.h

 ImageWarp.txx              ImageWarp.h
 MapInverter.h
)

INCLUDE(${MODULE_PATH}/FindJPEG.cmake)
IF (JPEG_FOUND)
  INCLUDE_DIRECTORIES(${JPEG_INCLUDE_DIR})

  SET(oxp_sources ${oxp_sources}
   JPEG_Decompressor.cxx    JPEG_Decompressor.h
   SGIMovieFile.cxx         SGIMovieFile.h      SGIMovieFilePrivates.h
   SGIMovieFileWrite.cxx    SGIMovieFileWrite.h
  )

  IF(CMAKE_COMPILER_IS_GNUCXX)
    SET_SOURCE_FILES_PROPERTIES(SGIMovieFile.cxx PROPERTIES COMPILE_FLAGS -O1)
  ENDIF(CMAKE_COMPILER_IS_GNUCXX)
ENDIF (JPEG_FOUND)

INCLUDE(${CMAKE_ROOT}/Modules/FindMPEG2.cmake)
IF (MPEG2_FOUND)
  INCLUDE_DIRECTORIES(${MPEG2_INCLUDE_DIR})
  ADD_DEFINITIONS(-DHAS_MPEG2)
  SET(oxp_sources ${oxp_sources}
      oxp_mpeg_codec.cxx      oxp_mpeg_codec.h
      oxp_vidl_mpeg_codec.cxx oxp_vidl_mpeg_codec.h
  )
ENDIF (MPEG2_FOUND)


AUX_SOURCE_DIRECTORY(Templates oxp_sources)

ADD_LIBRARY(oxp ${oxp_sources})
TARGET_LINK_LIBRARIES(oxp vidl_vil1 vnl vil1 vul vbl vpl ${CMAKE_THREAD_LIBS})
IF (MPEG2_FOUND)
  TARGET_LINK_LIBRARIES(oxp ${MPEG2_LIBRARIES})
ENDIF (MPEG2_FOUND)
