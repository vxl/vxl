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
#ifdef __GNUC__
#pragma implementation "vnl_least_squares_function.h"
#endif
//
// Class: vnl_least_squares_function
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 31 Aug 96
// Modifications:
//
//-----------------------------------------------------------------------------

#include "vnl_least_squares_function.h"
#include <vcl/vcl_cassert.h>

// Default ctor
vnl_least_squares_function::vnl_least_squares_function(int number_of_unknowns,
					   int number_of_residuals,
					   UseGradient g)
  : failure(false)
{
  n_ = number_of_residuals;
  p_ = number_of_unknowns;
  assert(n_>=0);
  assert(p_>=0);
  use_gradient_ = (g == use_gradient);
}

void vnl_least_squares_function::init(int number_of_unknowns,
				int number_of_residuals)
{
  n_ = number_of_residuals;
  p_ = number_of_unknowns;
  assert(n_>=0);
  assert(p_>=0);
}

// Destructor
vnl_least_squares_function::~vnl_least_squares_function()
{
}

void vnl_least_squares_function::throw_failure(void)
{
  failure=true;
}

void vnl_least_squares_function::clear_failure(void)
{
  failure=false;
}

void vnl_least_squares_function::gradf(const vnl_vector<double>& /*x*/,
				 vnl_matrix<double>& /*jacobian*/)
{
}

void vnl_least_squares_function::trace(int /* iteration */,
				 const vnl_vector<double>& /*x*/,
				 const vnl_vector<double>& /*fx*/)
{
}

double vnl_least_squares_function::rms(const vnl_vector<double>& x)
{
  vnl_vector<double> fx(n_);
  f(x, fx);
  return fx.magnitude();
}
