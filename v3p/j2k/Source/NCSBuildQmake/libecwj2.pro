# 	QT qmake file for libecwj2 (a.k.a. the ECW JPEG 2000 SDK)
#	Author: 	Stuart Nixon
#	Last update:	Tom Lynch, 2005-09-14
#
#	This qmake file can be used to create build files (makefiles/project 
#	files/xcode files etc) to compile this library on different platforms.
#
#	Build files from this qmake file have been created for common platforms.
#	e.g.
#		libecwj2-win32-static.dsp for Windows/Microsoft Visual C++ 6.0
#		Makefile-linux-shared for GNU/Linux
#
#	so you should only need to modify this file if you decide to add support
#	for a new platform, or discover a problem with the build configuration
#	for one of the platforms with existing support.
#
#	Output library will be put into the appropriate OS subdirectory of libs,
#	for example "lib/win32-msvc/libecwj2.lib" for Windows 32 compiled with VC++.
#
#	Change "staticlib" to "dll" on the CONFIG line to generate a shared library.
#	The default behaviour is to create a single static library.
#
#	This library builds the standard libecwj2 or ECW JPEG 2000 SDK. Refer to 
#	the main documentation for other build alternatives (for example to add 
#	Java bindings).
#
#	Common issues that may be faced when building and using libecwj2 are 
#	discussed in the README file distributed with the source code.  See this
#	resource for tips on how to resolve compilation errors that may occur and
#	details on enabling additional functionality.
#
#	Compile notes:
#		Win32/VC6++:
#			If you get errors about "_aligned_malloc" not defined, this is because the 
#			Processor Pack is not installed (for SSE instruction support). You must either:
#			-	Install the Processor Pack (http://msdn.microsoft.com/vstudio/downloads/tools/ppack/download.aspx)
#			-	Or use Microsoft Platform SDK Compiler  (http://www.microsoft.com/msdownload/platformsdk/sdkupdate)
#			-	Or compile using VC 2003
#			Note that is you have installed VC6++ Service Pack 6, you can not install the Processor Pack
#

# TODO BEFORE SHIP:
#	1.Utils library - fix so works with UNICODE
#		(NCSBase64.cpp, CNCSBase64Coder.cpp, NCSLog.cpp, etc)
#	2. Should detect absence of Processor Pack and give suitable error

TEMPLATE = lib
win32:libecwj2:TARGET = libecwj2
!win32:libecwj2:TARGET = ecwj2

# Recognised configuration options when creating project files
# NCSEcw	create NCSEcw library project (disabled)
# NCScnet	create NCScnet library project (disabled)
# NCSUtil	create NCSUtil library project (disabled)
# NCSEcwC	create NCSEcwC library project (disabled)
# libecwj2	create libecwj2 library project
# jni		include JNI material if creating NCSEcw project
# com		include COM/ATL material if creating NCSEcw DLL project
# stderrinfo	include debugging printfs to stderr
# limited	limit compression to inputs of < 500 MB with C API
# dll		change staticlib to dll for a shared library
#
# NOTE: many of the configuration options cannot be used simultaneously and 
# still have the desired effect:
# 	NCSEcw, NCScnet, NCSUtil and libecwj2 are mutually exclusive
# 	jni and com are applicable only to NCSEcw
#	limited is applicable only to NCSEcwC
#	dll will prevent the creation of the static library project
#
# The intent is that the various configurations desired be built one by 
# one using the batch file that accompanies this .pro file

# Default configuration
CONFIG -= qt
CONFIG *= warn_on thread 
# exceptions (removed from default config)

