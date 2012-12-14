#ifndef bstm_data_similarity_traits_h
#define bstm_data_similarity_traits_h
//:


#include <boxm2/boxm2_data_traits.h>
#include <bstm/bstm_data_traits.h>

#define SIMILARITY_T 0.3


template <bstm_data_type TYPE, boxm2_data_type BOXM2_TYPE>
class bstm_similarity_traits;

template<>
class bstm_similarity_traits<BSTM_MOG3_GREY, BOXM2_MOG3_GREY>
{
 public:
  static bool is_similar(bstm_data_traits<BSTM_MOG3_GREY>::datatype app, boxm2_data_traits<BOXM2_MOG3_GREY>::datatype boxm2_app, float p, float boxm2_p )
  {
    double isabel_measure = (bstm_data_traits<BSTM_MOG3_GREY>::processor::expected_color(app) + 1) * p;
    double isabel_measure_boxm2 = (boxm2_data_traits<BOXM2_MOG3_GREY>::processor::expected_color(boxm2_app) + 1) * boxm2_p;

    //hack:  boxm2_p > 0 to make sure empty voxels always lead to time division
    //otherwise it leads to motion artifacts.
    return vcl_fabs( isabel_measure - isabel_measure_boxm2) < SIMILARITY_T && (boxm2_p > 0 || p == 0);
  }
};

template<>
class bstm_similarity_traits<BSTM_MOG6_VIEW, BOXM2_MOG6_VIEW>
{
 public:
  static bool is_similar(bstm_data_traits<BSTM_MOG6_VIEW>::datatype app, boxm2_data_traits<BOXM2_MOG6_VIEW>::datatype boxm2_app, float p, float boxm2_p ) { return true; }
};

template<>
class bstm_similarity_traits<BSTM_MOG6_VIEW_COMPACT, BOXM2_MOG6_VIEW_COMPACT>
{
 public:
  static bool is_similar_asd(bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::datatype app, boxm2_data_traits<BOXM2_MOG6_VIEW_COMPACT>::datatype boxm2_app, float p, float boxm2_p )
  {
    //all 6 components must be similar
    bool similar = true;
    for(int i = 0; i < 6; i++)
    {
      double isabel_measure = ( ((float)app[2*i])/255.0 + 1) * p;
      double isabel_measure_boxm2 = ( ((float)boxm2_app[2*i])/255.0 + 1) * boxm2_p;

      //hack:  boxm2_p > 0 to make sure empty voxels always lead to time division
      //otherwise it leads to motion artifacts.
      similar = similar && (vcl_fabs( isabel_measure - isabel_measure_boxm2) < SIMILARITY_T && (boxm2_p > 0 || p == 0));
    }
    return similar;
  }
  static bool is_similar(bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::datatype app, boxm2_data_traits<BOXM2_MOG6_VIEW_COMPACT>::datatype boxm2_app, float p, float boxm2_p )
  {

    if(vcl_fabs( p - boxm2_p) < 0.1 ) //if p change is not over SIMILARITY_T && (boxm2_p > 0 || p == 0)
    {
      //all 8 components must be similar
      float max_change = 0;
      for(int i = 0; i < 8; i++)
        if(max_change >  ( vcl_fabs((float)app[2*i])/255.0 - ((float)boxm2_app[2*i])/255.0) )
          max_change  = vcl_fabs( ((float)app[2*i])/255.0 - ((float)boxm2_app[2*i])/255.0) ;

      if(max_change < 0.6)
        return true;
      else
        return false;
    }
    else
    {
      return false;
    }
  }
};


#endif
