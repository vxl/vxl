#include <iostream>
#include <utility>
#include <fstream>
#include "bwm_fiducial_io.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vsl/vsl_basic_xml_element.h>
#include <vsol/vsol_point_2d.h>
//: destructor
bwm_fiducial_io::~bwm_fiducial_io()
{
}

void bwm_fiducial_io::x_write(std::ostream& s)
{
    // write out the correspondence list
    if (this->fid_corrs_.size() > 0)
    {
        vsl_basic_xml_element xml_element0(FIDUCIAL_TABLEAU_TAG);
        xml_element0.add_attribute("site", site_name_);
        xml_element0.x_write_open(s);
        vsl_basic_xml_element xml_element1(IMAGE_PATH_TAG);
        xml_element1.append_cdata(image_path_);
        xml_element1.x_write(s);
        vsl_basic_xml_element xml_element2(FID_CORR_PATH_TAG);
        xml_element2.append_cdata(fiducial_path_);
        xml_element2.x_write(s);
        vsl_basic_xml_element xml_element3(CORRESPONDENCES_TAG);
        xml_element3.add_attribute("mode", "FIDUCIAL_IMAGE_LOCATION");
        xml_element3.add_attribute("type", "FIDUCIAL_CORR");
        xml_element3.x_write_open(s);
        for (unsigned i = 0; i < this->fid_corrs_.size(); i++) {
            vgl_point_2d<double>& loc = fid_corrs_[i].loc_;
            vsl_basic_xml_element xml_loc(FID_LOC_TAG);
            xml_loc.add_attribute("v", loc.y());
            xml_loc.add_attribute("u", loc.x());
            xml_loc.x_write(s);
        }
        xml_element3.x_write_close(s);
        xml_element0.x_write_close(s);
    }
}

