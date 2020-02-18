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

  float sigma{1.0f};         // Standard deviation of the smoothing kernel
  int max_width{50};         // Maximum smoothing kernel width
  float gauss_tail{0.0001f}; // Used in determining the kernel width
  float low{2.0f};           // Low hysteresis threshold
  float high{12.0f};         // High hysteresis threshold
  int edge_min{60};          // Minimum edge pixel intensity
  //                          Used in the follow part of canny.
  //                          The edgel image is scaled by scale_OX_
  //                          before comparing edgels to edge_min.
  int min_length{10};       // Minimum number of pixels in a curve . was 60
  int border_size{2};       // Border size around the image to be set
  float border_value{0.0f}; // to border_value_OX_ (usually 0) to
  //                          ensure follow won't overrun.
  float scale{5.0f}; // Value used in the follow part of canny to
  //                          scale image after the hysteresis part.
  int follow_strategy{2}; // Flag used in the Final_followOX()
  //                          to determined the order of neighboring
  //                          pixel checking (see Final_followOX()
  //                          function in CannyOX.C
  //                          Also used to decide whether to do the
  //                          Follow part of canny or not.
  //                          When equal to 0, only NMS and Hysteresis
  //                          are performed.
  bool join_flag{true};   // True to enable pixel jumping
  int junction_option{0}; // True to enable locating junctions

  bool verbose{true};
};

#endif // osl_canny_ox_params_h_