# Establish compilation options for the various platforms
# common options for any Linux platform
DEFINES *= UNICODE
linux-* {	
	DEFINES	*= LINUX _LARGEFILE_SOURCE _LARGEFILE64_SOURCE _FILE_OFFSET_BITS=64
}
# common options for any windows platform
win32-* {
	DEFINES	*= _MBCS _UNICODE
	QMAKE_CFLAGS_RELEASE = -O2
	QMAKE_CXXFLAGS_RELEASE = -O2
}
# common options for the g++ compiler
*-g++ {	
	DEFINES *= _GNU_SOURCE POSIX _REENTRANT 
	CFLAGS	*= -Wall -Wno-long-long -fpic -fno-common
	CXXFLAGS *= -Wall -Wno-long-long -fpic -fno-common
	dll:CFLAGS *= -shared
	dll:CXXFLAGS *= -shared
}
# solaris/g++ options
solaris-g++ {
	DEFINES	*= SOLARIS _REENTRANT _LARGEFILE_SOURCE _FILE_OFFSET_BITS=64 _LARGEFILE64_SOURCE=1
	dll:CFLAGS *= -mno-app-regs 
	dll:CXXFLAGS *= -mno-app-regs
	LFLAGS *= -G -z defs
}
# hpux/acc options - untested 
hpux-acc {		
	DEFINES *= HPUX+Z POSIX _LARGEFILE64_SOURCE _FILE_OFFSET_BITS=64 _PSTAT64
	DEFINES *= RWSTD_MULTI_THREAD RW_MULTI_THREAD 
	CFLAGS *= -Wall +O3 +DA2.0W +DD64 -Ae
	CXXFLAGS *= -Wall +O3 +DA2.0W +DD64 -AA +W 829,921,430 
}
# macosx options
macx-g++ {	
	DEFINES	*= MACOSX 
	LFLAGS *= -L/usr/lib -L/usr/local/lib
	dll:LFLAGS *= -dynamiclib -single_module
	LFLAGS *= /System/Library/Frameworks/Carbon.framework/Carbon
}

# Establish the location of the source and include directories, 
# the include search path, and other directories
SDIR = ../../Source
IDIR = ../../Source/include
BINDIR = ../../bin
LIBDIR = ../../lib
OBJDIR = ../../obj
INCLUDEPATH = $${IDIR}
macx-*:INCLUDEPATH *= /Developer/Headers/FlatCarbon/
STATIC_SUFFIX = S
DEBUG_SUFFIX = d

