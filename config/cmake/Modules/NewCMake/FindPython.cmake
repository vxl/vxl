# - Test for Python
# Once loaded this will define
#   PYTHON_FOUND           - system has Python
#   PYTHON_INCLUDE_DIR     - path to where Python.h is found
#   PYTHON_INCLUDE_DIRS    - combined include path
#   PYTHON_PC_INCLUDE_PATH - PC directory for Win
#   PYTHON_LIBRARY         - libraries you need to link to
#   PYTHON_DEBUG_LIBRARY   - path to the debug library

# Flag that determines if we were able to successfully build Python.
# Initialize to NO. Change below if yes.
set(PYTHON_FOUND "NO" CACHE INTERNAL "Was Python successfully built?" )
set(Python_ADDITIONAL_VERSIONS 1.5 1.6 1.7 1.8 1.9 2.0 2.1 2.2 2.3 2.4 2.5 2.6 2.7)
find_package(PythonLibs)

if(PYTHON_INCLUDE_DIR)
 if(PYTHON_LIBRARY OR PYTHON_DEBUG_LIBRARY)
  # everything found
  set(PYTHON_FOUND "YES" CACHE INTERNAL "Was Python successfully built?")

  if( WIN32 )
    find_path(PYTHON_PC_INCLUDE_PATH
      NAMES pyconfig.h

      PATHS
      ${PYTHON_INCLUDE_DIRS}
      ${PYTHON_FRAMEWORK_INCLUDES}
      [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.7\\InstallPath]/PC
      [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.6\\InstallPath]/PC
      [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.5\\InstallPath]/PC
      [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.4\\InstallPath]/PC
      [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.3\\InstallPath]/PC
      [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.2\\InstallPath]/PC
      [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.1\\InstallPath]/PC
      [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.0\\InstallPath]/PC
      [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\1.6\\InstallPath]/PC
      [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\1.5\\InstallPath]/PC

      PATH_SUFFIXES
       python2.7
       python2.6
       python2.5
       python2.4
       python2.3
       python2.2
       python2.1
       python2.0
       python1.6
       python1.5
    )

    set(PYTHON_INCLUDE_DIRS
      ${PYTHON_INCLUDE_DIR}
      ${PYTHON_PC_INCLUDE_PATH}
    )
    #message(${PYTHON_INCLUDE_DIRS})

    mark_as_advanced(
     PYTHON_PC_INCLUDE_PATH
    )

  endif()

 endif()
endif()
