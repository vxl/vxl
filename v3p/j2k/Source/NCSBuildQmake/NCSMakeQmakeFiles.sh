#!/bin/sh
#
# This script is used to generate the Qmake files used by the ECW JPEG 2000 SDK
# source distribution.  It is designed to be run under cygwin.  Distribution parameters 
# are hard coded at present - see immediately below.  
#
# Author:	Tom Lynch
# Updated:	2005-09-13

# SOURCEDIR=$1
SOURCEDIR="../.."
pushd .
cd $SOURCEDIR/Source/NCSBuildQmake
echo "directory changed: new directory is $PWD"

OLDSPEC=$QMAKESPEC
QMAKECMD="qmake"
CDIR="../../examples/compression"
DDIR="../../examples/decompression"

# Short function which builds Qmake output and removes some unwanted dependencies 
# from some of the generated makefiles
fixup ()
{
	local ARGS=$@

	for arg in $ARGS ; do
		case $arg in
			*.pro) 		local PROFILE=$arg ;;
			*Makefile*) 	local MAKEFILE=$arg ;;
		esac
	done
	
	if [ "$MAKEFILE" != "" ] ; 
	then	
		echo "fixup on $MAKEFILE ..." 
		# Process the output file to remove the unwanted stuff
		sed -e 's/Q:/\.\.\/\.\./g' "${MAKEFILE}" > "${MAKEFILE}.sed1" ;
		sed -e 's/INCPATH\(\s\+=\s\+\S\+\s\+\)/INCPATH	= /g' "${MAKEFILE}.sed1" > "${MAKEFILE}.sed2" ;
		sed -e 's/all:\s\+\S\+\s\+/all: /g' "${MAKEFILE}.sed2" > "${MAKEFILE}.sed3" ;
		sed -e 's/Makefile-.*//g' "${MAKEFILE}.sed3" > "${MAKEFILE}.sed4" ;
		sed -e 's/\s\+\$\(QMAKE\).*//g' "${MAKEFILE}.sed4" > "${MAKEFILE}.sed5" ;
		sed -e 's/qmake:.*//g' "${MAKEFILE}.sed5" > "${MAKEFILE}.sed6" ; 
		sed -e 's/\s\+@\$\(QMAKE\).*//g' "${MAKEFILE}.sed6" > "${MAKEFILE}.sed7" ;
		cp -vf "${MAKEFILE}.sed7" "${MAKEFILE}" ;
		dos2unix -U "${MAKEFILE}" ;
		if [ -f "${MAKEFILE}.bak" ] ; then rm -vf "${MAKEFILE}.bak" ; fi
		find . -name "*Makefile*.sed*" -type f | xargs rm -vf ;
	fi
}

# All of the library builds
echo "creating build files for libecwj2 libraries ..."
export QMAKESPEC="win32-msvc"
qmake -win32 -tp vc -o libecwj2-win32-static.dsp "CONFIG+=libecwj2 staticlib" libecwj2.pro
qmake -win32 -tp vc -o libecwj2-win32-shared.dsp "CONFIG+=libecwj2 dll" libecwj2.pro
export QMAKESPEC="win32-msvc.net"
qmake -win32 -tp vc -o libecwj2-win32-net-static.vcproj "CONFIG+=libecwj2 staticlib" libecwj2.pro
qmake -win32 -tp vc -o libecwj2-win32-net-shared.vcproj "CONFIG+=libecwj2 dll" libecwj2.pro
export QMAKESPEC="linux-g++"
qmake -unix -o Makefile-linux-static "CONFIG+=libecwj2 staticlib" libecwj2.pro 
fixup Makefile-linux-static
qmake -unix -o Makefile-linux-shared "CONFIG+=libecwj2 dll" libecwj2.pro
fixup Makefile-linux-shared
export QMAKESPEC="solaris-g++"
qmake -unix -o Makefile-solaris-static "CONFIG+=libecwj2 staticlib" libecwj2.pro
fixup Makefile-solaris-static
qmake -unix -o Makefile-solaris-shared "CONFIG+=libecwj2 dll" libecwj2.pro
fixup Makefile-solaris-shared

