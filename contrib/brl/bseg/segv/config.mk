IULIBS += -lsegv
USES += contrib/gel
include ${IUEROOT}/contrib/brl/bseg/sdet/config.mk
include ${IUEROOT}/contrib/brl/bseg/brip/config.mk
include ${IUEROOT}/contrib/brl/bseg/strk/config.mk
include ${IUEROOT}/contrib/brl/bbas/bgui/config.mk
include ${IUEROOT}/contrib/brl/bbas/bsol/config.mk
ifeq "$(HAS_XERCES)" "1"
 include ${IUEROOT}/contrib/brl/bbas/bxml/config.mk
endif
include ${IUEROOT}/contrib/gel/vtol/config.mk
include ${IUEROOT}/contrib/gel/vdgl/config.mk
include ${IUEROOT}/core/vil1/config.mk
include ${IUEROOT}/core/vbl/config.mk
