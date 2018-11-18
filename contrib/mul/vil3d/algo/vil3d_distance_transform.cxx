#include <iostream>
#include <algorithm>
#include "vil3d_distance_transform.h"
//:
// \file
// \brief Compute distance transform using chamfer distance
// \author Kola Babalola

#include <vil3d/vil3d_image_view.h>
#include <vil3d/algo/vil3d_fill_boundary.h>
#include <vil3d/algo/vil3d_threshold.h>
#include <vil3d/vil3d_slice.h>
#include <vil/vil_fill.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Compute signed distance transform in 3d from zeros in original image.
//  Image is assumed to be filled with max_dist
//  where there is background, and zero in the places of interest.
//  On exit, the values are the signed 26-connected distance
//  to the nearest original zero region. Positive values are
//  outside the bounded region and negative values are inside.
//  The values on the boundary are zero
void vil3d_signed_distance_transform(vil3d_image_view<float>& image, const float distance_link_i, const float distance_link_j, const float distance_link_k)
{
  unsigned ni = image.ni(),nj = image.nj(), nk = image.nk();
  unsigned nplanes = image.nplanes();
  // calculate a binary mask in which interior voxels are "on"
  // need to convert the image to a short datatype with "1" on boundary
  vil3d_image_view<bool> image2(ni,nj,nk,nplanes);
  vil3d_threshold_below(image,image2,0.1f);
  vil3d_fill_boundary(image2);

  // calculate the distance transform as usual
  vil3d_distance_transform(image,distance_link_i,distance_link_j,distance_link_k);

  // set all voxels in mask to negative values
  std::ptrdiff_t istep0 = image.istep();
  std::ptrdiff_t istep2 = image2.istep();
  float *p0 = image.origin_ptr();
  bool *p1 = image2.origin_ptr();

  for (unsigned int k=0; k<nk; ++k)
    for (unsigned int j=0; j<nj; ++j)
      for (unsigned int i=0; i<ni; ++i,p0+=istep0,p1+=istep2)
      {
        if (*p1)
          *p0 *= -1.0f;
      }
}

//  Print function to help debugging
template<class T> void print_values(const vil_image_view<T> &img)
{
    for (int j = 0; j < img.nj(); j++)
    {
      for (int i = 0; i < img.ni(); i++)
        std::cout << img(i,j) << ' ';
      std::cout << std::endl;
    }
}

template<class T> void print_values(const vil3d_image_view<T> &img)
{
  for (int k = 0; k < img.nk(); k++)
    for (int j = 0; j < img.nj(); j++)
    {
      for (int i = 0; i < img.ni(); i++)
        std::cout << img(i,j,k) << ' ';
      std::cout << std::endl;
    }
}


//: Compute 3d signed distance transform from true elements in mask.
//  On exit, values are 26 connected distance from the 'true' boundary.
//  There are no zero values because the true boundary is the infinitesimally
//  thin edge of the true and false regions of the mask. The values inside
//  the mask are negative and those outside are positive
void vil3d_signed_distance_transform(const vil3d_image_view<bool>& mask,
                                     vil3d_image_view<float>& image,
                                     float max_dist,
                                     const float distance_link_i,
                                     const float distance_link_j,
                                     const float distance_link_k)
{
  image.set_size(mask.ni(),mask.nj(),mask.nk(),mask.nplanes());
  image.fill(max_dist);

  unsigned ni = image.ni();
  unsigned nj = image.nj();
  unsigned nk = image.nk(), k;
  unsigned nplanes = image.nplanes();

  vil_image_view<bool> mask_slice(mask.ni(),mask.nk(),mask.nplanes());
  vil_image_view<float> image_slice(ni,nj,nplanes);

  image.fill(max_dist);

  vil3d_image_view<float> image_inverse(ni,nj,nk,nplanes);
  image_inverse.fill(0.0f);

  for (k=0;k<nk;k++)
  {
    mask_slice = vil3d_slice_ij(mask,k);
    image_slice = vil3d_slice_ij(image,k);
    vil_fill_mask(image_slice,mask_slice,0.0f);
    image_slice = vil3d_slice_ij(image_inverse,k);
    vil_fill_mask(image_slice,mask_slice,max_dist);
  }

  // find distance transform of image and inverse image and subtract
  vil3d_distance_transform(image,distance_link_i,distance_link_j,distance_link_k);
  vil3d_distance_transform(image_inverse,distance_link_i,distance_link_j,distance_link_k);

  float *p0 = image.origin_ptr();
  float *p1 = image_inverse.origin_ptr();

  for (unsigned int k=0; k<nk; ++k)
    for (unsigned int j=0; j<nj; ++j)
      for (unsigned int i=0; i<ni; ++i,p0+=image.istep(),p1+=image_inverse.istep())
        *p0 -= *p1;
}

