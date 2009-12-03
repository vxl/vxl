// This is mul/vil3d/algo/vil3d_overlap.h
#ifndef vil3d_overlap_h_
#define vil3d_overlap_h_
//:
// \file
// \brief Compute overlaps of images
// \author Tim Cootes


#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_scan_image.h>

//: Dice overlap = 2*intersection/(intersection+union)
double vil3d_overlap_dice(const vil3d_image_view<bool>& im1,
                          const vil3d_image_view<bool>& im2);

//: Jaccard overlap = intersection/union
double vil3d_overlap_jaccard(const vil3d_image_view<bool>& im1,
                             const vil3d_image_view<bool>& im2);

//: Functor to compute overlaps by thresholding voxel values
template <class T1, class T2>
class vil3d_overlap_functor
{
 public:
  //: Threshold for image 1
  T1 t1;
  //: Threshold for image 2
  T2 t2;

  //: N voxels true in 1 but not in 2
  unsigned n1not2;

  //: N voxels true in 2 but not in 1
  unsigned n2not1;

  //: N voxels true in 1 and 2
  unsigned n1and2;

  //: Constructor, defining thresholds for voxels to be true
  vil3d_overlap_functor(T1 t1a, T2 t2a)
    : t1(t1a),t2(t2a),n1not2(0),n2not1(0),n1and2(0) {}

  //: Operator function
  void operator()(T1 vox1, T2 vox2)
  {
    if (vox1>t1)
    {
      if (vox2>t2) n1and2++;
      else         n1not2++;
    }
    else
      if (vox2>t2) n2not1++;
  }

  unsigned n_union() const { return n1not2+ n2not1 + n1and2; }
  unsigned n_intersection() const { return n1and2; }
};

//: Dice overlap = 2*intersection/(intersection+union)
//  Voxel in image A is true if its value is strictly above the threshold t
template <class T1, class T2>
double vil3d_overlap_dice(const vil3d_image_view<T1>& im1, T1 t1,
                          const vil3d_image_view<T2>& im2, T2 t2)
{
  vil3d_overlap_functor<T1,T2> f(t1,t2);
  vil3d_scan_image(im1,im2,f);
  unsigned d=f.n_intersection()+f.n_union();
  if (d==0) return 0.0;
  return (2.0*f.n_intersection())/d;
}

//: Jaccard overlap = intersection/union
//  Voxel in image A is true if its value is strictly above the threshold t
template <class T1, class T2>
double vil3d_overlap_jaccard(const vil3d_image_view<T1>& im1, T1 t1,
                             const vil3d_image_view<T2>& im2, T2 t2)
{
  vil3d_overlap_functor<T1,T2> f(t1,t2);
  vil3d_scan_image(im1,im2,f);
  if (f.n_union()==0) return 0.0;
  return double(f.n_intersection())/f.n_union();
}


#endif // vil3d_overlap_h_
