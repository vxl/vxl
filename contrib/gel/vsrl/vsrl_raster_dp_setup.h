#ifndef vsrl_epipolar_dp_setup_h
#define vsrl_epipolar_dp_setup_h
//:
// \file
// this class will take a vsrl_image_correlation class
// and a given y value to define which raster is to be used.
// it will then create the dynamic program which will determine
// the optimal assignments between the two images

#include <vcl_vector.h>
#include <vnl/vnl_vector.h>
class vsrl_intensity_token;
class vsrl_image_correlation;

class vsrl_raster_dp_setup
{
 public:
  // constructor
  vsrl_raster_dp_setup(int raster_line, vsrl_image_correlation *image_correlation);
  // destructor
  ~vsrl_raster_dp_setup() { clear_token_list(tok_list1); clear_token_list(tok_list2); }

  // accessors

  //: get the raster line that this dynamic program is processing
  int get_raster_line() { return raster_line_; }
  //: get the assignment of pixel x
  int get_assignment(int x);
  //: the width of image1
  int get_image1_width();
  //: the width of image2
  int get_image2_width();

 // set the search range for the dynamic program

  void set_search_range(int range) { search_range_=range; }

  // set the inner null cost for the dynamic program

  void set_inner_cost(double ic) { inner_cost_=ic; }
   // set the outer null cost for the dynamic program

  void set_outer_cost(double oc) { outer_cost_=oc; }

  // set the prior raster
  void set_prior_raster(vsrl_raster_dp_setup *pr) { prior_raster_=pr; }

  // set the cost of deviating from the bias

  void set_bias_cost(double bias_cost) { bias_cost_=bias_cost; }

  // perform the dynamic program

  double execute();
  double execute(vnl_vector<int> curr_row);

 private:

  //: the image correlation mechanism
  vsrl_image_correlation *image_correlation_;

  //: the raster line to be processed
  int raster_line_;

  // the token lists
  vcl_vector<vsrl_intensity_token*> tok_list1;
  vcl_vector<vsrl_intensity_token*> tok_list2;

  void create_token_list(int width,
                         vsrl_image_correlation *image_correlation,
                         vcl_vector<vsrl_intensity_token*> &tok_list,
                         double step=1);

  // clear token list;
  void clear_token_list(vcl_vector<vsrl_intensity_token*> &tok_list);

  //: the search range used
  int search_range_;

  //: the bias cost
  double bias_cost_;

  //: the inner cost
  double inner_cost_;
  //: the outer cost
  double outer_cost_;
  //: the prior raster
  vsrl_raster_dp_setup *prior_raster_;

  // a method that uses the prior_raster to bias the next raster
  void set_token_biases();
};

#endif