//: Compute distance transform in 3d from zeros in original image.
//  Image is assumed to be filled with max_dist where
//  there is background, and zero in the places of interest.
//  On exit, the values are the 8-connected distance to the
//  nearest original zero region.
void vil3d_distance_transform(vil3d_image_view<float>& image, const float distance_link_i, const float distance_link_j, const float distance_link_k)
{
  // Low to high pass
  vil3d_distance_transform_one_way(image,distance_link_i,distance_link_j,distance_link_k);

  // Flip to achieve high to low pass
  unsigned ni = image.ni(), nj = image.nj(), nk = image.nk();
  vil3d_image_view<float> flip_image(image.memory_chunk(),
                                     &image(ni-1,nj-1,nk-1), ni,nj,nk,1,
                                     -image.istep(), -image.jstep(), -image.kstep(),
                                     image.nplanes());
  vil3d_distance_transform_one_way(flip_image,distance_link_i,distance_link_j,distance_link_k);
}

//: Compute distance transform in 3d from zeros in original image.
//  Image is assumed to be filled with max_dist where
//  there is background, and zero in the places of interest.
//  On exit, the values are the 8-connected distance to the
//  nearest original zero region.
void vil3d_distance_transform_with_dir(vil3d_image_view<float>& image,
                                       vil3d_image_view<vil_rgb<float> >& orient,
                                       const float distance_link_i,
                                       const float distance_link_j,
                                       const float distance_link_k)
{
  // Low to high pass
  vil3d_distance_transform_one_way_with_dir(image,orient,distance_link_i,distance_link_j,distance_link_k);

  // Flip to achieve high to low pass
  unsigned ni = image.ni(), nj = image.nj(), nk = image.nk();
  vil3d_image_view<float> flip_image(image.memory_chunk(),
                                     &image(ni-1,nj-1,nk-1), ni,nj,nk,1,
                                     -image.istep(), -image.jstep(), -image.kstep(),
                                     image.nplanes());

  // reverse the directions
  vil3d_image_view<vil_rgb<float> >::iterator it = orient.begin();
  while (it != orient.end()) {
    vil_rgb<float>& v = *it;
    v*=-1.0;
    it++;
  }

  //Flip the directions too
  vil3d_image_view<vil_rgb<float> > flip_orient(orient.memory_chunk(),
                                                &orient(ni-1,nj-1,nk-1), ni,nj,nk,1,
                                                -orient.istep(), -orient.jstep(), -orient.kstep(),
                                                orient.nplanes());


  vil3d_distance_transform_one_way_with_dir(flip_image,flip_orient,distance_link_i,distance_link_j,distance_link_k);
}

