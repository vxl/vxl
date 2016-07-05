/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "lsmrDense.h"

lsmrDense::lsmrDense()
{
  this->A = 0;
}


lsmrDense::~lsmrDense()
{
}


void
lsmrDense::SetMatrix( double ** inputA )
{
  this->A = inputA;
}


/**
 * computes y = y + A*x without altering x.
 */
void lsmrDense::
Aprod1(unsigned int m, unsigned int n, const double * x, double * y ) const
{
  for ( unsigned int row = 0; row < m; row++ )
    {
    const double * rowA = this->A[row];
    double sum = 0.0;

    for ( unsigned int col = 0; col < n; col++ )
      {
      sum += rowA[col] * x[col];
      }

    y[row] +=  sum;
    }
}


/**
 * computes x = x + A'*y without altering y.
 */
void lsmrDense::
Aprod2(unsigned int m, unsigned int n, double * x, const double * y ) const
{
  for ( unsigned int col = 0; col < n; col++ )
    {
    double sum = 0.0;

    for ( unsigned int row = 0; row < m; row++ )
      {
      sum += this->A[row][col] * y[row];
      }

    x[col] +=  sum;
    }
}
