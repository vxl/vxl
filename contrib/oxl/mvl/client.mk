ifndef mvl_client_mk
mvl_client_mk := 1
#
# fsm@robots.ox.ac.uk
#

#FIXME: IUELOCALROOT ?
include $(IUEROOT)/vxl/vcl/client.mk
include $(IUEROOT)/vxl/vbl/client.mk
include $(IUEROOT)/vxl/vnl/client.mk
include $(IUEROOT)/vxl/vil/client.mk
include $(IUEROOT)/vxl/vgl/client.mk

ald_libdeps += mvl:vcl,vbl,vnl-algo,vil,vgl

endif
