//:
// \file
// \brief Compute distance transform using chamfer distance
// \author Kola Babalola

#include <vil3d/algo/vil3d_distance_transform.h>
#include <vil3d/vil3d_image_view.h>
#include <vcl_cassert.h>
#include <vcl_algorithm.h>
#include <vil3d/algo/vil3d_fill_boundary.h>
#include <vil3d/algo/vil3d_threshold.h>
#include <vil3d/vil3d_slice.h>
#include <vil/vil_fill.h>


//: Compute signed distance transform in 3d from zeros in original image.
//  Image is assumed to be filled with max_dist
//  where there is background, and zero in the places of interest.
//  On exit, the values are the signed 26-connected distance
//  to the nearest original zero region. Positive values are
//  outside the bounded region and negative values are inside.
//  The values on the boundary are zero
void vil3d_signed_distance_transform(vil3d_image_view<float>& image)
{
  unsigned ni = image.ni(),nj = image.nj(), nk = image.nk();
  unsigned nplanes = image.nplanes();
  // calculate a binary mask in which interior voxels are "on"
  // need to convert the image to a short datatype with "1" on boundary
  vil3d_image_view<bool> image2(ni,nj,nk,nplanes);
  vil3d_threshold_below(image,image2,0.1f);
  vil3d_fill_boundary(image2);

  // calculate the distance transform as usual
  vil3d_distance_transform(image);

  // set all voxels in mask to negative values
  vcl_ptrdiff_t istep0 = image.istep();
  vcl_ptrdiff_t istep2 = image2.istep();
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
        vcl_cout << img(i,j) << ' ';
      vcl_cout << vcl_endl;
    }
}

template<class T> void print_values(const vil3d_image_view<T> &img)
{
  for (int k = 0; k < img.nk(); k++)
    for (int j = 0; j < img.nj(); j++)
    {
      for (int i = 0; i < img.ni(); i++)
        vcl_cout << img(i,j,k) << ' ';
      vcl_cout << vcl_endl;
    }
}


//: Compute 3d signed distance transform from true elements in mask.
//  On exit, values are 26 connected distance from the 'true' boundary.
//  There are no zero values because the true boundary is the infinitismally
//  thin edge of the true and false regions of the mask. The values inside
//  the mask are negative and those outside are positive
void vil3d_signed_distance_transform(const vil3d_image_view<bool>& mask,
                   vil3d_image_view<float>& image,
                   float max_dist)
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
  vil3d_distance_transform(image);
  vil3d_distance_transform(image_inverse);

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
void vil3d_distance_transform(vil3d_image_view<float>& image)
{
  // Low to high pass
  vil3d_distance_transform_one_way(image);

  // Flip to achieve high to low pass
  unsigned ni = image.ni(), nj = image.nj(), nk = image.nk();
  vil3d_image_view<float> flip_image(image.memory_chunk(),
                    &image(ni-1,nj-1,nk-1), ni,nj,nk,1,
                    -image.istep(), -image.jstep(), -image.kstep(),
                    image.nplanes());
  vil3d_distance_transform_one_way(flip_image);
}