# export QMAKESPEC=hpux-acc
# qmake -unix -o Makefile-hpux-static libecwj2.pro "CONFIG+=libecwj2 staticlib"
# fixup Makefile-hpux-static
# qmake -unix -o Makefile-hpux-shared libecwj2.pro "CONFIG+=libecwj2 dll"
# fixup Makefile-hpux-shared
# Mac OS X XCode files must be built on the mac itself
# export QMAKESPEC=macx-g++
# qmake -macx -o Makefile-macx-static libecwj2.pro "CONFIG+=libecwj2 staticlib"
# fixup Makefile-macx-static
# qmake -macx -o Makefile-macx-shared libecwj2.pro "CONFIG+=libecwj2 dll"
# fixup Makefile-macx-shared

# All of the example builds
echo "creating build files for sample code ..."
export QMAKESPEC="win32-msvc"
qmake -win32 -tp vc -o $DDIR/example1/DExample1-static.dsp "CONFIG+=dexample1 libecwj2 staticlink" examples.pro
qmake -win32 -tp vc -o $DDIR/example2/DExample2-static.dsp "CONFIG+=dexample2 libecwj2 staticlink" examples.pro
qmake -win32 -tp vc -o $DDIR/example4/DExample4-static.dsp "CONFIG+=dexample4 libecwj2 staticlink" examples.pro
qmake -win32 -tp vc -o $CDIR/example1/CExample1-static.dsp "CONFIG+=cexample1 libecwj2 staticlink" examples.pro
qmake -win32 -tp vc -o $CDIR/example2/CExample2-static.dsp "CONFIG+=cexample2 libecwj2 staticlink" examples.pro
qmake -win32 -tp vc -o $CDIR/example3/CExample3-static.dsp "CONFIG+=cexample3 libecwj2 staticlink" examples.pro
qmake -win32 -tp vc -o $CDIR/example4/CExample4-static.dsp "CONFIG+=cexample4 libecwj2 staticlink" examples.pro
qmake -win32 -tp vc -o $CDIR/example5/CExample5-static.dsp "CONFIG+=cexample5 libecwj2 staticlink" examples.pro

qmake -win32 -tp vc -o $DDIR/example1/DExample1-shared.dsp "CONFIG+=dexample1 libecwj2 sharedlink" examples.pro
qmake -win32 -tp vc -o $DDIR/example2/DExample2-shared.dsp "CONFIG+=dexample2 libecwj2 sharedlink" examples.pro
qmake -win32 -tp vc -o $DDIR/example4/DExample4-shared.dsp "CONFIG+=dexample4 libecwj2 sharedlink" examples.pro
qmake -win32 -tp vc -o $CDIR/example1/CExample1-shared.dsp "CONFIG+=cexample1 libecwj2 sharedlink" examples.pro
qmake -win32 -tp vc -o $CDIR/example2/CExample2-shared.dsp "CONFIG+=cexample2 libecwj2 sharedlink" examples.pro
qmake -win32 -tp vc -o $CDIR/example3/CExample3-shared.dsp "CONFIG+=cexample3 libecwj2 sharedlink" examples.pro
qmake -win32 -tp vc -o $CDIR/example4/CExample4-shared.dsp "CONFIG+=cexample4 libecwj2 sharedlink" examples.pro
qmake -win32 -tp vc -o $CDIR/example5/CExample5-shared.dsp "CONFIG+=cexample5 libecwj2 sharedlink" examples.pro

export QMAKESPEC="linux-g++"
qmake -unix -o $DDIR/example1/Makefile-linux-static "CONFIG+=dexample1 libecwj2 staticlink" examples.pro
fixup $DDIR/example1/Makefile-linux-static
qmake -unix -o $DDIR/example2/Makefile-linux-static "CONFIG+=dexample2 libecwj2 staticlink" examples.pro
fixup $DDIR/example2/Makefile-linux-static
qmake -unix -o $DDIR/example4/Makefile-linux-static "CONFIG+=dexample4 libecwj2 staticlink" examples.pro
fixup $DDIR/example4/Makefile-linux-static
qmake -unix -o $CDIR/example1/Makefile-linux-static "CONFIG+=cexample1 libecwj2 staticlink" examples.pro
fixup $CDIR/example1/Makefile-linux-static
qmake -unix -o $CDIR/example2/Makefile-linux-static "CONFIG+=cexample2 libecwj2 staticlink" examples.pro
fixup $CDIR/example2/Makefile-linux-static
qmake -unix -o $CDIR/example3/Makefile-linux-static "CONFIG+=cexample3 libecwj2 staticlink" examples.pro
fixup $CDIR/example3/Makefile-linux-static
qmake -unix -o $CDIR/example5/Makefile-linux-static "CONFIG+=cexample5 libecwj2 staticlink" examples.pro
fixup $CDIR/example5/Makefile-linux-static

