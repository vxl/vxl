//:
// \file
// \brief Apply gradient operator to 2D planes of data
// \author Tim Cootes

#include "vil_sobel_1x3.h"

//: Compute gradients of single plane of 2D data using 1x3 Sobel filters
//  Computes both i and j gradients of an ni x nj plane of data
void vil_sobel_1x3_1plane(const unsigned char* src,
                          vcl_ptrdiff_t s_istep, vcl_ptrdiff_t s_jstep,
                          float* gi, vcl_ptrdiff_t gi_istep, vcl_ptrdiff_t gi_jstep,
                          float* gj, vcl_ptrdiff_t gj_istep, vcl_ptrdiff_t gj_jstep,
                          unsigned ni, unsigned nj)
{
  const unsigned char* s_data = src;
  float *gi_data = gi;
  float *gj_data = gj;

  if (ni==0 || nj==0) return;
  if (ni==1)
  {
      // Zero the elements in the column
    for (unsigned j=0;j<nj;++j)
    {
      *gi_data = 0;
      *gj_data = 0;
      gi_data += gi_jstep;
      gj_data += gj_jstep;
    }
    return;
  }
  if (nj==1)
  {
      // Zero the elements in the column
    for (unsigned i=0;i<ni;++i)
    {
      *gi_data = 0;
      *gj_data = 0;
      gi_data += gi_istep;
      gj_data += gj_istep;
    }
    return;
  }

  // Compute relative grid positions
  //     o2
  //  o4    o5
  //     o7
  const vcl_ptrdiff_t o2 = s_jstep;
  const vcl_ptrdiff_t o4 = -s_istep;
  const vcl_ptrdiff_t o5 = s_istep;
  const vcl_ptrdiff_t o7 = -s_jstep;

  const unsigned ni1 = ni-1;
  const unsigned nj1 = nj-1;

  s_data += s_istep + s_jstep;
  gi_data += gi_jstep;
  gj_data += gj_jstep;

  for (unsigned j=1;j<nj1;++j)
  {
    const unsigned char* s = s_data;
    float* pgi = gi_data;
    float* pgj = gj_data;

    // Zero the first elements in the rows
    *pgi = 0; pgi+=gi_istep;
    *pgj = 0; pgj+=gj_istep;

    for (unsigned i=1;i<ni1;++i)
    {
      // Compute gradient in i
      // Note: Multiply each element individually
      //      to ensure conversion to float before addition
      *pgi = 0.5f*s[o5] - 0.5f*s[o4];
      // Compute gradient in j
      *pgj = 0.5f*s[o2] - 0.5f*s[o7];

      s+=s_istep;
      pgi += gi_istep;
      pgj += gj_istep;
    }

    // Zero the last elements in the rows
    *pgi = 0;
    *pgj = 0;

    // Move to next row
    s_data  += s_jstep;
    gi_data += gi_jstep;
    gj_data += gj_jstep;
  }

  // Zero the first and last rows
  for (unsigned i=0;i<ni;++i)
  {
    *gi=0; gi+=gi_istep;
    *gj=0; gj+=gj_istep;
    *gi_data = 0; gi_data+=gi_istep;
    *gj_data = 0; gj_data+=gj_istep;
  }
}

