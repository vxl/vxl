// This is brl/bseg/betr/pro/processes/betr_execute_etr_multi_chimg_evt_poly_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
#include <bpro/core/bbas_pro/bbas_1d_array_double.h>
#include <bpro/core/bbas_pro/bbas_1d_array_string.h>
#include <bpro/core/bbas_pro/bbas_1d_array_byte.h>
#include <bpro/core/bbas_pro/bbas_1d_array_int.h>
#include <vcl_string.h>
#include <vgl/vgl_point_2d.h>
#include <vsol/vsol_point_3d.h>
#include <vil/vil_image_view.h>
//:
// \file
// \brief  A process for executing an event_trigger (process change)
//
// the process handles multiple event objects and returns the base polygon
// for each object. For a polygonal object the base is the polygon itself.
// For a 3-d mesh object the base is the lowest face of the mesh.
// 
#include <vcl_compiler.h>
#include <betr/betr_event_trigger.h>
#include <vgl/vgl_point_3d.h>

namespace betr_execute_etr_multi_chimg_evt_poly_process_globals
{
  const unsigned n_inputs_  = 3;
  const unsigned n_outputs_ = 6;
}

bool betr_execute_etr_multi_chimg_evt_poly_process_cons(bprb_func_process& pro)
{
  using namespace betr_execute_etr_multi_chimg_evt_poly_process_globals;

  //process takes 2 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0]  = "betr_event_trigger_sptr"; //event_trigger
  input_types_[1]  = "vcl_string"; //algorithm name
  input_types_[2]  = "vcl_string"; //json prarameters
  // process has 6 outputs
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "bbas_1d_array_double_sptr"; // change probability
  output_types_[1] = "bbas_1d_array_string_sptr"; // event region names
  output_types_[2] = "bbas_1d_array_int_sptr";    // dimensions and offset for each change image
  output_types_[3] = "bbas_1d_array_byte_sptr";   // byte pixels for all change images in one array
  output_types_[4] = "bbas_1d_array_int_sptr";    // number of vertices in each polygon
  output_types_[5] = "bbas_1d_array_double_sptr"; // the event polygon vertices
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool betr_execute_etr_multi_chimg_evt_poly_process(bprb_func_process& pro)
{
  using namespace betr_execute_etr_multi_chimg_evt_poly_process_globals;
  
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  betr_event_trigger_sptr event_trigger = pro.get_input<betr_event_trigger_sptr>(i++);
  std::string algorithm_name = pro.get_input<std::string>(i++);
  std::string algorithm_params = pro.get_input<std::string>(i);
  if(!event_trigger)
    return false;
  std::vector<double> prob_change;
  std::vector<vil_image_resource_sptr> change_imgs;
  std::vector<vgl_point_2d<unsigned> > offsets;
  std::vector<std::string> event_region_names;
  std::cout << "executing " << algorithm_name << " with params " << algorithm_params << std::endl;
  bool good = event_trigger->process(algorithm_name, prob_change, event_region_names, change_imgs, offsets, algorithm_params);
  if(!good)
    return good;
  const std::map<std::string, betr_geo_object_3d_sptr>& evt_objs = event_trigger->evt_objects();
  unsigned n = prob_change.size();
  bbas_1d_array_string_sptr evt_names = new bbas_1d_array_string(n);
  bbas_1d_array_double_sptr change_probs = new bbas_1d_array_double(n);
  bbas_1d_array_int_sptr dims_offset = new bbas_1d_array_int(4*n);

  // get offset and change image size(in bytes)
  unsigned area_total = 0;
  unsigned k = 0;
  for(unsigned i = 0; i<n; ++i, k+=4){
    vil_image_resource_sptr imgr = change_imgs[i];
    if(!imgr)
      continue;
    unsigned ni = imgr->ni(), nj = imgr->nj();
    unsigned area  = ni*nj;
    area_total += area;
    dims_offset->data_array[k]=ni;
    dims_offset->data_array[k+1]=nj;
    dims_offset->data_array[k+2]=offsets[i].x();
    dims_offset->data_array[k+3]=offsets[i].y();
  }
  //store image pixels in the 1-d byte array
  bbas_1d_array_byte_sptr pix = new bbas_1d_array_byte(area_total);
  unsigned byte_index = 0;
  for(unsigned k = 0; k<n; ++k){
    vil_image_resource_sptr imgr = change_imgs[k];
    if(!imgr)
      continue;
    vil_image_view<vxl_byte> view = imgr->get_view();
    unsigned ni = imgr->ni(), nj = imgr->nj();
    for(unsigned j = 0; j<nj; ++j)
      for(unsigned i = 0; i<ni; ++i)
        pix->data_array[byte_index++]= view(i,j);
  }
  // store the array of change scores
  unsigned j = 0;
  for(std::vector<double>::iterator pit = prob_change.begin();
      pit != prob_change.end(); ++pit, ++j)
    change_probs->data_array[j]=*pit;

  // fill out event object names and extract the polygon vertex array
  std::vector<unsigned> nverts;
  std::vector<double> verts;
  
  for(unsigned j = 0; j<n; ++j){
    std::string enam = event_region_names[j];
    //look up the geo object by name
    std::map<std::string, betr_geo_object_3d_sptr>::const_iterator oit = evt_objs.find(enam);
    if(oit == evt_objs.end()){
      std::cout << "In betr_execute_multi_chimg_evt_poly_process - event object " << enam << " not found" << std::endl;
      return false;
    }
    //output the name
    evt_names->data_array[j]=enam;
    vsol_polygon_3d_sptr bpoly = (oit->second)->base_polygon();
    if(!bpoly){
      nverts.push_back(0);
      continue;
    }
    //encode the polygon in the 1-d arrays
    unsigned nv = bpoly->size();
    nverts.push_back(nv);
    for(unsigned i = 0; i<nv; ++i){
      vsol_point_3d_sptr v = bpoly->vertex(i);
      verts.push_back(v->x());
      verts.push_back(v->y());
      verts.push_back(v->z());
    }
  }
  // fill out output bbas_1d polygon arrays
  bbas_1d_array_int_sptr poly_nverts = new bbas_1d_array_int(nverts.size());
  bbas_1d_array_double_sptr poly_verts = new bbas_1d_array_double(verts.size());
  unsigned nv = 0;
  for(std::vector<unsigned>::iterator nit =  nverts.begin();
      nit != nverts.end(); ++nit, ++nv)
    poly_nverts->data_array[nv]=*nit;
  unsigned iv = 0;
  for(std::vector<double>::iterator vit =  verts.begin();
      vit != verts.end(); ++vit, ++iv)
    poly_verts->data_array[iv]=*vit;
  //save outputs and exit
  pro.set_output_val<bbas_1d_array_double_sptr>(0, change_probs);
  pro.set_output_val<bbas_1d_array_string_sptr>(1, evt_names);
  pro.set_output_val<bbas_1d_array_int_sptr>(2, dims_offset);
  pro.set_output_val<bbas_1d_array_byte_sptr>(3, pix);
  pro.set_output_val<bbas_1d_array_int_sptr>(4, poly_nverts);
  pro.set_output_val<bbas_1d_array_double_sptr>(5, poly_verts);
  return good;
}
