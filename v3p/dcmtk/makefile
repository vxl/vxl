USES = # nothing
USE_STDC++ := 0

LIBRARY := dcmtk
VERSION := 1.0

INCDIRS += ./ofstd/include ./dcmdata/include ./dcmimgle/include

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

# This is where configure will put osconfig.h
INCDIRS += $(IUEROOT_libbase)/v3p/$(tj_libdir)

SOURCES += ofstd/libsrc/ofcond.cxx
SOURCES += ofstd/libsrc/ofconsol.cxx
SOURCES += ofstd/libsrc/ofcrc32.cxx
SOURCES += ofstd/libsrc/ofdate.cxx
SOURCES += ofstd/libsrc/ofdatime.cxx
SOURCES += ofstd/libsrc/oflist.cxx
SOURCES += ofstd/libsrc/ofstd.cxx
SOURCES += ofstd/libsrc/ofstring.cxx
SOURCES += ofstd/libsrc/ofthread.cxx
SOURCES += ofstd/libsrc/oftime.cxx
SOURCES += dcmdata/libsrc/dcbytstr.cxx
SOURCES += dcmdata/libsrc/dcchrstr.cxx
SOURCES += dcmdata/libsrc/dccodec.cxx
SOURCES += dcmdata/libsrc/dcdatset.cxx
SOURCES += dcmdata/libsrc/dcdicent.cxx
SOURCES += dcmdata/libsrc/dcdict.cxx
SOURCES += dcmdata/libsrc/dcdictzz.cxx
SOURCES += dcmdata/libsrc/dcdirrec.cxx
SOURCES += dcmdata/libsrc/dcelem.cxx
SOURCES += dcmdata/libsrc/dcerror.cxx
SOURCES += dcmdata/libsrc/dcfilefo.cxx
SOURCES += dcmdata/libsrc/dchashdi.cxx
SOURCES += dcmdata/libsrc/dcistrma.cxx
SOURCES += dcmdata/libsrc/dcistrmf.cxx
SOURCES += dcmdata/libsrc/dcitem.cxx
SOURCES += dcmdata/libsrc/dclist.cxx
SOURCES += dcmdata/libsrc/dcmetinf.cxx
SOURCES += dcmdata/libsrc/dcobject.cxx
SOURCES += dcmdata/libsrc/dcostrma.cxx
SOURCES += dcmdata/libsrc/dcostrmf.cxx
SOURCES += dcmdata/libsrc/dcpcache.cxx
SOURCES += dcmdata/libsrc/dcpixel.cxx
SOURCES += dcmdata/libsrc/dcpixseq.cxx
SOURCES += dcmdata/libsrc/dcpxitem.cxx
SOURCES += dcmdata/libsrc/dcsequen.cxx
SOURCES += dcmdata/libsrc/dcstack.cxx
SOURCES += dcmdata/libsrc/dcswap.cxx
SOURCES += dcmdata/libsrc/dctag.cxx
SOURCES += dcmdata/libsrc/dctagkey.cxx
SOURCES += dcmdata/libsrc/dctypes.cxx
SOURCES += dcmdata/libsrc/dcuid.cxx
SOURCES += dcmdata/libsrc/dcvm.cxx
SOURCES += dcmdata/libsrc/dcvr.cxx
SOURCES += dcmdata/libsrc/dcvrae.cxx
SOURCES += dcmdata/libsrc/dcvras.cxx
SOURCES += dcmdata/libsrc/dcvrat.cxx
SOURCES += dcmdata/libsrc/dcvrcs.cxx
SOURCES += dcmdata/libsrc/dcvrda.cxx
SOURCES += dcmdata/libsrc/dcvrds.cxx
SOURCES += dcmdata/libsrc/dcvrdt.cxx
SOURCES += dcmdata/libsrc/dcvrfd.cxx
SOURCES += dcmdata/libsrc/dcvrfl.cxx
SOURCES += dcmdata/libsrc/dcvris.cxx
SOURCES += dcmdata/libsrc/dcvrlo.cxx
SOURCES += dcmdata/libsrc/dcvrlt.cxx
SOURCES += dcmdata/libsrc/dcvrobow.cxx
SOURCES += dcmdata/libsrc/dcvrof.cxx
SOURCES += dcmdata/libsrc/dcvrpn.cxx
SOURCES += dcmdata/libsrc/dcvrpobw.cxx
SOURCES += dcmdata/libsrc/dcvrsh.cxx
SOURCES += dcmdata/libsrc/dcvrsl.cxx
SOURCES += dcmdata/libsrc/dcvrss.cxx
SOURCES += dcmdata/libsrc/dcvrst.cxx
SOURCES += dcmdata/libsrc/dcvrtm.cxx
SOURCES += dcmdata/libsrc/dcvrui.cxx
SOURCES += dcmdata/libsrc/dcvrul.cxx
SOURCES += dcmdata/libsrc/dcvrulup.cxx
SOURCES += dcmdata/libsrc/dcvrus.cxx
SOURCES += dcmdata/libsrc/dcvrut.cxx
SOURCES += dcmdata/libsrc/dcxfer.cxx
SOURCES += dcmdata/libsrc/dcdebug.cxx
SOURCES += dcmdata/libsrc/dcistrmz.cxx
SOURCES += dcmdata/libsrc/dcostrmz.cxx
SOURCES += dcmimgle/libsrc/dcmimage.cxx
SOURCES += dcmimgle/libsrc/dibaslut.cxx
SOURCES += dcmimgle/libsrc/didislut.cxx
SOURCES += dcmimgle/libsrc/didispfn.cxx
SOURCES += dcmimgle/libsrc/didocu.cxx
SOURCES += dcmimgle/libsrc/digsdfn.cxx
SOURCES += dcmimgle/libsrc/digsdlut.cxx
SOURCES += dcmimgle/libsrc/diimage.cxx
SOURCES += dcmimgle/libsrc/diinpx.cxx
SOURCES += dcmimgle/libsrc/diluptab.cxx
SOURCES += dcmimgle/libsrc/dimo1img.cxx
SOURCES += dcmimgle/libsrc/dimo2img.cxx
SOURCES += dcmimgle/libsrc/dimoimg.cxx
SOURCES += dcmimgle/libsrc/dimoimg3.cxx
SOURCES += dcmimgle/libsrc/dimoimg4.cxx
SOURCES += dcmimgle/libsrc/dimoimg5.cxx
SOURCES += dcmimgle/libsrc/dimomod.cxx
SOURCES += dcmimgle/libsrc/dimoopx.cxx
SOURCES += dcmimgle/libsrc/dimopx.cxx
SOURCES += dcmimgle/libsrc/diovdat.cxx
SOURCES += dcmimgle/libsrc/diovlay.cxx
SOURCES += dcmimgle/libsrc/diovlimg.cxx
SOURCES += dcmimgle/libsrc/diovpln.cxx
SOURCES += dcmimgle/libsrc/diutils.cxx

# configure stuff:
xall:: $(IUEROOT_libbase)/v3p/$(tj_libdir)/osconfig.h
$(IUEROOT_libbase)/v3p/$(tj_libdir)/osconfig.h: config/include/osconfig_cmake.h.in
	$(MKDIR_P) config/include; PwD=`pwd`; perl -p -e \
	's/\#cmake/\#/;s/define (DCMTK_HAVE_((IO|UNIX|LIBC)_H|STRL))/undef $$1/;s/\@DCMTK_SIZEOF_LONG\@/8/;s!\@DCMTK_PATH_SEPARATOR\@!/!;s/\@DCMTK_ENVIRONMENT_PATH_SEPARATOR\@/:/;s!\@DCMTK_DCM_DICT_DEFAULT_PATH\@!'$$PwD'/dcmdata/libsrc/dicom.dic!;'\
	$^ > $@

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

# Always optimize this lib
debug = $(optimize)
