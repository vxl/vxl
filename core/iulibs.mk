IUE_PACKAGE_LIBRARIES += -lvsl

IUE_PACKAGE_LIBRARIES += -lvgl-algo -lvgl

#IUE_PACKAGE_LIBRARIES += -lvil-algo
IUE_PACKAGE_LIBRARIES += -lvil

# For libTIFF.so libJPEG.so and libnetlib.so
LIBDIRS += $(wildcard $(IUELOCALROOT_libbase)/v3p/$(tj_libdir))

IUE_PACKAGE_LIBRARIES += -lvnl-algo -lvnl -lnetlib

IUE_PACKAGE_LIBRARIES += -lvbl

# There is currently nothing in libval
#IUE_PACKAGE_LIBRARIES += -lval

IUE_PACKAGE_LIBRARIES += -lvcl
