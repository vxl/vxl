//: \file
//  \brief Apply gradient operator to 2D planes of data
//  \author Tim Cootes

#include <vnl/vnl_math.h>

//: Compute gradients of single plane of 2D data using 3x3 Sobel filters
//  Computes both x and y gradients of an nx x ny plane of data
void mil_algo_grad_3x3(float* gx, int gx_xstep, int gx_ystep,
                       float* gy, int gy_xstep, int gy_ystep,
                       const unsigned char* src,
             int s_xstep, int s_ystep, int nx, int ny)
{
  const unsigned char* s_data = src;
  float *gx_data = gx;
  float *gy_data = gy;

  // Compute relative grid positions
  //  o1 o2 o3
  //  o4    o5
  //  o6 07 o8
  int o1 = s_ystep - s_xstep;
  int o2 = -s_ystep;
  int o3 = s_xstep + s_ystep;
  int o4 = -s_xstep;
  int o5 = s_xstep;
  int o6 = -s_xstep - s_ystep;
  int o7 = -s_ystep;
  int o8 = s_xstep - s_ystep;

  int nx1 = nx-1;
  int ny1 = ny-1;

  s_data += s_xstep + s_ystep;
  gx_data += gx_ystep;
  gy_data += gy_ystep;

  for (int y=1;y<ny1;++y)
  {
    const unsigned char* s = s_data;
  float* pgx = gx_data;
  float* pgy = gy_data;

  // Zero the first elements in the rows
  *pgx = 0; pgx+=gx_xstep;
  *pgy = 0; pgy+=gy_xstep;

  for (int x=1;x<nx1;++x)
  {
    // Compute gradient in x
      *pgx = float(s[o3]) + 2*float(s[o5]) + float(s[o8])
           - (float(s[o1]) + 2*float(s[o4]) + float(s[o6]));
    // Compute gradient in y
      *pgy = float(s[o1]) + 2*float(s[o2]) + float(s[o3])
           - (float(s[o6]) + 2*float(s[o7]) + float(s[o8]));

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
  for (int x=0;x<nx;++x)
  {
    *gx=0; gx+=gx_xstep;
    *gy=0; gy+=gy_xstep;
  *gx_data = 0; gx_data+=gx_xstep;
  *gy_data = 0; gy_data+=gy_xstep;
  }
}

//: Compute gradients of single plane of 2D data using 3x3 Sobel filters
//  Computes both x and y gradients of an nx x ny plane of data
void mil_algo_grad_3x3(float* gx, int gx_xstep, int gx_ystep,
                       float* gy, int gy_xstep, int gy_ystep,
                       const float* src,
             int s_xstep, int s_ystep, int nx, int ny)
{
  const float* s_data = src;
  float *gx_data = gx;
  float *gy_data = gy;

  // Compute relative grid positions
  //  o1 o2 o3
  //  o4    o5
  //  o6 07 o8
  int o1 = s_ystep - s_xstep;
  int o2 = -s_ystep;
  int o3 = s_xstep + s_ystep;
  int o4 = -s_xstep;
  int o5 = s_xstep;
  int o6 = -s_xstep - s_ystep;
  int o7 = -s_ystep;
  int o8 = s_xstep - s_ystep;

  int nx1 = nx-1;
  int ny1 = ny-1;

  s_data += s_xstep + s_ystep;
  gx_data += gx_ystep;
  gy_data += gy_ystep;

  for (int y=1;y<ny1;++y)
  {
    const float* s = s_data;
  float* pgx = gx_data;
  float* pgy = gy_data;

  // Zero the first elements in the rows
  *pgx = 0; pgx+=gx_xstep;
  *pgy = 0; pgy+=gy_xstep;

  for (int x=1;x<nx1;++x)
  {
    // Compute gradient in x
      *pgx = s[o3] + 2*s[o5] + s[o8] - (s[o1] + 2*s[o4] + s[o6]);
    // Compute gradient in y
      *pgy = s[o1] + 2*s[o2] + s[o3] - (s[o6] + 2*s[o7] + s[o8]);

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
  for (int x=0;x<nx;++x)
  {
    *gx=0; gx+=gx_xstep;
    *gy=0; gy+=gy_xstep;
  *gx_data = 0; gx_data+=gx_xstep;
  *gy_data = 0; gy_data+=gy_xstep;
  }
}

