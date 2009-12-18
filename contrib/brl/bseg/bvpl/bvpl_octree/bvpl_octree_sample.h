// This is bvpl_octree_sample.h
#ifndef bvpl_octree_sample_h
#define bvpl_octree_sample_h

//:
// \file
// \brief  A data structture to contain- bvpl_kernel response, axis of rotations and rotation around axis
// \author Isabel Restrepo mir@lems.brown.edu
// \date  12/9/09
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vnl/vnl_float_3.h>
#include <vcl_iostream.h>
#include <vsl/vsl_binary_io.h>

template<class T_data>
class bvpl_octree_sample
{
public:
  //: Default Constructor. Becareful when using default constructor, the value of response may be nonsense
  //  This is indicated by the negative id.
  bvpl_octree_sample():response_(),id_(-1){}
  
  //: Constructor from response and id
  bvpl_octree_sample(const T_data& response, const int& id): 
  response_(response),id_(id){}
  
  short version_no() const{ return 1; }
  
  void  print(vcl_ostream& os) const;

  //Member variables
  T_data response_;
  int id_;
};

//I/O
template<class T_data>
void vsl_b_write(vsl_b_ostream & os, bvpl_octree_sample<T_data> const &sample);

template<class T_data>
void vsl_b_write(vsl_b_ostream & os, bvpl_octree_sample<T_data> const * &sample);

template<class T_data>
void vsl_b_read(vsl_b_istream & is, bvpl_octree_sample<T_data> &sample);

template<class T_data>
void vsl_b_read(vsl_b_istream & is, bvpl_octree_sample<T_data> *&sample);

template<class T_data>
vcl_ostream& operator << (vcl_ostream& os, const bvpl_octree_sample<T_data>& sample);
#endif
