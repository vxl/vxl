#ifndef boxm2_data_traits_h_
#define boxm2_data_traits_h_
//:
// \file
// \brief traits for data types
//
// \author Vishal Jain
// \date nov 17, 2010

#include <vcl_string.h>
#include <vcl_cstddef.h> // for std::size_t
#include <vnl/vnl_vector_fixed.h>
class boxm2_mog3_grey_processor;

enum boxm2_data_type
{
  BOXM2_ALPHA=0,
  BOXM2_MOG3_GREY,
  BOXM2_NUM_OBS,
  BOXM2_AUX,   
  BOXM2_SEG_LEN,
  BOXM2_MEAN_OBS,
  BOXM2_VIS,
  BOXM2_BETA,   
  BOXM2_UNKNOWN
};

//: Pixel properties for templates.
template <boxm2_data_type type>
class boxm2_data_traits;

//: traits for a mixture of gaussian appearance model of gray-scale images
template<>
class boxm2_data_traits<BOXM2_ALPHA>
{
 public:
  typedef float datatype;
  static vcl_size_t datasize(){return sizeof(datatype);}
  static vcl_string prefix(){ return "alpha";}
};

template<>
class boxm2_data_traits<BOXM2_MOG3_GREY>
{
 public:
  typedef boxm2_mog3_grey_processor processor;
  typedef vnl_vector_fixed<unsigned char, 8> datatype;
  static vcl_size_t datasize(){return sizeof(datatype);}
  static vcl_string prefix(){ return "boxm2_mog3_grey";}
};

template<>
class boxm2_data_traits<BOXM2_NUM_OBS>
{
  public:
    typedef vnl_vector_fixed<unsigned short, 4> datatype;
    static vcl_size_t datasize(){return sizeof(datatype);}
    static vcl_string prefix(){ return "boxm2_num_obs";}
};


//: Aux data contains four values to assist update calculations:
// * seg_len_array,   (total cell segment length)
// * mean_obs_array,  (total cell mean obs)
// * vis_array,       (total cell visibility)
// * beta_array,      (total cell bayes update factor)
///// PROBLEM with int32 atomics. Need to use four distinct arrays.
template<>
class boxm2_data_traits<BOXM2_SEG_LEN>
{
  public:
    typedef int datatype;
    static vcl_size_t datasize(){return sizeof(datatype);}
    static vcl_string prefix(){ return "seg_len";}
};
template<>
class boxm2_data_traits<BOXM2_MEAN_OBS>
{
  public:
    typedef int datatype;
    static vcl_size_t datasize(){return sizeof(datatype);}
    static vcl_string prefix(){ return "mean_obs";}
};
template<>
class boxm2_data_traits<BOXM2_VIS>
{
  public:
    typedef int datatype;
    static vcl_size_t datasize(){return sizeof(datatype);}
    static vcl_string prefix(){ return "vis";}
};
template<>
class boxm2_data_traits<BOXM2_BETA>
{
  public:
    typedef int datatype;
    static vcl_size_t datasize(){return sizeof(datatype);}
    static vcl_string prefix(){ return "beta";}
};
///////////////////////////////////////////////////////////////////////////////

template<>
class boxm2_data_traits<BOXM2_AUX>
{
 public:
  typedef vnl_vector_fixed<float, 4> datatype;
  static vcl_size_t datasize(){return sizeof(datatype);}
  static vcl_string prefix(){ return "aux";}
};

//: HACKY WAY TO GENERICALLY GET DATASIZES -
class boxm2_data_info
{
  public: 
    static vcl_size_t datasize(vcl_string prefix) {
      
      if(prefix == boxm2_data_traits<BOXM2_ALPHA>::prefix()) 
        return boxm2_data_traits<BOXM2_ALPHA>::datasize(); 
        
      if(prefix == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix())
        return boxm2_data_traits<BOXM2_MOG3_GREY>::datasize(); 
        
      if(prefix == boxm2_data_traits<BOXM2_NUM_OBS>::prefix())
        return boxm2_data_traits<BOXM2_NUM_OBS>::datasize(); 
        
      if(prefix == boxm2_data_traits<BOXM2_AUX>::prefix())
        return boxm2_data_traits<BOXM2_AUX>::datasize();
        
      //alternate aux data for GPU (Need to combine this with one AUX type...)
      if(prefix == boxm2_data_traits<BOXM2_SEG_LEN>::prefix())
        return boxm2_data_traits<BOXM2_SEG_LEN>::datasize(); 
      if(prefix == boxm2_data_traits<BOXM2_MEAN_OBS>::prefix())
        return boxm2_data_traits<BOXM2_MEAN_OBS>::datasize(); 
      if(prefix == boxm2_data_traits<BOXM2_VIS>::prefix())
        return boxm2_data_traits<BOXM2_VIS>::datasize(); 
      if(prefix == boxm2_data_traits<BOXM2_BETA>::prefix())
        return boxm2_data_traits<BOXM2_BETA>::datasize(); 
      
      return 0;
    }
};


#endif