# SOURCES FOR THE NCSEcw PROJECT
# This includes:	JPEG 2000 codec source
#			ECW codec source
#			LittleCMS ICC profile management source
#			Win32 renderer source
DIR = $${SDIR}/C/NCSEcw/NCSJP2		
ECW_SOURCES += $${DIR}/NCSJP2.cpp 
ECW_SOURCES += $${DIR}/NCSJP2BitsPerComponentBox.cpp 
ECW_SOURCES += $${DIR}/NCSJP2Box.cpp
ECW_SOURCES += $${DIR}/NCSJP2CaptureResolutionBox.cpp
ECW_SOURCES += $${DIR}/NCSJP2ChannelDefinitionBox.cpp 
ECW_SOURCES += $${DIR}/NCSJP2ColorSpecificationBox.cpp 
ECW_SOURCES += $${DIR}/NCSJP2ComponentMappingBox.cpp
ECW_SOURCES += $${DIR}/NCSJP2ContiguousCodestreamBox.cpp 
ECW_SOURCES += $${DIR}/NCSJP2DataEntryURLBox.cpp 
ECW_SOURCES += $${DIR}/NCSJP2DefaultDisplayResolutionBox.cpp
ECW_SOURCES += $${DIR}/NCSJP2File.cpp 
ECW_SOURCES += $${DIR}/NCSJP2FileTypeBox.cpp 
ECW_SOURCES += $${DIR}/NCSJP2FileView.cpp
ECW_SOURCES += $${DIR}/NCSJP2GMLGeoLocationBox.cpp
ECW_SOURCES += $${DIR}/NCSJP2HeaderBox.cpp
ECW_SOURCES += $${DIR}/NCSJP2ImageHeaderBox.cpp
ECW_SOURCES += $${DIR}/NCSJP2IntellectualPropertyBox.cpp
ECW_SOURCES += $${DIR}/NCSJP2PaletteBox.cpp
ECW_SOURCES += $${DIR}/NCSJP2PCSBox.cpp
ECW_SOURCES += $${DIR}/NCSJP2ResolutionBox.cpp
ECW_SOURCES += $${DIR}/NCSJP2SignatureBox.cpp
ECW_SOURCES += $${DIR}/NCSJP2SuperBox.cpp
ECW_SOURCES += $${DIR}/NCSJP2UUIDBox.cpp
ECW_SOURCES += $${DIR}/NCSJP2UUIDInfoBox.cpp
ECW_SOURCES += $${DIR}/NCSJP2UUIDListBox.cpp
ECW_SOURCES += $${DIR}/NCSJP2WorldBox.cpp
ECW_SOURCES += $${DIR}/NCSJP2XMLBox.cpp
ECW_SOURCES += $${DIR}/NCSJPC.cpp
ECW_SOURCES += $${DIR}/NCSJPCBuffer.cpp
ECW_SOURCES += $${DIR}/NCSJPCCOCMarker.cpp
ECW_SOURCES += $${DIR}/NCSJPCCodeBlock.cpp
ECW_SOURCES += $${DIR}/NCSJPCCodingStyleParameter.cpp
ECW_SOURCES += $${DIR}/NCSJPCCODMarker.cpp
ECW_SOURCES += $${DIR}/NCSJPCCOMMarker.cpp
ECW_SOURCES += $${DIR}/NCSJPCComponent.cpp
ECW_SOURCES += $${DIR}/NCSJPCComponentDepthType.cpp
ECW_SOURCES += $${DIR}/NCSJPCCRGMarker.cpp
ECW_SOURCES += $${DIR}/NCSJPCDCShiftNode.cpp 
ECW_SOURCES += $${DIR}/NCSJPCDump.cpp
ECW_SOURCES += $${DIR}/NCSJPCEcwpIOStream.cpp
ECW_SOURCES += $${DIR}/NCSJPCEOCMarker.cpp
ECW_SOURCES += $${DIR}/NCSJPCEPHMarker.cpp
ECW_SOURCES += $${DIR}/NCSJPCFileIOStream.cpp
ECW_SOURCES += $${DIR}/NCSJPCICCNode.cpp
ECW_SOURCES += $${DIR}/NCSJPCIOStream.cpp
ECW_SOURCES += $${DIR}/NCSJPCMainHeader.cpp
ECW_SOURCES += $${DIR}/NCSJPCMarker.cpp
ECW_SOURCES += $${DIR}/NCSJPCMCTNode.cpp
ECW_SOURCES += $${DIR}/NCSJPCMemoryIOStream.cpp
ECW_SOURCES += $${DIR}/NCSJPCMQCoder.cpp
ECW_SOURCES += $${DIR}/NCSJPCNode.cpp
ECW_SOURCES += $${DIR}/NCSJPCNodeTiler.cpp
ECW_SOURCES += $${DIR}/NCSJPCPacket.cpp
ECW_SOURCES += $${DIR}/NCSJPCPacketLengthType.cpp
ECW_SOURCES += $${DIR}/NCSJPCPaletteNode.cpp
ECW_SOURCES += $${DIR}/NCSJPCPLMMarker.cpp
ECW_SOURCES += $${DIR}/NCSJPCPLTMarker.cpp
ECW_SOURCES += $${DIR}/NCSJPCPOCMarker.cpp
ECW_SOURCES += $${DIR}/NCSJPCPPMMarker.cpp
ECW_SOURCES += $${DIR}/NCSJPCPPTMarker.cpp
ECW_SOURCES += $${DIR}/NCSJPCPrecinct.cpp
ECW_SOURCES += $${DIR}/NCSJPCProgression.cpp
ECW_SOURCES += $${DIR}/NCSJPCProgressionOrderType.cpp
ECW_SOURCES += $${DIR}/NCSJPCQCCMarker.cpp
ECW_SOURCES += $${DIR}/NCSJPCQCDMarker.cpp	
ECW_SOURCES += $${DIR}/NCSJPCQuantizationParameter.cpp
ECW_SOURCES += $${DIR}/NCSJPCResample.cpp
ECW_SOURCES += $${DIR}/NCSJPCResolution.cpp
ECW_SOURCES += $${DIR}/NCSJPCRGNMarker.cpp
ECW_SOURCES += $${DIR}/NCSJPCSegment.cpp
ECW_SOURCES += $${DIR}/NCSJPCSIZMarker.cpp
ECW_SOURCES += $${DIR}/NCSJPCSOCMarker.cpp
ECW_SOURCES += $${DIR}/NCSJPCSODMarker.cpp
ECW_SOURCES += $${DIR}/NCSJPCSOPMarker.cpp
ECW_SOURCES += $${DIR}/NCSJPCSOTMarker.cpp
ECW_SOURCES += $${DIR}/NCSJPCSubBand.cpp
ECW_SOURCES += $${DIR}/NCSJPCT1Coder.cpp
ECW_SOURCES += $${DIR}/NCSJPCTagTree.cpp
ECW_SOURCES += $${DIR}/NCSJPCTilePartHeader.cpp
ECW_SOURCES += $${DIR}/NCSJPCTLMMarker.cpp
ECW_SOURCES += $${DIR}/NCSJPCYCbCrNode.cpp
# directory containing ECW and other non-JP2 source code
DIR = $${SDIR}/C/NCSEcw/NCSEcw
# source code for win32 only features
win32 {
	ECW_SOURCES += $${DIR}/NCSOutputFile.cpp
	ECW_SOURCES += $${DIR}/NCSRenderer.cpp
}
ECW_SOURCES += $${DIR}/NCSWorldFile.cpp
ECW_SOURCES += $${DIR}/NCSAffineTransform.cpp
ECW_SOURCES += $${DIR}/NCSBlockFile.cpp
ECW_SOURCES += $${DIR}/ncscbm.c
ECW_SOURCES += $${DIR}/ncscbmidwt.c 
ECW_SOURCES += $${DIR}/ncscbmnet.c 
ECW_SOURCES += $${DIR}/ncscbmopen.c
ECW_SOURCES += $${DIR}/ncscbmpurge.c 
ECW_SOURCES += $${DIR}/NCSEcw.cpp
ECW_SOURCES += $${DIR}/NCSFile.cpp
ECW_SOURCES += $${DIR}/NCSHuffmanCoder.cpp
# directory containing shared ECW compression and decompression source code
DIR = $${SDIR}/C/NCSEcw/shared_src
ECW_SOURCES += $${DIR}/collapse_pyr.c 
ECW_SOURCES += $${DIR}/ecw_open.c 
ECW_SOURCES += $${DIR}/ecw_read.c 
ECW_SOURCES += $${DIR}/fileio_compress.c
ECW_SOURCES += $${DIR}/fileio_decompress.c 
ECW_SOURCES += $${DIR}/build_pyr.c 
ECW_SOURCES += $${DIR}/pack.c 
ECW_SOURCES += $${DIR}/qmf_util.c 
ECW_SOURCES += $${DIR}/quantize.c 
ECW_SOURCES += $${DIR}/unpack.c
# libcms ICC profiling library source code - see relevant IP declarations
DIR = $${SDIR}/C/NCSEcw/lcms/src
ECW_SOURCES += $${DIR}/cmscnvrt.c
ECW_SOURCES += $${DIR}/cmserr.c
ECW_SOURCES += $${DIR}/cmsgamma.c
ECW_SOURCES += $${DIR}/cmsgmt.c
ECW_SOURCES += $${DIR}/cmsintrp.c
ECW_SOURCES += $${DIR}/cmsio1.c
ECW_SOURCES += $${DIR}/cmslut.c
ECW_SOURCES += $${DIR}/cmsmatsh.c
ECW_SOURCES += $${DIR}/cmsmtrx.c
ECW_SOURCES += $${DIR}/cmsnamed.c
ECW_SOURCES += $${DIR}/cmspack.c
ECW_SOURCES += $${DIR}/cmspcs.c
ECW_SOURCES += $${DIR}/cmssamp.c
ECW_SOURCES+= $${DIR}/cmsvirt.c
ECW_SOURCES+= $${DIR}/cmswtpnt.c
ECW_SOURCES+= $${DIR}/cmsxform.c
# Geocoding utility support
DIR = $${SDIR}/C/NCSGDT2
ECW_SOURCES += $${DIR}/NCSGDTEpsg.cpp
ECW_SOURCES += $${DIR}/NCSGDTEPSGKey.cpp
ECW_SOURCES += $${DIR}/NCSGDTLocation.cpp
# Some additional headers
ECW_HEADERS += $${IDIR}/ECW.h $${IDIR}/NCSMisc.h

