ifneq (,$(wildcard ${IUEROOT}/v3p/xerces-c-src1_4_0/include/dom/DOM.hpp))
 HAS_XERCES:=1
 include ${IUEROOT}/config/top-params.mk
 IULIBS += -lbxml $(XERCES_LIBS)
 USES += contrib/gel
 include ${IUEROOT}/contrib/brl/bbas/bdgl/config.mk
 include ${IUEROOT}/contrib/gel/vtol/config.mk
 include ${IUEROOT}/contrib/gel/vdgl/config.mk
 include ${IUEROOT}/contrib/gel/vsol/config.mk
 include ${IUEROOT}/core/vbl/config.mk
endif
