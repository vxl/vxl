#ifndef mil_algo_gaussian_filter_h_
#define mil_algo_gaussian_filter_h_

//: \file
//  \brief Smooths images.
//  \author Ian Scott


//=======================================================================
// inclusions

#include <mil/mil_image_2d_of.h>


//: Smooths images.

template <class srcT, class destT>
class mil_algo_gaussian_filter
{
private:
    //: the sigma width of the gaussian in pixels
    double sigma_;
    //: image workspace
    mutable mil_image_2d_of<destT> worka_;

    //: Filter tap value
    // The value of the two outside elements of the 5 tap 1D FIR filter
    double filt2_;
    //: Filter tap value
    // The value of elements 2 and 4 of the 5 tap 1D FIR filter
    double filt1_;
    //: Filter tap value
    // The value of the central element of the 5 tap 1D FIR filter
    double filt0_;

    //: Filter tap value
    // The value of the first element of the 3 tap 1D FIR filter for use at the edge of the window
    // Corresponds to the filt2_ elements in a symmetrical filter
    double filt_edge2_;
    //: Filter tap value
    // The value of the second element of the 3 tap 1D FIR filter for use at the edge of the window
    // Corresponds to the filt1_ elements in a symmetrical filter
    double filt_edge1_;
    //: Filter tap value
    // The value of the third element of the 3 tap 1D FIR filter for use at the edge of the window
    // Corresponds to the filt0_ element in a symmetrical filter
    double filt_edge0_;

    //: Filter tap value
    // The value of the first element of the 4 tap 1D FIR filter for use 1 pixel away the edge of the window
    // Corresponds to the filt2_ elements in a symmetrical filter
    double filt_pen_edge2_;
    //: Filter tap value
    // The value of the second element of the 4 tap 1D FIR filter for use 1 pixel away the edge of the window
    // Corresponds to the filt1_ elements in a symmetrical filter
    double filt_pen_edge1_;
    //: Filter tap value
    // The value of the third element of the 4 tap 1D FIR filter for use 1 pixel away the edge of the window
    // Corresponds to the filt0_ elements in a symmetrical filter
    double filt_pen_edge0_;
    //: Filter tap value
    // The value of the fourth element of the 4 tap 1D FIR filter for use 1 pixel away the edge of the window
    // Corresponds to the filt1_ elements in a symmetrical filter
    double filt_pen_edge_n1_;

    //: Smooth src_im to produce dest_im
    //  Applies 5 element FIR filter in x and y.
    //  Assumes dest_im has suffient data allocated.
    void filter(destT* dest_im, int dest_ystep,
                      const srcT* src_im,
                      unsigned nx, unsigned ny,
                      int src_ystep) const;
public:
    //: Dflt ctor
    mil_algo_gaussian_filter();

    //: Consturct with given scale_step
    mil_algo_gaussian_filter(double scale_step);

    //: Destructor
    ~mil_algo_gaussian_filter();

    //: the Gaussian's sigma width in pixels.
    double width() const { return sigma_; }

    //: Set the Gaussian's sigma width in pixels.
    void set_width(double width);

    //: Smooth src_im to produce dest_im.
    void filter(mil_image_2d_of<destT>& dest_im,
                      const mil_image_2d_of<srcT>& src_im) const;

};

#endif