//: Compute gradients of single plane of 2D data using 1x3 Sobel filters
//  Computes both i and j gradients of an ni x nj plane of data
void vil_sobel_1x3_1plane(const unsigned char* src,
                          vcl_ptrdiff_t s_istep, vcl_ptrdiff_t s_jstep,
                          double* gi, vcl_ptrdiff_t gi_istep, vcl_ptrdiff_t gi_jstep,
                          double* gj, vcl_ptrdiff_t gj_istep, vcl_ptrdiff_t gj_jstep,
                          unsigned ni, unsigned nj)
{
  const unsigned char* s_data = src;
  double *gi_data = gi;
  double *gj_data = gj;

  if (ni==0 || nj==0) return;
  if (ni==1)
  {
      // Zero the elements in the column
    for (unsigned j=0;j<nj;++j)
    {
      *gi_data = 0;
      *gj_data = 0;
      gi_data += gi_jstep;
      gj_data += gj_jstep;
    }
    return;
  }
  if (nj==1)
  {
      // Zero the elements in the column
    for (unsigned i=0;i<ni;++i)
    {
      *gi_data = 0;
      *gj_data = 0;
      gi_data += gi_istep;
      gj_data += gj_istep;
    }
    return;
  }

  // Compute relative grid positions
  //     o2
  //  o4    o5
  //     o7
  const vcl_ptrdiff_t o2 = s_jstep;
  const vcl_ptrdiff_t o4 = -s_istep;
  const vcl_ptrdiff_t o5 = s_istep;
  const vcl_ptrdiff_t o7 = -s_jstep;

  const unsigned ni1 = ni-1;
  const unsigned nj1 = nj-1;

  s_data += s_istep + s_jstep;
  gi_data += gi_jstep;
  gj_data += gj_jstep;

  for (unsigned j=1;j<nj1;++j)
  {
    const unsigned char* s = s_data;
    double* pgi = gi_data;
    double* pgj = gj_data;

    // Zero the first elements in the rows
    *pgi = 0; pgi+=gi_istep;
    *pgj = 0; pgj+=gj_istep;

    for (unsigned i=1;i<ni1;++i)
    {
      // Compute gradient in i
      // Note: Multiply each element individually
      //      to ensure conversion to double before addition
      *pgi = 0.5*s[o5] - 0.5*s[o4];
      // Compute gradient in j
      *pgj = 0.5*s[o2] - 0.5*s[o7];

      s+=s_istep;
      pgi += gi_istep;
      pgj += gj_istep;
    }

    // Zero the last elements in the rows
    *pgi = 0;
    *pgj = 0;

    // Move to next row
    s_data  += s_jstep;
    gi_data += gi_jstep;
    gj_data += gj_jstep;
  }

  // Zero the first and last rows
  for (unsigned i=0;i<ni;++i)
  {
    *gi=0; gi+=gi_istep;
    *gj=0; gj+=gj_istep;
    *gi_data = 0; gi_data+=gi_istep;
    *gj_data = 0; gj_data+=gj_istep;
  }
}

//: Compute gradients of single plane of 2D data using 1x3 Sobel filters
//  Computes both x and j gradients of an nx x nj plane of data
void vil_sobel_1x3_1plane(const float* src,
                          vcl_ptrdiff_t s_istep, vcl_ptrdiff_t s_jstep,
                          float* gi, vcl_ptrdiff_t gi_istep, vcl_ptrdiff_t gi_jstep,
                          float* gj, vcl_ptrdiff_t gj_istep, vcl_ptrdiff_t gj_jstep,
                          unsigned ni, unsigned nj)
{
  const float* s_data = src;
  float *gi_data = gi;
  float *gj_data = gj;

  if (ni==0 || nj==0) return;
  if (ni==1)
  {
      // Zero the elements in the column
    for (unsigned j=0;j<nj;++j)
    {
      *gi_data = 0;
      *gj_data = 0;
      gi_data += gi_jstep;
      gj_data += gj_jstep;
    }
    return;
  }
  if (nj==1)
  {
      // Zero the elements in the column
    for (unsigned i=0;i<ni;++i)
    {
      *gi_data = 0;
      *gj_data = 0;
      gi_data += gi_istep;
      gj_data += gj_istep;
    }
    return;
  }

  // Compute relative grid positions
  //     o2
  //  o4    o5
  //     o7
  const vcl_ptrdiff_t o2 = s_jstep;
  const vcl_ptrdiff_t o4 = -s_istep;
  const vcl_ptrdiff_t o5 = s_istep;
  const vcl_ptrdiff_t o7 = -s_jstep;

  const unsigned ni1 = ni-1;
  const unsigned nj1 = nj-1;

  s_data += s_istep + s_jstep;
  gi_data += gi_jstep;
  gj_data += gj_jstep;

  for (unsigned j=1;j<nj1;++j)
  {
    const float* s = s_data;
    float* pgi = gi_data;
    float* pgj = gj_data;

    // Zero the first elements in the rows
    *pgi = 0; pgi+=gi_istep;
    *pgj = 0; pgj+=gj_istep;

    for (unsigned i=1;i<ni1;++i)
    {
    // Compute gradient in i
      *pgi = 0.5f*(s[o5]-s[o4]);
    // Compute gradient in j
      *pgj = 0.5f*(s[o2]-s[o7]);

      s+=s_istep;
      pgi += gi_istep;
      pgj += gj_istep;
    }

    // Zero the last elements in the rows
    *pgi = 0;
    *pgj = 0;

    // Move to next row
    s_data  += s_jstep;
    gi_data += gi_jstep;
    gj_data += gj_jstep;
  }

  // Zero the first and last rows
  for (unsigned i=0;i<ni;++i)
  {
    *gi=0; gi+=gi_istep;
    *gj=0; gj+=gj_istep;
    *gi_data = 0; gi_data+=gi_istep;
    *gj_data = 0; gj_data+=gj_istep;
  }
}