//: Compute directed 3D distance function from zeros in original image.
//  Image is assumed to be filled with max_dist where there is
//  background, and zero at the places of interest.
//  On exit, the values are the 8-connected distance to the nearest
//  original zero region above or to the left of current point.
//  One pass of distance transform, going from low to high i,j,k.
void vil3d_distance_transform_one_way(vil3d_image_view<float>& image,
                                      const float distance_link_i,
                                      const float distance_link_j,
                                      const float distance_link_k)
{
  assert(image.nplanes()==1);
  unsigned ni = image.ni();
  unsigned nj = image.nj();
  unsigned nk = image.nk();
  unsigned ni2 = ni-2;
  unsigned nj2 = nj-2;

  std::ptrdiff_t istep = image.istep(), jstep = image.jstep(), kstep = image.kstep();
  std::ptrdiff_t o1 = -istep, o2 = -jstep-istep, o3 = -jstep, o4 = -jstep+istep;
  std::ptrdiff_t o5 = -kstep, o6 = -kstep-istep, o7 = -kstep-jstep-istep;
  std::ptrdiff_t o8 = -kstep-jstep, o9 = -kstep-jstep+istep, o10 = -kstep+istep;
  std::ptrdiff_t o11 = -kstep+jstep+istep, o12 = -kstep+jstep, o13 = -kstep+jstep-istep;

  float *page0 = image.origin_ptr();

  const float distance_link_ij=std::sqrt(distance_link_i*distance_link_i+distance_link_j*distance_link_j);
  const float distance_link_ik=std::sqrt(distance_link_i*distance_link_i+distance_link_k*distance_link_k);
  const float distance_link_jk=std::sqrt(distance_link_j*distance_link_j+distance_link_k*distance_link_k);
  const float distance_link_ijk=std::sqrt(distance_link_i*distance_link_i+distance_link_j*distance_link_j+distance_link_k*distance_link_k);

  // Process the first page
  float *p0 = page0 + istep;

  // Process the first row of first page
  for (unsigned i=0;i<=ni2;++i,p0+=istep)
  {
    *p0 = std::min(p0[-istep]+distance_link_i,*p0);
  }

  // Process subsequent rows of first page
  float *row0 = page0+jstep;

  for (unsigned j=1;j<nj;++j,row0+=jstep)
  {
    // for first column - special case
    *row0 = std::min(row0[o3]+distance_link_j,*row0);
    *row0 = std::min(row0[o4]+distance_link_ij,*row0);

    // for subsequent columns
    float *p0 = row0+istep;
    for (unsigned i=0;i<ni2;i++,p0+=istep)
    {
      *p0 = std::min(p0[o1]+distance_link_i ,*p0);  // (-1,0)
      *p0 = std::min(p0[o2]+distance_link_ij,*p0);  // (-1,-1)
      *p0 = std::min(p0[o3]+distance_link_j ,*p0);  // (0,-1)
      *p0 = std::min(p0[o4]+distance_link_ij,*p0);  // (1,-1)
    }

    // for last column - special case
    *p0 = std::min(p0[o1]+distance_link_i ,*p0);  // (-1,0)
    *p0 = std::min(p0[o2]+distance_link_ij,*p0);  // (-1,-1)
    *p0 = std::min(p0[o3]+distance_link_j ,*p0);  // (0,-1)
  }

  // process subsequent pages
  page0 += kstep;
  for (unsigned k=1;k<nk;k++,page0+=kstep)
  {
    row0 = page0;

    // first row is still special, and this is first column of first row
    *row0 = std::min(row0[o5] +distance_link_k, *row0);
    *row0 = std::min(row0[o10]+distance_link_ik,*row0);
    *row0 = std::min(row0[o11]+distance_link_ijk,*row0);
    *row0 = std::min(row0[o12]+distance_link_jk,*row0);

    float *p0 = row0+istep;
    // subsequent columns of first row
    for (unsigned i=0;i<ni2;i++,p0+=istep)
    {
      *p0 = std::min(p0[o1] +distance_link_i ,*p0);
      *p0 = std::min(p0[o5] +distance_link_k ,*p0);
      *p0 = std::min(p0[o6] +distance_link_ik,*p0);
      *p0 = std::min(p0[o10]+distance_link_ik,*p0);
      *p0 = std::min(p0[o11]+distance_link_ijk,*p0);
      *p0 = std::min(p0[o12]+distance_link_jk,*p0);
      *p0 = std::min(p0[o13]+distance_link_ijk,*p0);
    }

    // last column of first row
    *p0 = std::min(p0[o1] +distance_link_i ,*p0);
    *p0 = std::min(p0[o5] +distance_link_k ,*p0);
    *p0 = std::min(p0[o6] +distance_link_ik,*p0);
    *p0 = std::min(p0[o12]+distance_link_jk,*p0);
    *p0 = std::min(p0[o13]+distance_link_ijk,*p0);

    // process subsequent rows
    row0 += jstep;

    for (unsigned j=0;j<nj2;j++,row0+=jstep)
    {
      // again first column is special case
      *row0 = std::min(row0[o3] +distance_link_j, *row0);
      *row0 = std::min(row0[o4] +distance_link_ij,*row0);
      *row0 = std::min(row0[o5] +distance_link_k, *row0);
      *row0 = std::min(row0[o8] +distance_link_jk,*row0);
      *row0 = std::min(row0[o9] +distance_link_ijk,*row0);
      *row0 = std::min(row0[o10]+distance_link_ik,*row0);
      *row0 = std::min(row0[o11]+distance_link_ijk,*row0);
      *row0 = std::min(row0[o12]+distance_link_jk,*row0);

      // process subsequent columns
      p0 = row0 + istep;
      for (unsigned i=0;i<ni2;i++,p0+=istep)
      {
        *p0 = std::min(p0[o1] +distance_link_i, *p0);
        *p0 = std::min(p0[o2] +distance_link_ij,*p0);
        *p0 = std::min(p0[o3] +distance_link_j, *p0);
        *p0 = std::min(p0[o4] +distance_link_ij,*p0);
        *p0 = std::min(p0[o5] +distance_link_k, *p0);
        *p0 = std::min(p0[o6] +distance_link_ik,*p0);
        *p0 = std::min(p0[o7] +distance_link_ijk,*p0);
        *p0 = std::min(p0[o8] +distance_link_jk,*p0);
        *p0 = std::min(p0[o9] +distance_link_ijk,*p0);
        *p0 = std::min(p0[o10]+distance_link_ik,*p0);
        *p0 = std::min(p0[o11]+distance_link_ijk,*p0);
        *p0 = std::min(p0[o12]+distance_link_jk,*p0);
        *p0 = std::min(p0[o13]+distance_link_ijk,*p0);
      }

      // last column
      *p0 = std::min(p0[o1] +distance_link_i, *p0);
      *p0 = std::min(p0[o2] +distance_link_ij,*p0);
      *p0 = std::min(p0[o3] +distance_link_j, *p0);
      *p0 = std::min(p0[o5] +distance_link_k, *p0);
      *p0 = std::min(p0[o6] +distance_link_ik,*p0);
      *p0 = std::min(p0[o7] +distance_link_ijk,*p0);
      *p0 = std::min(p0[o8] +distance_link_jk,*p0);
      *p0 = std::min(p0[o12]+distance_link_jk,*p0);
      *p0 = std::min(p0[o13]+distance_link_ijk,*p0);
    }

    // process last row

    // process fist column of last row
    *row0 = std::min(row0[o3] +distance_link_j, *row0);
    *row0 = std::min(row0[o4] +distance_link_ij,*row0);
    *row0 = std::min(row0[o5] +distance_link_k, *row0);
    *row0 = std::min(row0[o8] +distance_link_jk,*row0);
    *row0 = std::min(row0[o9] +distance_link_ijk,*row0);
    *row0 = std::min(row0[o10]+distance_link_ik,*row0);

    // subsequent columns of last row
    p0 = row0 + istep;
    for (unsigned i=0;i<ni2;i++,p0+=istep)
    {
      *p0 = std::min(p0[o1] +distance_link_i, *p0);
      *p0 = std::min(p0[o2] +distance_link_ij,*p0);
      *p0 = std::min(p0[o3] +distance_link_j, *p0);
      *p0 = std::min(p0[o4] +distance_link_ij,*p0);
      *p0 = std::min(p0[o5] +distance_link_k, *p0);
      *p0 = std::min(p0[o6] +distance_link_ik,*p0);
      *p0 = std::min(p0[o7] +distance_link_ijk,*p0);
      *p0 = std::min(p0[o8] +distance_link_jk,*p0);
      *p0 = std::min(p0[o9] +distance_link_ijk,*p0);
      *p0 = std::min(p0[o10]+distance_link_ik,*p0);
    }

    // last column of last row
    *p0 = std::min(p0[o1] +distance_link_i, *p0);
    *p0 = std::min(p0[o2] +distance_link_ij,*p0);
    *p0 = std::min(p0[o3] +distance_link_j, *p0);
    *p0 = std::min(p0[o5] +distance_link_k, *p0);
    *p0 = std::min(p0[o6] +distance_link_ik,*p0);
    *p0 = std::min(p0[o7] +distance_link_ijk,*p0);
    *p0 = std::min(p0[o8] +distance_link_jk,*p0);
  }
}

