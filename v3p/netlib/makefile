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
SOURCES += d_sign.c
SOURCES += d_cnjg.c
SOURCES += d_imag.c
SOURCES += z_div.c
SOURCES += z_abs.c
SOURCES += cabs.c
SOURCES += sig_die.c
SOURCES += r_sign.c

SOURCES += pow_ii.c
SOURCES += pow_ri.c
SOURCES += pow_di.c

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
SOURCES += dgemv.c
SOURCES += dger.c

SOURCES += saxpy.c
SOURCES += scopy.c
SOURCES += sdot.c
SOURCES += snrm2.c
SOURCES += srot.c
SOURCES += srotg.c
SOURCES += sscal.c
SOURCES += sasum.c
SOURCES += sswap.c
SOURCES += sgemv.c
SOURCES += sger.c

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
SOURCES += isamax.c
SOURCES += idamax.c

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

# from DATAPAC
SOURCES += camsun.c

# from LASO2
SOURCES += dnlaso.c

# from LAPACK
SOURCES += xerbla.c
SOURCES += lsame.c
SOURCES += dlamch.c
SOURCES += slamch.c

SOURCES += dgeqpf.c
SOURCES += dgeqr2.c
SOURCES += dgerq2.c
SOURCES += dggsvd.c
SOURCES += dggsvp.c
SOURCES += dlacpy.c
SOURCES += dlags2.c
SOURCES += dlange.c
SOURCES += dlapll.c
SOURCES += dlapmt.c
SOURCES += dlapy2.c
SOURCES += dlarf.c
SOURCES += dlarfg.c
SOURCES += dlartg.c
SOURCES += dlas2.c
SOURCES += dlaset.c
SOURCES += dlassq.c
SOURCES += dlasv2.c
SOURCES += dorg2r.c
SOURCES += dorm2r.c
SOURCES += dormr2.c
SOURCES += dtgsja.c
SOURCES += sggsvd.c
SOURCES += sgeqpf.c
SOURCES += sgeqr2.c
SOURCES += sgerq2.c
SOURCES += sggsvp.c
SOURCES += slacpy.c
SOURCES += slags2.c
SOURCES += slange.c
SOURCES += slapll.c
SOURCES += slapmt.c
SOURCES += slapy2.c
SOURCES += slarf.c
SOURCES += slarfg.c
SOURCES += slartg.c
SOURCES += slas2.c
SOURCES += slaset.c
SOURCES += slassq.c
SOURCES += slasv2.c
SOURCES += sorg2r.c
SOURCES += sorm2r.c
SOURCES += sormr2.c
SOURCES += stgsja.c

ifneq ($(COMPILER)$(IU_COMPILER),)
  NODEPENDS=1
endif

##########################################
include $(SYS_IUPACKAGES)/config/top.mk
##########################################

ifeq ($(OS),hpux10.20)
  FLAGS_gpfa5f.c = -O0
  FLAGS_dgpfa5f.c = -O0
endif

ifneq ($(OS),win32)
  debug := 
  APP_CFLAGS = $(optimize)
endif

ifeq ($(OS),solaris)
  pic := $(PIC)
endif
