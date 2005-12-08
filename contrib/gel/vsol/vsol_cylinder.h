#ifndef vsol_cylinder_h_
#define vsol_cylinder_h_
//*****************************************************************************
//:
// \file  vsol_cylinder.h
// \brief a class to hold cylinder data in 3D 
//
// This class inherits from vsol_spatial_object_3d.
//
// \author Gamze D. Tunali
// \date   2005/11/07
//
// \verbatim
//  Modifications
//   2004/10/07 Gamze D. Tunali  Created
// \endverbatim
//*****************************************************************************

#include <vgl/vgl_cylinder.h>
#include <vsol/vsol_spatial_object_3d.h>
#include <vsol/vsol_cylinder_sptr.h>
#include <vsl/vsl_binary_io.h>

class vsol_cylinder : public  vsol_spatial_object_3d
{
  vgl_cylinder<double> cyl_;

public:
    vsol_cylinder(void) {};

    vsol_cylinder(vgl_point_3d<double> centre, double radius, double length)
    : cyl_(centre, radius, length){};

    vsol_cylinder(vgl_point_3d<double> centre, double radius, double length, 
    vgl_vector_3d<double> orient)
    : cyl_(centre, radius, length){ cyl_.set_orientation(orient); }

    // copy constructor
    vsol_cylinder(vsol_cylinder const& cyl) { this->cyl_ = cyl.cyl_; }

    //: getters
    vgl_point_3d<double> center() { return cyl_.center(); }
    double radius() {return cyl_.radius(); }
    double length() {return cyl_.length(); }
    vgl_vector_3d<double> orientation() { return cyl_.orientation(); }

    //: setters
    void set_orientation(vgl_vector_3d<double> orient) {cyl_.set_orientation(orient); }
    // ==== Binary IO methods ======

    //: Binary save self to stream.
    void b_write(vsl_b_ostream &os) const;

    //: Binary load self from stream.
    void b_read(vsl_b_istream &is);

    //: Return IO version number;
    short version() const;

    //: Print an ascii summary to the stream
    void print_summary(vcl_ostream &os) const;

    void describe(vcl_ostream &strm, int blanking) const;

    //: Return a platform independent string identifying the class
    vcl_string is_a() const { return vcl_string("vsol_cylinder"); }

    //: Return true if the argument matches the string identifying the class or any parent class
    bool is_class(const vcl_string& cls) const { return cls == is_a(); }

   // implementing virtual methods of vsol_spatial_object_3d
    vsol_spatial_object_3d_type spatial_type() const { return vsol_spatial_object_3d::VOLUME; }

    vsol_spatial_object_3d* clone() const { return new vsol_cylinder(*this); }

    virtual ~vsol_cylinder(void) {};
};

//: Binary save vsol_cylinder* to stream.
void vsl_b_write(vsl_b_ostream &os, const vsol_cylinder* p);

//: Binary load vsol_cylinder_sptr from stream.
void vsl_b_write(vsl_b_istream &is, vsol_cylinder_sptr &p);

//: Binary load vsol_cylinder* from stream.
void vsl_b_read(vsl_b_istream &is, vsol_cylinder* &p);

//: Binary load vsol_cylinder_sptr from stream.
void vsl_b_read(vsl_b_istream &is, vsol_cylinder_sptr &p);

#endif
