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
  BOXM2_MOG3_GREY_16,
  BOXM2_BATCH_HISTOGRAM,
  BOXM2_GAUSS_RGB,
  BOXM2_MOG2_RGB,
  BOXM2_NUM_OBS,
  BOXM2_NUM_OBS_SINGLE,
  BOXM2_AUX,   
  BOXM2_INTENSITY,
  BOXM2_AUX0,
  BOXM2_AUX1,
  BOXM2_AUX2,
  BOXM2_AUX3,
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
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "alpha"; else return "alpha_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_MOG3_GREY>
{
 public:
  typedef boxm2_mog3_grey_processor processor;
  typedef vnl_vector_fixed<unsigned char, 8> datatype;
  static vcl_size_t datasize(){return sizeof(datatype);}
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_mog3_grey"; else return "boxm2_mog3_grey_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_MOG3_GREY_16>
{
 public:
  typedef boxm2_mog3_grey_processor processor;
  typedef vnl_vector_fixed<unsigned short, 8> datatype;
  static vcl_size_t datasize(){return sizeof(datatype);}
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_mog3_grey_16"; else return "boxm2_mog3_grey_16_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_GAUSS_RGB>
{
 public:
  typedef vnl_vector_fixed<unsigned char, 8> datatype;
  static vcl_size_t datasize(){return sizeof(datatype);}
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_gauss_rgb"; else return "boxm2_gauss_rgb_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_MOG2_RGB>
{
 public:
  typedef vnl_vector_fixed<unsigned char, 16> datatype;
  static vcl_size_t datasize(){return sizeof(datatype);}
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_mog2_rgb"; else return "boxm2_mog2_rgb_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_NUM_OBS>
{
  public:
    typedef vnl_vector_fixed<unsigned short, 4> datatype;
    static vcl_size_t datasize(){return sizeof(datatype);}
    static vcl_string prefix(const vcl_string& identifier = "")
    { if (!identifier.size()) return "boxm2_num_obs"; else return "boxm2_num_obs_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_NUM_OBS_SINGLE>
{
  public:
    typedef unsigned short datatype;
    static vcl_size_t datasize(){return sizeof(datatype);}
    static vcl_string prefix(const vcl_string& identifier = "")
    { if (!identifier.size()) return "boxm2_num_obs_single"; else return "boxm2_num_obs_single_"+identifier; }
};


//: Aux data contains four values to assist update calculations:
// * seg_len_array,   (total cell segment length)
// * mean_obs_array,  (total cell mean obs)
// * vis_array,       (total cell visibility)
// * beta_array,      (total cell bayes update factor)
template<>
class boxm2_data_traits<BOXM2_AUX>
{
 public:
  typedef vnl_vector_fixed<float, 4> datatype;
  static vcl_size_t datasize(){return sizeof(datatype);}
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "aux"; else return "aux_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_AUX0>
{
 public:
  typedef float datatype;
  static vcl_size_t datasize(){return sizeof(datatype);}
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "aux0"; else return "aux0_"+identifier; }
};
template<>
class boxm2_data_traits<BOXM2_AUX1>
{
 public:
  typedef float datatype;
  static vcl_size_t datasize(){return sizeof(datatype);}
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "aux1"; else return "aux1_"+identifier; }
};
template<>
class boxm2_data_traits<BOXM2_AUX2>
{
 public:
  typedef float datatype;
  static vcl_size_t datasize(){return sizeof(datatype);}
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "aux2"; else return "aux2_"+identifier; }
};
template<>
class boxm2_data_traits<BOXM2_AUX3>
{
 public:
  typedef float datatype;
  static vcl_size_t datasize(){return sizeof(datatype);}
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "aux3"; else return "aux3_"+identifier; }
};
template<>
class boxm2_data_traits<BOXM2_BATCH_HISTOGRAM>
{
 public:
  typedef vnl_vector_fixed<float, 8> datatype;
  static vcl_size_t datasize(){return sizeof(datatype);}
  static vcl_string prefix(){ return "boxm2_batch_histogram";}
};
template<>
class boxm2_data_traits<BOXM2_INTENSITY>
{
 public:
  typedef float datatype;
  static vcl_size_t datasize(){return sizeof(datatype);}
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_intensity"; else return "boxm2_intensity_"+identifier; }
};
//: HACKY WAY TO GENERICALLY GET DATASIZES -
class boxm2_data_info
{
  public: 
    static vcl_size_t datasize(vcl_string prefix) {
      
      // some of them changed to using find method to account for identifiers 
      if(prefix.find(boxm2_data_traits<BOXM2_ALPHA>::prefix()) != vcl_string::npos) 
        return boxm2_data_traits<BOXM2_ALPHA>::datasize(); 

       if(prefix.find(boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix()) != vcl_string::npos)
        return boxm2_data_traits<BOXM2_MOG3_GREY_16>::datasize(); 
        
      if(prefix.find(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix()) != vcl_string::npos)
        return boxm2_data_traits<BOXM2_MOG3_GREY>::datasize(); 

      if(prefix == boxm2_data_traits<BOXM2_BATCH_HISTOGRAM>::prefix())
        return boxm2_data_traits<BOXM2_BATCH_HISTOGRAM>::datasize(); 

      if(prefix.find(boxm2_data_traits<BOXM2_MOG2_RGB>::prefix()) != vcl_string::npos)
        return boxm2_data_traits<BOXM2_MOG2_RGB>::datasize(); 
        
      if(prefix == boxm2_data_traits<BOXM2_NUM_OBS>::prefix())
        return boxm2_data_traits<BOXM2_NUM_OBS>::datasize(); 
        
      if(prefix.find(boxm2_data_traits<BOXM2_AUX0>::prefix()) != vcl_string::npos)
        return boxm2_data_traits<BOXM2_AUX0>::datasize();
      if(prefix.find(boxm2_data_traits<BOXM2_AUX1>::prefix()) != vcl_string::npos)
        return boxm2_data_traits<BOXM2_AUX1>::datasize();
      if(prefix.find(boxm2_data_traits<BOXM2_AUX2>::prefix()) != vcl_string::npos)
        return boxm2_data_traits<BOXM2_AUX2>::datasize();
      if(prefix.find(boxm2_data_traits<BOXM2_AUX3>::prefix()) != vcl_string::npos)
        return boxm2_data_traits<BOXM2_AUX3>::datasize();
      if(prefix.find(boxm2_data_traits<BOXM2_AUX>::prefix()) != vcl_string::npos)
        return boxm2_data_traits<BOXM2_AUX>::datasize();
     
      if(prefix.find(boxm2_data_traits<BOXM2_INTENSITY>::prefix()) != vcl_string::npos)
        return boxm2_data_traits<BOXM2_INTENSITY>::datasize();
     
      if(prefix.find(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix()) != vcl_string::npos)
        return boxm2_data_traits<BOXM2_GAUSS_RGB>::datasize();   
     
      if(prefix == boxm2_data_traits<BOXM2_NUM_OBS_SINGLE>::prefix())
        return boxm2_data_traits<BOXM2_NUM_OBS_SINGLE>::datasize();   
        
      return 0;
    }
};


#endif
