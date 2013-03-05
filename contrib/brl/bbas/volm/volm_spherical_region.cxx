#include "volm_spherical_region.h"

#define stringify( name ) # name
bool volm_spherical_region::is_attribute(spherical_region_attributes att)
{
    if (attributes_.find(att)!=attributes_.end())
        return true;
    else
        return false;
}

bool volm_spherical_region::attribute_value(spherical_region_attributes att, unsigned char & value)
{
    if (this->is_attribute(att))
    {
        value = attributes_[att];
        return true;
    }
    else
    {
        value = 255;
        return false;
    }
}

void volm_spherical_region::print(vcl_ostream& os)
{
    os<<"Box "<<box_<<' ';
    vcl_map<spherical_region_attributes,unsigned char>::iterator iter = attributes_.begin();
    for (;iter!= attributes_.end();iter++)
    {
        os<<'('<<iter->first<<", "<<(int)iter->second<<") " ;
    }
    os<<'\n';
}

vcl_vector<spherical_region_attributes> volm_spherical_region::attribute_types()
{
    vcl_vector<spherical_region_attributes> attributes;
    vcl_map<spherical_region_attributes,unsigned char>::iterator iter = attributes_.begin();
    for (;iter!= attributes_.end();iter++)
    {
        attributes.push_back(iter->first);
    }
    return attributes;
}

void volm_spherical_regions_layer::add_region(volm_spherical_region region)
{
    regions_.push_back(region);
    this->update_attribute_map(regions_.size()-1);
}


vcl_vector<unsigned int>
volm_spherical_regions_layer::attributed_regions(spherical_region_attributes att) const
{
    vcl_vector<unsigned int> ids;
    if ( attributed_regions_.find(att) != attributed_regions_.end())
    {
        // a map is not changed by looking up one of its values ...
        ids = (const_cast<volm_spherical_regions_layer*>(this))->attributed_regions_[att];
    }
    return ids;
}

void volm_spherical_regions_layer::update_attribute_map(int id)
{
    vcl_vector<spherical_region_attributes> attribute_types = regions_[id].attribute_types();
    for (unsigned i =0; i < attribute_types.size(); i++)
    {
        attributed_regions_[attribute_types[i]].push_back(id);
    }
}
