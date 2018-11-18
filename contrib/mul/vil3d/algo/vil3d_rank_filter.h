#ifndef vil3d_rank_filter_h_
#define vil3d_rank_filter_h_
//:
// \file
// \brief Perform median filtering on 3D images
// \author Tim Cootes

#include <iostream>
#include <algorithm>
#include <vil3d/algo/vil3d_structuring_element.h>
#include <vil3d/vil3d_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Return r-th sorted value of im[offset[k]]
//  Values im[offset[k]] placed into values[k] then sorted.
//  values should be a random access iterator into a container of T such
//  that the range [values,values+n) is valid.
template <class T, class Iter>
inline T vil3d_sorted_value(const T* im, const std::ptrdiff_t* offset, Iter values,
                          unsigned n, unsigned r)
{
  Iter v = values;
  for (unsigned i=0;i<n;++i,++v) *v=im[offset[i]];
  std::nth_element(values, values+r, values+n);
  return values[r];
}

//: Return (n*r)-th sorted value of pixels under element centred at (i0,j0,k0)
// \param r in [0,1].
// \param values used to store values sampled from image before sorting
// Checks boundary overlap
// \relatesalso vil3d_image_view
// \relatesalso vil3d_structuring_element
template <class T>
inline T vil3d_sorted_value(const vil3d_image_view<T>& image, unsigned plane,
                          const vil3d_structuring_element& element, int i0, int j0, int k0,
                          std::vector<T>& values, double r)
{
  values.clear();
  std::size_t n = element.p_i().size();
  for (std::size_t a=0;a<n;++a)
  {
    unsigned i = i0+element.p_i()[a];
    unsigned j = j0+element.p_j()[a];
    unsigned k = k0+element.p_k()[a];
    if (i<image.ni() && j<image.nj() && k<image.nk())
      values.push_back(image(i,j,k,plane));
  }
  std::nth_element(values.begin(),values.begin()+std::size_t(r*(values.size()-1)),
    values.end());
  return values[std::size_t(r*(values.size()-1))];
}

//: Apply rank filter to a 3D image
//  Each voxel in the output is the n-th ranked voxel
//  in the region under the structuring element, where n = r*volume_of_element
template<class T>
inline void vil3d_rank_filter(const vil3d_image_view<T>& src_image,
                        vil3d_image_view<T>& dest_image,
                        const vil3d_structuring_element& element,
                        double r)
{
  assert(src_image.nplanes()==1);
  unsigned ni = src_image.ni(); assert(ni>0);
  unsigned nj = src_image.nj(); assert(nj>0);
  unsigned nk = src_image.nk(); assert(nk>0);
  dest_image.set_size(ni,nj,nk,1);

  std::ptrdiff_t s_istep = src_image.istep(),  s_jstep = src_image.jstep();
  std::ptrdiff_t s_kstep = src_image.kstep();
  std::ptrdiff_t d_istep = dest_image.istep();

  std::vector<std::ptrdiff_t> offset;
  vil3d_compute_offsets(offset,element,s_istep,s_jstep,s_kstep);

  // Define box in which all elements will be valid
  int ilo = -element.min_i();
  int ihi = ni-1-element.max_i();
  int jlo = -element.min_j();
  int jhi = nj-1-element.max_j();
  int klo = -element.min_k();
  int khi = nk-1-element.max_k();

  std::vector<T> value_wkspce;

  // ========= Deal with edges ============
  //  i<ilo
  for (int i=0;i<ilo;++i)
    for (int j=0;j<nj;++j)
      for (int k=0;k<nk;++k)
        dest_image(i,j,k)=vil3d_sorted_value(src_image,0,element,i,j,k,value_wkspce,r);

  //  i>ihi
  for (int i=ihi+1;i<ni;++i)
    for (int j=0;j<nj;++j)
      for (int k=0;k<nk;++k)
        dest_image(i,j,k)=vil3d_sorted_value(src_image,0,element,i,j,k,value_wkspce,r);

  //  j<jlo
  for (int i=ilo;i<=ihi;++i)
    for (int j=0;j<jlo;++j)
      for (int k=0;k<nk;++k)
        dest_image(i,j,k)=vil3d_sorted_value(src_image,0,element,i,j,k,value_wkspce,r);

  //  j>jhi
  for (int i=ilo;i<=ihi;++i)
    for (int j=jhi+1;j<nj;++j)
      for (int k=0;k<nk;++k)
        dest_image(i,j,k)=vil3d_sorted_value(src_image,0,element,i,j,k,value_wkspce,r);

  //  k<klo
  for (int i=ilo;i<=ihi;++i)
    for (int j=jlo;j<=jhi;++j)
      for (int k=0;k<klo;++k)
        dest_image(i,j,k)=vil3d_sorted_value(src_image,0,element,i,j,k,value_wkspce,r);

  //  k>khi
  for (int i=ilo;i<=ihi;++i)
    for (int j=jlo;j<=jhi;++j)
      for (int k=khi+1;k<nk;++k)
        dest_image(i,j,k)=vil3d_sorted_value(src_image,0,element,i,j,k,value_wkspce,r);

  // Do the interior
  // No bounds checks in the interior, so we must make sure there is enough space in
  // the workspace.
  value_wkspce.resize( offset.size() );
  int rank = int(r*(offset.size()-1));

  for (int k=klo;k<=khi;++k)
    for (int j=jlo;j<=jhi;++j)
    {
      const T* src_p = &src_image(ilo,j,k);
      T* dest_p = &dest_image(ilo,j,k);

      for (int i=ilo;i<=ihi;++i,src_p+=s_istep,dest_p+=d_istep)
        *dest_p=vil3d_sorted_value(src_p,&offset[0],value_wkspce.begin(),offset.size(),rank);
    }

}

//: Apply rank filter to a 3D image
//  Each voxel in the output is the n-th ranked voxel
//  in the region under the structuring element, where n = r*volume_of_element
template<class T>
inline void vil3d_median_filter(const vil3d_image_view<T>& src_image,
                        vil3d_image_view<T>& dest_image,
                        const vil3d_structuring_element& element)
{
  vil3d_rank_filter(src_image,dest_image,element,0.5);
}

#endif // vil3d_rank_filter_h_
