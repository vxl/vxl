IUE_PACKAGE_LIBRARIES += -lvsl

IUE_PACKAGE_LIBRARIES += -lvgl-algo
IUE_PACKAGE_LIBRARIES += -lvgl

#IUE_PACKAGE_LIBRARIES += -lvil-algo
IUE_PACKAGE_LIBRARIES += -lvil

# For libTIFF.so, and libJPEG.so we may need these in the future.
# At present they serve no purpose and confuse the SGI linker.
# fsm
# LIBDIRS += $(wildcard $(IUELOCALROOT_libbase)/v3p/tiff/$(tj_libdir))
# LIBDIRS += $(wildcard $(IUELOCALROOT_libbase)/v3p/jpeg/$(tj_libdir))
# LIBDIRS += $(wildcard $(IUELOCALROOT_libbase)/v3p/netlib/$(tj_libdir))

IUE_PACKAGE_LIBRARIES += -lvnl-algo

# A library path is needed here.
# LIBDIRS += $(IUELOCALROOT_libbase)/v3p/netlib/$(tj_libdir)
LIBDIRS += $(IUELOCALROOT_libbase)/Numerics/$(tj_libdir)
IUE_PACKAGE_LIBRARIES += -lnetlib

IUE_PACKAGE_LIBRARIES += -lvnl

IUE_PACKAGE_LIBRARIES += -lvbl

# There is currently nothing in libval
#IUE_PACKAGE_LIBRARIES += -lval

IUE_PACKAGE_LIBRARIES += -lvcl
