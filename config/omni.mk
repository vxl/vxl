# This makefile is used in params.mk to extract make information
# from OMNI_ORB makefiles which are needed to build with OMNI_ORB

-include $(OMNI_DIR)/config/config.mk

TOP = $(OMNI_DIR)
THIS_IMPORT_TREE = $(OMNI_DIR)
-include $(OMNI_DIR)/mk/platforms/$(platform).mk
echovar-%:
	@echo $($(@:echovar-%=%))
