IULIBS += -lvidl
include ${IUEROOT}/vxl/vul/config.mk
include ${IUEROOT}/vxl/vil/config.mk
TRY_MPEG2 := 1
include ${IUEROOT}/config/try.mk
ifeq ($(HAS_MPEG2),1)
IULIBS += $(MPEG2_LIB_DIR)
endif
