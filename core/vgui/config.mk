TRY_GLUT := 1
TRY_OPENGL := 1
TRY_X11 := 1
IULIBS += -lvgui -lQv
include ${IUEROOT}/oxl/vgui/impl/config.mk
IULIBS += -lvgui
include ${IUEROOT}/vxl/vnl/algo/config.mk
include ${IUEROOT}/vxl/vil/config.mk
include ${IUEROOT}/vxl/vgl/config.mk
include ${IUEROOT}/vxl/vpl/config.mk
include ${IUEROOT}/vxl/vbl/config.mk
include ${IUEROOT}/vxl/vul/config.mk
