#
# ./v3p/Qv/CMakeLists.txt
#

ADD_LIBRARY( Qv Qv_sources )

SOURCE_FILES( Qv_sources
  vecutil
  QvLib
  wrltraverse
  wrlbuild
  QvVisitor
  QvVrmlFile
)

AUX_SOURCE_DIRECTORY( Templates Qv_sources )