qmake -unix -o $DDIR/example1/Makefile-linux-shared "CONFIG+=dexample1 libecwj2 sharedlink" examples.pro
fixup $DDIR/example1/Makefile-linux-shared
qmake -unix -o $DDIR/example2/Makefile-linux-shared "CONFIG+=dexample2 libecwj2 sharedlink" examples.pro
fixup $DDIR/example2/Makefile-linux-shared
qmake -unix -o $DDIR/example4/Makefile-linux-shared "CONFIG+=dexample4 libecwj2 sharedlink" examples.pro
fixup $DDIR/example4/Makefile-linux-shared
qmake -unix -o $CDIR/example1/Makefile-linux-shared "CONFIG+=cexample1 libecwj2 sharedlink" examples.pro
fixup $CDIR/example1/Makefile-linux-shared
qmake -unix -o $CDIR/example2/Makefile-linux-shared "CONFIG+=cexample2 libecwj2 sharedlink" examples.pro
fixup $CDIR/example2/Makefile-linux-shared
qmake -unix -o $CDIR/example3/Makefile-linux-shared "CONFIG+=cexample3 libecwj2 sharedlink" examples.pro
fixup $CDIR/example3/Makefile-linux-shared
qmake -unix -o $CDIR/example5/Makefile-linux-shared "CONFIG+=cexample5 libecwj2 sharedlink" examples.pro
fixup $CDIR/example5/Makefile-linux-shared

export QMAKESPEC="solaris-g++"
qmake -unix -o $DDIR/example1/Makefile-solaris-static "CONFIG+=dexample1 libecwj2 staticlink" examples.pro
fixup $DDIR/example1/Makefile-solaris-static
qmake -unix -o $DDIR/example2/Makefile-solaris-static "CONFIG+=dexample2 libecwj2 staticlink" examples.pro
fixup $DDIR/example2/Makefile-solaris-static
qmake -unix -o $DDIR/example4/Makefile-solaris-static "CONFIG+=dexample4 libecwj2 staticlink" examples.pro
fixup $DDIR/example4/Makefile-solaris-static
qmake -unix -o $CDIR/example1/Makefile-solaris-static "CONFIG+=cexample1 libecwj2 staticlink" examples.pro
fixup $CDIR/example1/Makefile-solaris-static
qmake -unix -o $CDIR/example2/Makefile-solaris-static "CONFIG+=cexample2 libecwj2 staticlink" examples.pro
fixup $CDIR/example2/Makefile-solaris-static
qmake -unix -o $CDIR/example3/Makefile-solaris-static "CONFIG+=cexample3 libecwj2 staticlink" examples.pro
fixup $CDIR/example3/Makefile-solaris-static
qmake -unix -o $CDIR/example5/Makefile-solaris-static "CONFIG+=cexample5 libecwj2 staticlink" examples.pro
fixup $CDIR/example5/Makefile-solaris-static

qmake -unix -o $DDIR/example1/Makefile-solaris-shared "CONFIG+=dexample1 libecwj2 sharedlink" examples.pro
fixup $DDIR/example1/Makefile-solaris-shared
qmake -unix -o $DDIR/example2/Makefile-solaris-shared "CONFIG+=dexample2 libecwj2 sharedlink" examples.pro
fixup $DDIR/example2/Makefile-solaris-shared
qmake -unix -o $DDIR/example4/Makefile-solaris-shared "CONFIG+=dexample4 libecwj2 sharedlink" examples.pro
fixup $DDIR/example4/Makefile-solaris-shared
qmake -unix -o $CDIR/example1/Makefile-solaris-shared "CONFIG+=cexample1 libecwj2 sharedlink" examples.pro
fixup $CDIR/example1/Makefile-solaris-shared
qmake -unix -o $CDIR/example2/Makefile-solaris-shared "CONFIG+=cexample2 libecwj2 sharedlink" examples.pro
fixup $CDIR/example2/Makefile-solaris-shared
qmake -unix -o $CDIR/example3/Makefile-solaris-shared "CONFIG+=cexample3 libecwj2 sharedlink" examples.pro
fixup $CDIR/example3/Makefile-solaris-shared
qmake -unix -o $CDIR/example5/Makefile-solaris-shared "CONFIG+=cexample5 libecwj2 sharedlink" examples.pro
fixup $CDIR/example5/Makefile-solaris-shared

