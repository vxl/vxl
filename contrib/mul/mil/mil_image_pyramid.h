#ifndef mil_image_pyramid_h_
#define mil_image_pyramid_h_
#ifdef __GNUC__
#pragma interface
#endif

//: \file
//  \brief Pyramid of images of arbitrary type
//  \author Tim Cootes

#include <vsl/vsl_binary_io.h>
#include <vcl_vector.h>
#include <vcl_string.h>

class mil_image;

//: Pyramid of images of arbitrary type
class mil_image_pyramid {
private:
    vcl_vector<mil_image*> image_;
    double base_pixel_width_;
    double scale_step_;

    void deleteImages();
public:

        //: Dflt ctor
    mil_image_pyramid();

        //: Destructor
    virtual ~mil_image_pyramid();

        //: Resize to n_levels pyramid
        //  Each level of which is a clone of im_type
    void resize(int n_levels, const mil_image& im_type);

        //: Lowest level (highest resolution image) of pyramid
    int lo() const;

        //: Highest level (lowest resolution image) of pyramid
    int hi() const;

        //: Number of levels
    int nLevels() const;

        //: Image at level L
    mil_image& operator()(int L);

        //: Image at level L
    const mil_image& operator()(int L) const;

        //: Mean width (in world coords) of pixels at level zero
    double basePixelWidth() const;

        //: Scaling per level
        //  Pixels at level L have width
        //  basePixelWidth() * scaleStep()^L
    double scaleStep() const;

        //: Access to image data
        //  Should only be used by pyramid builders
    vcl_vector<mil_image*>& data();

        //: Define pixel widths
    void setWidths(double base_pixel_width, double scale_step);

    void print_summary(vcl_ostream& os) const;

        //: Print whole of each image to os
    void print_all(vcl_ostream& os) const;
};

vcl_ostream& operator<<(vcl_ostream& os, const mil_image_pyramid& im_pyr);
vcl_ostream& operator<<(vcl_ostream& os, const mil_image_pyramid* im_pyr);
void vsl_print_summary(vcl_ostream& os, const mil_image_pyramid& im_pyr);
void vsl_print_summary(vcl_ostream& os, const mil_image_pyramid* im_pyr);

#endif
