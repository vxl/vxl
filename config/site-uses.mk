#ifdef OXFORDVERSION
#ifndef LEUVENVERSION
#ifdef USE_HORATIO
## Horatio for Oxford.

## MUST evaluate $(shell arch) only once.
## Of the 5 seconds that a do-nothing "make" used to take,
## 4.3 were spent evalling the $(shell arch) in LIBDIRS.
##OXFORD_ARCH := $(shell arch -k)
##LIBDIRS += /data/lav/horatio/lib-$(OXFORD_ARCH)
##STDLIBS += # -lhoratio -lXt -lmit -liff
#endif
#endif
#endif
