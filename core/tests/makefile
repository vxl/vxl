USES += core
include ${IUEROOT}/core/testlib/config.mk

# This is where configure will put test_build_info.h
INCDIRS += $(RELOBJDIR)

TESTMAIN = test_driver.cxx
TESTS += test_config.cxx
TESTS += test_build_info.cxx
MINI_PROG_SOURCES += vxl_echo.cxx
MANPAGE_SOURCES += test_build_info.h.in

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################

# configure stuff:
$(RELOBJDIR)/test_build_info.$(OBJ_EXT): $(RELOBJDIR)/test_build_info.h
$(RELOBJDIR)/test_build_info.h: test_build_info.h.in
	mkdir -p $(RELOBJDIR)/core $(RELOBJDIR)/v3p $(RELOBJDIR)/Testing/HTML/TestingResults/Sites/site.net/$(OS)-$(COMPILER)
	sed -e 's+@vxl_BINARY_DIR@+$(RELOBJDIR)+' -e 's+@BUILDNAME@+$(OS)-$(COMPILER)+' -e 's+@SITE@+site.net+' $^ > $@
	ln -s $(IUELOCALROOT)/vcl/config.$(OS)-$(COMPILER)/*.h $(RELOBJDIR)/core/
	ln -s $(IUELOCALROOT)/vcl/config.$(OS)-$(COMPILER) $(RELOBJDIR)/vcl
	ln -s $(IUELOCALROOT)/v3p/mpeg2/$(RELOBJDIR) $(RELOBJDIR)/v3p/mpeg2
	ln -s $(IUELOCALROOT)/v3p/dcmtk/$(RELOBJDIR)/include $(RELOBJDIR)/v3p/dcmtk
	ln -s $(IUELOCALROOT)/core/vnl $(RELOBJDIR)/core/vnl
	ln -s $(IUELOCALROOT)/core/vil/$(RELOBJDIR)/include/vil $(RELOBJDIR)/core/vil
	touch $(RELOBJDIR)/Note.txt $(RELOBJDIR)/CMakeCache.txt $(RELOBJDIR)/CMakeError.log

