ifndef vbl_client_mk
vbl_client_mk := 1
#
# fsm@robots.ox.ac.uk
#

#FIXME: IUELOCALROOT ?
include $(IUEROOT)/vxl/vcl/client.mk

ald_libdeps += vbl:vpl,vcl

endif
