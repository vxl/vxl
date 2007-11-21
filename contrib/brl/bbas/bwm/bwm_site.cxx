#include "bwm_site.h"
#include <vul/vul_file.h>
#include <vsl/vsl_basic_xml_element.h>

//: destructor
bwm_site::~bwm_site()
{
  // delete the tableau structures
  for (unsigned i=0; i<tableaus_.size(); i++) {
    delete tableaus_[i];
  }
}

void bwm_site::x_write(vcl_ostream& s) 
{
  s << "<BWM_CONFIG>" << vcl_endl;
  s << "<TABLEAUS>" << vcl_endl;

  for (unsigned i=0; i<tableaus_.size(); i++) {
    bwm_io_tab_config* t = tableaus_[i];

    if (t->type_name.compare("ImageTableau") == 0) {
      bwm_io_tab_config_img* img_tab = static_cast<bwm_io_tab_config_img* > (t);
      bool active = img_tab->status;
      vsl_basic_xml_element tab("ImageTableau");
      tab.add_attribute("name", img_tab->name);
      tab.add_attribute("status", active == true ? "active" : "inactive");
      tab.x_write_open(s);

      vsl_basic_xml_element img_path("imagePath");
      img_path.append_cdata(img_tab->img_path);
      img_path.x_write(s);
      tab.x_write_close(s);

    } else if (t->type_name.compare("CameraTableau") == 0) {  
      bwm_io_tab_config_cam* cam_tab = static_cast<bwm_io_tab_config_cam* > (t);
      bool active = cam_tab->status;
      vsl_basic_xml_element tab("CameraTableau");
      tab.add_attribute("name", cam_tab->name);
      tab.add_attribute("status", active == true ? "active" : "inactive");
      tab.x_write_open(s);

      vsl_basic_xml_element img_path("imagePath");
      img_path.append_cdata(cam_tab->img_path);
      img_path.x_write(s);

      vsl_basic_xml_element camera("cameraPath");
      camera.add_attribute("type", cam_tab->cam_type);
      camera.append_cdata(cam_tab->cam_path);
      camera.x_write(s);
      tab.x_write_close(s);
    }
  }
  s << "</TABLEAUS>" << vcl_endl;

  // add the LVCS
  vsl_basic_xml_element lvcs("lvcs");
  lvcs.add_attribute("lat", lvcs_.x());
  lvcs.add_attribute("lon", lvcs_.y());
  lvcs.add_attribute("elev", lvcs_.z());
  lvcs.x_write(s);

  // add the object files
  s << "<objects>" << vcl_endl;
  for (unsigned i=0; i<objects_.size(); i++) {
    if (objects_[i].first.size() > 0) {
      vsl_basic_xml_element obj("object");
      obj.add_attribute("type",objects_[i].second); 
      obj.append_cdata(objects_[i].first);
      obj.x_write(s);
    }
  }
  s << "</objects>" << vcl_endl;

  s << "</BWM_CONFIG>" << vcl_endl;
}