# SOURCES FOR THE NCScnet PROJECT
# This includes:	Win32-specific streaming imagery network services (cnet2)
#			Generic streaming imagery network services (cnet3)
win32 {
	DIR = $${SDIR}/C/NCSnet/NCScnet2
	CNET_SOURCES += $${DIR}/connect.c 
	CNET_SOURCES += $${DIR}/NCScnet.c 
	CNET_SOURCES += $${DIR}/NCSWinHttp.c 
	CNET_SOURCES += $${DIR}/packet.c 
	CNET_SOURCES += $${DIR}/cnet2util.c
}
!win32 {
	DIR = $${SDIR}/C/NCSnet/NCScnet3
	CNET_SOURCES += $${DIR}/NCScnet.cpp
	CNET_SOURCES += $${DIR}/NCSGetPasswordDlg.cpp
	CNET_SOURCES += $${DIR}/NCSGetRequest.cpp
	CNET_SOURCES += $${DIR}/NCSPostRequest.cpp
	CNET_SOURCES += $${DIR}/NCSProxy.cpp
	CNET_SOURCES += $${DIR}/NCSRequest.cpp
	CNET_SOURCES += $${DIR}/NCSSocket.cpp
}

# SOURCES FOR THE NCSUtil PROJECT
DIR = $${SDIR}/C/NCSUtil
win32 {
	UTIL_SOURCES += $${DIR}/CNCSMetabaseEdit.cpp
	UTIL_SOURCES += $${DIR}/CNCSMultiSZ.cpp
	UTIL_SOURCES += $${DIR}/exception_catch.c 
	UTIL_SOURCES += $${DIR}/main.c 
	UTIL_SOURCES += $${DIR}/NCSCoordinateConverter.cpp
	UTIL_SOURCES += $${DIR}/NCSCoordinateSystem.cpp
	UTIL_SOURCES += $${DIR}/NCSCoordinateTransform.cpp
	UTIL_SOURCES += $${DIR}/NCSCrypto.cpp
	UTIL_SOURCES += $${DIR}/NCSExtent.cpp
	UTIL_SOURCES += $${DIR}/NCSExtents.cpp
	UTIL_SOURCES += $${DIR}/NCSObjectList.cpp
	UTIL_SOURCES += $${DIR}/NCSPoint.cpp
	UTIL_SOURCES += $${DIR}/NCSRasterCoordinateConverter.cpp
	UTIL_SOURCES += $${DIR}/NCSScreenPoint.cpp
	UTIL_SOURCES += $${DIR}/NCSServerState.cpp
	UTIL_SOURCES += $${DIR}/NCSWorldPoint.cpp
	UTIL_SOURCES += $${DIR}/quadtree.cpp
	UTIL_SOURCES += $${DIR}/timer.c
	UTIL_SOURCES += $${DIR}/NCSPrefsWin.cpp
}
!win32 {
	UTIL_SOURCES += $${DIR}/NCSPrefsXML.cpp
}
UTIL_SOURCES += $${DIR}/NCSPrefs.cpp
UTIL_SOURCES += $${DIR}/CNCSBase64Coder.cpp
UTIL_SOURCES += $${DIR}/dynamiclib.c	
UTIL_SOURCES += $${DIR}/error.c 
UTIL_SOURCES += $${DIR}/file.c 
UTIL_SOURCES += $${DIR}/log.cpp
UTIL_SOURCES += $${DIR}/malloc.c
UTIL_SOURCES += $${DIR}/mutex.c 
UTIL_SOURCES += $${DIR}/NCSBase64.cpp
UTIL_SOURCES += $${DIR}/NCSError.cpp
UTIL_SOURCES += $${DIR}/NCSEvent.cpp	
UTIL_SOURCES += $${DIR}/NCSLog.cpp
UTIL_SOURCES += $${DIR}/NCSMutex.cpp
UTIL_SOURCES += $${DIR}/NCSObject.cpp
UTIL_SOURCES += $${DIR}/NCSThread.cpp
UTIL_SOURCES += $${DIR}/pool.c 
UTIL_SOURCES += $${DIR}/queue.c 
UTIL_SOURCES += $${DIR}/thread.c 
UTIL_SOURCES += $${DIR}/util.c
UTIL_SOURCES += $${DIR}/NCSString.cpp

