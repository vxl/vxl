# Imakefile for netlib library
# This library contains public domain routines from netlib.att.com

USES = 

USE_IUTEMPLATES := 0

LIBRARY := netlib
VERSION := 1.0

#*********************
# Std Fortran Library 
#*********************
SOURCES += F77_aloc.c
SOURCES += exit.c
SOURCES += s_cmp.c
SOURCES += d_lg10.c
SOURCES += s_cat.c
SOURCES += s_copy.c
SOURCES += pow_di.c
SOURCES += d_sign.c
SOURCES += d_cnjg.c
SOURCES += d_imag.c
SOURCES += z_div.c
SOURCES += z_abs.c
SOURCES += cabs.c
SOURCES += sig_die.c
SOURCES += r_sign.c

# BLAS 
SOURCES += daxpy.c
SOURCES += dcopy.c
SOURCES += ddot.c
SOURCES += dnrm2.c
SOURCES += drot.c
SOURCES += drotg.c
SOURCES += dscal.c
SOURCES += dasum.c
SOURCES += dswap.c

SOURCES += saxpy.c
SOURCES += scopy.c
SOURCES += sdot.c
SOURCES += snrm2.c
SOURCES += srot.c
SOURCES += srotg.c
SOURCES += sscal.c
SOURCES += sasum.c
SOURCES += sswap.c

# LINPACK 
SOURCES += dsvdc.c
SOURCES += zsvdc.c
SOURCES += ssvdc.c
SOURCES += dqrdc.c
SOURCES += dqrsl.c
SOURCES += dpofa.c
SOURCES += dpoco.c
SOURCES += dposl.c
SOURCES += dpodi.c

# EISPACK 
SOURCES += rsg.c
SOURCES += rg.c
SOURCES += rs.c
SOURCES += tql1.c
SOURCES += tql2.c
SOURCES += tred1.c
SOURCES += tred2.c

# MINPACK 
SOURCES += lmdif.c
SOURCES += lmder.c
SOURCES += lmder1.c
SOURCES += pythag.c
SOURCES += rpoly.c

# TOMS Misc 
SOURCES += dtrans.c
SOURCES += trans.c

# General 
SOURCES += machineparams.c

# Temperton FFT routines
SOURCES += dgpfa.c  
SOURCES += dgpfa3f.c  
SOURCES += dsetgpfa.c 
SOURCES += gpfa2f.c 
SOURCES += gpfa5f.c
SOURCES += dgpfa2f.c 
SOURCES += dgpfa5f.c 
SOURCES += gpfa.c  
SOURCES += gpfa3f.c 
SOURCES += setgpfa.c
SOURCES += pow_ii.c

##########################################
include $(SYS_IUPACKAGES)/config/top.mk
##########################################

ifneq ($(OS),win32)
  APP_CFLAGS = -O2 
endif
