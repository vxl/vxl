#
# Server configuration
#
SET (CVS_WEB_URL "http://cvs.sourceforge.net/cgi-bin/viewcvs.cgi/vxl/vxl/")
SET (USE_GNATS "OFF")

SET (DELIVER_BROKEN_BUILD_EMAIL "Nightly Continuous")
SET (DELIVER_BROKEN_BUILD_EMAIL_WITH_CONFIGURE_FAILURES "1")
SET (DELIVER_BROKEN_BUILD_EMAIL_WITH_BUILD_ERRORS "1")
SET (DELIVER_BROKEN_BUILD_EMAIL_WITH_BUILD_WARNINGS "0")
SET (DELIVER_BROKEN_BUILD_EMAIL_WITH_TEST_NOT_RUNS "0")
SET (DELIVER_BROKEN_BUILD_EMAIL_WITH_TEST_FAILURES "0")
SET (EMAIL_FROM "perera@cs.rpi.edu")
SET (SMTP_MAILHOST "mail.cs.rpi.edu")
SET (CVS_IDENT_TO_EMAIL "\\
  {vxl_robots DO_NOT_EMAIL} \\
  {iscott DO_NOT_EMAIL} \\
  {.* {\\0@users.sourceforge.net}} \\
")
SET (DARTBOARD_BASE_URL "http://www.cs.rpi.edu/research/vision/vxl/Testing")
SET (EMAIL_PROJECT_NAME "VXL")
SET (BUILD_MONITORS "\\
  {cs\\.rpi\\.edu .* amithaperera@users.sourceforge.net} \\
  {esat\\.kuleuven\\.ac\\.be .* peter_vanroose@users.sourceforge.net} \\
  {wheeler.*\\.crd\\.ge\\.com .* fred_wheeler@users.sourceforge.net} \\
")

SET (PROJECT_URL http://vxl.sourceforge.net/)
SET (EXPECTED_BUILDS "\\
  {cs.rpi.edu FreeBSD-4.9-gcc-2.95.4} \\
  {cs.rpi.edu FreeBSD-4.9-gcc-2.95.4-static} \\
  {cs.rpi.edu FreeBSD-4.9-gcc-3.3} \\
  {esat.kuleuven.ac.be IRIX-6.5-CC-n32} \\
  {esat.kuleuven.ac.be Linux-2.6-gcc-3.4-O3} \\
  {esat.kuleuven.ac.be Linux64-2.6-gcc-3.4-O3} \\
  {esat.kuleuven.ac.be OSF1-V5.1-gcc-3.4} \\
  {crd.ge.com Solaris-2.8_gcc-3.0.4_profile_static} \\
  {crd.ge.com Win2k_bcc-5.5.1_Release} \\
  {esat.kuleuven.ac.be SunOS-5.8-gcc-3.4-O2} \\
  {esat.kuleuven.ac.be HP-UX-B.11.00-gcc-3.4-O1} \\
  {crd.ge.com Playstation2-Linux-2.2.1_gcc-2.95.2_-Wall_static} \\
  {crd.ge.com Cygwin-1.5.11_gcc-3.3.3_-Wall_-O2_static} \\
  {crd.ge.com Win2k_msvc-6.0sp5_Release} \\
  {crd.ge.com FreeBSD-5.0_gcc-3.2.1_-Wall_-O_shared} \\
  {crd.ge.com Linux-2.4.18_gcc-3.2_-Wall_profile_static} \\
  {crd.ge.com Linux-2.4.9_icc-8.0_-w1_-O2_shared} \\
  {crd.ge.com Win2k_msvc-7.0_Release} \\
  {nk.crd.ge.com Win2k_msvc-7.1_Release} \\
  {imorphics Win2k-msvc-6sp5-iMorphicsRelDeb} \\
  {imorphics Win2k-msvc-6sp5-Debug} \\
  {crd.ge.com MinGW-3.5_gcc-3.3.3_-Wall_-O2_static} \\
")
