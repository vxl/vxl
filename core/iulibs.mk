IUE_PACKAGE_LIBRARIES += -lvsl

IUE_PACKAGE_LIBRARIES += -lvgl-algo
IUE_PACKAGE_LIBRARIES += -lvgl

#IUE_PACKAGE_LIBRARIES += -lvil-algo
IUE_PACKAGE_LIBRARIES += -lvil
LIBDIRS += $(IUELOCALROOT_libbase)/v3p/tiff/$(tj_libdir) # for libTIFF.so
LIBDIRS += $(IUELOCALROOT_libbase)/v3p/jpeg/$(tj_libdir) # for libJPEG.so

IUE_PACKAGE_LIBRARIES += -lvnl-algo
IUE_PACKAGE_LIBRARIES += -lnetlib
LIBDIRS += $(IUELOCALROOT_libbase)/v3p/netlib/$(tj_libdir) # for libnetlib.so

IUE_PACKAGE_LIBRARIES += -lvnl

IUE_PACKAGE_LIBRARIES += -lvbl

IUE_PACKAGE_LIBRARIES += -lval

IUE_PACKAGE_LIBRARIES += -lvcl
