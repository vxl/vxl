#
# fsm@robots.ox.ac.uk
#

fmk_libdeps += jpeg:fmk_libc,fmk_libm
fmk_libdeps += tiff:jpeg
fmk_libdeps += z:fmk_libc,fmk_libm
fmk_libdeps += png:z
fmk_libdeps += netlib:fmk_libc,fmk_libm

# yikes!
fmk_libdeps += Qv:vcl

