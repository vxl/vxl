#
# Server configuration
#

CMAKE_MINIMUM_REQUIRED( VERSION 2.0 )

SET (CVS_WEB_URL "http://vxl.cvs.sourceforge.net/vxl/vxl/")
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
  {grc\\.ge\\.com_DISABLED .* fred_wheeler@users.sourceforge.net} \\
")

SET (PROJECT_URL http://vxl.sourceforge.net/)
SET (EXPECTED_BUILDS "\\
  {cs.rpi.edu FreeBSD-fresh-gcc-3.4.2} \\
  {GE FreeBSD-6.2_gcc-3.4.6_profile_static} \\
  {GE FreeBSD-6.2_gcc-4.0.4_-O2_shared} \\
  {GE FreeBSD-6.2_gcc-4.2.2_-O2_shared} \\
  {GE Linux-2.6.9_icc-9.1-64bit_-O2_shared} \\
  {GE MinGW-3.8_gcc-3.4.4_-O2_static} \\
  {GE Win2k_bcc-5.5.1_Release} \\
  {GE Win2k_msvc-7.1_Debug} \\
  {imorphics.com WinXP_msvc-7.1_RelWithDebInfo} \\
  {imorphics.com Linux-2.6.22_gcc-4.1.2_RelWithDebInfo} \\
  {lems.brown.edu Linux-2.6_gcc-4.1.3_-Wall} \\
  {lems.brown.edu Linux-2.6.18_gcc-4.1.2} \\
")
