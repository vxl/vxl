// This is oxl/osl/osl_canny_ox_params.h
#ifndef osl_canny_ox_params_h_
#define osl_canny_ox_params_h_
// .NAME osl_canny_ox_params
// .INCLUDE osl/osl_canny_ox_params.h
// .FILE osl_canny_ox_params.cxx
// \author fsm

class osl_canny_ox_params
{
 public:
  osl_canny_ox_params();
  ~osl_canny_ox_params();

  float sigma;             // Standard deviation of the smoothing kernel
  int  max_width;          // Maximum smoothing kernel width
  float gauss_tail;        // Used in determining the kernel width
  float low;               // Low hysteresis threshold
  float high;              // High hysteresis threshold
  int  edge_min;           // Minimum edge pixel intensity
  //                          Used in the follow part of canny.
  //                          The edgel image is scaled by scale_OX_
  //                          before comparing edgels to edge_min.
  int  min_length;         // Minimum number of pixels in a curve . was 60
  int  border_size;        // Border size around the image to be set
  float  border_value;     // to border_value_OX_ (usually 0) to
  //                          ensure follow won't overrun.
  float  scale;            // Value used in the follow part of canny to
  //                          scale image after the hysteresis part.
  int  follow_strategy;    // Flag used in the Final_followOX()
  //                          to determined the order of neighboring
  //                          pixel checking (see Final_followOX()
  //                          function in CannyOX.C
  //                          Also used to decide whether to do the
  //                          Follow part of canny or not.
  //                          When equal to 0, only NMS and Hysteresis
  //                          are performed.
  bool  join_flag;         // True to enable pixel jumping
  int  junction_option;    // True to enable locating junctions

  bool verbose;
};

#endif // osl_canny_ox_params_h_
