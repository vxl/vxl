ifndef vsl_client_mk
vsl_client_mk := 1
#
# fsm@robots.ox.ac.uk
#

#FIXME: IUELOCALROOT?
include $(IUEROOT)/vxl/vcl/client.mk
include $(IUEROOT)/vxl/vbl/client.mk
include $(IUEROOT)/vxl/vnl/client.mk
include $(IUEROOT)/vxl/vil/client.mk

ald_libdeps += vsl:vcl,vil,vbl,vnl-algo

endif
