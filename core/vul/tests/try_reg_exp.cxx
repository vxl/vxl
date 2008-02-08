// This is core/vul/tests/try_reg_exp.cxx

//:
// \file
// \brief Try a regular expression.
// \author Ian Scott
//

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vul/vul_arg.h>
#include <vul/vul_reg_exp.h>



//=======================================================================
int main(int argc, char *argv[])
{

  vul_arg<vcl_string> re_string(0, "Regular expression");
  vul_arg_parse(argc,argv);

  vcl_string s;
  vul_reg_exp re(re_string().c_str());
  if (!re.is_valid())
  {
    vcl_cerr << "RE is not valid.\n";
    return 3;
  }

  while (!(!vcl_getline(vcl_cin, s)))
  {
    if (re.find(s))
    {
      vcl_cout << "MATCHED" << vcl_endl;
    }
    else
    {
      vcl_cout << "NOT MATCHED" << vcl_endl;
    }
  }
  return 0;
}

