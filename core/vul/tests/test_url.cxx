// This is core/vul/tests/test_url.cxx
// Copyright: (C) 2000 British Telecommunications PLC

//:
// \file
// \brief Tests vul_url http access, base64 encoding, etc.
// \author Ian Scott
//
// Test construction, IO etc.

#include <vul/vul_url.h>

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_algorithm.h>
#include <vcl_utility.h>
#include <vcl_cstring.h> // for strncmp()

#include <testlib/testlib_test.h>


//=======================================================================
void test_url()
{
  vcl_cout << "\n"
           << "*****************\n"
           << " Testing vul_url\n"
           << "*****************\n\n";


  vcl_cout<<"======== base64 stuff ===========\n";

  unsigned const nTests = 300;
  vcl_string data;
  bool correct_size = true;
  bool correct_decode = true;
  for (unsigned i=0; i < nTests; ++i)
  {
    data += (char)(i % 255);


    vcl_string encoded = vul_url::encode_base64(data);
    if (!(encoded.size() >= i * 4 / 3 && encoded.size() <= 4+i * 5 / 3))
      correct_size=false;
    vcl_string decoded = vul_url::decode_base64(encoded);

    //Can be renambled to help track down errors.
    vcl_pair<vcl_string::iterator,vcl_string::iterator> x
      = vcl_mismatch(decoded.begin(), decoded.end(), data.begin());
    if (x.first != decoded.begin() + i+1)
      correct_decode = false;
    if (decoded != data)
    {
      //Can be renambled to help track down errors.
      x = vcl_mismatch(decoded.begin(), decoded.end(), data.begin());
      vcl_cout << "mismatch offset " << decoded.begin() +i+1 - x.first << '\n';
      correct_decode = false;
    }
  }
  TEST("All encoded strings are roughly correct size", correct_size, true);
  TEST("All decoded strings == original", correct_decode, true);

  vcl_cout<<"======== url determination ===========\n";

  TEST("http://vxl.sourceforge.net/ is a URL",
       vul_url::is_url("http://vxl.sourceforge.net/"), true);

  TEST("/tmp/file is not URL",
       vul_url::is_url("/tmp/file"), false);

  vcl_cout<<"======== http downloading ===========\n";

  bool no_download = !vul_url::exists("http://vxl.sourceforge.net/index.html");
  if (no_download)
    vcl_cout<<"vul_url::exists() and vil_url::open() tests disabled: probably behind firewall\n";
  else
  {
    TEST("vul_url::exists(\"http://vxl.sourceforge.net/index.html\")",
         vul_url::exists("http://vxl.sourceforge.net/index.html"), true);

    TEST("! vul_url::exists(\"http://vxl.sourceforge.net/foobarwobble.html\")",
         vul_url::exists("http://vxl.sourceforge.net/foobarwobble.html"), false);

    vcl_istream* i = vul_url::open("http://vxl.sourceforge.net/");
    TEST("vul_url::open(\"http://vxl.sourceforge.net/\")", i==0, false);

    char b[]="<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n<html>\n  <head>\n    <title>VXL homepage</title>";
    if (i)
    {
      int l = vcl_strlen(b);
      char a[256];
      i->read(a,1+l); a[1+l] = '\0';
      vcl_cout << a;
      TEST("test contents", vcl_strncmp(a,b,vcl_strlen(b)), 0);
    }
    delete i;
  }
}

TESTMAIN(test_url);
