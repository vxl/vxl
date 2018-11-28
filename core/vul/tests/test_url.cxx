// This is core/vul/tests/test_url.cxx
// Copyright: (C) 2000 British Telecommunications PLC

//:
// \file
// \brief Tests vul_url http access, base64 encoding, etc.
// \author Ian Scott
//
// Test construction, IO etc.

#include <iostream>
#include <string>
#include <algorithm>
#include <utility>
#include <cstring>
#include <vul/vul_url.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <testlib/testlib_test.h>


//=======================================================================
void test_url()
{
  std::cout <<"\n*****************\n"
           <<  " Testing vul_url\n"
           <<  "*****************\n\n"

           << "======== base64 stuff ===========\n";

  unsigned const nTests = 300;
  std::string data;
  bool correct_size = true;
  bool correct_decode = true;
  for (unsigned i=0; i < nTests; ++i)
  {
    data += (char)(i % 255);

    std::string encoded = vul_url::encode_base64(data);
    if (!(encoded.size() >= i * 4 / 3 && encoded.size() <= 4+i * 5 / 3))
      correct_size=false;
    std::string decoded = vul_url::decode_base64(encoded);

    //Can be renambled to help track down errors.
    std::pair<std::string::iterator,std::string::iterator> x
      = std::mismatch(decoded.begin(), decoded.end(), data.begin());
    if (x.first != decoded.begin() + i+1)
      correct_decode = false;
    if (decoded != data)
    {
      //Can be renambled to help track down errors.
      x = std::mismatch(decoded.begin(), decoded.end(), data.begin());
      std::cout << "mismatch offset " << decoded.begin() +i+1 - x.first << '\n';
      correct_decode = false;
    }
  }
  TEST("All encoded strings are roughly correct size", correct_size, true);
  TEST("All decoded strings == original", correct_decode, true);

  std::cout<<"======== url determination ===========\n";

  TEST("https://vxl.github.io/ is a URL",
       vul_url::is_url("https://vxl.github.io/"), true);

  TEST("/tmp/file is not URL",
       vul_url::is_url("/tmp/file"), false);

  std::cout<<"======== http downloading ===========\n";

  bool no_download = !vul_url::exists("https://vxl.github.io/index.html");
  if (no_download)
    std::cout << "vul_url::exists() and vil_url::open() tests disabled: probably behind firewall\n";
  else
  {
    TEST("vul_url::exists(\"https://vxl.github.io/index.html\")",
         vul_url::exists("https://vxl.github.io/index.html"), true);

    TEST("! vul_url::exists(\"https://vxl.github.io/foobarwobble.html\")",
         vul_url::exists("https://vxl.github.io/foobarwobble.html"), false);

    std::istream* i = vul_url::open("https://vxl.github.io/");
    TEST("vul_url::open(\"https://vxl.github.io/\")", i==0, false);

    char b[]="<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n"
             "<html>\n  <head>\n    <title>VXL - C++ Libraries for Computer Vision</title>";
    if (i)
    {
      int l = std::strlen(b);
      char a[256];
      i->read(a,1+l); a[1+l] = '\0';
      std::cout << a;
      TEST("test contents", std::strncmp(a,b,std::strlen(b)), 0);
    }
    delete i;
  }
}

TEST_MAIN(test_url);
