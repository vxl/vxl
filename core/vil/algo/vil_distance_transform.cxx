#include <algorithm>
#include <cmath>
#include "vil_distance_transform.h"
//:
// \file
// \brief Compute distance function
// \author Tim Cootes

#include <vil/vil_fill.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//: Compute distance function from zeros in original image
//  Image is assumed to be filled with max_dist where there
//  is background, and zero at the places of interest.
//  On exit, the values are the 8-connected distance to the
//  nearest original zero region.
void vil_distance_transform(vil_image_view<float>& image)
{
  // Low to high pass
  vil_distance_transform_one_way(image);

  // Flip to achieve high to low pass
  // Don't use vil_flip* as they assume const images.
  unsigned ni = image.ni(), nj = image.nj();
  vil_image_view<float> flip_image(image.memory_chunk(),
                                   &image(ni-1,nj-1), ni,nj,1,
                                   -image.istep(), -image.jstep(),
                                   image.nplanes());
  vil_distance_transform_one_way(flip_image);
}

//: Compute directed distance function from zeros in original image
//  Image is assumed to be filled with max_dist where there
//  is background, and zero at the places of interest.
//  On exit, the values are the 8-connected distance to the
//  nearest original zero region above or to the left of current point.
//  One pass of distance transform, going from low to high i,j.
void vil_distance_transform_one_way(vil_image_view<float>& image)
{
  assert(image.nplanes()==1);
  unsigned ni = image.ni();
  unsigned nj = image.nj();
  unsigned ni1 = ni-1;
  std::ptrdiff_t istep = image.istep(),  jstep = image.jstep();
  std::ptrdiff_t o1 = -istep, o2 = -jstep-istep, o3 = -jstep, o4 = istep-jstep;
  float* row0 = image.top_left_ptr();

  const float sqrt2 = std::sqrt(2.0f);

  // Process the first row
  float* p0 = row0+istep;
  for (unsigned i=1;i<ni;++i,p0+=istep)
  {
      *p0 = std::min(p0[-istep]+1.0f,*p0);
  }

  row0 += jstep;  // Move to next row

  // Process each subsequent row from low to high values of j
  for (unsigned j=1;j<nj;++j,row0+=jstep)
  {
    // Check first element against first two in previous row
    *row0 = std::min(row0[o3]+1.0f,*row0);
    *row0 = std::min(row0[o4]+sqrt2,*row0);

    float* p0 = row0+istep;
    for (unsigned i=1;i<ni1;++i,p0+=istep)
    {
      *p0 = std::min(p0[o1]+1.0f ,*p0); // (-1,0)
      *p0 = std::min(p0[o2]+sqrt2,*p0); // (-1,-1)
      *p0 = std::min(p0[o3]+1.0f ,*p0); // (0,-1)
      *p0 = std::min(p0[o4]+sqrt2,*p0); // (1,-1)
    }

    // Check last element in row
    *p0 = std::min(p0[o1]+1.0f ,*p0); // (-1,0)
    *p0 = std::min(p0[o2]+sqrt2,*p0); // (-1,-1)
    *p0 = std::min(p0[o3]+1.0f ,*p0); // (0,-1)
  }
}

//: Compute distance function from true elements in mask
//  On exit, the values are the 8-connected distance to the
//  nearest original zero region (or max_dist, if that is smaller).
void vil_distance_transform(const vil_image_view<bool>& mask,
                            vil_image_view<float>& distance_image,
                            float max_dist)
{
  distance_image.set_size(mask.ni(),mask.nj());
  distance_image.fill(max_dist);
  vil_fill_mask(distance_image,mask,0.0f);

  vil_distance_transform(distance_image);
}