//: Compute directed 3D distance function from zeros in original image.
//  Image is assumed to be filled with max_dist where there is
//  background, and zero at the places of interest.
//  On exit, the values are the 8-connected distance to the nearest
//  original zero region above or to the left of current point.
//  One pass of distance transform, going from low to high i,j,k.
void vil3d_distance_transform_one_way(vil3d_image_view<float> &image)
{
  assert(image.nplanes()==1);
  unsigned ni = image.ni();
  unsigned nj = image.nj();
  unsigned nk = image.nk();
  unsigned ni2 = ni-2;
  unsigned nj2 = nj-2;

  vcl_ptrdiff_t istep = image.istep(), jstep = image.jstep(), kstep = image.kstep();
  vcl_ptrdiff_t o1 = -istep, o2 = -jstep-istep, o3 = -jstep, o4 = -jstep+istep;
  vcl_ptrdiff_t o5 = -kstep, o6 = -kstep-istep, o7 = -kstep-jstep-istep;
  vcl_ptrdiff_t o8 = -kstep-jstep, o9 = -kstep-jstep+istep, o10 = -kstep+istep;
  vcl_ptrdiff_t o11 = -kstep+jstep+istep, o12 = -kstep+jstep, o13 = -kstep+jstep-istep;

  float *page0 = image.origin_ptr();

  const float sqrt3 = 1.7320508f;
  const float sqrt2 = 1.4142135f;

  // Process the first page
  float *p0 = page0 + istep;

  // Process the first row of first page
  for (unsigned i=0;i<=ni2;++i,p0+=istep)
  {
    *p0 = vcl_min(p0[-istep]+1.0f,*p0);
  }

  // Process subsequent rows of first page
  float *row0 = page0+jstep;

  for (unsigned j=1;j<nj;++j,row0+=jstep)
  {
    // for first column - special case
    *row0 = vcl_min(row0[o3]+1.0f,*row0);
    *row0 = vcl_min(row0[o4]+sqrt2,*row0);

    // for subsequent columns
    float *p0 = row0+istep;
    for (unsigned i=0;i<ni2;i++,p0+=istep)
    {
      *p0 = vcl_min(p0[o1]+1.0f ,*p0);  // (-1,0)
      *p0 = vcl_min(p0[o2]+sqrt2,*p0);  // (-1,-1)
      *p0 = vcl_min(p0[o3]+1.0f ,*p0);  // (0,-1)
      *p0 = vcl_min(p0[o4]+sqrt2,*p0);  // (1,-1)
    }

    // for last column - special case
    *p0 = vcl_min(p0[o1]+1.0f ,*p0);  // (-1,0)
    *p0 = vcl_min(p0[o2]+sqrt2,*p0);  // (-1,-1)
    *p0 = vcl_min(p0[o3]+1.0f ,*p0);  // (0,-1)
  }

  // process subsequent pages
  page0 += kstep;
  for (unsigned k=1;k<nk;k++,page0+=kstep)
  {
    row0 = page0;

    // first row is still special, and this is first column of first row
    *row0 = vcl_min(row0[o5] +1.0f, *row0);
    *row0 = vcl_min(row0[o10]+sqrt2,*row0);
    *row0 = vcl_min(row0[o11]+sqrt3,*row0);
    *row0 = vcl_min(row0[o12]+sqrt2,*row0);

    float *p0 = row0+istep;
    // subsequent columns of first row
    for (unsigned i=0;i<ni2;i++,p0+=istep)
    {
      *p0 = vcl_min(p0[o1] +1.0f ,*p0);
      *p0 = vcl_min(p0[o5] +1.0f ,*p0);
      *p0 = vcl_min(p0[o6] +sqrt2,*p0);
      *p0 = vcl_min(p0[o10]+sqrt2,*p0);
      *p0 = vcl_min(p0[o11]+sqrt3,*p0);
      *p0 = vcl_min(p0[o12]+sqrt2,*p0);
      *p0 = vcl_min(p0[o13]+sqrt3,*p0);
    }

    // last column of first row
    *p0 = vcl_min(p0[o1] +1.0f ,*p0);
    *p0 = vcl_min(p0[o5] +1.0f ,*p0);
    *p0 = vcl_min(p0[o6] +sqrt2,*p0);
    *p0 = vcl_min(p0[o12]+sqrt2,*p0);
    *p0 = vcl_min(p0[o13]+sqrt3,*p0);

    // process subsequent rows
    row0 += jstep;

    for (unsigned j=0;j<nj2;j++,row0+=jstep)
    {
      // again first column is special case
      *row0 = vcl_min(row0[o3] +1.0f, *row0);
      *row0 = vcl_min(row0[o4] +sqrt2,*row0);
      *row0 = vcl_min(row0[o5] +1.0f, *row0);
      *row0 = vcl_min(row0[o8] +sqrt2,*row0);
      *row0 = vcl_min(row0[o9] +sqrt3,*row0);
      *row0 = vcl_min(row0[o10]+sqrt2,*row0);
      *row0 = vcl_min(row0[o11]+sqrt3,*row0);
      *row0 = vcl_min(row0[o12]+sqrt2,*row0);

      // process subsequent columns
      p0 = row0 + istep;
      for (unsigned i=0;i<ni2;i++,p0+=istep)
      {
        *p0 = vcl_min(p0[o1] +1.0f, *p0);
        *p0 = vcl_min(p0[o2] +sqrt2,*p0);
        *p0 = vcl_min(p0[o3] +1.0f, *p0);
        *p0 = vcl_min(p0[o4] +sqrt2,*p0);
        *p0 = vcl_min(p0[o5] +1.0f, *p0);
        *p0 = vcl_min(p0[o6] +sqrt2,*p0);
        *p0 = vcl_min(p0[o7] +sqrt3,*p0);
        *p0 = vcl_min(p0[o8] +sqrt2,*p0);
        *p0 = vcl_min(p0[o9] +sqrt3,*p0);
        *p0 = vcl_min(p0[o10]+sqrt2,*p0);
        *p0 = vcl_min(p0[o11]+sqrt3,*p0);
        *p0 = vcl_min(p0[o12]+sqrt2,*p0);
        *p0 = vcl_min(p0[o13]+sqrt3,*p0);
      }

      // last column
      *p0 = vcl_min(p0[o1] +1.0f, *p0);
      *p0 = vcl_min(p0[o2] +sqrt2,*p0);
      *p0 = vcl_min(p0[o3] +1.0f, *p0);
      *p0 = vcl_min(p0[o5] +1.0f, *p0);
      *p0 = vcl_min(p0[o6] +sqrt2,*p0);
      *p0 = vcl_min(p0[o7] +sqrt3,*p0);
      *p0 = vcl_min(p0[o8] +sqrt2,*p0);
      *p0 = vcl_min(p0[o12]+sqrt2,*p0);
      *p0 = vcl_min(p0[o13]+sqrt3,*p0);
    }

    // process last row

    // process fist column of last row
    *row0 = vcl_min(row0[o3] +1.0f, *row0);
    *row0 = vcl_min(row0[o4] +sqrt2,*row0);
    *row0 = vcl_min(row0[o5] +1.0f, *row0);
    *row0 = vcl_min(row0[o8] +sqrt2,*row0);
    *row0 = vcl_min(row0[o9] +sqrt3,*row0);
    *row0 = vcl_min(row0[o10]+sqrt2,*row0);

    // subsequent columns of last row
    p0 = row0 + istep;
    for (unsigned i=0;i<ni2;i++,p0+=istep)
    {
      *p0 = vcl_min(p0[o1] +1.0f, *p0);
      *p0 = vcl_min(p0[o2] +sqrt2,*p0);
      *p0 = vcl_min(p0[o3] +1.0f, *p0);
      *p0 = vcl_min(p0[o4] +sqrt2,*p0);
      *p0 = vcl_min(p0[o5] +1.0f, *p0);
      *p0 = vcl_min(p0[o6] +sqrt2,*p0);
      *p0 = vcl_min(p0[o7] +sqrt3,*p0);
      *p0 = vcl_min(p0[o8] +sqrt2,*p0);
      *p0 = vcl_min(p0[o9] +sqrt3,*p0);
      *p0 = vcl_min(p0[o10]+sqrt2,*p0);
    }

    // last column of last row
    *p0 = vcl_min(p0[o1] +1.0f, *p0);
    *p0 = vcl_min(p0[o2] +sqrt2,*p0);
    *p0 = vcl_min(p0[o3] +1.0f, *p0);
    *p0 = vcl_min(p0[o5] +1.0f, *p0);
    *p0 = vcl_min(p0[o6] +sqrt2,*p0);
    *p0 = vcl_min(p0[o7] +sqrt3,*p0);
    *p0 = vcl_min(p0[o8] +sqrt2,*p0);
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
