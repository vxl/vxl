#ifndef mil_image_2d_h_
#define mil_image_2d_h_

//: \file
//  \brief A base class for arbitrary 2D images
//  \author Tim Cootes

#include <mil/mil_image.h>

class mil_transform_2d;

//: A base class for arbitrary 2D images
//  The valid region is [0,nx()-1][0,ny()-1]
//  world2im() gives transformation from world to image co-ordinates
class mil_image_2d : public mil_image {

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

    //: Resize to [0..nx-1][0..ny-1]
    virtual void resize(int nx, int ny) = 0;

    //: Width of image
    virtual int nx() const = 0;

    //: Height of image
    virtual int ny() const =0;

    //: Number of planes of image
    virtual int nPlanes() const = 0;
};

#endif // mil_image_2d_h_
