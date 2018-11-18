// This is core/vul/tests/try_reg_exp.cxx

//:
// \file
// \brief Try a regular expression.
// \author Ian Scott
//

#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <vul/vul_reg_exp.h>



//=======================================================================
int main(int argc, char *argv[])
{

  vul_arg<std::string> re_string(nullptr, "Regular expression");
  vul_arg_parse(argc,argv);

  std::string s;
  vul_reg_exp re(re_string().c_str());
  if (!re.is_valid())
  {
    std::cerr << "RE is not valid.\n";
    return 3;
  }

  while (!(!std::getline(std::cin, s)))
  {
    if (re.find(s))
    {
      std::cout << "MATCHED chars " << re.start(0) << " to " << re.end(0) << " (0-start to end+1)"<< std::endl;
    }
    else
    {
      std::cout << "NOT MATCHED" << std::endl;
    }
  }
  return 0;
}
