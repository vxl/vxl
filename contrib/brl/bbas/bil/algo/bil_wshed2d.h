// This is contrib/brl/bbas/bil/algo/bil_wshed2d.h

#ifndef bil_wshed_2d_h_
#define bil_wshed_2d_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief The watershed transform algorithm as explained in Soille-Vincent paper
//
// \author H.C. Aras
// \date 02/17/05 (committed), written in August'2004
//
// \verbatim
//  Modifications:
// \endverbatim
//--------------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_list.h>

#include <vil/vil_image_view.h>

#define WSHED 0

struct bil_wshed_2d_region
{
    int number_of_pixels;
    long int total_sum;
    double mean_intensity;
};

class bil_wshed_2d
{
public:
  struct bil_wshed_2d_region *wshed_regions_;
  double max_mean_intensity_;
  double min_mean_intensity_;

protected:

  vil_image_view< unsigned char > input_img_;
  vil_image_view< unsigned char > gradient_img_;
  vil_image_view< int > distance_map_;
  vil_image_view< int > output_img_;
  //the output image without any pixels marked as watershed
  vil_image_view< int > output_img_wout_wsheds_;
  vil_image_view< unsigned char > output_img_uchar_;
  //color map
  vil_image_view< unsigned char > output_img_wout_wsheds_uchar_;

  int width_, height_, image_size_;
  unsigned char min_value_src_, max_value_src_;

  int h_start_, h_end_;
  int current_distance_;
  int current_label_;

  int *sorted_pixels_x_; //holds the x position of the pixel
  int *sorted_pixels_y_; //holds the y position of the pixel
    
  vcl_list< int > queue_x;
  vcl_list< int > queue_y;

public:

  bil_wshed_2d();
  virtual ~bil_wshed_2d();

  //:
  // This is the main function that the user all needs to know about
  // The first sigma (gsigma1) is for the Gaussian smoothing of the input image. Not performed if zero.
  // The second sigma (gsigma2) is for the Gaussian smoothing of the gradient image. Not performed if zero.
  // (min_x,min_y) and (max_x,max_y) specify the region of the image that the algorithm is to be applied
  // All boundary values are set to zero as default, in which case the algorithm is applied on the whole image
  // The necessary boundary checks are also performed
  vcl_vector< vil_image_view< unsigned char > > 
  bil_wshed_2d_main(vil_image_view< unsigned char > src_img, 
                     double gsigma1, double gsigma2, int min_x, int min_y, int max_x, int max_y);

protected:
  void add_connected_pixels_to_queue(int pos_x, int pos_y);
  void calculate_region_properties();
  void collect_garbage();
  void compute_watershed_regions();
  void flood_new_basin_from_given_point(int pos_x, int pos_y);
  void flood_current_height();
  int get_the_smallest_neighbor_region_label(int pos_x, int pos_y);
  void highlight_region_boundaries();
  int is_there_a_neighbor_of_smaller_label(int pos_x, int pos_y);
  void process_connected_pixels_in_queue();
  void process_given_connected_pixel(int pos_x, int pos_y);
  void process_new_discovered_minima();
  void remove_watershed_pixels();
  void smooth_and_gradient_img(double gsigma1, double gsigma2);
  void sort_pixels();
};


#endif
