ifndef oxp_client_mk
oxp_client_mk := 1
#
# fsm@robots.ox.ac.uk
#

include $(IUELOCALROOT)/vxl/vcl/client.mk
include $(IUELOCALROOT)/vxl/vbl/client.mk
include $(IUELOCALROOT)/vxl/vnl/client.mk
include $(IUELOCALROOT)/vxl/vil/client.mk

TRY_JPEG := 1

ald_libdeps += oxp:vcl,vbl,vnl-algo,vil

endif
