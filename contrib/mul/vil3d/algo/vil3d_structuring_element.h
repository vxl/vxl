#ifndef vil3d_structuring_element_h_
#define vil3d_structuring_element_h_
//:
// \file
// \brief Structuring element for 3D morphology represented as a list of non-zero pixels
// \author Tim Cootes

#include <vector>
#include <iosfwd>
#include <iostream>
#include <cstddef>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Structuring element for morphology represented as a list of non-zero pixels
// Elements in box bounded by [min_i(),max_i()][min_j(),max_j()]
// Non-zero pixels are given by (p_i[k],p_j[k])
class vil3d_structuring_element
{
  //: i position of elements (i,j,k)
  std::vector<int> p_i_;
  //: j position of elements (i,j,k)
  std::vector<int> p_j_;
  //: k position of elements (i,j,k)
  std::vector<int> p_k_;
  //: i range of elements is [min_i_,max_i_]
  int min_i_;
  //: i range of elements is [min_i_,max_i_]
  int max_i_;
  //: j range of elements is [min_j_,max_j_]
  int min_j_;
  //: j range of elements is [min_j_,max_j_]
  int max_j_;
  //: k range of elements is [min_k_,max_k_]
  int min_k_;
  //: k range of elements is [min_k_,max_k_]
  int max_k_;

 public:
  vil3d_structuring_element()
    : min_i_(0),max_i_(-1),min_j_(0),max_j_(-1),min_k_(0),max_k_(-1) {}

  //: Define elements { (p_i[a],p_j[a],p_k[a]) }
  vil3d_structuring_element(const std::vector<int>& p_i,
                            const std::vector<int>& p_j,
                            const std::vector<int>& p_k);

  //: Define elements { (p_i[a],p_j[a],p_k[a]) }
  void set(const std::vector<int>& p_i,
           const std::vector<int>& p_j,
           const std::vector<int>& p_k);

  //: Set to sphere of radius r
  //  Select pixels in disk s.t. x*x+y*y+z*z<=r^r
  void set_to_sphere(double r);

  //: Set to circle of radius r
  //  Select pixels in circle s.t. y*y+z*z<=r^r
  void set_to_circle_i(double r);

  //: Set to circle of radius r
  //  Select pixels in circle s.t. y*y+z*z<=r^r
  void set_to_circle_j(double r);

  //: Set to circle of radius r
  //  Select pixels in circle s.t. y*y+z*z<=r^r
  void set_to_circle_k(double r);

  //: Set to 6 axis-aligned neighbours plus self
  void set_to_7();

  //: Set to 26 touching neighbours plus self
  void set_to_27();


  //: Set to sphere of radius r, but with non isotropic voxel sizes
  //  Select pixels in disk s.t. (sx*x)^2+(sy*y)^2+(sz*z)^2<=r^2
  void set_to_sphere_noniso(double r, double sx, double sy, double sz);

  //: Set to line along i (ilo,0)..(ihi,0)
  void set_to_line_i(int ilo, int ihi);

  //: Set to line along j (jlo,0)..(jhi,0)
  void set_to_line_j(int jlo, int jhi);

  //: Set to line along k (klo,0)..(khi,0)
  void set_to_line_k(int klo, int khi);

  //: i position of elements (i,j,k)
  const std::vector<int>& p_i() const { return p_i_; }
  //: j position of elements (i,j,k)
  const std::vector<int>& p_j() const { return p_j_; }
  //: k position of elements (i,j,k)
  const std::vector<int>& p_k() const { return p_k_; }

  //: i range of elements is [min_i(),max_i()]
  int min_i() const { return min_i_; }
  //: i range of elements is [min_i_,max_i()]
  int max_i() const { return max_i_; }
  //: j range of elements is [min_j_,max_j()]
  int min_j() const { return min_j_; }
  //: j range of elements is [min_j_,max_j()]
  int max_j() const { return max_j_; }
  //: k range of elements is [min_k(),max_k()]
  int min_k() const { return min_k_; }
  //: k range of elements is [min_k(),max_k()]
  int max_k() const { return max_k_; }
};

//: Write details to stream
std::ostream& operator<<(std::ostream&, const vil3d_structuring_element& element);

//: Generate a list of offsets for use on image with istep,jstep
//  On exit offset[a] = element.p_i()[a]*istep +  element.p_j()[a]*jstep
//                      +  element.p_k()[a]*kstep
//
//  Gives an efficient way of looping through all the pixels in the structuring element
void vil3d_compute_offsets(std::vector<std::ptrdiff_t>& offset,
                           const vil3d_structuring_element& element,
                           std::ptrdiff_t istep,
                           std::ptrdiff_t jstep,
                           std::ptrdiff_t kstep);

#endif // vil3d_structuring_element_h_
