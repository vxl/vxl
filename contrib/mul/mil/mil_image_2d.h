#ifndef mil_image_2d_h_
#define mil_image_2d_h_
//:
//  \file
//  \brief A base class for arbitrary 2D images
//  \author Tim Cootes

#include <mil/mil_image.h>

class mil_transform_2d;

//: A base class for arbitrary 2D images
//  The valid region is [0,nx()-1][0,ny()-1]
//  world2im() gives transformation from world to image co-ordinates
class mil_image_2d : public mil_image {
    //: Resize current planes to [0..nx-1][0..ny-1]
    //  Provided because we can't overload virtual functions
    virtual void resize2(int nx, int ny) = 0;

    //: Resize to n_planes of [0..nx-1][0..ny-1]
    //  Provided because we can't overload virtual functions
    virtual void resize3(int nx, int ny, int n_planes) = 0;

 public:
    //: Dflt ctor
    mil_image_2d() {};

    //: Destructor
    virtual ~mil_image_2d() {};

    //: Number of dimensions of image (2)
    virtual int n_dims() const { return 2; }

    //: Current world-to-image transformation
    virtual const mil_transform_2d& world2im() const =0;

    //: Set world-to-image transformation
    virtual void setWorld2im(const mil_transform_2d& w2i) =0;

    //: Resize current planes to [0..nx-1][0..ny-1]
    //  Implemented using resize2()
    void resize(int nx, int ny)
    { resize2(nx,ny); }

    //: Resize to n_planes of [0..nx-1][0..ny-1]
    //  Implemented using resize2()
    void resize(int nx, int ny, int n_planes)
    { resize3(nx,ny,n_planes); }

    //: Depth of image (set to 1 for 2D images)
    virtual int nz() const { return 1; }

    //: Does the name of the class match the argument?
    virtual bool is_class(vcl_string const& s) const
    { return s=="mil_image_2d"; }
};

#endif // mil_image_2d_h_
