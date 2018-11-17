//this is boxm2_class_generate_samples.h
#ifndef boxm2_class_generate_samples_h_
#define boxm2_class_generate_samples_h_
//:
// \file
#include <string>
#include <iostream>
#include <cstddef>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_polygon.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Randomly samples the pixels of an image for training a model
class boxm2_class_generate_samples
{
  public:
    boxm2_class_generate_samples(std::string xml_label,
                                 const std::string& eoPath,
                                 const std::string& irPath,
                                 std::size_t K=1000);

    //return ground truth samples
    std::vector<float> intensities() { return intensities_; }
    std::vector<float> r() { return r_; }
    std::vector<float> g() { return g_; }
    std::vector<float> b() { return b_; }

    //ground truth classes
    std::vector<std::string> classes() { return classes_; }
    bool is_rgb() { return is_rgb_; }

    //points sampled on this image
    std::vector<vgl_point_2d<int> > points() { return points_; }

  private:
    //pixel values that are sampled
    std::vector<vgl_point_2d<int> > points_;

    //class for each pixel
    std::vector<std::string> classes_;

    //intensities for rgbs
    std::vector<float> r_, g_, b_;

    //intensities for grey scale images
    std::vector<float> intensities_;

    //input image is rgb (3 channel) or not
    bool is_rgb_;
};

std::string
pixel_class(int i, int j, std::vector<vgl_polygon<double> >& polygons, std::vector<std::string>& classes);

#endif // boxm2_class_generate_samples_h_
