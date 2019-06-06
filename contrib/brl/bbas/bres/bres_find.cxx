//:
// \file
#include "bres_find.h"
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <utility>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Helper function - Check if file exists
bool file_exists(std::string const& fname) {
  if (FILE* f = fopen(fname.c_str(), "r")) {
    fclose(f);
    return true;
  } else {
    return false;
  }
}

//: Get path to resource
std::string bres_find::locate(std::string const& resource_name)
{
  // check environment
  const char* env_data_dir_ptr = std::getenv("VXL_DATA_DIR");
  if (env_data_dir_ptr != NULL) {
    std::string env_data_dir(env_data_dir_ptr);

    // concatenate
    std::string env_resource_path = env_data_dir + std::string("/") + resource_name;

    if (file_exists(env_resource_path)) {
      return env_resource_path;
    }
  }

  // Load CMake paths
  const std::string source_dir("");
  const std::string install_dir("");
  #ifdef BRES_WHERE_H_EXISTS
    #include "bres_where.h"
    const std::string source_dir = std::string(BRES_SOURCE_DIR);
    const std::string install_dir = std::string(BRES_INSTALL_DIR);
  #endif

  // check source dir
  if (!source_dir.empty()) {

    // concatenate
    std::string source_resource_path = source_dir + std::string("/") + resource_name;

    if (file_exists(source_resource_path)) {
      return source_resource_path;
    }
  }

  // check install dir
  if (!install_dir.empty()) {

    // concatenate
    std::string install_resource_path = install_dir + std::string("/") + resource_name;

    if (file_exists(install_resource_path)) {
      return install_resource_path;
    }
  }

  // TODO: Raise exception instead?
  return "";
}
