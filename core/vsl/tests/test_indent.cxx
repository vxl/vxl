#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>
#include <vcl_string.h>

#include <vsl/vsl_indent.h>

void test_indent1()
{
  vcl_cout<<"Tab Size is "<<vsl_indent_tab(vcl_cout)<<vcl_endl;
  vcl_cout<<vsl_indent()<<"No Indent"<<vcl_endl;
  vsl_inc_indent(vcl_cout);
  vcl_cout<<vsl_indent()<<"1 Indent"<<vcl_endl;
  vsl_inc_indent(vcl_cout);
  vcl_cout<<vsl_indent()<<"2 Indent"<<vcl_endl;
  vsl_dec_indent(vcl_cout);
  vcl_cout<<vsl_indent()<<"1 Indent"<<vcl_endl;
  vsl_dec_indent(vcl_cout);
  vcl_cout<<vsl_indent()<<"No Indent"<<vcl_endl;
  vcl_cout<<vcl_endl;
}

void test_indent()
{
  vcl_cout << "*******************" << vcl_endl;
  vcl_cout << "Testing vsl_indent" << vcl_endl;
  vcl_cout << "*******************" << vcl_endl;

  vcl_cout<<"Using Tab size 2: "<<vcl_endl<<vcl_endl;
  vsl_indent_set_tab(vcl_cout,2);
  test_indent1();


  vcl_cout<<"Using Tab size 4: "<<vcl_endl<<vcl_endl;
  vsl_indent_set_tab(vcl_cout,4);
  test_indent1();

  vcl_cout<<vsl_indent()<<"Indent 0"<<vcl_endl;
  vsl_inc_indent(vcl_cout);
  vcl_cout<<vsl_indent()<<"Indent 1"<<vcl_endl;
  vsl_inc_indent(vcl_cout);
  vcl_cout<<vsl_indent()<<"Indent 2"<<vcl_endl;
  vsl_clear_indent(vcl_cout);
  vcl_cout<<vsl_indent()<<"Indent Cleared"<<vcl_endl;

}

int main()
{
  test_indent();
  return 0;
}
