// This is ./vxl/vul/tests/vul_test_url.cxx
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

#include <testlib/testlib_test.h>


//=======================================================================
void test_url()
{
  vcl_cout << "\n\n\n";
  vcl_cout << "*****************" << vcl_endl;
  vcl_cout << " Testing vul_url " << vcl_endl;
  vcl_cout << "*****************" << vcl_endl;


  vcl_cout<<"======== base64 stuff ==========="<<vcl_endl;

  unsigned const nTests = 300;
  vcl_string data;
  bool correct_size = true;
  bool correct_decode = true;
  for (unsigned i=0; i < nTests; ++i)
  {
    data += (char)(i % 255);


    vcl_string encoded = vul_url::encode_base64(data);
    if(!(encoded.size() >= i * 4 / 3 && encoded.size() <= 4+i * 5 / 3))
      correct_size=false;
    vcl_string decoded = vul_url::decode_base64(encoded);

    //Can be renambled to help track down errors.
    vcl_pair<vcl_string::iterator,vcl_string::iterator> x
      = vcl_mismatch(decoded.begin(), decoded.end(), data.begin());
    int k = x.first - decoded.begin();
    if (k!=int(i+1))
      correct_decode = false;
    if (decoded != data)
    {
      //Can be renambled to help track down errors.
      vcl_pair<vcl_string::iterator,vcl_string::iterator> x
        = vcl_mismatch(decoded.begin(), decoded.end(), data.begin());
      k = x.first - decoded.begin();

      correct_decode = false;
    }
  }
  TEST("All encoded strings are roughly correct size",
    correct_size, true);


  TEST("All decoded strings == original", correct_decode, true);


  vcl_cout<<"======== url determination ==========="<<vcl_endl;

  TEST("http://vxl.sourceforge.net/ is a URL",
    vul_url::is_url("http://vxl.sourceforge.net/"), true);

  TEST("/tmp/file is not URL",
    vul_url::is_url("/tmp/file"), false);

  vcl_cout<<"======== http downloading ==========="<<vcl_endl;

#ifndef DOWNLOAD
  vcl_cout<<"Tests disabled, because too " <<
    "many users are behind mandatory caches" << vcl_endl;
  vcl_cout<<"vul_url does not support HTTP via a cache" << vcl_endl;
#else
  TEST("vul_url::exists(http://vxl.sourceforge.net/index.html)",
    vul_url::exists("http://vxl.sourceforge.net/index.html"), true);

  TEST("! vul_url::exists(http://vxl.sourceforge.net/foobarwobble.html)",
    vul_url::exists("http://vxl.sourceforge.net/foobarwobble.html"), false);

  vcl_istream* i = vul_url::open("http://vxl.sourceforge.net/");
  TEST("vul_url::open(http://vxl.sourceforge.net/)", bool(i), true);

  if (i) {
    char a[1024];
    i->read(a,1024);
    vcl_cout << a;
  }
#endif
}

TESTMAIN(test_url)
