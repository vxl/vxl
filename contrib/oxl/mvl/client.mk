ifndef mvl_client_mk
mvl_client_mk := 1
#
# fsm@robots.ox.ac.uk
#

include $(IUELOCALROOT)/vxl/vcl/client.mk
include $(IUELOCALROOT)/vxl/vbl/client.mk
include $(IUELOCALROOT)/vxl/vnl/client.mk
include $(IUELOCALROOT)/vxl/vil/client.mk
include $(IUELOCALROOT)/vxl/vgl/client.mk

ald_libdeps += mvl:vcl,vbl,vnl-algo,vil,vgl

endif
