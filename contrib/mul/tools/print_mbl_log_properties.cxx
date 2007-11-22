#include <vcl_exception.h>
#include <vcl_iostream.h>
#include <mbl/mbl_log.h>
#include <vul/vul_string.h>

vcl_string find_config_file()
{
    // Need to keep this uptodate w.r.t. mbl_log.cxx
  vcl_ifstream config_file("./mbl_log.properties");
  if (config_file.is_open()) return "./mbl_log.properties";
  config_file.open("~/mbl_log.properties");
  if (config_file.is_open()) return "~/mbl_log.properties";
  config_file.open("~/.mbl_log.properties");
  if (config_file.is_open()) return "~/.mbl_log.properties";
  
  vcl_string home1("${HOME}/mbl_log.properties");
  vcl_string home2("${HOME}/.mbl_log.properties");
  vcl_string home3("${HOMESHARE}/mbl_log.properties");
  vcl_string home4("${HOMEDRIVE}${HOMEDIR}/mbl_log.properties");
  vcl_string home5("${HOMEDRIVE}${HOMEPATH}/mbl_log.properties");
  vcl_string home6("${USERPROFILE}/mbl_log.properties");
  if (vul_string_expand_var(home1))
  {
    config_file.open(home1.c_str());
    if (config_file.is_open()) return home1;
  }
  if (vul_string_expand_var(home2))
  {
    config_file.open(home2.c_str());
    if (config_file.is_open()) return home2;
  }
  if (vul_string_expand_var(home3))
  {
    config_file.open(home3.c_str());
    if (config_file.is_open()) return home3;
  }
  if (vul_string_expand_var(home4))
  {
    config_file.open(home4.c_str());
    if (config_file.is_open()) return home4;
  }
  if (vul_string_expand_var(home5))
  {
    config_file.open(home5.c_str());
    if (config_file.is_open()) return home5;
  }

  if (vul_string_expand_var(home6))
  {
    config_file.open(home6.c_str());
    if (config_file.is_open()) return home6;
  }

  config_file.open("C:\\mbl_log.properties");
  if (config_file.is_open()) return "C:\\mbl_log.properties";
    
  return "";
}

int main()
{

  try
  {
    vcl_string location = find_config_file();
    if (!location.empty()) vcl_cout << "Found " << location << vcl_endl;

    mbl_logger::root().load_log_config_file();
    mbl_logger::root().categories().print(vcl_cout);
  }
  catch (vcl_exception& e)
  {
    vcl_cout << "print_mbl_log_properties caught exception " << e.what() << vcl_endl;
    return 3;
  }
  catch (...)
  {
    vcl_cout << "print_mbl_log_properties caught unknown exception " << vcl_endl;
    return 3;
  }


  return 0;
}
