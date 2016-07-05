/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Language:  C++
  Date:      $Date: 2009-06-14 11:52:00 $
  Version:   $Revision: 1.69 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "lsqrDense.h"

lsqrDense::lsqrDense()
{
  this->A = 0;
}


lsqrDense::~lsqrDense()
{
}


void
lsqrDense::SetMatrix( double ** inputA )
{
  this->A = inputA;
}


/**
 * computes y = y + A*x without altering x.
 */
void lsqrDense::
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
void lsqrDense::
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


/*

  returns x = (I - 2*z*z')*x.

  Implemented as x = x - z * ( 2*( z'*x ) )

*/
void lsqrDense::
HouseholderTransformation(unsigned int n, const double * z, double * x ) const
{
  // First, compute z'*x as a scalar product.
  double scalarProduct = 0.0;
  const double * zp = z;
  const double * zend = zp + n;
  double * xp = x;
  while( zp != zend )
    {
    scalarProduct += (*zp++) * (*xp++);
    }

  // Second, double the value of the scalar product.
  scalarProduct += scalarProduct;

  // Last, compute x = x - z * (2*z'*x) This subtract from x, double
  // the componenent of x that is parallel to z, effectively reflecting
  // x across the hyperplane whose normal is defined by z.
  zp = z;
  xp = x;
  zend = zp + n;
  while( zp != zend )
    {
    *xp++ -= scalarProduct * (*zp++);
    }
}
