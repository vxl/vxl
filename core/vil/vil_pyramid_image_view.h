#ifndef vil_pyramid_image_view_h_
#define vil_pyramid_image_view_h_
//:
// \file
// \brief Representation of a pyramid hierarchy of image views.
//        The images can be passed to the view (in this case they are any
//        size or scale but sorted based on the scale in the descending order,
//        or can be generated in here (default case).
//        By default: the levels of views are half the size of the previous
//        level and image scales are in the descending order. The biggest image
//        is at level 0 and with scale 1.0 (1,1.0), and the next levels goes like:
//        (1,0.5), (2,0.25) etc..
//
// \author Gamze D. Tunali
// \date   Aug 16, 2010

#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>

template <class T>
class vil_pyramid_image_view
{
 public:
    //: Default constructor, creates an empty list of pyramid
    vil_pyramid_image_view(): nlevels_(0), max_levels_(256) {}

    //: Creates a pyramid with one image only and its scale is set to 1.0 (biggest)
    vil_pyramid_image_view(vil_image_view_base_sptr image): nlevels_(1), max_levels_(256)
    { images_.push_back(image); scales_.push_back(1.0); }

    //: Creates a pyramid with one image only and its scale is set to 1.0 (biggest)
    vil_pyramid_image_view(const vil_image_view<T>& image): nlevels_(1), max_levels_(256)
    { images_.push_back(new vil_image_view<T>(image)); scales_.push_back(1.0); }

    //: Creates a pyramid of nlevels and sets the image at the scale 1.0.
    // It creates smaller images for the smaller scales. Each image is
    // half the size of the previous image
    vil_pyramid_image_view(vil_image_view_base_sptr image, unsigned nlevels);

    //: creates a pyramid of empty image views
    vil_pyramid_image_view(unsigned levels, unsigned ni, unsigned nj, unsigned n_planes=1);

    //: Creates a pyramid of given image views with associated scales.
    // post: sorted in the descending order of the scales
    vil_pyramid_image_view(std::vector<vil_image_view_base_sptr> const& images,
                           std::vector<double> const& scales);

    //: Copy constructor.
    // The new object will point to the same underlying image as the rhs.
    vil_pyramid_image_view(const vil_pyramid_image_view<T>& rhs);

    virtual ~vil_pyramid_image_view() = default;

    //: adds a view to the list of view sorted in the right place based on the scale in descending order
    void add_view(vil_image_view_base_sptr &image, double scale);

    vil_image_view_base_sptr get_view(unsigned level, double& scale) { scale=scales_[level]; return images_[level];  }

    double scale(unsigned level){return scales_[level];}

    //: The pixel type of the images
    typedef T pixel_type;

    void set_max_level(unsigned l) { max_levels_=l; }

    unsigned max_levels() const { return max_levels_; }

    //: Number of pyramid levels
    unsigned nlevels() const { return nlevels_; }

    const vil_pyramid_image_view<T>& operator=(const vil_pyramid_image_view<T>& rhs);

    vil_image_view<T>& operator()(unsigned l) { return static_cast<vil_image_view<T>&>(*images_[l]); }

    // iterators
    typedef vil_image_view_base_sptr iterator;
    inline iterator begin() { return images_[0]; }
    inline iterator end  () { return images_[images_.size()-1]; }

 protected:
    //: the list of image vieas
    std::vector<vil_image_view_base_sptr> images_;

    //: the associated scales of images, scales_.size() is always equals to images_.size()
    std::vector<double> scales_;

    // the number of images in the view, 0 if it is empty
    unsigned nlevels_;

    // this is the number of levels that cannot be exceeded, by default it is 256
    unsigned max_levels_;

    //: returns true if the image size is < 4x4 or the max_level is reached
    inline bool limit_reached(unsigned i, unsigned j)
    { return i<2 || j<2 || nlevels_==max_levels_; }

    //: generates an image half the size of the given image and takes the averages
    // of pixel values in 4x4 neighborhoods
    void scale_down(const vil_image_view<T>&, vil_image_view_base_sptr& image_out);
};

#endif
