IUE_PACKAGE_LIBRARIES += -lvgl-algo -lvgl

#IUE_PACKAGE_LIBRARIES += -lvil-algo
IUE_PACKAGE_LIBRARIES += -lvil

# For libtiff.so libjpeg.so and libnetlib.so
# This is wrong if there are native versions of tiff and jpeg.
LIBDIRS += $(wildcard $(IUELOCALROOT_libbase)/v3p/$(tj_libdir))

IUE_PACKAGE_LIBRARIES += -lvnl-algo -lvnl -lnetlib

IUE_PACKAGE_LIBRARIES += -lvbl

# This is added by the make configuration files
#IUE_PACKAGE_LIBRARIES += -lvcl