//: Compute gradients of single plane of 2D data using 1x3 Sobel filters
//  Computes both x and j gradients of an nx x nj plane of data
void vil_sobel_1x3_1plane(const double* src,
                          vcl_ptrdiff_t s_istep, vcl_ptrdiff_t s_jstep,
                          double* gi, vcl_ptrdiff_t gi_istep, vcl_ptrdiff_t gi_jstep,
                          double* gj, vcl_ptrdiff_t gj_istep, vcl_ptrdiff_t gj_jstep,
                          unsigned ni, unsigned nj)
{
  const double* s_data = src;
  double *gi_data = gi;
  double *gj_data = gj;

  if (ni==0 || nj==0) return;
  if (ni==1)
  {
      // Zero the elements in the column
    for (unsigned j=0;j<nj;++j)
    {
      *gi_data = 0;
      *gj_data = 0;
      gi_data += gi_jstep;
      gj_data += gj_jstep;
    }
    return;
  }
  if (nj==1)
  {
      // Zero the elements in the column
    for (unsigned i=0;i<ni;++i)
    {
      *gi_data = 0;
      *gj_data = 0;
      gi_data += gi_istep;
      gj_data += gj_istep;
    }
    return;
  }

  // Compute relative grid positions
  //     o2
  //  o4    o5
  //     o7
  const vcl_ptrdiff_t o2 = s_jstep;
  const vcl_ptrdiff_t o4 = -s_istep;
  const vcl_ptrdiff_t o5 = s_istep;
  const vcl_ptrdiff_t o7 = -s_jstep;

  const unsigned ni1 = ni-1;
  const unsigned nj1 = nj-1;

  s_data += s_istep + s_jstep;
  gi_data += gi_jstep;
  gj_data += gj_jstep;

  for (unsigned j=1;j<nj1;++j)
  {
    const double* s = s_data;
    double* pgi = gi_data;
    double* pgj = gj_data;

    // Zero the first elements in the rows
    *pgi = 0; pgi+=gi_istep;
    *pgj = 0; pgj+=gj_istep;

    for (unsigned i=1;i<ni1;++i)
    {
    // Compute gradient in i
      *pgi = 0.5*(s[o5]-s[o4]);
    // Compute gradient in j
      *pgj = 0.5*(s[o2]-s[o7]);

      s+=s_istep;
      pgi += gi_istep;
      pgj += gj_istep;
    }

    // Zero the last elements in the rows
    *pgi = 0;
    *pgj = 0;

    // Move to next row
    s_data  += s_jstep;
    gi_data += gi_jstep;
    gj_data += gj_jstep;
  }

  // Zero the first and last rows
  for (unsigned i=0;i<ni;++i)
  {
    *gi=0; gi+=gi_istep;
    *gj=0; gj+=gj_istep;
    *gi_data = 0; gi_data+=gi_istep;
    *gj_data = 0; gj_data+=gj_istep;
  }
}

