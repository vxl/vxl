#
# fsm@robots.ox.ac.uk
#
fmk_libdeps += lib_jpeg,lib_tiff,lib_z,lib_png,lib_netlib,lib_Qv:pkg_v3p

fmk_libdeps += lib_jpeg:lib_fmkc,lib_fmkm
fmk_libdeps += lib_tiff:lib_jpeg
fmk_libdeps += lib_z:lib_fmkc,lib_fmkm
fmk_libdeps += lib_png:lib_z
fmk_libdeps += lib_netlib:lib_fmkc,lib_fmkm
# yikes!
fmk_libdeps += lib_Qv:lib_vcl
