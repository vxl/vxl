ifndef vgui_client_mk
vgui_client_mk := 1
#
# fsm@robots.ox.ac.uk
#

USE_OPENGL := 1
TRY_GLUT := 1
TRY_HERMES := 1

include $(IUELOCALROOT)/vxl/vbl/client.mk
include $(IUELOCALROOT)/vxl/vul/client.mk
include $(IUELOCALROOT)/vxl/vnl/client.mk
include $(IUELOCALROOT)/vxl/vil/client.mk
include $(IUELOCALROOT)/vxl/vgl/client.mk

ald_libdeps += vgui:vpl,vbl,vul,vnl-algo,vil,vgl,Qv

endif
