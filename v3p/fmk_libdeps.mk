#
# fsm@robots.ox.ac.uk
#

fmk_libdeps += lib_jpeg:lib_fmkc,lib_fmkm
fmk_libdeps += lib_jpeg:tpp_JPEG

fmk_libdeps += lib_tiff:lib_jpeg
fmk_libdeps += lib_tiff:tpp_TIFF

fmk_libdeps += lib_z:lib_fmkc,lib_fmkm
fmk_libdeps += lib_z:tpp_ZLIB

fmk_libdeps += lib_png:lib_z
fmk_libdeps += lib_png:tpp_PNG

fmk_libdeps += lib_netlib:lib_fmkc,lib_fmkm
fmk_libdeps += lib_netlib:pkg_v3p

# yikes!
fmk_libdeps += lib_Qv:lib_vcl
fmk_libdeps += lib_Qv:pkg_v3p
