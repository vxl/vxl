// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 Tvbl_argetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name Tvbl_argetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE Tvbl_argETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
//-*- c++ -*-------------------------------------------------------------------
//
// Class: test_vbl_arg
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 21 Jan 97
// Modifications:
//
//-----------------------------------------------------------------------------

#include <vbl/vbl_arg.h>
#include <vcl/vcl_cstring.h> // needed for strcmp()

void Assert(char const* msg, bool expr)
{
  cout << msg << " - " << (expr?"passed":"failed") << "." << endl;
}

char * vbl_argv_1[] = {
  "progname",
  "f",
  "-int", "3",
  "g",
  "-bool1",
  "-bool1",
  "-bool2",
  "h",
  "i",
  "j",
  0
};

int count_vbl_args(char ** vbl_argv)
{
  int c = 0;
  for(; *vbl_argv; ++vbl_argv)
    ++c;
  return c;
}

void test_do_vbl_arg()
{
  vbl_arg<int> int1("-int", "A help string", 1);
  vbl_arg<int> int2("-int2", "Another help string", 2);
  vbl_arg<bool> bool1("-bool1", "And another", false);
  vbl_arg<bool> bool2("-bool2", "And another", true);
  vbl_arg<bool> bool3("-bool3", "And a final help test just to finish off...", true);
  vbl_arg<char*> filename1;

  int vbl_argc = count_vbl_args(vbl_argv_1);
  char ** vbl_argv = vbl_argv_1;
  cerr << "vbl_argc = " << vbl_argc
       << ", bool1 = " << (bool)bool1
       << ", bool2 = " << (bool)bool2
       << ", bool3 = " << (bool)bool3 << endl;

  vbl_arg_base::parse(vbl_argc, vbl_argv);
  
  bool b = (int)int1 == 3;
  Assert("int1", b);
  Assert("int2", (int)int2 == 2);
  Assert("filename == f", !strcmp((char*)filename1, "f"));  
}

extern "C"
void test_vbl_arg()
{
  test_do_vbl_arg();
}

main()
{
  cout << "Running" << endl;
  test_vbl_arg();
}
