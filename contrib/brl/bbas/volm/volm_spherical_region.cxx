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

void volm_spherical_region::print(std::ostream& os)
{
    os<<"Box "<<box_<<' ';
    auto iter = attributes_.begin();
    for (;iter!= attributes_.end();iter++)
    {
        os<<'('<<iter->first<<", "<<(int)iter->second<<") " ;
    }
    os<<'\n';
}

std::vector<spherical_region_attributes> volm_spherical_region::attribute_types()
{
    std::vector<spherical_region_attributes> attributes;
    auto iter = attributes_.begin();
    for (;iter!= attributes_.end();iter++)
    {
        attributes.push_back(iter->first);
    }
    return attributes;
}

void volm_spherical_regions_layer::add_region(const volm_spherical_region& region)
{
    regions_.push_back(region);
    this->update_attribute_map(regions_.size()-1);
}


std::map<unsigned char,std::vector<unsigned int > >
volm_spherical_regions_layer::attributed_regions_by_type(spherical_region_attributes att)
{
    if ( attributed_regions_.find(att)!= attributed_regions_.end())
    {
        return  attributed_regions_[att];
    }
    else // return an empty vector
    {
        std::map<unsigned char, std::vector<unsigned int> > ids;
        return ids;
    }
}

std::vector<unsigned int >
volm_spherical_regions_layer::attributed_regions_by_type_only(spherical_region_attributes att)
{
    if ( attributed_regions_.find(att)!= attributed_regions_.end())
    {
        auto iter = attributed_regions_[att].begin();
        std::vector<unsigned int>  sids;
        for (;iter!=attributed_regions_[att].end();iter++)
            sids.insert(sids.begin(),iter->second.begin(),iter->second.end());
        return  sids;
    }
    else // return an empty vector
    {
         std::vector<unsigned int>  ids;
        return ids;
    }
}

std::vector<unsigned int>
    volm_spherical_regions_layer::attributed_regions_by_value(spherical_region_attributes att,unsigned char & val)
{
    std::map<unsigned char,std::vector<unsigned int > > temp = this->attributed_regions_by_type(att);

    if ( temp.find(val)!= temp.end())
    {
        return temp[val];
    }
    std::vector<unsigned int> ids;
    return ids;
}

void volm_spherical_regions_layer::update_attribute_map(int id)
{
    std::vector<spherical_region_attributes> attribute_types = regions_[id].attribute_types();
    for (auto & attribute_type : attribute_types)
    {
        unsigned char val =255;
        if (regions_[id].attribute_value(attribute_type,val))
            attributed_regions_[attribute_type][val].push_back(id);
    }
}
