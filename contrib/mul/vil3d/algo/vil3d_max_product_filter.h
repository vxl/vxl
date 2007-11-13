#ifndef vil3d_max_product_filter_h_
#define vil3d_max_product_filter_h_
//:
//  \file
//  \brief Apply non-linear filter to a 3D image in raster order.
//  \author Tim Cootes

#include <vil3d/algo/vil3d_structuring_element.h>
#include <vil3d/vil3d_image_view.h>


//: Return max(im[0],max_i (f[i]*im[offset[i]]))
//  Thus compare current voxel with weighted versions of n offset,
//  voxels, returning the largest value.  
//  Assumes bound checking has already been done
template <class T>
inline T vil3d_max_product_filter(const T* im, 
                                  const vcl_ptrdiff_t* offset, 
                                  const double* f, unsigned n)
{
  T max_v = im[0];
  for (unsigned i=0;i<n;++i) 
  {
    T v = T(f[i]*im[offset[i]]);
    if (v>max_v) max_v=v;
  }
  return max_v;
}

//: Compare voxel (i0,j0,k0) with weighted versions of offset voxels.
// Return max(image(i0,i1,i2),max_i (f[i]*image(i0+p_i[i],j0+p_j[i],k0+p_k[i],plane)
//  Performs bound checks to ensure voxels are valid
template <class T>
inline T vil3d_max_product_filter(const vil3d_image_view<T>& image,
                                const vil3d_structuring_element& se,
                                const vcl_vector<double>& f,
                                int i0, int j0, int k0)
{
  unsigned n = se.p_i().size();
  T max_v = image(i0,j0,k0);

  for (unsigned int a=0;a<n;++a)
  {
    // Note that -ives become huge positives and are thus ignored by the if
    unsigned int i = i0+se.p_i()[a];
    unsigned int j = j0+se.p_j()[a];
    unsigned int k = k0+se.p_k()[a];
    if (i<image.ni() && j<image.nj() && k<image.nk())
    {
      T v = T(f[a]*image(i,j,k));
      if (v>max_v) max_v=v;
    }
  }
  return max_v;
}

//: Preforms raster scan through image, comparing each voxel with nbours
// Runs through every voxel in raster order.  Each voxel is compared
// with weighted versions of offset voxels, and is replaced with the
// maximum value.  
//
// Replace each voxel with 
// max(im(i,j,k),max_a (f[a]*im(i+p_i[a],j+p_j[a],k+p_k[a],plane))
//
// If a suitable combination of se and f is chosen,
// this performs half an 'exponential distance transform'.
// For an example of use, see vil3d_exp_distance_transform.
// \relates vil3d_image_view
// \relates vil3d_structuring_element
template<class T>
void vil3d_max_product_filter(vil3d_image_view<T>& image,
                        const vil3d_structuring_element& se,
                        const vcl_vector<double>& f)
{
  assert(image.nplanes()==1);  // Change this later
  unsigned ni = image.ni();
  unsigned nj = image.nj();
  unsigned nk = image.nk();

  vcl_ptrdiff_t istep = image.istep();
  vcl_ptrdiff_t jstep = image.jstep();
  vcl_ptrdiff_t kstep = image.kstep();

  vcl_vector<vcl_ptrdiff_t> offset;
  vil3d_compute_offsets(offset,se,istep,jstep,kstep);
  const vcl_ptrdiff_t *o_data = &offset[0];
  const double *f_data = &f[0];
  const unsigned n = f.size();
  assert(n==offset.size());

  // Define box in which all elements will be valid
  int ilo = -se.min_i();
  int ihi = ni-1-se.max_i();
  int jlo = -se.min_j();
  int jhi = nj-1-se.max_j();
  int klo = -se.min_k();
  int khi = nk-1-se.max_k();

  // Process first z slices using bound checking
  for (int k=0;k<klo;++k)
  {
    for (int j=0;j<nj;++j)
      for (int i=0;i<ni;++i)
        image(i,j,k)=vil3d_max_product_filter(image,se,f,i,j,k);
  }
  // Process middle z slices (still require i, j checks)
  for (int k=klo;k<=khi;++k)
  {
    // First j require bound checks
    for (int j=0;j<jlo;++j)
      for (int i=0;i<ni;++i)
        image(i,j,k)=vil3d_max_product_filter(image,se,f,i,j,k);
    for (int j=jlo;j<=jhi;++j)
    {
       // first i require bound checks
       for (int i=0;i<ilo;++i) 
         image(i,j,k)=vil3d_max_product_filter(image,se,f,i,j,k);

       T* im = &image(ilo,j,k);

       // Middle values of i require no bound checks
       for (int i=ilo;i<=ihi;++i,im+=istep)
         *im=vil3d_max_product_filter(im,o_data,f_data,n);

       // Last i require bound checks
       for (int i=ihi+1;i<ni;++i) 
         image(i,j,k)=vil3d_max_product_filter(image,se,f,i,j,k);
    }
    // Last j require bound checks
    for (int j=jhi+1;j<nj;++j)
      for (int i=0;i<ni;++i)
        image(i,j,k)=vil3d_max_product_filter(image,se,f,i,j,k);
  }
  // process final z slices using bound checking
  for (int k=khi+1;k<nk;++k)
  {
    for (int j=0;j<nj;++j)
      for (int i=0;i<ni;++i)
        image(i,j,k)=vil3d_max_product_filter(image,se,f,i,j,k);
  }
}


#endif // vil3d_max_product_filter_h_