# export QMAKESPEC="hpux-acc"
# qmake -unix -o $DDIR/example1/Makefile-hpux-static "CONFIG+=dexample1 libecwj2 staticlink" examples.pro
# fixup $DDIR/example1/Makefile-hpux-static
# qmake -unix -o $DDIR/example2/Makefile-hpux-static "CONFIG+=dexample2 libecwj2 staticlink" examples.pro
# fixup $DDIR/example2/Makefile-hpux-static
# qmake -unix -o $DDIR/example4/Makefile-hpux-static "CONFIG+=dexample4 libecwj2 staticlink" examples.pro
# fixup $DDIR/example4/Makefile-hpux-static
# qmake -unix -o $CDIR/example1/Makefile-hpux-static "CONFIG+=cexample1 libecwj2 staticlink" examples.pro
# fixup $CDIR/example1/Makefile-hpux-static
# qmake -unix -o $CDIR/example2/Makefile-hpux-static "CONFIG+=cexample2 libecwj2 staticlink" examples.pro
# fixup $CDIR/example2/Makefile-hpux-static
# qmake -unix -o $CDIR/example3/Makefile-hpux-static "CONFIG+=cexample3 libecwj2 staticlink" examples.pro
# fixup $CDIR/example3/Makefile-hpux-static
# qmake -unix -o $CDIR/example5/Makefile-hpux-static "CONFIG+=cexample5 libecwj2 staticlink" examples.pro
# fixup $CDIR/example5/Makefile-hpux-static

# qmake -unix -o $DDIR/example1/Makefile-hpux-shared "CONFIG+=dexample1 libecwj2 sharedlink" examples.pro
# fixup $DDIR/example1/Makefile-hpux-shared
# qmake -unix -o $DDIR/example2/Makefile-hpux-shared "CONFIG+=dexample2 libecwj2 sharedlink" examples.pro
# fixup $DDIR/example2/Makefile-hpux-shared
# qmake -unix -o $DDIR/example4/Makefile-hpux-shared "CONFIG+=dexample4 libecwj2 sharedlink" examples.pro
# fixup $DDIR/example4/Makefile-hpux-shared
# qmake -unix -o $CDIR/example1/Makefile-hpux-shared "CONFIG+=cexample1 libecwj2 sharedlink" examples.pro
# fixup $CDIR/example1/Makefile-hpux-shared
# qmake -unix -o $CDIR/example2/Makefile-hpux-shared "CONFIG+=cexample2 libecwj2 sharedlink" examples.pro
# fixup $CDIR/example2/Makefile-hpux-shared
# qmake -unix -o $CDIR/example3/Makefile-hpux-shared "CONFIG+=cexample3 libecwj2 sharedlink" examples.pro
# fixup $CDIR/example3/Makefile-hpux-shared
# qmake -unix -o $CDIR/example5/Makefile-hpux-shared "CONFIG+=cexample5 libecwj2 sharedlink" examples.pro
# fixup $CDIR/example5/Makefile-hpux-shared

# Mac OS X build files must be created on a Mac
# export QMAKESPEC="macx-pbuilder"
# qmake -unix -o $DDIR/example1/Makefile-macosx-static "CONFIG+=dexample1 libecwj2 staticlink" examples.pro
# fixup $DDIR/example1/Makefile-macosx-static
# qmake -unix -o $DDIR/example2/Makefile-macosx-static "CONFIG+=dexample2 libecwj2 staticlink" examples.pro
# fixup $DDIR/example2/Makefile-macosx-static
# qmake -unix -o $DDIR/example4/Makefile-macosx-static "CONFIG+=dexample4 libecwj2 staticlink" examples.pro
# fixup $DDIR/example4/Makefile-macosx-static
# qmake -unix -o $CDIR/example1/Makefile-macosx-static "CONFIG+=cexample1 libecwj2 staticlink" examples.pro
# fixup $CDIR/example1/Makefile-macosx-static
# qmake -unix -o $CDIR/example2/Makefile-macosx-static "CONFIG+=cexample2 libecwj2 staticlink" examples.pro
# fixup $CDIR/example2/Makefile-macosx-static
# qmake -unix -o $CDIR/example3/Makefile-macosx-static "CONFIG+=cexample3 libecwj2 staticlink" examples.pro
# fixup $CDIR/example3/Makefile-macosx-static
# qmake -unix -o $CDIR/example5/Makefile-macosx-static "CONFIG+=cexample5 libecwj2 staticlink" examples.pro
# fixup $CDIR/example5/Makefile-macosx-static

