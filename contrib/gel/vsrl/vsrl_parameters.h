#ifndef vsrl_parameters_h
#define vsrl_parameters_h

// This class will be used to set all the paramters used in the dense matching process.
// We use a singlton arangement so that the paramters can be considered global. This
// will make it much easier to do parametric optimization of the system performance 


class vsrl_parameters
{

public:	

  // the constructor
  vsrl_parameters();
  
  // destructor
  ~vsrl_parameters();
  
  // Access to the singleton

  static vsrl_parameters* instance();
  
  // the data values 
  
  // ****** used in dynamic_program ******

  // the largest amount of allowable disparaty -  default 10
  int correlation_range;

  // the cost of not assigning an inner pixel - default 1.0
  double inner_cost; 

  // the cost of not assigning an inner pixel - default 0.5
  double outer_cost; 

  // the cost for discontinuous pixel assignments  - default 0.1
  double continuity_cost; 
  
  // **** used in image correlation *****
  
  // the correlation window width - default 20

  int correlation_window_width;

  // the correlation window height - default 20 

  int correlation_window_height;
  
  // *** used in the raster dp setup *****
  
  // the cost of a bias towards the previous assignment - default 0.2 

  double bias_cost;

  // the difference in intensity which defines a common region - default 0.0
  
  double common_intensity_diff;
  
  // a method to read in the parameters

  void load(char *filename);


 protected:

  static vsrl_parameters* _instance;
  
};

#endif
