ifndef vgl_client_mk
vgl_client_mk := 1
#
# fsm@robots.ox.ac.uk
#

#FIXME: IUELOCALROOT ?
include $(IUEROOT)/vxl/vcl/client.mk

ald_libdeps += vgl:vcl

endif