# qmake -unix -o $DDIR/example1/Makefile-macosx-static "CONFIG+=dexample1 libecwj2 sharedlink" examples.pro
# fixup $DDIR/example1/Makefile-macosx-static
# qmake -unix -o $DDIR/example2/Makefile-macosx-static "CONFIG+=dexample2 libecwj2 sharedlink" examples.pro
# fixup $DDIR/example2/Makefile-macosx-static
# qmake -unix -o $DDIR/example4/Makefile-macosx-static "CONFIG+=dexample4 libecwj2 sharedlink" examples.pro
# fixup $DDIR/example4/Makefile-macosx-static
# qmake -unix -o $CDIR/example1/Makefile-macosx-static "CONFIG+=cexample1 libecwj2 sharedlink" examples.pro
# fixup $CDIR/example1/Makefile-macosx-static
# qmake -unix -o $CDIR/example2/Makefile-macosx-static "CONFIG+=cexample2 libecwj2 sharedlink" examples.pro
# fixup $CDIR/example2/Makefile-macosx-static
# qmake -unix -o $CDIR/example3/Makefile-macosx-static "CONFIG+=cexample3 libecwj2 sharedlink" examples.pro
# fixup $CDIR/example3/Makefile-macosx-static
# qmake -unix -o $CDIR/example5/Makefile-macosx-static "CONFIG+=cexample5 libecwj2 sharedlink" examples.pro
# fixup $CDIR/example5/Makefile-macosx-static

export QMAKESPEC="win32-msvc.net"
qmake -win32 -tp vc -o $DDIR/example1/Dexample1-static.vcproj "CONFIG+=dexample1 libecwj2 staticlink" examples.pro
qmake -win32 -tp vc -o $DDIR/example2/Dexample2-static.vcproj "CONFIG+=dexample2 libecwj2 staticlink" examples.pro
qmake -win32 -tp vc -o $DDIR/example4/Dexample4-static.vcproj "CONFIG+=dexample4 libecwj2 staticlink" examples.pro
qmake -win32 -tp vc -o $CDIR/example1/Cexample1-static.vcproj "CONFIG+=cexample1 libecwj2 staticlink" examples.pro
qmake -win32 -tp vc -o $CDIR/example2/Cexample2-static.vcproj "CONFIG+=cexample2 libecwj2 staticlink" examples.pro
qmake -win32 -tp vc -o $CDIR/example3/Cexample3-static.vcproj "CONFIG+=cexample3 libecwj2 staticlink" examples.pro
qmake -win32 -tp vc -o $CDIR/example4/Cexample4-static.vcproj "CONFIG+=cexample4 libecwj2 staticlink" examples.pro
qmake -win32 -tp vc -o $CDIR/example5/Cexample5-static.vcproj "CONFIG+=cexample5 libecwj2 staticlink" examples.pro

qmake -win32 -tp vc -o $DDIR/example1/Dexample1-shared.vcproj "CONFIG+=dexample1 libecwj2 sharedlink" examples.pro
qmake -win32 -tp vc -o $DDIR/example2/Dexample2-shared.vcproj "CONFIG+=dexample2 libecwj2 sharedlink" examples.pro
qmake -win32 -tp vc -o $DDIR/example4/Dexample4-shared.vcproj "CONFIG+=dexample4 libecwj2 sharedlink" examples.pro
qmake -win32 -tp vc -o $CDIR/example1/Cexample1-shared.vcproj "CONFIG+=cexample1 libecwj2 sharedlink" examples.pro
qmake -win32 -tp vc -o $CDIR/example2/Cexample2-shared.vcproj "CONFIG+=cexample2 libecwj2 sharedlink" examples.pro
qmake -win32 -tp vc -o $CDIR/example3/Cexample3-shared.vcproj "CONFIG+=cexample3 libecwj2 sharedlink" examples.pro
qmake -win32 -tp vc -o $CDIR/example4/Cexample4-shared.vcproj "CONFIG+=cexample4 libecwj2 sharedlink" examples.pro
qmake -win32 -tp vc -o $CDIR/example5/Cexample5-shared.vcproj "CONFIG+=cexample5 libecwj2 sharedlink" examples.pro

export QMAKESPEC=$OLDSPEC
popd

exit
