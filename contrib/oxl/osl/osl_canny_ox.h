//-*- c++ -*-------------------------------------------------------------------
#ifndef osl_canny_ox_h
#define osl_canny_ox_h
//
// .NAME osl_canny_ox - The `Sheffield' Canny edge detector
// .INCLUDE osl/osl_canny_ox.h
// .FILE osl_canny_ox.cxx
//
// .SECTION Description
//
// A class for performing the AZ-standard version of Canny's edge detector.
// The basic implementatation is similar to that described in Canny's thesis,
// though sub-pixel interpolation of the final edge output has been included,
// and there is patching of single pixel gaps in the edgel chains.
//
// In order not to ignore a border of size _width (half the kernel size)
// around the image, as is done in class Canny, this implementation
// uses _sub_area_OX to normalise the (gaussian) smoothed border pixels---see
// the description of Smooth_imageOX().
//
// .SECTION Author
//   Samer Abdallah (samer@robots.ox.ac.uk) - 5/10/95
//   Robotics Research Group, University of Oxford
//
// .SECTION Modifications
// The original C implementation of this canny edge detector
// came from AIRUV at Sheffield. It was later modified by
// Charlie Rothwell, Nic Pillow and Sven Utcke.
//
//   Samer Abdallah - 18/05/96 - Cleared some compiler warnings
//   Samer Abdallah - 24/05/96 - Fixed _theta[x][y] in Non_maximal_supressionOX
//                               _theta[x][y] now stores the edge normal
//   Maarten Vergauwen (vergauwe@esat.kuleuven.ac.be) - 08/10/98 - Added AdjustForMask method
//   Peter Vanroose - 30/12/99 - Link_edgelsOX rewritten and documented
//   F. Schaffaliztky 2-apr-99   converted from Segmentation to osl

#include <vcl_vector.h>
#include <osl/osl_canny_base.h>

class osl_canny_ox_params;

struct osl_LINK {
  int to;
  osl_LINK *nextl;
};


class osl_canny_ox : public osl_canny_base {
public:
  osl_canny_ox(osl_canny_ox_params const &params);
  void detect_edges(vil_image const &image, vcl_list<osl_edge*>*);
  ~osl_canny_ox();

protected:
  osl_edgel_chain *Get_NMS_edgelsOX(int n_edgels_NMS, int *_x, int *_y);

  // Functions used in performing the hysteresis part of canny
  int HysteresisOX(osl_edgel_chain *&, int *&);
  void Initial_followOX(int,int,osl_edgel_chain *&,osl_LINK *[],int *&,float);
  void Add_linkOX(int,int,osl_LINK *[]);
  void Link_edgelsOX(vcl_vector<unsigned> const &, vcl_vector<unsigned> const &,osl_LINK *[]);
  int Get_n_edgels_hysteresisOX(osl_edgel_chain *&,int *&);
  void Get_hysteresis_edgelsOX(osl_edgel_chain *&,int *&, osl_edgel_chain *&, int *_x, int *_y);
  void Delete_linksOX(osl_LINK **, int);
  osl_edge *NO_FollowerOX(osl_edgel_chain *);

  // Functions used in the follow part of canny
  void FollowerOX(vcl_list<osl_edge*> *);
  void Final_followOX(int,int,vcl_list<int> *,vcl_list<int> *,vcl_list<float> *,int);
  int Join_dotsOX(int,int,int,int,int&,int&);
  void Scale_imageOX(float **, float);
  void Set_image_borderOX(float **, int, float);

  // Functions used in locating junctions
  void Find_junctionsOX();
  void Find_junction_clustersOX();

protected:
  int _max_width_OX;       // The maximum smoothing kernel width
  float *_sub_area_OX;     // Used in smoothing the image near the borders

  int _edge_min_OX;        // Minimum edge pixel intensity

  int _min_length_OX;      // Minimum number of pixels in a curve
  int _border_size_OX;     // Border size around the image to be set
  float _border_value_OX;  //   to _border_value_OX (usually 0) to ensure
                           //   follow won't overrun.
  float _scale_OX;         // Value used in the follow part of canny to
                           //   scale image after the hysteresis part.
  int _follow_strategy_OX; // Flag used in the Final_followOX() to determined
                           //  the order of neighboring pixel checking
                           //  (see Final_followOX() function in osl_canny_ox.C
                           //  When equal to 0, only NMS and Hysteresis
                           //  are performed; FollowerOX is not performed.
                           //  Locating Junctions in not performed either.
                           //  See Do_osl_canny_ox(...)

  bool _join_flag_OX;      // True to enable pixel jumping
  int _junction_option_OX; // True if we want to locate junctions
};

#endif // osl_canny_ox_h