//: Distance function, using neighbours +/-2 in x,y
//  More accurate than vil_distance_function_one_way
void vil_distance_transform_r2_one_way(vil_image_view<float>& image)
{
  assert(image.nplanes()==1);
  unsigned ni = image.ni();
  unsigned nj = image.nj();
  unsigned ni2 = ni-2;
  std::ptrdiff_t istep = image.istep(),  jstep = image.jstep();

  //   Kernel defining points to consider (relative to XX)
  //   -- o6 -- o7 --
  //   o5 o2 o3 o4 o8
  //   -- o1 XX -- --
  std::ptrdiff_t o1 = -istep, o2 = -jstep-istep;
  std::ptrdiff_t o3 = -jstep, o4 = istep-jstep;
  std::ptrdiff_t o5 = -2*istep-jstep;
  std::ptrdiff_t o6 = -istep-2*jstep;
  std::ptrdiff_t o7 =  istep-2*jstep;
  std::ptrdiff_t o8 =  2*istep-jstep;

  float* row0 = image.top_left_ptr();

  const float sqrt2 = std::sqrt(2.0f);
  const float sqrt5 = std::sqrt(5.0f);

  // Process the first row
  float* p0 = row0+istep;
  for (unsigned i=1;i<ni;++i,p0+=istep)
  {
      *p0 = std::min(p0[-istep]+1.0f,*p0);
  }

  row0 += jstep;  // Move to next row

  // ==== Process second row ====
  // Check first element against elements in previous row
  *row0 = std::min(row0[o3]+1.0f,*row0);  // (0,-1)
  *row0 = std::min(row0[o4]+sqrt5,*row0); // (1,-1)
  *row0 = std::min(row0[o8]+sqrt5,*row0); // (2,-1)

  p0 = row0+istep;  // Move to element 1
  *p0 = std::min(p0[o1]+1.0f,*p0); // (-1,0)
  *p0 = std::min(p0[o2]+sqrt2,*p0); // (-1,-1)
  *p0 = std::min(p0[o3]+1.0f,*p0); // (0,-1)
  *p0 = std::min(p0[o4]+sqrt2,*p0); // (1,-1)
  *p0 = std::min(p0[o8]+sqrt5,*p0); // (2,-1)

  p0+=istep;  // Move to element 2
  for (unsigned i=2;i<ni2;++i,p0+=istep)
  {
    *p0 = std::min(p0[o1]+1.0f,*p0); // (-1,0)
    *p0 = std::min(p0[o2]+sqrt2,*p0); // (-1,-1)
    *p0 = std::min(p0[o3]+1.0f,*p0); // (0,-1)
    *p0 = std::min(p0[o4]+sqrt2,*p0); // (1,-1)
    *p0 = std::min(p0[o5]+sqrt5,*p0); // (-2,-1)
    *p0 = std::min(p0[o8]+sqrt5,*p0); // (2,-1)
  }

  // Check element ni-2
  *p0 = std::min(p0[o1]+1.0f,*p0); // (-1,0)
  *p0 = std::min(p0[o2]+sqrt2,*p0); // (-1,-1)
  *p0 = std::min(p0[o3]+1.0f,*p0); // (0,-1)
  *p0 = std::min(p0[o4]+sqrt2,*p0); // (1,-1)
  *p0 = std::min(p0[o5]+sqrt5,*p0); // (-2,-1)

  p0+=istep;  // Move to element ni-1
  // Check last element in row
  *p0 = std::min(p0[o1]+1.0f,*p0); // (-1,0)
  *p0 = std::min(p0[o2]+sqrt2,*p0); // (-1,-1)
  *p0 = std::min(p0[o3]+1.0f,*p0); // (0,-1)
  *p0 = std::min(p0[o5]+sqrt5,*p0); // (-2,-1)

  row0 += jstep;  // Move to next row (2)

  // Process each subsequent row from low to high values of j
  for (unsigned j=2;j<nj;++j,row0+=jstep)
  {
    // Check first element
    *row0 = std::min(row0[o3]+1.0f,*row0);  // (0,-1)
    *row0 = std::min(row0[o4]+sqrt2,*row0); // (1,-1)
    *row0 = std::min(row0[o7]+sqrt5,*row0); // (1,-2)
    *row0 = std::min(row0[o8]+sqrt5,*row0); // (2,-1)

    float* p0 = row0+istep;  // Element 1
    // Check second element, allowing for boundary conditions
    *p0 = std::min(p0[o1]+1.0f,*p0); // (-1,0)
    *p0 = std::min(p0[o2]+sqrt2,*p0); // (-1,-1)
    *p0 = std::min(p0[o3]+1.0f,*p0); // (0,-1)
    *p0 = std::min(p0[o4]+sqrt2,*p0); // (1,-1)
    *p0 = std::min(p0[o6]+sqrt5,*p0); // (-1,-2)
    *p0 = std::min(p0[o7]+sqrt5,*p0); // (1,-2)
    *p0 = std::min(p0[o8]+sqrt5,*p0); // (2,-1)

    p0+=istep;  // Move to next element (2)
    for (unsigned i=2;i<ni2;++i,p0+=istep)
    {
      *p0 = std::min(p0[o1]+1.0f,*p0); // (-1,0)
      *p0 = std::min(p0[o2]+sqrt2,*p0); // (-1,-1)
      *p0 = std::min(p0[o3]+1.0f,*p0); // (0,-1)
      *p0 = std::min(p0[o4]+sqrt2,*p0); // (1,-1)
      *p0 = std::min(p0[o5]+sqrt5,*p0); // (-2,-1)
      *p0 = std::min(p0[o6]+sqrt5,*p0); // (-1,-2)
      *p0 = std::min(p0[o7]+sqrt5,*p0); // (1,-2)
      *p0 = std::min(p0[o8]+sqrt5,*p0); // (2,-1)
    }
    // p0 points to element (ni-2,y)

    // Check last but one element in row
    *p0 = std::min(p0[o1]+1.0f,*p0); // (-1,0)
    *p0 = std::min(p0[o2]+sqrt2,*p0); // (-1,-1)
    *p0 = std::min(p0[o3]+1.0f,*p0); // (0,-1)
    *p0 = std::min(p0[o4]+sqrt2,*p0); // (1,-1)
    *p0 = std::min(p0[o5]+sqrt5,*p0); // (-2,-1)
    *p0 = std::min(p0[o6]+sqrt5,*p0); // (-1,-2)
    *p0 = std::min(p0[o7]+sqrt5,*p0); // (1,-2)

    p0+=istep; // Move to last element (ni-1,y)
    // Process last element in row
    *p0 = std::min(p0[o1]+1.0f,*p0); // (-1,0)
    *p0 = std::min(p0[o2]+sqrt2,*p0); // (-1,-1)
    *p0 = std::min(p0[o3]+1.0f,*p0); // (0,-1)
    *p0 = std::min(p0[o5]+sqrt5,*p0); // (-2,-1)
    *p0 = std::min(p0[o6]+sqrt5,*p0); // (-1,-2)
  }
}

//: Compute distance function from zeros in original image
//  Image is assumed to be filled with max_dist where there
//  is background, and zero at the places of interest.
//  On exit, the values are the 24-connected distance to the
//  nearest original zero region. (ie considers neighbours in
//  a +/-2 pixel region around each point).
//  More accurate than vil_distance_transform(image), but
//  approximately twice the processing required.
void vil_distance_transform_r2(vil_image_view<float>& image)
{
  // Low to high pass
  vil_distance_transform_r2_one_way(image);

  // Flip to achieve high to low pass
  // Don't use vil_flip* as they assume const images.
  unsigned ni = image.ni(), nj = image.nj();
  vil_image_view<float> flip_image(image.memory_chunk(),
                                   &image(ni-1,nj-1), ni,nj,1,
                                   -image.istep(), -image.jstep(),
                                   image.nplanes());
  vil_distance_transform_r2_one_way(flip_image);
}
