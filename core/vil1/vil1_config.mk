
# fsm
HAS_JPEG := 1
DEFINES += -DHAS_JPEG

ifeq ($(HAS_JPEG),1)
LIBDIRS += $(IUELOCALROOT_libbase)/Image/$(tj_libdir) # for JPEG
IULIBS += -lJPEG
endif

TRY_PNG := 1
