// This is brl/bseg/bmdl/bmdl_classify.h
#ifndef bmdl_classify_h_
#define bmdl_classify_h_
//:
// \file
// \brief Classify each pixel in lidar images
//
// \author Matt Leotta
// \date 10/14/2008
//      

#include <vcl_vector.h>
#include <vil/vil_image_view.h>
//#include <vgl/vgl_box_2d.h>

class bmdl_classify {
public:
  //: fill infinite values with the median of its 8 neighborhood
  static void median_fill(vil_image_view<double>& image);

  //: expand the range (minv, maxv) with the data in \a image
  // only finite values count
  static void range(const vil_image_view<double>& image,
                          double& minv, double& maxv);

#if 0
  //: read one or more FLIMAP ASCII files and build a pair of LIDAR images (like Buckeye format)
  static void generate_lidar_images(const vcl_string& glob, const vgl_box_2d<double>& bbox,
                                          vil_image_view<double>& return1,
                                          vil_image_view<double>& return2,
                                          vil_image_view<vxl_byte>& rgb_img);
#endif

  //: classify each pixel as Ground (0), Vegitation (1), or Building (2)
  // also return the ground height and a cleaned up image of heights
  static double label_lidar(const vil_image_view<double>& first_return,
                            const vil_image_view<double>& last_return,
                                  vil_image_view<unsigned int>& labels,
                                  vil_image_view<double>& heights);

  //: find the ground value as the most common point
  // estimate the standard deviation in the ground by locally fitting a gaussian
  static double find_ground(const vil_image_view<double>& image,
                            double minv, double maxv,
                            double& gnd_stdev);

  static void cluster_buildings(const vil_image_view<double>& first_return,
                                const vil_image_view<double>& last_return,
                                      double init_var,
                                      vil_image_view<unsigned int>& labels,
                                      vcl_vector<double>& means,
                                      vcl_vector<unsigned int>& sizes);

  //: Search for nearby pixel that can be added to each building
  // return true if any changes are made
  static bool expand_buildings(const vil_image_view<double>& first_return,
                               const vil_image_view<double>& last_return,
                                     double init_var,
                                     vil_image_view<unsigned int>& labels,
                                     vcl_vector<double>& means,
                                     vcl_vector<unsigned int>& sizes);

  static vcl_vector<bool> close_buildings(vil_image_view<unsigned int>& labels,
                                          unsigned int num_labels);
};

#endif // bmdl_classify_h_
