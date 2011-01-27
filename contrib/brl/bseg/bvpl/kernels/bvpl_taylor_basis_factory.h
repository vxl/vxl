// This is bvpl_taylor_basis_factory.h
#ifndef bvpl_taylor_basis_factory_h
#define bvpl_taylor_basis_factory_h

//:
// \file
// \brief A factory that reads from file a taylor-kernel. 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  25-Jan-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bvpl/kernels/bvpl_kernel_factory.h>


//: This class reads from disk the taylor kernels.
class bvpl_taylor_basis_factory: public bvpl_kernel_factory
{
  
public:
  //: Constructor from directory where the kernel files are located. The approximation degree defaults to 2
  bvpl_taylor_basis_factory(vcl_string filename);

  //: Identifying string  
  static vcl_string name() {return "taylor"; }

protected:
  //: Filename containing the kernel
  vcl_string filename_;
  
  //: Creates canonical kernel form file 
  virtual void create_canonical();
  
};


class bvpl_taylor_basis_loader 
{
public:
  
  bvpl_taylor_basis_loader(vcl_string path, unsigned degree=2): path_(path), degree_(degree){}
  
  //: Returns a map of kernels and their names
  void create_basis(vcl_map<vcl_string, bvpl_kernel_sptr> &taylor_basis);
  
  //: List of filenames that should be present in path_
  void files(vcl_vector<vcl_string> &filenames);
  
private:
  //: Path to all kernels (must be a directory)
  vcl_string path_;
  
  //: The degree of taylor approximatio
  unsigned degree_;
  
  
};
#endif
