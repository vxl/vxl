#
# fsm@robots.ox.ac.uk
#

# FIXME
fmk_libdeps += pkg_vxl:pkg_vcl

fmk_libdeps += lib_vpl:lib_vcl
fmk_libdeps += lib_vbl:lib_vcl
fmk_libdeps += lib_vtl:lib_vcl
fmk_libdeps += lib_vgl:lib_vcl

fmk_libdeps += lib_vil:lib_vcl
ifeq (1,$(tpp_HASPKG_JPEG))
fmk_libdeps += lib_vil:lib_jpeg
endif
ifeq (1,$(tpp_HASPKG_TIFF))
fmk_libdeps += lib_vil:lib_tiff
endif
ifeq (1,$(tpp_HASPKG_PNG))
fmk_libdeps += lib_vil:lib_png
endif

fmk_libdeps += lib_vnl:lib_vcl
fmk_libdeps += lib_vnl-algo:lib_vnl,lib_netlib
fmk_libdeps += lib_netlib:lib_fmkc,lib_fmkm

fmk_libdeps += lib_vpl,lib_vbl,lib_vtl,lib_vgl,lib_vil,lib_vnl:pkg_vxl
