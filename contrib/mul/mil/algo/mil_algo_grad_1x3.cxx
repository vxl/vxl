//: \file
//  \brief Apply gradient operator to 2D planes of data
//  \author Ian Scott

#include "mil_algo_grad_1x3.h"

//: Compute gradients of single plane of 2D data using simple 1x3 filters
//  Computes both x and y gradients of an nx x ny plane of data
void mil_algo_grad_1x3(float* gx, int gx_xstep, int gx_ystep,
                       float* gy, int gy_xstep, int gy_ystep,
                       const unsigned char* src,
                       int s_xstep, int s_ystep, unsigned nx, unsigned ny)
{
  const unsigned char* s_data = src;
  float *gx_data = gx;
  float *gy_data = gy;

  if (nx==0 || ny==0) return;
  if (nx==1)
  {
      // Zero the elements in the column
    for (unsigned y=0;y<ny;++y)
    {
      *gx_data = 0;
      *gy_data = 0;
      gx_data += gx_ystep;
      gy_data += gy_ystep;
    }
    return;
  }
  if (ny==1)
  {
      // Zero the elements in the column
    for (unsigned x=0;x<nx;++x)
    {
      *gx_data = 0;
      *gy_data = 0;
      gx_data += gx_xstep;
      gy_data += gy_xstep;
    }
    return;
  }

  // Compute relative grid positions
  //     o2
  //  o4    o5
  //     o7
  const int o2 = s_ystep;
  const int o4 = -s_xstep;
  const int o5 = s_xstep;
  const int o7 = -s_ystep;

  const unsigned nx1 = nx-1;
  const unsigned ny1 = ny-1;

  s_data += s_xstep + s_ystep;
  gx_data += gx_ystep;
  gy_data += gy_ystep;

  for (unsigned y=1;y<ny1;++y)
  {
    const unsigned char* s = s_data;
    float* pgx = gx_data;
    float* pgy = gy_data;

    // Zero the first elements in the rows
    *pgx = 0; pgx+=gx_xstep;
    *pgy = 0; pgy+=gy_xstep;

    for (unsigned x=1;x<nx1;++x)
    {
      // Compute gradient in x
      // Note: Multiply each element individually
      //      to ensure conversion to float before addition
        *pgx = 0.5f*s[o5] - 0.5f*s[o4];
      // Compute gradient in y
        *pgy = 0.5f*s[o2] - 0.5f*s[o7];

      s+=s_xstep;
      pgx += gx_xstep;
      pgy += gy_xstep;
    }

    // Zero the last elements in the rows
    *pgx = 0;
    *pgy = 0;

    // Move to next row
    s_data  += s_ystep;
    gx_data += gx_ystep;
    gy_data += gy_ystep;
  }

  // Zero the first and last rows
  for (unsigned x=0;x<nx;++x)
  {
    *gx=0; gx+=gx_xstep;
    *gy=0; gy+=gy_xstep;
    *gx_data = 0; gx_data+=gx_xstep;
    *gy_data = 0; gy_data+=gy_xstep;
  }
}

//: Compute gradients of single plane of 2D data using simple 1x3 filters
//  Computes both x and y gradients of an nx x ny plane of data
void mil_algo_grad_1x3(float* gx, int gx_xstep, int gx_ystep,
                       float* gy, int gy_xstep, int gy_ystep,
                       const float* src,
                       int s_xstep, int s_ystep, unsigned nx, unsigned ny)
{
  const float* s_data = src;
  float *gx_data = gx;
  float *gy_data = gy;

  if (nx==0 || ny==0) return;
  if (nx==1)
  {
      // Zero the elements in the column
    for (unsigned y=0;y<ny;++y)
    {
      *gx_data = 0;
      *gy_data = 0;
      gx_data += gx_ystep;
      gy_data += gy_ystep;
    }
    return;
  }
  if (ny==1)
  {
      // Zero the elements in the column
    for (unsigned x=0;x<nx;++x)
    {
      *gx_data = 0;
      *gy_data = 0;
      gx_data += gx_xstep;
      gy_data += gy_xstep;
    }
    return;
  }


  // Compute relative grid positions
  //     o2
  //  o4    o5
  //     o7
  int o2 = s_ystep;
  int o4 = -s_xstep;
  int o5 = s_xstep;
  int o7 = -s_ystep;

  const unsigned nx1 = nx-1;
  const unsigned ny1 = ny-1;

  s_data += s_xstep + s_ystep;
  gx_data += gx_ystep;
  gy_data += gy_ystep;

  for (unsigned y=1;y<ny1;++y)
  {
    const float* s = s_data;
    float* pgx = gx_data;
    float* pgy = gy_data;

    // Zero the first elements in the rows
    *pgx = 0; pgx+=gx_xstep;
    *pgy = 0; pgy+=gy_xstep;

    for (unsigned x=1;x<nx1;++x)
    {
      // Compute gradient in x
        *pgx = 0.5f*(s[o5]-s[o4]);
      // Compute gradient in y
        *pgy = 0.5f*(s[o2]-s[o7]);

      s+=s_xstep;
      pgx += gx_xstep;
      pgy += gy_xstep;
    }

    // Zero the last elements in the rows
    *pgx = 0;
    *pgy = 0;

    // Move to next row
    s_data  += s_ystep;
    gx_data += gx_ystep;
    gy_data += gy_ystep;
  }

  // Zero the first and last rows
  for (unsigned x=0;x<nx;++x)
  {
    *gx=0; gx+=gx_xstep;
    *gy=0; gy+=gy_xstep;
    *gx_data = 0; gx_data+=gx_xstep;
    *gy_data = 0; gy_data+=gy_xstep;
  }
}

