#ifndef vsrl_stereo_dense_matcher_h
#define vsrl_stereo_dense_matcher_h

// this class will take a pair of stereo images 
// where each point on one raster matches a point 
// on the corresponding raster. 
// once two images are incerted, dynamic programing on
// each raster is performed. The user can then find the
// point corespondences between each pixel in the image.

#include <vsrl/vsrl_raster_dp_setup.h>
#include <vil/vil_image.h>
#include <vsrl/vsrl_dense_matcher.h>
#include <vsrl/vsrl_image_correlation.h>


class vsrl_stereo_dense_matcher : public vsrl_dense_matcher
{
 public:
  
  // constructor

  vsrl_stereo_dense_matcher(const vil_image &im1, const vil_image &im2);

  // destructor
  
  ~vsrl_stereo_dense_matcher();


  // There may be some initial calculalations that we want to 
  // perform without computing all of the dense calculations 

  void initial_calculations();

  // perform the dence matching

  void execute();
  
  // get the assigned coordinate

  virtual  int get_assignment(int x, int y);
  
  // get the disparaty of pixel x,y

  virtual int get_disparaty(int x, int y);

  // write an image of the disparaty 

  void write_disparaty_image(char *filename);

  // print out the correlation costs for point x,y 

  void print_correlation_cost(int x, int y);

  

 private:

  vsrl_image_correlation _image_correlation; 
  vsrl_raster_dp_setup **_raster_array;
  int _num_raster;
  int _correlation_range; // the correlation range of the data 
  
  void evaluate_raster(int i); // performs the dynamic program on the raster


  
};

#endif
  

