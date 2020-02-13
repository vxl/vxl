//:
// \file
// \brief Compute overlaps of images
// \author Tim Cootes

#include "vil3d_overlap.h"

class vil3d_bool_overlap_functor
{
public:
  //: N voxels true in A but not in B
  unsigned nAnotB;

  //: N voxels true in B but not in A
  unsigned nBnotA;

  //: N voxels true in A and B
  unsigned nAandB;

  //: Constructor
  vil3d_bool_overlap_functor()
    : nAnotB(0),nBnotA(0),nAandB(0) {}

  //: Operator function
  void operator()(bool voxA, bool voxB)
  {
    if (voxA)
    {
      if (voxB) nAandB++;
      else      nAnotB++;
    }
    else
      if (voxB) nBnotA++;
  }

  unsigned n_union() const { return nAnotB+ nBnotA + nAandB; }
  unsigned n_intersection() const { return nAandB; }
};


//: Dice overlap = 2*intersection/(intersection+union)
double vil3d_overlap_dice(const vil3d_image_view<bool>& im1,
                        const vil3d_image_view<bool>& im2)
{
  vil3d_bool_overlap_functor f;
  vil3d_scan_image(im1,im2,f);
  unsigned d=f.n_intersection()+f.n_union();
  if (d==0) return 0.0;
  return double(2*f.n_intersection())/d;
}

//: Jaccard overlap = intersection/union
double vil3d_overlap_jaccard(const vil3d_image_view<bool>& im1,
                        const vil3d_image_view<bool>& im2)
{
  vil3d_bool_overlap_functor f;
  vil3d_scan_image(im1,im2,f);
  if (f.n_union()==0) return 0.0;
  return double(f.n_intersection())/f.n_union();
}
