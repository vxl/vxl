ifndef oxp_client_mk
oxp_client_mk := 1
#
# fsm@robots.ox.ac.uk
#

#FIXME: IUELOCALROOT ?
include $(IUEROOT)/vxl/vcl/client.mk
include $(IUEROOT)/vxl/vbl/client.mk
include $(IUEROOT)/vxl/vnl/client.mk
include $(IUEROOT)/vxl/vil/client.mk

USE_JPEG := 1

ald_libdeps += oxp:vcl,vbl,vnl-algo,vil

endif
