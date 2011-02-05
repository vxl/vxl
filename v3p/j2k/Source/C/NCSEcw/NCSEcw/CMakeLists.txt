project( NCSEcw )
SUBDIRS(../../NCSnet/NCScnet2)
SUBDIRS(../../NCSUtil/NCSUtil_Static)
SUBDIRS(../../NCSnet/NCScnet2/NCScnet2_Static)


set(NCSECW_SOURCES
  ../shared_src/build_pyr.c
  ../lcms/src/cmscnvrt.c
  ../lcms/src/cmserr.c
  ../lcms/src/cmsgamma.c
  ../lcms/src/cmsgmt.c
  ../lcms/src/cmsintrp.c
  ../lcms/src/cmsio1.c
  ../lcms/src/cmslut.c
  ../lcms/src/cmsmatsh.c
  ../lcms/src/cmsmtrx.c
  ../lcms/src/cmsnamed.c
  ../lcms/src/cmspack.c
  ../lcms/src/cmspcs.c
  ../lcms/src/cmssamp.c
  ../lcms/src/cmsvirt.c
  ../lcms/src/cmswtpnt.c
  ../lcms/src/cmsxform.c
  ../shared_src/collapse_pyr.c
  ComNCSRenderer.cpp
  ecw_jni.c
  ecw_jni_config.c
  ../shared_src/ecw_open.c
  ../shared_src/ecw_read.c
  ../shared_src/fileio_compress.c
  ../shared_src/fileio_decompress.c
  NCSAffineTransform.cpp
  NCSBlockFile.cpp
  ncscbm.c
  ncscbmidwt.c
  ncscbmnet.c
  ncscbmopen.c
  ncscbmpurge.c
  NCSEcw.cpp
  NCSEcw.rc
  NCSFile.cpp
  ../../NCSGDT2/NCSGDTEpsg.cpp
  ../../NCSGDT2/NCSGDTEPSGKey.cpp
  ../../NCSGDT2/NCSGDTLocation.cpp
  NCSHuffmanCoder.cpp
  ../NCSJP2/NCSJP2.cpp
  ../NCSJP2/NCSJP2BitsPerComponentBox.cpp
  ../NCSJP2/NCSJP2Box.cpp
  ../NCSJP2/NCSJP2CaptureResolutionBox.cpp
  ../NCSJP2/NCSJP2ChannelDefinitionBox.cpp
  ../NCSJP2/NCSJP2ColorSpecificationBox.cpp
  ../NCSJP2/NCSJP2ComponentMappingBox.cpp
  ../NCSJP2/NCSJP2ContiguousCodestreamBox.cpp
  ../NCSJP2/NCSJP2DataEntryURLBox.cpp
  ../NCSJP2/NCSJP2DefaultDisplayResolutionBox.cpp
  ../NCSJP2/NCSJP2File.cpp
  ../NCSJP2/NCSJP2FileTypeBox.cpp
  ../NCSJP2/NCSJP2FileView.cpp
  ../NCSJP2/NCSJP2GMLGeoLocationBox.cpp
  ../NCSJP2/NCSJP2HeaderBox.cpp
  ../NCSJP2/NCSJP2ImageHeaderBox.cpp
  ../NCSJP2/NCSJP2IntellectualPropertyBox.cpp
  ../NCSJP2/NCSJP2PaletteBox.cpp
  ../NCSJP2/NCSJP2PCSBox.cpp
  ../NCSJP2/NCSJP2ResolutionBox.cpp
  ../NCSJP2/NCSJP2SignatureBox.cpp
  ../NCSJP2/NCSJP2SuperBox.cpp
  ../NCSJP2/NCSJP2UUIDBox.cpp
  ../NCSJP2/NCSJP2UUIDInfoBox.cpp
  ../NCSJP2/NCSJP2UUIDListBox.cpp
  ../NCSJP2/NCSJP2WorldBox.cpp
  ../NCSJP2/NCSJP2XMLBox.cpp
  ../NCSJP2/NCSJPC.cpp
  ../NCSJP2/NCSJPCBuffer.cpp
  ../NCSJP2/NCSJPCCOCMarker.cpp
  ../NCSJP2/NCSJPCCodeBlock.cpp
  ../NCSJP2/NCSJPCCodingStyleParameter.cpp
  ../NCSJP2/NCSJPCCODMarker.cpp
  ../NCSJP2/NCSJPCCOMMarker.cpp
  ../NCSJP2/NCSJPCComponent.cpp
  ../NCSJP2/NCSJPCComponentDepthType.cpp
  ../NCSJP2/NCSJPCCRGMarker.cpp
  ../NCSJP2/NCSJPCDCShiftNode.cpp
  ../NCSJP2/NCSJPCDump.cpp
  ../NCSJP2/NCSJPCEcwpIOStream.cpp
  ../NCSJP2/NCSJPCEOCMarker.cpp
  ../NCSJP2/NCSJPCEPHMarker.cpp
  ../NCSJP2/NCSJPCFileIOStream.cpp
  ../NCSJP2/NCSJPCICCNode.cpp
  ../NCSJP2/NCSJPCIOStream.cpp
  ../NCSJP2/NCSJPCMainHeader.cpp
  ../NCSJP2/NCSJPCMarker.cpp
  ../NCSJP2/NCSJPCMCTNode.cpp
  ../NCSJP2/NCSJPCMemoryIOStream.cpp
  ../NCSJP2/NCSJPCMQCoder.cpp
  ../NCSJP2/NCSJPCNode.cpp
  ../NCSJP2/NCSJPCNodeTiler.cpp
  ../NCSJP2/NCSJPCPacket.cpp
  ../NCSJP2/NCSJPCPacketLengthType.cpp
  ../NCSJP2/NCSJPCPaletteNode.cpp
  ../NCSJP2/NCSJPCPLMMarker.cpp
  ../NCSJP2/NCSJPCPLTMarker.cpp
  ../NCSJP2/NCSJPCPOCMarker.cpp
  ../NCSJP2/NCSJPCPPMMarker.cpp
  ../NCSJP2/NCSJPCPPTMarker.cpp
  ../NCSJP2/NCSJPCPrecinct.cpp
  ../NCSJP2/NCSJPCProgression.cpp
  ../NCSJP2/NCSJPCProgressionOrderType.cpp
  ../NCSJP2/NCSJPCQCCMarker.cpp
  ../NCSJP2/NCSJPCQCDMarker.cpp
  ../NCSJP2/NCSJPCQuantizationParameter.cpp
  ../NCSJP2/NCSJPCResample.cpp
  ../NCSJP2/NCSJPCResolution.cpp
  ../NCSJP2/NCSJPCRGNMarker.cpp
  ../NCSJP2/NCSJPCSegment.cpp
  ../NCSJP2/NCSJPCSIZMarker.cpp
  ../NCSJP2/NCSJPCSOCMarker.cpp
  ../NCSJP2/NCSJPCSODMarker.cpp
  ../NCSJP2/NCSJPCSOPMarker.cpp
  ../NCSJP2/NCSJPCSOTMarker.cpp
  ../NCSJP2/NCSJPCSubBand.cpp
  ../NCSJP2/NCSJPCT1Coder.cpp
  ../NCSJP2/NCSJPCTagTree.cpp
  ../NCSJP2/NCSJPCTilePartHeader.cpp
  ../NCSJP2/NCSJPCTLMMarker.cpp
  ../NCSJP2/NCSJPCYCbCrNode.cpp
  NCSOutputFile.cpp
  NCSRenderer.cpp
  NCSWorldFile.cpp
  ../shared_src/pack.c
  ../shared_src/qmf_util.c
  ../shared_src/quantize.c
 # NCSEcw.idl
  StdAfx.cpp
  ../shared_src/unpack.c
  ComNCSRenderer.rgs
  NCSEcw.def
 )

set(CMAKE_MFC_FLAG 2)

include_directories(
  ../../../include
  ../lcms/include
  ../../../../../geotiff
)

add_definitions(
  -DNCS_BUILD_WITH_COM
  -D_ATL_STATIC_REGISTRY
  -D_USRDLL
  -D_AFXEXT
  -D_AFXDLL
  -D_CRT_SECURE_NO_WARNINGS
  -D_UNICODE
  -DUNICODE
  -DNOASSEMBLER
)
if (UNIX)
add_definitions( -DLINUX)
ENDIF(UNIX)
if (APPLE)
add_definitions( -DMACINTOSH)
ENDIF(APPLE)
add_library( NCSEcw SHARED ${NCSECW_SOURCES} )
SET_TARGET_PROPERTIES(NCSEcw PROPERTIES OUTPUT_NAME NCSEcw DEBUG_POSTFIX d)
target_link_libraries( NCSEcw NCScnet2 NCSUtil )
INSTALL_TARGETS( /lib NCSEcw )

