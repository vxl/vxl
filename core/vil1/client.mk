ifndef vil_client_mk
vil_client_mk := 1
#
# fsm@robots.ox.ac.uk
#

include $(IUELOCALROOT)/vxl/vcl/client.mk

TRY_TIFF := 1
TRY_JPEG := 1
TRY_PNG  := 1

ald_libdeps += vil:vcl

endif
