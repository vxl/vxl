// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
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
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
#include <vcl/vcl_new.h>
#include <vcl/vcl_cstdlib.h>
#include <vcl/vcl_iostream.h>

#include <vnl/vnl_test.h>

#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_linear_operators_3.h>

bool verbose_malloc = false;
int malloc_count = 0;

void testvnl_matrix_fixed()
{
  verbose_malloc = true;
  double datablock[9] = {
    11, 12, 13,
    21, 22, 23,
    31, 32, 33,
  };
  
  cout << "Calling ctor -- should be no mallocs\n";
  //Refvnl_double_3x3 X(datablock);
  malloc_count = 0;
  vnl_double_3x3 X(datablock);
  cout << "X = [" << X << "]\n";

  vnl_double_3 v(10,11,12);
  cout << "v = [ " << v << "]\n";
  
  cout << "X v = " << X * (v + v) << endl;
  // This shouldn't compile...
  // vnl_matrix<double>* base = new vnl_double_3x3(datablock);

  // FIXME: Win32 will have different operator new in vnl dll from
  // the one generated here, so this test fails - RWMC.
#ifndef WIN32
  Assert("mallocs", malloc_count <= 1);
#endif
  
  cout << "Now watch them mallocs\n";
  vnl_matrix<double>& CX = X;
  vnl_vector<double>& cv = v;
  cout << "X v = " << CX * (cv + cv) << endl;
  
  verbose_malloc = false;  
}

#if !defined(VCL_SUNPRO_CC_50)
// For SunPro5.0, these functions produce compiler
// warnings and cause runtime segfault.
void* operator new(size_t s)
#ifdef VCL_KAI
  // [18.4.1] lib.new.delete
  throw (std::bad_alloc)
#endif
{
  void *r = malloc(s);
  
  ++malloc_count;
  
  if (verbose_malloc) 
    cout << "malloc: " << r << " for " << s << endl;
 
  return r;
}
 
void operator delete(void* s)
{
  if (verbose_malloc)
    cout << "delete: " << s << endl;
  free(s);
}
#endif

TESTMAIN(testvnl_matrix_fixed);
