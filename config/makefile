all: README.vars
	date
	@echo "Use  make manpages   to build iu_class_table"
	@echo "Use  make link_info  to build library_table and link_dependencies"

mkfiles = rules.mk tpp-rules.mk params.mk tpp-params.mk
README.vars: $(mkfiles) doc.pl
	perl doc.pl $(mkfiles) > $@

##########################################
include ${IUEROOT}/config/top-params.mk
##########################################

library_table :
	$(sys_or_iu_Scripts)/Shell/library-extract

link_dependencies :
	$(sys_or_iu_Scripts)/Shell/lib-dependencies-extract

link_info : library_table link_dependencies

ifneq ($(OS),win32)
class_table:

iu_class_table: class_table
	cd $(IUELOCALROOT) && perl $(sys_or_iu_Scripts)/Perl/generate_class_table -s config/iu_class_table

rh_classes: class_table
	cd $(IUELOCALROOT) && perl $(sys_or_iu_Scripts)/Perl/generate_class_table -s -new config/rh_classes

manpages: iu_class_table rh_classes
	cd $(IUELOCALROOT) && $(sys_or_iu_Scripts)/Shell/tj-make-etags

endif

##########################################
include ${IUEROOT}/config/top-rules.mk
##########################################
