#ifndef boxm2_render_exp_image_functor_h
#define boxm2_render_exp_image_functor_h
//:
// \file
#include <boxm2/cpp/boxm2_cast_ray_function.h>
#include <boxm2/cpp/boxm2_mog3_grey_processor.h>

class boxm2_render_exp_image_functor
{
 public:
  //: "default" constructor
  boxm2_render_exp_image_functor() {}

  bool init_data(vcl_vector<boxm2_data_base_sptr> & datas)
  {
    if ((alpha_data_=dynamic_cast<boxm2_data<BOXM2_ALPHA> *>(datas[0].ptr())) &&
        (mog3_data_ =dynamic_cast<boxm2_data<BOXM2_MOG3_GREY> *>(datas[1].ptr())) )
       return true;
    else
      return false;
  }

  inline bool step_cell(float seg_len,int index,vcl_vector<float> & vals)
  {
    boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];
    float vis=vals[1];
    float exp_exp_int=vals[0];
    float curr_p=(1-vcl_exp(-alpha*seg_len))*vis;

    exp_exp_int+=curr_p*boxm2_data_traits<BOXM2_MOG3_GREY>::processor::expected_color(mog3_data_->data()[index]);
    vis*=vcl_exp(-alpha*seg_len);

    vals[1]=vis;
    vals[0]=exp_exp_int;

    return true;
  }
 private:
  boxm2_data<BOXM2_ALPHA> * alpha_data_;
  boxm2_data<BOXM2_MOG3_GREY> * mog3_data_;
};

#if 0
//: Functor class to normalize expected image
template<class T_data>
class normalize_expected_functor_rt
{
 public:
  normalize_expected_functor_rt(bool use_black_background) : use_black_background_(use_black_background) {}

  void operator()(float mask, typename T_data::obs_datatype &pix) const
  {
    if (!use_black_background_) {
      pix += mask*1.0f;
    }
  }
  bool use_black_background_;
};
#endif // 0

void boxm2_render_exp_image(boxm2_scene_info * linfo,
                            boxm2_block_sptr blk_sptr,
                            vcl_vector<boxm2_data_base_sptr> & datas,
                            vpgl_camera_double_sptr cam ,
                            vil_image_view<float> *expected,
                            vil_image_view<float> * vis,
                            unsigned int roi_ni,
                            unsigned int roi_nj,
                            unsigned int roi_ni0=0,
                            unsigned int roi_nj0=0)
{
    boxm2_render_exp_image_functor render_functor;
    render_functor.init_data(datas);
    vcl_vector<float> vals(2,0.0);
    if (vpgl_perspective_camera<double> * pcam=dynamic_cast<vpgl_perspective_camera<double> *>(cam.ptr()))
    {
        for (unsigned i=roi_ni0;i<roi_ni;++i)
        {
            for (unsigned j=roi_nj0;j<roi_nj;++j)
            {
                vgl_ray_3d<double> ray_ij=pcam->backproject_ray(i,j);

                vgl_point_3d<float> block_origin((ray_ij.origin().x()-linfo->scene_origin[0])/linfo->block_len,
                                                 (ray_ij.origin().y()-linfo->scene_origin[1])/linfo->block_len,
                                                 (ray_ij.origin().z()-linfo->scene_origin[2])/linfo->block_len);

                float dray_ij_x=ray_ij.direction().x(),dray_ij_y=ray_ij.direction().y(),dray_ij_z=ray_ij.direction().z();

                //thresh ray direction components - too small a treshhold causes axis aligned
                //viewpoints to hang in infinite loop (block loop)
                float thresh = vcl_exp(-12.0f);
                if (vcl_fabs(dray_ij_x)  < thresh) dray_ij_x = thresh;
                if (vcl_fabs(dray_ij_y)  < thresh) dray_ij_y = thresh;
                if (vcl_fabs(dray_ij_z)  < thresh) dray_ij_z = thresh;

                vgl_vector_3d<float> direction(dray_ij_x,dray_ij_y,dray_ij_z);
                vgl_ray_3d<float> norm_ray_ij(block_origin,direction);
                vals[0]=(*expected)(i,j);
                vals[1]=(*vis)(i,j);
                boxm2_cast_ray_function<boxm2_render_exp_image_functor>(norm_ray_ij,linfo,blk_sptr,vals,render_functor);
                (*expected)(i,j)=vals[0];
                (*vis)(i,j)=vals[1];
            }
        }
    }
}

#endif
