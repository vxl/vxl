// This is brl/bpro/core/vil_pro/processes/vil_pixelwise_roc_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <bbas_pro/bbas_1d_array_float.h>
//: Constructor
bool vil_pixelwise_roc_process_cons(bprb_func_process& pro)
{
  //this process takes 1 input:
  // input(0): Filename containing the list of images to combine
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr");
  input_types.push_back("vil_image_view_base_sptr");
  input_types.push_back("vil_image_view_base_sptr");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  vcl_vector<vcl_string> output_types;
  output_types.push_back("bbas_1d_array_float_sptr");  // tp
  output_types.push_back("bbas_1d_array_float_sptr");  // tn
  output_types.push_back("bbas_1d_array_float_sptr");  // fp
  output_types.push_back("bbas_1d_array_float_sptr");  // fp
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

//: Execute the process
bool vil_pixelwise_roc_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 3) {
    vcl_cout << "vil_pixelwise_roc_process: The input number should be 3" << vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  vil_image_view_base_sptr detection_map_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr ground_truth_map_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr mask_map_sptr = pro.get_input<vil_image_view_base_sptr>(i++);

  bbas_1d_array_float * tp=new bbas_1d_array_float(10);
  bbas_1d_array_float * tn=new bbas_1d_array_float(10);
  bbas_1d_array_float * fp=new bbas_1d_array_float(10);
  bbas_1d_array_float * fn=new bbas_1d_array_float(10);

  vil_image_view<float> * detection_map;

  if(vil_image_view<unsigned char> * detection_map_uchar=dynamic_cast<vil_image_view<unsigned char> *>(detection_map_sptr.ptr()))
  {
      detection_map =new vil_image_view<float>(detection_map_uchar->ni(),detection_map_uchar->nj());
      vil_convert_stretch_range_limited<unsigned char>(*detection_map_uchar,*detection_map,0,255,0.0f,1.0f);

  }
  else if(detection_map=dynamic_cast<vil_image_view<float> *>(detection_map_sptr.ptr()))
  {

  }
  else
  {
      return false;
  }

  if(vil_image_view<unsigned char> * ground_truth_map=dynamic_cast<vil_image_view<unsigned char> *>(ground_truth_map_sptr.ptr()))
      {
          if(vil_image_view<unsigned char> * mask_map=dynamic_cast<vil_image_view<unsigned char> *>(mask_map_sptr.ptr()))
          {
                if(detection_map->ni()==ground_truth_map->ni() && 
                   detection_map->ni()==mask_map->ni() && 
                   detection_map->nj()==ground_truth_map->nj() && 
                   detection_map->nj()==mask_map->nj() )
                {
                    int cnt=0;
                    
                    for(float t=0.1f;t<=1.0f;cnt++)
                    {
                    tp->data_array[cnt]=0.0f;
                    fp->data_array[cnt]=0.0f;
                    tn->data_array[cnt]=0.0f;
                    fn->data_array[cnt]=0.0f;
                    for(unsigned i=0;i<detection_map->ni();i++)
                    {
                        for(unsigned j=0;j<detection_map->nj();j++)
                        {
                            bool pos=false;
                            bool truth=false;
                            if((*mask_map)(i,j)!=0)
                                continue;
                            if((* detection_map)(i,j)>=t)
                                pos=true;
                            else
                                pos=false;
                            if((*ground_truth_map)(i,j)>0)
                                truth=true;
                            else
                                truth=false;
                            if(pos && truth)        tp->data_array[cnt]++;
                            if((!pos) && truth)     fn->data_array[cnt]++;
                            if((!pos) && (!truth))  tn->data_array[cnt]++;
                            if(pos && (!truth))     fp->data_array[cnt]++;
                        }
                    }
                     t+=0.1f;
                    }
                }
          }
      }
  
  pro.set_output_val<bbas_1d_array_float_sptr>(0, tp);
  pro.set_output_val<bbas_1d_array_float_sptr>(1, tn);
  pro.set_output_val<bbas_1d_array_float_sptr>(2, fp);
  pro.set_output_val<bbas_1d_array_float_sptr>(3, fn);

  return true;
}