float vil3d_min_comp(float const& a, float const& b, bool& comp)
{
  if (a < b)
    comp=true;
  else
    comp=false;
  return std::min(a,b);
}

//: Compute directed 3D distance function from zeros in original image.
//  Image is assumed to be filled with max_dist where there is
//  background, and zero at the places of interest.
//  Directions are assumed to be filled with max_dist.
//  On exit, the values are the 8-connected distance to the nearest
//  original zero region above or to the left of current point. The
//  direction shows the relative position of the closest pixel with value 0.
//  e.g. pixel (10,10,10) has vector (-2,-2,-2), so the closest 0 is at (8,8,8).
//  One pass of distance transform, going from low to high i,j,k.
void vil3d_distance_transform_one_way_with_dir(vil3d_image_view<float>& image,
                                               vil3d_image_view<vil_rgb<float> >& orient,
                                               const float distance_link_i,
                                               const float distance_link_j,
                                               const float distance_link_k)
{
  assert(image.nplanes()==1);
  unsigned ni = image.ni();
  unsigned nj = image.nj();
  unsigned nk = image.nk();
  assert(ni == orient.ni() && nj == orient.nj() && nk == orient.nk());
  unsigned ni2 = ni-2;
  unsigned nj2 = nj-2;

  std::ptrdiff_t istep = image.istep(), jstep = image.jstep(), kstep = image.kstep();
  std::ptrdiff_t orient_istep = orient.istep(), orient_jstep = orient.jstep(), orient_kstep = orient.kstep();

  std::ptrdiff_t o1 = -istep, o2 = -jstep-istep, o3 = -jstep, o4 = -jstep+istep;
  std::ptrdiff_t o5 = -kstep, o6 = -kstep-istep, o7 = -kstep-jstep-istep;
  std::ptrdiff_t o8 = -kstep-jstep, o9 = -kstep-jstep+istep, o10 = -kstep+istep;
  std::ptrdiff_t o11 = -kstep+jstep+istep, o12 = -kstep+jstep, o13 = -kstep+jstep-istep;

  std::ptrdiff_t oo1 = -orient_istep;
  std::ptrdiff_t oo2 = -orient_jstep-orient_istep;
  std::ptrdiff_t oo3 = -orient_jstep;
  std::ptrdiff_t oo4 = -orient_jstep+orient_istep;
  std::ptrdiff_t oo5 = -orient_kstep;
  std::ptrdiff_t oo6 = -orient_kstep-orient_istep;
  std::ptrdiff_t oo7 = -orient_kstep-orient_jstep-orient_istep;
  std::ptrdiff_t oo8 = -orient_kstep-orient_jstep;
  std::ptrdiff_t oo9 = -orient_kstep-orient_jstep+orient_istep;
  std::ptrdiff_t oo10 = -orient_kstep+orient_istep;
  std::ptrdiff_t oo11 = -orient_kstep+orient_jstep+orient_istep;
  std::ptrdiff_t oo12 = -orient_kstep+orient_jstep;
  std::ptrdiff_t oo13 = -orient_kstep+orient_jstep-orient_istep;


  // distance vectors to the neighbors of a pixel
  vil_rgb<float> v1(1,0,0);
  vil_rgb<float> v2(1,1,0);
  vil_rgb<float> v3(0,1,0);
  vil_rgb<float> v4(-1,1,0);
  vil_rgb<float> v5(0,0,1);
  vil_rgb<float> v6(1,0,1);
  vil_rgb<float> v7(1,1,1);
  vil_rgb<float> v8(0,1,1);
  vil_rgb<float> v9(-1,1,1);
  vil_rgb<float> v10(-1,0,1);
  vil_rgb<float> v11(-1,-1,1);
  vil_rgb<float> v12(0,-1,1);
  vil_rgb<float> v13(1,-1,1);

  float *page0 = image.origin_ptr();
  vil_rgb<float> *orient_page0 = orient.origin_ptr();

  const float distance_link_ij=std::sqrt(distance_link_i*distance_link_i+distance_link_j*distance_link_j);
  const float distance_link_ik=std::sqrt(distance_link_i*distance_link_i+distance_link_k*distance_link_k);
  const float distance_link_jk=std::sqrt(distance_link_j*distance_link_j+distance_link_k*distance_link_k);
  const float distance_link_ijk=std::sqrt(distance_link_i*distance_link_i+distance_link_j*distance_link_j+distance_link_k*distance_link_k);

  // Process the first page
  float *p0 = page0 + istep;
  vil_rgb<float> *orient_p0 = orient_page0 + orient_istep;

  bool found;  // set to true if a smaller value found and to be replaced at the pixel

  // Process the first row of first page
  for (unsigned i=0;i<=ni2;++i,p0+=istep,orient_p0+=orient_istep)
  {
    *p0 = vil3d_min_comp(p0[-istep]+distance_link_i,*p0,found);
    if (found) *orient_p0 = orient_p0[-orient_istep]+v1;
  }

  // Process subsequent rows of first page
  float *row0 = page0+jstep;
  vil_rgb<float> *orient_row0 = orient_page0 + orient_jstep;

  for (unsigned j=1;j<nj;++j,row0+=jstep,orient_row0+=orient_jstep)
  {
    // for first column - special case
    *row0 = vil3d_min_comp(row0[o3]+distance_link_j,*row0,found);
    if (found) *orient_row0=orient_row0[oo3]+v3;

    *row0 = vil3d_min_comp(row0[o4]+distance_link_ij,*row0,found);
    if (found) *orient_row0=orient_row0[oo4]+v4;

    // for subsequent columns
    float *p0 = row0+istep;
    vil_rgb<float> *orient_p0 = orient_row0 + orient_istep;
    for (unsigned i=0;i<ni2;i++,p0+=istep,orient_p0+=orient_istep)
    {
      *p0 = vil3d_min_comp(p0[o1]+distance_link_i ,*p0,found);  // (-1,0)
      if (found) *orient_p0=orient_p0[oo1]+v1;
      *p0 = vil3d_min_comp(p0[o2]+distance_link_ij,*p0,found);  // (-1,-1)
      if (found) *orient_p0=orient_p0[oo2]+v2;
      *p0 = vil3d_min_comp(p0[o3]+distance_link_j ,*p0,found);  // (0,-1)
      if (found) *orient_p0=orient_p0[oo3]+v3;
      *p0 = vil3d_min_comp(p0[o4]+distance_link_ij,*p0,found);  // (1,-1)
      if (found) *orient_p0=orient_p0[oo4]+v4;
    }

    // for last column - special case
    *p0 = vil3d_min_comp(p0[o1]+distance_link_i ,*p0,found);  // (-1,0)
    if (found) *orient_p0=orient_p0[oo1]+v1;
    *p0 = vil3d_min_comp(p0[o2]+distance_link_ij,*p0,found);  // (-1,-1)
    if (found) *orient_p0=orient_p0[oo2]+v2;
    *p0 = vil3d_min_comp(p0[o3]+distance_link_j ,*p0,found);  // (0,-1)
    if (found) *orient_p0=orient_p0[oo3]+v3;
  }

  // process subsequent pages
  page0 += kstep;
  orient_page0+=orient_kstep;
  for (unsigned k=1;k<nk;k++,page0+=kstep, orient_page0+=orient_kstep)
  {
    row0 = page0;
    orient_row0 = orient_page0;
    // first row is still special, and this is first column of first row
    *row0 = vil3d_min_comp(row0[o5] +distance_link_k, *row0,found);
    if (found) *orient_row0 = orient_row0[oo5]+v5;
    *row0 = vil3d_min_comp(row0[o10]+distance_link_ik,*row0,found);
    if (found) *orient_row0 = orient_row0[oo10]+v10;
    *row0 = vil3d_min_comp(row0[o11]+distance_link_ijk,*row0,found);
    if (found) *orient_row0 = orient_row0[oo11]+v11;
    *row0 = vil3d_min_comp(row0[o12]+distance_link_jk,*row0,found);
    if (found) *orient_row0 = orient_row0[oo12]+v12;

    float *p0 = row0+istep;
    vil_rgb<float> *orient_p0 = orient_row0+orient_istep;
    // subsequent columns of first row
    for (unsigned i=0;i<ni2;i++,p0+=istep,orient_p0+=orient_istep)
    {
      *p0 = vil3d_min_comp(p0[o1] +distance_link_i ,*p0,found);
      if (found) *orient_p0=orient_p0[oo1]+v1;
      *p0 = vil3d_min_comp(p0[o5] +distance_link_k ,*p0,found);
      if (found) *orient_p0=orient_p0[oo5]+v5;
      *p0 = vil3d_min_comp(p0[o6] +distance_link_ik,*p0,found);
      if (found) *orient_p0=orient_p0[oo6]+v6;
      *p0 = vil3d_min_comp(p0[o10]+distance_link_ik,*p0,found);
      if (found) *orient_p0=orient_p0[oo10]+v10;
      *p0 = vil3d_min_comp(p0[o11]+distance_link_ijk,*p0,found);
      if (found) *orient_p0=orient_p0[oo11]+v11;
      *p0 = vil3d_min_comp(p0[o12]+distance_link_jk,*p0,found);
      if (found) *orient_p0=orient_p0[oo12]+v12;
      *p0 = vil3d_min_comp(p0[o13]+distance_link_ijk,*p0,found);
      if (found) *orient_p0=orient_p0[oo13]+v13;
    }

    // last column of first row
    *p0 = vil3d_min_comp(p0[o1] +distance_link_i ,*p0,found);
    if (found) *orient_p0=orient_p0[oo1]+v1;
    *p0 = vil3d_min_comp(p0[o5] +distance_link_k ,*p0,found);
    if (found) *orient_p0=orient_p0[oo5]+v5;
    *p0 = vil3d_min_comp(p0[o6] +distance_link_ik,*p0,found);
    if (found) *orient_p0=orient_p0[oo6]+v6;
    *p0 = vil3d_min_comp(p0[o12]+distance_link_jk,*p0,found);
    if (found) *orient_p0=orient_p0[oo12]+v12;
    *p0 = vil3d_min_comp(p0[o13]+distance_link_ijk,*p0,found);
    if (found) *orient_p0=orient_p0[oo13]+v13;

    // process subsequent rows
    row0 += jstep;
    orient_row0 += orient_jstep;
    for (unsigned j=0;j<nj2;j++,row0+=jstep,orient_row0+=orient_jstep)
    {
      // again first column is special case
      *row0 = vil3d_min_comp(row0[o3] +distance_link_j, *row0,found);
      if (found) *orient_row0=orient_row0[oo3]+v3;
      *row0 = vil3d_min_comp(row0[o4] +distance_link_ij,*row0,found);
      if (found) *orient_row0=orient_row0[oo4]+v4;
      *row0 = vil3d_min_comp(row0[o5] +distance_link_k, *row0,found);
      if (found) *orient_row0=orient_row0[oo5]+v5;
      *row0 = vil3d_min_comp(row0[o8] +distance_link_jk,*row0,found);
      if (found) *orient_row0=orient_row0[oo8]+v8;
      *row0 = vil3d_min_comp(row0[o9] +distance_link_ijk,*row0,found);
      if (found) *orient_row0=orient_row0[oo9]+v9;
      *row0 = vil3d_min_comp(row0[o10]+distance_link_ik,*row0,found);
      if (found) *orient_row0=orient_row0[oo10]+v10;
      *row0 = vil3d_min_comp(row0[o11]+distance_link_ijk,*row0,found);
      if (found) *orient_row0=orient_row0[oo11]+v11;
      *row0 = vil3d_min_comp(row0[o12]+distance_link_jk,*row0,found);
      if (found) *orient_row0=orient_row0[oo12]+v12;

      // process subsequent columns
      p0 = row0 + istep;
      orient_p0 = orient_row0 + orient_istep;
      for (unsigned i=0;i<ni2;i++,p0+=istep,orient_p0+=orient_istep)
      {
        *p0 = vil3d_min_comp(p0[o1] +distance_link_i, *p0,found);
        if (found) *orient_p0=orient_p0[oo1]+v1;
        *p0 = vil3d_min_comp(p0[o2] +distance_link_ij,*p0,found);
        if (found) *orient_p0=orient_p0[oo2]+v2;
        *p0 = vil3d_min_comp(p0[o3] +distance_link_j, *p0,found);
        if (found) *orient_p0=orient_p0[oo3]+v3;
        *p0 = vil3d_min_comp(p0[o4] +distance_link_ij,*p0,found);
        if (found) *orient_p0=orient_p0[oo4]+v4;
        *p0 = vil3d_min_comp(p0[o5] +distance_link_k, *p0,found);
        if (found) *orient_p0=orient_p0[oo5]+v5;
        *p0 = vil3d_min_comp(p0[o6] +distance_link_ik,*p0,found);
        if (found) *orient_p0=orient_p0[oo6]+v6;
        *p0 = vil3d_min_comp(p0[o7] +distance_link_ijk,*p0,found);
        if (found) *orient_p0=orient_p0[oo7]+v7;
        *p0 = vil3d_min_comp(p0[o8] +distance_link_jk,*p0,found);
        if (found) *orient_p0=orient_p0[oo8]+v8;
        *p0 = vil3d_min_comp(p0[o9] +distance_link_ijk,*p0,found);
        if (found) *orient_p0=orient_p0[oo9]+v9;
        *p0 = vil3d_min_comp(p0[o10]+distance_link_ik,*p0,found);
        if (found) *orient_p0=orient_p0[oo10]+v10;
        *p0 = vil3d_min_comp(p0[o11]+distance_link_ijk,*p0,found);
        if (found) *orient_p0=orient_p0[oo11]+v11;
        *p0 = vil3d_min_comp(p0[o12]+distance_link_jk,*p0,found);
        if (found) *orient_p0=orient_p0[oo12]+v12;
        *p0 = vil3d_min_comp(p0[o13]+distance_link_ijk,*p0,found);
        if (found) *orient_p0=orient_p0[oo13]+v13;
      }

      // last column
      *p0 = vil3d_min_comp(p0[o1] +distance_link_i, *p0,found);
      if (found) *orient_p0=orient_p0[oo1]+v1;
      *p0 = vil3d_min_comp(p0[o2] +distance_link_ij,*p0,found);
      if (found) *orient_p0=orient_p0[oo2]+v2;
      *p0 = vil3d_min_comp(p0[o3] +distance_link_j, *p0,found);
      if (found) *orient_p0=orient_p0[oo3]+v3;
      *p0 = vil3d_min_comp(p0[o5] +distance_link_k, *p0,found);
      if (found) *orient_p0=orient_p0[oo5]+v5;
      *p0 = vil3d_min_comp(p0[o6] +distance_link_ik,*p0,found);
      if (found) *orient_p0=orient_p0[oo6]+v6;
      *p0 = vil3d_min_comp(p0[o7] +distance_link_ijk,*p0,found);
      if (found) *orient_p0=orient_p0[oo7]+v7;
      *p0 = vil3d_min_comp(p0[o8] +distance_link_jk,*p0,found);
      if (found) *orient_p0=orient_p0[oo8]+v8;
      *p0 = vil3d_min_comp(p0[o12]+distance_link_jk,*p0,found);
      if (found) *orient_p0=orient_p0[oo12]+v12;
      *p0 = vil3d_min_comp(p0[o13]+distance_link_ijk,*p0,found);
      if (found) *orient_p0=orient_p0[oo13]+v13;
    }

    // process last row

    // process fist column of last row
    *row0 = vil3d_min_comp(row0[o3] +distance_link_j, *row0,found);
    if (found) *orient_row0=orient_row0[oo3]+v3;
    *row0 = vil3d_min_comp(row0[o4] +distance_link_ij,*row0,found);
    if (found) *orient_row0=orient_row0[oo4]+v4;
    *row0 = vil3d_min_comp(row0[o5] +distance_link_k, *row0,found);
    if (found) *orient_row0=orient_row0[oo5]+v5;
    *row0 = vil3d_min_comp(row0[o8] +distance_link_jk,*row0,found);
    if (found) *orient_row0=orient_row0[oo8]+v8;
    *row0 = vil3d_min_comp(row0[o9] +distance_link_ijk,*row0,found);
    if (found) *orient_row0=orient_row0[oo9]+v9;
    *row0 = vil3d_min_comp(row0[o10]+distance_link_ik,*row0,found);
    if (found) *orient_row0=orient_row0[oo10]+v10;

    // subsequent columns of last row
    p0 = row0 + istep;
    orient_p0 = orient_row0 + orient_istep;
    for (unsigned i=0;i<ni2;i++,p0+=istep,orient_p0+=orient_istep)
    {
      *p0 = vil3d_min_comp(p0[o1] +distance_link_i, *p0,found);
      if (found) *orient_p0=orient_p0[oo1]+v1;
      *p0 = vil3d_min_comp(p0[o2] +distance_link_ij,*p0,found);
      if (found) *orient_p0=orient_p0[oo2]+v2;
      *p0 = vil3d_min_comp(p0[o3] +distance_link_j, *p0,found);
      if (found) *orient_p0=orient_p0[oo3]+v3;
      *p0 = vil3d_min_comp(p0[o4] +distance_link_ij,*p0,found);
      if (found) *orient_p0=orient_p0[oo4]+v4;
      *p0 = vil3d_min_comp(p0[o5] +distance_link_k, *p0,found);
      if (found) *orient_p0=orient_p0[oo5]+v5;
      *p0 = vil3d_min_comp(p0[o6] +distance_link_ik,*p0,found);
      if (found) *orient_p0=orient_p0[oo6]+v6;
      *p0 = vil3d_min_comp(p0[o7] +distance_link_ijk,*p0,found);
      if (found) *orient_p0=orient_p0[oo7]+v7;
      *p0 = vil3d_min_comp(p0[o8] +distance_link_jk,*p0,found);
      if (found) *orient_p0=orient_p0[oo8]+v8;
      *p0 = vil3d_min_comp(p0[o9] +distance_link_ijk,*p0,found);
      if (found) *orient_p0=orient_p0[oo9]+v9;
      *p0 = vil3d_min_comp(p0[o10]+distance_link_ik,*p0,found);
      if (found) *orient_p0=orient_p0[oo10]+v10;
    }

    // last column of last row
    *p0 = vil3d_min_comp(p0[o1] +distance_link_i, *p0,found);
    if (found) *orient_p0=orient_p0[oo1]+v1;
    *p0 = vil3d_min_comp(p0[o2] +distance_link_ij,*p0,found);
    if (found) *orient_p0=orient_p0[oo2]+v2;
    *p0 = vil3d_min_comp(p0[o3] +distance_link_j, *p0,found);
    if (found) *orient_p0=orient_p0[oo3]+v3;
    *p0 = vil3d_min_comp(p0[o5] +distance_link_k, *p0,found);
    if (found) *orient_p0=orient_p0[oo5]+v5;
    *p0 = vil3d_min_comp(p0[o6] +distance_link_ik,*p0,found);
    if (found) *orient_p0=orient_p0[oo6]+v6;
    *p0 = vil3d_min_comp(p0[o7] +distance_link_ijk,*p0,found);
    if (found) *orient_p0=orient_p0[oo7]+v7;
    *p0 = vil3d_min_comp(p0[o8] +distance_link_jk,*p0,found);
    if (found) *orient_p0=orient_p0[oo8]+v8;
  }
}


//: Compute 3D distance function from true elements in mask.
//  On exit, the values are the 8-connected distance to the
//  nearest original zero region (or max_dist, if that is smaller).
void vil3d_distance_transform(const vil3d_image_view<bool>& mask,
                              vil3d_image_view<float>& distance_image,
                              float max_dist)
{
  distance_image.set_size(mask.ni(),mask.nj(),mask.nk());
  distance_image.fill(max_dist);
  //vil3d_fill_mask(distance_image,mask,0.0f); // function not yet written

  vil3d_distance_transform(distance_image);
}
