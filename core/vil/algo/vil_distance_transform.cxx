//:
//  \file
//  \brief Compute distance function
//  \author Tim Cootes

#include <vil/algo/vil_distance_transform.h>
#include <vil/vil_fill.h>
#include <vcl_algorithm.h>

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
  unsigned ni2 = ni-2;
  vcl_ptrdiff_t istep = image.istep(),  jstep = image.jstep();
  vcl_ptrdiff_t o1 = -istep, o2 = -jstep-istep, o3 = -jstep, o4 = istep-jstep;
  float* row0 = image.top_left_ptr();

  const float sqrt2 = 1.4142135f;

  // Process the first row
  float* p0 = row0+istep;
  for (unsigned i=0;i<=ni2;++i,p0+=istep)
  {
      *p0 = vcl_min(p0[-istep]+1.0f,*p0);
  }

  row0 += jstep;  // Move to next row

  // Process each subsequent row from low to high values of j
  for (unsigned j=1;j<nj;++j,row0+=jstep)
  {
    // Check first element against first two in previous row
    *row0 = vcl_min(row0[o3]+1.0f,*row0);
    *row0 = vcl_min(row0[o4]+sqrt2,*row0);

    float* p0 = row0+istep;
    for (unsigned i=0;i<ni2;++i,p0+=istep)
    {
      *p0 = vcl_min(p0[o1]+1.0f ,*p0); // (-1,0)
      *p0 = vcl_min(p0[o2]+sqrt2,*p0); // (-1,-1)
      *p0 = vcl_min(p0[o3]+1.0f ,*p0); // (0,-1)
      *p0 = vcl_min(p0[o4]+sqrt2,*p0); // (1,-1)
    }

    // Check last element in row
    *p0 = vcl_min(p0[o1]+1.0f ,*p0); // (-1,0)
    *p0 = vcl_min(p0[o2]+sqrt2,*p0); // (-1,-1)
    *p0 = vcl_min(p0[o3]+1.0f ,*p0); // (0,-1)

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

