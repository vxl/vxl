ifdef HAS_XERCES
IULIBS += -lbxml
include ../bdgl/config.mk
include ${IUEROOT}/contrib/gel/vtol/config.mk
include ${IUEROOT}/contrib/gel/vdgl/config.mk
include ${IUEROOT}/contrib/gel/vsol/config.mk
include ${IUEROOT}/core/vul/config.mk
include ${IUEROOT}/core/vbl/config.mk
endif
