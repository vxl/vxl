// This is mul/vimt/vimt_gaussian_pyramid_builder_2d_general.h
#ifndef vimt_gaussian_pyramid_builder_2d_general_h_
#define vimt_gaussian_pyramid_builder_2d_general_h_
//:
// \file
// \brief Build gaussian image pyramids at any scale separation
// \author Ian Scott
//=======================================================================

#include <iostream>
#include <iosfwd>
#include <vimt/vimt_gaussian_pyramid_builder_2d.h>
#include <vil/vil_image_view.h>
#include <vil/algo/vil_gauss_reduce.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Build pyramids of vimt_image_2d_of<vxl_byte> at any scale step

template <class T>
class vimt_gaussian_pyramid_builder_2d_general : public vimt_gaussian_pyramid_builder_2d<T>
{
 private:
    //: image workspace
    mutable vil_image_view<T> worka_;
    //: image workspace
    mutable vil_image_view<T> workb_;

    //: Pre-calculated scale parameters and filter taps.
    vil_gauss_reduce_params scale_params_;

 public:
    //: Build pyramid
    void build(vimt_image_pyramid& im_pyr, const vimt_image& im) const override;

    //: Dflt ctor
    vimt_gaussian_pyramid_builder_2d_general();

    //: Construct with given scale_step
    vimt_gaussian_pyramid_builder_2d_general(double scale_step);

    //: Destructor
    ~vimt_gaussian_pyramid_builder_2d_general() override;

    //: get the current scale step;
    double scale_step() const override { return scale_params_.scale_step(); }

    //: Set the Scale step
    void set_scale_step(double scale_step);

    //: Extend pyramid
    // The first layer of the pyramid must already be set.
    void extend(vimt_image_pyramid& image_pyr) const override;

    vimt_image_pyramid_builder* clone() const override;
    //: Version number for I/O
    short version_no() const ;
    //: Name of the class
    std::string is_a() const override ;
    //: Does the name of the class match the argument?
    bool is_class(std::string const& s) const override;
    //: Print class to os
    void print_summary(std::ostream&) const override;
    //: Save class to binary file stream
    void b_write(vsl_b_ostream& bfs) const override;
    //: Load class from binary file stream
    void b_read(vsl_b_istream& bfs) override;
};

#endif // vimt_gaussian_pyramid_builder_2d_general_h_