# SOURCES FOR THE NCSEcwC PROJECT
DIR = $${SDIR}/C/NCSEcw/shared_src
ECWC_SOURCES = $${DIR}/compress.cpp

# Adjust the source files for the config options
jni {
	ECW_SOURCES *= ecw_jni.c ecw_jni_config.c
	ECW_HEADERS *= JNCSFile.h JNCSEcwConfig.h
}
# NOTE: to build the Win32 DLL with COM support, it is necessary to produce COM interface 
# headers from the .idl file shipped with the distribution.  This is done using
# the Microsoft MIDL compiler, the correct invocation of which is
# 	midl /tlb "NCSEcw.tlb" /h "NCSEcwCom.h" /iid "NCSEcw_i.c" /I "../../../include" /Oicf NCSEcw.idl
#
# dll:NCSEcw {
#	NCSEcw {
#		ECW_SOURCES *= ComNCSRenderer.cpp
#		ECW_HEADERS *= NCSEcwCom.h ComNCSRenderer.h NCSEcw_i.c	
#	}
# }


# Locations, export files and configuration for the projects
#
# NOTE: the experimental Qmake-generated versions of NCSEcw, NCSUtil and NCScnet 
# below are not supported in this beta distribution
#
# NCSEcw {
#	!com:DEFINES *= LIBECWJ2
#	SOURCES = $${ECW_SOURCES}
#	HEADERS = $${ECW_HEADERS}
#	# The LittleCMS include path must be added
#	INCLUDEPATH *= $${SDIR}/C/NCSEcw/lcms/include
#	TARGET = NCSEcw
#	staticlib {
#		OBJECTS_DIR = $${OBJDIR}/$$(QMAKESPEC)/static
#	}
#	dll {
#		DIR = $${SDIR}/C/NCSEcw/NCSEcw
#		win32:com:SOURCES *= $${DIR}/ComNCSRenderer.cpp
#		jni {
#			SOURCES *= $${DIR}/ecw_jni.c $${DIR}/ecw_jni_config.c
#			HEADERS *= $${IDIR}/JNCSFile.h $${IDIR}/JNCSEcwConfig.h
#		}
#		OBJECTS_DIR = $${OBJDIR}/$$(QMAKESPEC)/shared
#		win32:com:DEF_FILE = NCSEcwCom.def
#		win32:!com:DEF_FILE = NCSEcw.def
#		win32:com:DEFINES *= _ATL_STATIC_REGISTRY _ATL_MIN_CRT _USRDLL
#		win32:LIBS += NCSUtil.lib NCScnet.lib
#	}
# }
# NCScnet {
#	TARGET = NCScnet
#	SOURCES = $${CNET_SOURCES}
#	staticlib {
#		OBJECTS_DIR = $${OBJDIR}/$$(QMAKESPEC)/static
#			}
#	dll {
#		OBJECTS_DIR = $${OBJDIR}/$$(QMAKESPEC)/shared
#		win32:DEF_FILE = NCScnet.def
#		#win32:LIBS += NCSUtil.lib Crypt32.lib
#	}
# }
# NCSUtil {
#	TARGET = NCSUtil
#	DEFINES *= _USRDLL NCSUTIL_EXPORTS NCSMALLOCLOG
#	SOURCES = $${UTIL_SOURCES}
#	staticlib {
#		OBJECTS_DIR = $${OBJDIR}/$$(QMAKESPEC)/static
#	}
#	dll {
#		OBJECTS_DIR = $${OBJDIR}/$$(QMAKESPEC)/shared
#		win32:DEF_FILE = NCSUtil.def
#		win32:dll:LIBS *= imagehlp.lib version.lib shlwapi.lib	
#	}
# }
# NCSEcwC {
#	SOURCES = $${ECWC_SOURCES}
#	staticlib {
#		TARGET = NCSEcwCu
#		OBJECTS_DIR = $${OBJDIR}/$$(QMAKESPEC)/static
#	}
#	dll {
#		TARGET = NCSEcwC
#		OBJECTS_DIR = $${OBJDIR}/$$(QMAKESPEC)/shared
#		win32:DEF_FILE = ncsecwc.def
#	}	
# }
libecwj2 {
	DEFINES *= LIBECWJ2
	SOURCES = $${ECW_SOURCES} $${CNET_SOURCES} $${UTIL_SOURCES} $${ECWC_SOURCES}
	HEADERS = $${ECW_HEADERS}
	INCLUDEPATH *= $${SDIR}/C/NCSEcw/lcms/include
	DEFINES *= LIBECWJ2
	staticlib {
		DESTDIR = $${LIBDIR}
		OBJECTS_DIR = ../../obj/$$(QMAKESPEC)/static
	}
	dll {
		DESTDIR = $${LIBDIR}
		OBJECTS_DIR = ../../obj/$$(QMAKESPEC)/shared
		win32:DEF_FILE = libecwj2.def
		win32:dll:LIBS *= imagehlp.lib version.lib Crypt32.lib shlwapi.lib ws2_32.lib
	}
}

libecwj2:win32:TARGET = libecwj2
libecwj2:!win32:TARGET = ecwj2

# !libecwj2:staticlib:DESTDIR = $${LIBDIR}
# !libecwj2:dll:DESTDIR = $${LIBDIR}
dll:DLLDESTDIR = $${BINDIR}

# Adjust the preprocessor definitions for the config options
staticlib:DEFINES *= NCSECW_STATIC_LIBS
limited:DEFINES *= ECW_COMPRESS_SDK_VERSION
stderrinfo:DEFINES *= NCS_BUILD_WITH_STDERR_DEBUG_INFO

staticlib:TARGET = $${TARGET}$${STATIC_SUFFIX}
debug:TARGET = $${TARGET}$${DEBUG_SUFFIX}

