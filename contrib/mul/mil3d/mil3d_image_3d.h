#ifndef mil3d_image_3d_h_
#define mil3d_image_3d_h_
//:
//  \file
//  \brief A base class for arbitrary 3D images
//  \author Graham Vincent following scheme for mil_image_2d by Tim Cootes)

#include <mil/mil_image.h>

class mil3d_transform_3d;

//: A base class for arbitrary 3D images
//  The valid region is [0,nx()-1][0,ny()-1][0..nz()-1]
//  world2im() gives transformation from world to image co-ordinates
class mil3d_image_3d : public mil_image {
 public:
    enum Axis { XAXIS=0, YAXIS, ZAXIS };

 private:
    //: Resize current planes to [0..nx-1][0..ny-1][0..nz-1]
    //  Provided because we can't overload virtual functions
    virtual void resize2(int nx, int ny, int nz) = 0;

    //: Resize to n_planes of [0..nx-1][0..ny-1][0..nz-1]
    //  Provided because we can't overload virtual functions
    virtual void resize3(int nx, int ny, int nz, int n_planes) = 0;

 public:
    //: Dflt ctor
    mil3d_image_3d() {};

    //: Destructor
    virtual ~mil3d_image_3d() {};

    //: Number of dimensions of image (3)
    virtual int n_dims() const { return 3; }

    //: Current world-to-image transformation
    virtual const mil3d_transform_3d& world2im() const =0;

    //: Set world-to-image transformation
//    virtual void setWorld2im(const mil3d_transform_3d& w2i) =0;

    //: Resize current planes to [0..nx-1][0..ny-1][0..nz-1]
    //  Implemented using resize2()
    void resize(int nx, int ny, int nz)
    { resize2(nx,ny,nz); }

    //: Resize to n_planes of [0..nx-1][0..ny-1][0..nz-1]
    //  Implemented using resize2()
    void resize(int nx, int ny, int nz, int n_planes)
    { resize3(nx,ny,nz,n_planes); }

    //: Does the name of the class match the argument?
    virtual bool is_class(vcl_string const& s) const
    { return s=="mil3d_image_3d"; }
};

#endif // mil3d_image_3d_h_
