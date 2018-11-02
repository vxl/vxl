// This is brl/bseg/bvpl/bvpl_octree/sample/bvpl_octree_sample.h
#ifndef bvpl_octree_sample_h
#define bvpl_octree_sample_h
//:
// \file
// \brief  A data structure to contain bvpl_kernel response, and id corresponding to axis of rotations and rotation around axis
// \author Isabel Restrepo mir@lems.brown.edu
// \date  December 9, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>

template<class T_data>
class bvpl_octree_sample
{
 public:
  //: Default Constructor. Be careful when using default constructor, the value of response may be nonsense
  //  This is indicated by the negative id.
  bvpl_octree_sample():response_(),id_(-1){}

  //: Constructor from response and id
  bvpl_octree_sample(const T_data& response, const int& id):
  response_(response),id_(id){}

  short version_no() const{ return 1; }

  void  print(std::ostream& os) const;

  //Member variables
  T_data response_;
  int id_;
};


template<class T_data>
void vsl_b_write(vsl_b_ostream & os, bvpl_octree_sample<T_data> const &sample);

template<class T_data>
void vsl_b_write(vsl_b_ostream & os, bvpl_octree_sample<T_data> const * &sample);

template<class T_data>
void vsl_b_read(vsl_b_istream & is, bvpl_octree_sample<T_data> &sample);

template<class T_data>
void vsl_b_read(vsl_b_istream & is, bvpl_octree_sample<T_data> *&sample);

template<class T_data>
std::ostream& operator << (std::ostream& os, const bvpl_octree_sample<T_data>& sample);
#endif
