// This is brl/bseg/bvpl/kernels/bvpl_taylor_basis_factory.h
#ifndef bvpl_taylor_basis_factory_h
#define bvpl_taylor_basis_factory_h
//:
// \file
// \brief A factory that reads from file a taylor-kernel.
// \author Isabel Restrepo mir@lems.brown.edu
// \date  25-Jan-2011
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bvpl/kernels/bvpl_kernel_factory.h>
#include <vgl/vgl_point_3d.h>

//: This class reads from disk the taylor kernels.
class bvpl_taylor_basis_factory: public bvpl_kernel_factory
{
 public:
  //: Constructor from directory where the kernel files are located. The approximation degree defaults to 2
  bvpl_taylor_basis_factory(vcl_string filename);

  //: Identifying string
  static vcl_string name() { return "taylor"; }

 protected:
  //: Filename containing the kernel
  vcl_string filename_;

  //: Creates canonical kernel from file
  virtual void create_canonical();
};


//: Helper class to load taylor kernels, response scenes and hold paths
class bvpl_taylor_basis_loader
{
 public:
  bvpl_taylor_basis_loader():path_(""), min_point_(vgl_point_3d<int>()), max_point_(vgl_point_3d<int>()), degree_(0) {}

  bvpl_taylor_basis_loader(vcl_string path,
                           vgl_point_3d<int> min_point = vgl_point_3d<int>(-2,-2,-2),
                           vgl_point_3d<int> max_point = vgl_point_3d<int>(2,2,2),
                           unsigned degree=2): path_(path), min_point_(min_point), max_point_(max_point),degree_(degree) {}

  //: Returns a map of kernels and their names
  void create_basis(vcl_map<vcl_string, bvpl_kernel_sptr> &taylor_basis);

  //: List of filenames that should be present in path_
  void files(vcl_vector<vcl_string> &filenames);

  //: Return main path
  vcl_string path() const { return path_; }

  //: Return the min point of bounding box of this set of kernels
  vgl_point_3d<int> min_point() const { return min_point_; }

  //: Return the max point of bounding box of this set of kernels
  vgl_point_3d<int> max_point() const { return max_point_; }

 private:
  //: Path to all kernels (must be a directory)
  vcl_string path_;

  //: Min point of bounding box of these kernels
  vgl_point_3d<int> min_point_;

  //: Max point of bounding box of these kernels
  vgl_point_3d<int> max_point_;

  //: The degree of taylor approximation
  unsigned degree_;
};

#endif
