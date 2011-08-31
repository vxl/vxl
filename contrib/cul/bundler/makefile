USES += contrib/gel contrib/rpl contrib/brl/bseg core

LIBRARY := bundler
VERSION := 1.0

SOURCES += bundler.cxx
SOURCES += bundler_tracks_impl.cxx
SOURCES += bundler_sfm_impl.cxx
SOURCES += bundler_utils.cxx
MANPAGE_SOURCES += bundler_inters.h
MANPAGE_SOURCES += bundler_sfm.h
MANPAGE_SOURCES += bundler_settings.h
MANPAGE_SOURCES += bundler_tracks.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
