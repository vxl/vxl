//:
// \file
// \brief Simple tool to print out the mbl log settings.
// \author Ian Scott

#include <iostream>
#include <fstream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_string.h>
#include <mbl/mbl_log.h>


static bool find_ENV_file(std::string path)
{
  if (!vul_string_expand_var(path))
    return false;
  std::ifstream config_file(path.c_str());
  if (config_file.is_open())
  {
    std::cout << "Found " << path << "\n";
    return true;
  }
  return false;
}

static bool find_file(const std::string & path)
{
  std::ifstream config_file(path.c_str());
  if (config_file.is_open())
  {
    std::cout << "Found " << path << "\n";
    return true;
  }
  return false;
}

static void find_properties_file()
{
  std::ifstream config_file("mbl_log.properties");
  if (find_file("./mbl_log.properties")) return;
  if (find_file("~/mbl_log.properties")) return;
  if (find_file("~/.mbl_log.properties")) return;
  if (find_ENV_file("${HOME}/mbl_log.properties")) return;
  if (find_ENV_file("${HOME}/.mbl_log.properties")) return;
  if (find_ENV_file("${HOMESHARE}/mbl_log.properties")) return;
  if (find_ENV_file("${HOMEDRIVE}${HOMEDIR}/mbl_log.properties")) return;
  if (find_ENV_file("${HOMEDRIVE}${HOMEPATH}/mbl_log.properties")) return;
  if (find_ENV_file("${USERPROFILE}/mbl_log.properties")) return;
  if (find_file("C:\\mbl_log.properties")) return;
}

int main()
{
  // This code needs to be kept manually in sync

  find_properties_file();

  // Initialize the logger
  mbl_logger::root().load_log_config_file();
  std::cout << "mbl_logger settings: ";
  mbl_logger::root().categories().print(std::cout);
  std::cout << std::endl;

  return 0;
}
