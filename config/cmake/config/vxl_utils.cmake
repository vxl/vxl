#
# INSTALL_NOBASE_HEADER_FILES(prefix file file file ...)
# Will create install rules for those files of the list
# which are headers (.h or .txx).
# If .in files are given, the .in extension is removed.
#

MACRO(INSTALL_NOBASE_HEADER_FILES prefix)
FOREACH(file ${ARGN})
  IF(${file} MATCHES "\\.(h|txx)(\\.in)?$")
    STRING(REGEX REPLACE "\\.in$" "" install_file ${file})
    GET_FILENAME_COMPONENT(dir ${install_file} PATH)
    INSTALL_FILES(${prefix}/${dir} FILES ${install_file})
  ENDIF(${file} MATCHES "\\.(h|txx)(\\.in)?$")
ENDFOREACH(file ${filelist})
ENDMACRO(INSTALL_NOBASE_HEADER_FILES)
