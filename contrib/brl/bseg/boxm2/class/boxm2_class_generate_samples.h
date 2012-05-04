//this is boxm2_class_generate_samples.h
#ifndef boxm2_class_generate_samples_h_
#define boxm2_class_generate_samples_h_
//:
// \file
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_polygon.h>
#include <vcl_string.h>
#include <vcl_cstddef.h> // for std::size_t

//: Randomly samples the pixels of an image for training a model
class boxm2_class_generate_samples
{
  public:
    boxm2_class_generate_samples(vcl_string xml_label,
                                 vcl_string eoPath,
                                 vcl_string irPath,
                                 vcl_size_t K=1000);

    //return ground truth samples
    vcl_vector<float> intensities() { return intensities_; }
    vcl_vector<float> r() { return r_; }
    vcl_vector<float> g() { return g_; }
    vcl_vector<float> b() { return b_; }

    //ground truth classes
    vcl_vector<vcl_string> classes() { return classes_; }
    bool is_rgb() { return is_rgb_; }

    //points sampled on this image
    vcl_vector<vgl_point_2d<int> > points() { return points_; }

  private:
    //pixel values that are sampled
    vcl_vector<vgl_point_2d<int> > points_;

    //class for each pixel
    vcl_vector<vcl_string> classes_;

    //intensities for rgbs
    vcl_vector<float> r_, g_, b_;

    //intensities for grey scale images
    vcl_vector<float> intensities_;

    //input image is rgb (3 channel) or not
    bool is_rgb_;
};

vcl_string
pixel_class(int i, int j, vcl_vector<vgl_polygon<double> >& polygons, vcl_vector<vcl_string>& classes);

#endif // boxm2_class_generate_samples_h_
