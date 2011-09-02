// This is brl/bpro/core/vil_pro/processes/vil_pixelwise_roc_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <bbas_pro/bbas_1d_array_float.h>

//: Constructor
bool vil_pixelwise_roc_process_cons(bprb_func_process& pro)
{
  //this process takes 3 inputs:
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr");  // change image
  input_types.push_back("vil_image_view_base_sptr");  // ground truth map
  input_types.push_back("vil_image_view_base_sptr");  // mask image
  if (! pro.set_input_types(input_types))
    return false;

  //default arguments  
  brdb_value_sptr empty_mask = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<unsigned char>(1,1)); 
  pro.set_input(2, empty_mask); 

  //this process takes 4 outputs:
  vcl_vector<vcl_string> output_types;
  output_types.push_back("bbas_1d_array_float_sptr");  // tp
  output_types.push_back("bbas_1d_array_float_sptr");  // tn
  output_types.push_back("bbas_1d_array_float_sptr");  // fp
  output_types.push_back("bbas_1d_array_float_sptr");  // fp
  return pro.set_output_types(output_types);
}

//: Execute the process
bool vil_pixelwise_roc_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 3) {
    vcl_cerr << "vil_pixelwise_roc_process: The number of inputs should be 2 (with optional 3rd (num thresh) and 4th (mask image))\n";
    return false;
  }
  
  // get the inputs
  unsigned i=0;
  vil_image_view_base_sptr detection_map_sptr    = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr ground_truth_map_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr mask_map_sptr         = pro.get_input<vil_image_view_base_sptr>(i++);
  unsigned ni = ground_truth_map_sptr->ni(); 
  unsigned nj = ground_truth_map_sptr->nj(); 
  
  //catch a "null" mask (not really null becuase that throws an error)
  bool use_mask = true; 
  if(mask_map_sptr->ni()==1 && mask_map_sptr->nj()==1) {
    vcl_cout<<"USE mask = false"<<vcl_endl;
    use_mask = false; 
  }
  
  //true positive, true negative, false positive, false negative
  bbas_1d_array_float * tp=new bbas_1d_array_float(10);
  bbas_1d_array_float * tn=new bbas_1d_array_float(10);
  bbas_1d_array_float * fp=new bbas_1d_array_float(10);
  bbas_1d_array_float * fn=new bbas_1d_array_float(10);
  vil_image_view<float> * detection_map;

  //check bounds to make sure they match
  if(detection_map_sptr->ni() != ground_truth_map_sptr->ni() || 
      detection_map_sptr->nj() != ground_truth_map_sptr->nj() ) {
    vcl_cout<<"vil_pixelwise_roc_process:: detection map doesn't match ground truth map"<<vcl_endl;
    return false; 
  }
  if(use_mask) {
    if(detection_map_sptr->ni()!=mask_map_sptr->ni() ||
         detection_map_sptr->nj()!=mask_map_sptr->nj() ) {
      vcl_cout<<"vil_pixelwise_roc_process:: detection map doesn't match mask map"<<vcl_endl;
      return false; 
    }
  }

  //convert detection map to [0,1] float
  if (vil_image_view<unsigned char> * detection_map_uchar=dynamic_cast<vil_image_view<unsigned char> *>(detection_map_sptr.ptr()))
  {
    detection_map =new vil_image_view<float>(detection_map_uchar->ni(),detection_map_uchar->nj());
    vil_convert_stretch_range_limited<unsigned char>(*detection_map_uchar,*detection_map,0,255,0.0f,1.0f);
  }
  else if (dynamic_cast<vil_image_view<float>*>(detection_map_sptr.ptr()))
  {
    detection_map=dynamic_cast<vil_image_view<float>*>(detection_map_sptr.ptr());
  }
  else
  { 
    vcl_cout<<"Detection Map cannot be converted to float image"<<vcl_endl;
    return false;
  }
  
  //cast to usable image views
  vil_image_view<unsigned char> * ground_truth_map = dynamic_cast<vil_image_view<unsigned char> *>(ground_truth_map_sptr.ptr());
  if( !ground_truth_map ) 
  {
    vcl_cout<<"vil_pixelwise_roc_process:: gt map is not an unsigned char map"<<vcl_endl;
    return false; 
  }
  vil_image_view<unsigned char> * mask_map=dynamic_cast<vil_image_view<unsigned char> *>(mask_map_sptr.ptr());
  if (!mask_map)
  {
    vcl_cout<<"vil_pixelwise_roc_process:: mask map is not an unsigned char map"<<vcl_endl;
    return false;
  }
  
  //count true positves, false positves, true negatives, false negatives
  int cnt=0;
  for (float t=0.1f;t<1.05f;++cnt,t+=0.1f)
  {
    tp->data_array[cnt]=0.0f;
    fp->data_array[cnt]=0.0f;
    tn->data_array[cnt]=0.0f;
    fn->data_array[cnt]=0.0f;
    for (unsigned i=0;i<detection_map->ni();++i)
    {
      for (unsigned j=0;j<detection_map->nj();++j)
      {
        //don't do work unless mask pixel is 0
        if (use_mask && (*mask_map)(i,j)!=0) {
          vcl_cout<<"NOT DOING WORK!!!"<<vcl_endl;
          continue;
        }  
          
        bool pos   = (* detection_map)(i,j)>=t;
        bool truth = (*ground_truth_map)(i,j)>0;
        if (pos && truth)   tp->data_array[cnt]++;
        if (!pos && truth)  fn->data_array[cnt]++;
        if (!pos && !truth) tn->data_array[cnt]++;
        if (pos && !truth)  fp->data_array[cnt]++;
      }
    }
  }
  
  //set outputs
  if (pro.n_outputs() < 4) {
    vcl_cerr << "vil_pixelwise_roc_process: The number of outputs should be 4\n";
    return false;
  }
  pro.set_output_val<bbas_1d_array_float_sptr>(0, tp);
  pro.set_output_val<bbas_1d_array_float_sptr>(1, tn);
  pro.set_output_val<bbas_1d_array_float_sptr>(2, fp);
  pro.set_output_val<bbas_1d_array_float_sptr>(3, fn);

  return true;
}

