//:
// \file
// \brief Simple tool to print out the mbl log settings.
// \author Ian Scott

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vul/vul_string.h>
#include <mbl/mbl_log.h>


static bool find_ENV_file(vcl_string path)
{
  if (!vul_string_expand_var(path))
    return false;
  vcl_ifstream config_file(path.c_str());
  if (config_file.is_open())
  {
    vcl_cout << "Found " << path << "\n";
    return true;
  }
  return false;
}

static bool find_file(const vcl_string & path)
{
  vcl_ifstream config_file(path.c_str());
  if (config_file.is_open())
  {
    vcl_cout << "Found " << path << "\n";
    return true;
  }
  return false;
}

static void find_properties_file()
{
  vcl_ifstream config_file("mbl_log.properties");
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

int main(int argc, char** argv)
{
  // This code needs to be kept manually in sync with 

  find_properties_file();


  // Initialize the logger
  mbl_logger::root().load_log_config_file();
  vcl_cout << "mbl_logger settings: ";
  mbl_logger::root().categories().print(vcl_cout);
  vcl_cout << vcl_endl;

  return 0;
}
