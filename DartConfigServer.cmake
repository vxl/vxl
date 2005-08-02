#
# Server configuration
#

CMAKE_MINIMUM_REQUIRED( VERSION 2.0 )

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
  {^vanroose DO_NOT_EMAIL} \\
  {uid1820 DO_NOT_EMAIL} \\
  {targetjr DO_NOT_EMAIL} \\
  {remcvs_tu_crd DO_NOT_EMAIL} \\
  {remcvs_tgtjr_oxford DO_NOT_EMAIL} \\
  {remcvs_targetjr_leuven DO_NOT_EMAIL} \\
  {remcvs_targetjr_grumpie DO_NOT_EMAIL} \\
  {remcvs_perera_rpi DO_NOT_EMAIL} \\
  {remcvs_manchester_scott DO_NOT_EMAIL} \\
  {remcvs_manchester_cootes DO_NOT_EMAIL} \\
  {remcvs_hoffman_kitware DO_NOT_EMAIL} \\
  {remcvs_hamilton_crd DO_NOT_EMAIL} \\
  {remcvs_geoff_oxford DO_NOT_EMAIL} \\
  {remcvs_geoff_ge DO_NOT_EMAIL} \\
  {remcvs_fsm_oxford DO_NOT_EMAIL} \\
  {remcvs_awf_oxford DO_NOT_EMAIL} \\
  {remcvs_depot_balltown DO_NOT_EMAIL} \\
  {^perera DO_NOT_EMAIL} \\
  {iscott DO_NOT_EMAIL} \\
  {jhoulihan DO_NOT_EMAIL} \\
  {.* {\\0@users.sourceforge.net}} \\
")
SET (DARTBOARD_BASE_URL "http://www.cs.rpi.edu/research/groups/vxl/Testing")
SET (EMAIL_PROJECT_NAME "VXL")
SET (BUILD_MONITORS "\\
  {cs\\.rpi\\.edu .* amithaperera@users.sourceforge.net} \\
  {esat\\.kuleuven\\.be .* peter_vanroose@users.sourceforge.net} \\
  {crd\\.ge\\.com_DISABLED .* fred_wheeler@users.sourceforge.net} \\
")

SET (PROJECT_URL http://vxl.sourceforge.net/)
SET (EXPECTED_BUILDS "\\
  {crd.ge.com Cygwin_gcc-3.4.4_-Wall_-O2_static} \\
  {cs.rpi.edu FreeBSD-4.9-gcc-2.95.4} \\
  {cs.rpi.edu FreeBSD-4.9-gcc-2.95.4-static} \\
  {cs.rpi.edu FreeBSD-4.9-gcc-3.3} \\
  {crd.ge.com FreeBSD-5.0_gcc-3.2.1_-Wall_-O_shared} \\
  {crd.ge.com Linux-2.4.18_gcc-3.2_-Wall_profile_static} \\
  {crd.ge.com Linux-2.4.9_icc-8.0_-w1_-O2_shared} \\
  {crd.ge.com MinGW-3.7_gcc-3.3.3_-Wall_-O2_static} \\
  {crd.ge.com Solaris-2.8_gcc-3.4.3_static} \\
  {crd.ge.com Win2k_bcc-5.5.1_Release} \\
  {crd.ge.com Win2k_msvc-6.0sp5_Release} \\
  {crd.ge.com Win2k_msvc-7.0_Release} \\
  {imorphics Win2k-msvc-6sp5-Debug} \\
  {imorphics Win2k-msvc-6sp5-iMorphicsRelDeb} \\
")
