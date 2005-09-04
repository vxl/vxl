USES += core
include ${VXLROOT}/core/testlib/config.mk

# This is where configure will put test_build_info.h
INCDIRS += $(RELOBJDIR)

TESTMAIN = test_driver.cxx
TESTS += test_config.cxx
TESTS += test_build_info.cxx
MINI_PROG_SOURCES += vxl_echo.cxx
MANPAGE_SOURCES += test_build_info.h.in

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################

# configure stuff:
$(RELOBJDIR)/test_build_info.$(OBJ_EXT): $(RELOBJDIR)/test_build_info.h
$(RELOBJDIR)/test_build_info.h: test_build_info.h.in
	mkdir -p $(RELOBJDIR)/core $(RELOBJDIR)/v3p $(RELOBJDIR)/Testing/HTML/TestingResults/Sites/site.net/$(OS)-$(COMPILER)
	sed -e 's+@vxl_BINARY_DIR@+$(RELOBJDIR)+' -e 's+@BUILDNAME@+$(OS)-$(COMPILER)+' -e 's+@SITE@+site.net+' $^ > $@
	ln -s $(VXLROOT)/vcl/config.$(OS)-$(COMPILER)/*.h $(RELOBJDIR)/core/
	ln -s $(VXLROOT)/vcl/config.$(OS)-$(COMPILER) $(RELOBJDIR)/vcl
	ln -s $(VXLROOT)/v3p/mpeg2/$(RELOBJDIR) $(RELOBJDIR)/v3p/mpeg2
	ln -s $(VXLROOT)/v3p/dcmtk/$(RELOBJDIR)/include $(RELOBJDIR)/v3p/dcmtk
	ln -s $(VXLROOT)/core/vnl $(RELOBJDIR)/core/vnl
	ln -s $(VXLROOT)/core/vil/$(RELOBJDIR)/include/vil $(RELOBJDIR)/core/vil
	touch $(RELOBJDIR)/Note.txt $(RELOBJDIR)/CMakeCache.txt $(RELOBJDIR)/CMakeError.log

