TEST += vcl_cv_cxx_has_bool
TEST += vcl_cv_cxx_has_typename
TEST += vcl_cv_cxx_has_export
TEST += vcl_cv_cxx_has_mutable
TEST += vcl_cv_cxx_has_explicit
TEST += vcl_cv_cxx_has_dynamic_cast
TEST += vcl_cv_cxx_check_for_scope
TEST += vcl_cv_cxx_default_value
TEST += vcl_cv_cxx_has_member_templates
TEST += vcl_cv_cxx_can_do_partial_specialization
TEST += vcl_cv_cxx_define_specialization
TEST += VCL_ALLOWS_INLINE_INSTANTIATION
TEST += VCL_NEEDS_INLINE_INSTANTIATION
TEST += vcl_cv_cxx_static_const_init_int
TEST += VCL_STATIC_CONST_INIT_FLOAT
TEST += VCL_CAN_DO_STATIC_TEMPLATE_MEMBER
TEST += VCL_CAN_DO_NON_TYPE_FUNCTION_TEMPLATE_PARAMETER
TEST += VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD
TEST += VCL_OVERLOAD_CAST
TEST += VCL_NULL_TMPL_ARGS
TEST += VCL_NO_STATIC_DATA_MEMBERS
TEST += VCL_HAS_TEMPLATE_SYMBOLS
TEST += VCL_CAN_DO_IMPLICIT_TEMPLATES
TEST += VCL_CAN_DO_COMPLETE_DEFAULT_TYPE_PARAMETER
TEST += VCL_CAN_DO_TEMPLATE_DEFAULT_TYPE_PARAMETER
TEST += VCL_SUNPRO_CLASS_SCOPE_HACK
TEST += vcl_cv_cxx_has_exceptions
TEST += vcl_cv_cxx_has_namespaces
TEST += vcl_cv_cxx_allows_namespace_std
TEST += vcl_cv_cxx_needs_namespace_std
TEST += VXL_UNISTD_USLEEP_IS_VOID
TEST += ac_vxl_has_qsort
TEST += VXL_MATH_HAS_FINITE
TEST += VXL_MATH_HAS_SQRTF
TEST += VXL_IEEEFP_HAS_FINITE
TEST += VXL_STDLIB_HAS_LRAND48
TEST += VXL_STDLIB_HAS_DRAND48

#######################################
include $(IUEROOT)/config/top-params.mk
#######################################

$(RELOBJDIR)$/%.exe : vxl_platform_tests.cxx $(RELOBJDIR)$/vcl_config_compiler.h
	@$(MKDIR_P) $(@D)
	-t=0; $(C++) $(force_c++) $(DOS_FULL_SRC_PATH)vxl_platform_tests.cxx \
	  $(no_implicit_templates) $(tj_CCFLAGS) $(extraflags_for_object) \
	  -D$(patsubst %.exe,%,$(@F)) -c -o $(patsubst %.exe,%.$(OBJ_EXT),$@) && \
	 $(MAKE_EXE) $(link_op) $(patsubst %.exe,%.$(OBJ_EXT),$@) $(tj_LDFLAGS) \
	  $(LDPATH) $(STDLIBS) $(LDOPTS) -o $@ && t=1; \
	 perl -pi -e 'm/^$(patsubst %.exe,%,$(@F))\=/ && s/\d*$$/'$$t/\; $(RELOBJDIR)$/vcl_config_compiler.h

xall:: $(patsubst %,$(RELOBJDIR)$/%.exe,$(TEST))

$(RELOBJDIR)$/vcl_config_compiler.h :
	echo "$(TEST) " | perl -p -e 's/\s+/=\n/g;' > $@

######################################
include $(IUEROOT)/config/top-rules.mk
######################################
