/*
<begin copyright notice>
---------------------------------------------------------------------------

                  Copyright (c) 1997 TargetJr Consortium
              GE Corporate Research and Development (GE CRD)
                            1 Research Circle
                           Niskayuna, NY 12309
                           All Rights Reserved
             Reproduction rights limited as described below.
                              
     Permission to use, copy, modify, distribute, and sell this software
     and its documentation for any purpose is hereby granted without fee,
     provided that (i) the above copyright notice and this permission
     notice appear in all copies of the software and related documentation,
     (ii) the name TargetJr Consortium (represented by GE CRD), may not be
     used in any advertising or publicity relating to the software without
     the specific, prior written permission of GE CRD, and (iii) any
     modifications are clearly marked and summarized in a change history
     log.
      
     THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
     EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
     WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
     IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
     INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
     DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
     WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
     ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
     USE OR PERFORMANCE OF THIS SOFTWARE.

---------------------------------------------------------------------------
<end copyright notice>
*/

#include <iostream.h>
#include <vnl/vnl_numeric_limits.h>
//#include <math/numeric_limits_float.h>
//#include <math/numeric_limits_double.h>

main()
{
  cout << "dmax  = " << vnl_numeric_limits<double>::max() << endl;
  cout << "dmin  = " << vnl_numeric_limits<double>::min() << endl;
  cout << "deps  = " << vnl_numeric_limits<double>::epsilon() << endl;
  cout << "dnmin = " << vnl_numeric_limits<double>::denorm_min() << endl;
  cout << "dnan  = " << vnl_numeric_limits<double>::quiet_NaN() << endl;
  cout << "dinf  = " << vnl_numeric_limits<double>::infinity() << endl;
  cout << "dninf = " << -vnl_numeric_limits<double>::infinity() << endl;

  cout << "fmax  = " << vnl_numeric_limits<float>::max() << endl;
  cout << "fmin  = " << vnl_numeric_limits<float>::min() << endl;
  cout << "feps  = " << vnl_numeric_limits<float>::epsilon() << endl;
  cout << "fnmin = " << vnl_numeric_limits<float>::denorm_min() << endl;
  cout << "fnan  = " << vnl_numeric_limits<float>::quiet_NaN() << endl;
  cout << "finf  = " << vnl_numeric_limits<float>::infinity() << endl;
  cout << "fninf = " << -vnl_numeric_limits<float>::infinity() << endl;
  return 0;
}
