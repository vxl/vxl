//This is brl/bbas/volm/volm_spherical_region.h
#ifndef volm_spherical_region_h_
#define volm_spherical_region_h_
//:
// \file
// \brief A class to store a spherical  region in the form of a polygon and its bounding box.
// Units are in meters
//
// \author Vishal Jain
// \date Feb 27, 2013
// \verbatim
//  Modifications
// None
// \endverbatim

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vsph/vsph_sph_box_2d.h>
#include <vcl_algorithm.h>

//const char* spherical_attributes_names[] = {"MIN_DEPTH","MAX_DEPTH","DEPTH_ORDER","DEPTH_INTERVAL","ORIENTATION","NLCD","SKY"};
enum spherical_region_attributes
{
    MIN_DEPTH = 0,
    MAX_DEPTH,
    DEPTH_ORDER,
    DEPTH_INTERVAL,
    ORIENTATION,
    NLCD,
    SKY,
    GROUND
};

// Currently the spherical region has a bounding box for a region but will be appended to have a polygon as well.
class volm_spherical_region
{
  public:
    volm_spherical_region(vsph_sph_box_2d box):box_(box){}
    //:accessors
    const vsph_sph_box_2d& bbox_ref() {return box_;}

    //: function to check if an attribute exists for this region
    bool is_attribute(spherical_region_attributes att);

    //: set the value of the attribute
    void set_attribute(spherical_region_attributes att, unsigned char value){attributes_[att]=value;}

    //: returns false if the attribute does not exist.
    bool attribute_value(spherical_region_attributes att, unsigned char & value);


    //: returns the existing attribute types for this region
    vcl_vector<spherical_region_attributes> attribute_types();
    void print(vcl_ostream& os) ;

  private:
    //: box in spherical coordinates
    vsph_sph_box_2d box_;

    //: asusming all the ttrinuites require values 0-255.
    vcl_map<spherical_region_attributes,unsigned char > attributes_;
};


// container to store a group of regions and a dictionary for the attributes.
class volm_spherical_regions_layer
{
  public:
    volm_spherical_regions_layer(){}
    void add_region(volm_spherical_region region);
    vcl_vector<volm_spherical_region> & regions(){return regions_;}
    int size(){return regions_.size();}
    //vcl_vector<unsigned int>  attributed_regions(spherical_region_attributes att, unsigned char value);
    //: returns the existing attribute types for this region
    vcl_map<unsigned char,vcl_vector<unsigned int > >  attributed_regions_by_type(spherical_region_attributes att);
    vcl_vector<unsigned int > attributed_regions_by_type_only(spherical_region_attributes att);
    //: returns the existing attribute vlaues for this region
    vcl_vector<unsigned int>  attributed_regions_by_value(spherical_region_attributes att,unsigned char & val);

  private:
    vcl_vector<volm_spherical_region> regions_;
    void update_attribute_map(int id);
    //: map of attribute types and values ( need to make a map of attrobute values )
    vcl_map<spherical_region_attributes,vcl_map<unsigned char, vcl_vector<unsigned int> > > attributed_regions_;
};

#endif // volm_spherical_region_h_
