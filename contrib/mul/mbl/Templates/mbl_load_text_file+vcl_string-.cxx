#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_load_text_file.hxx>
MBL_LOAD_TEXT_FILE_INSTANTIATE_PATH(std::string);
MBL_LOAD_TEXT_FILE_INSTANTIATE_STREAM(std::string);
