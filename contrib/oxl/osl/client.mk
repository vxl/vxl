ifndef osl_client_mk
osl_client_mk := 1
#
# fsm@robots.ox.ac.uk
#

include $(IUELOCALROOT)/vxl/vbl/client.mk
include $(IUELOCALROOT)/vxl/vnl/client.mk
include $(IUELOCALROOT)/vxl/vil/client.mk
include $(IUELOCALROOT)/vxl/vgl/client.mk

ald_libdeps += osl:vil,vbl,vgl,vnl-algo

endif
