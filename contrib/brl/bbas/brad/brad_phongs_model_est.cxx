#include <brad/brad_phongs_model_est.h>
#include <vnl/vnl_identity_3x3.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
brad_phongs_model::brad_phongs_model(float kd, float ks, float gamma, float normal_elev, float normal_azim)
{

    kd_=kd;
    ks_=ks;
    gamma_=gamma;

    normal_[0]=vcl_sin(normal_elev)*vcl_cos(normal_azim);
    normal_[1]=vcl_sin(normal_elev)*vcl_sin(normal_azim);
    normal_[2]=vcl_cos(normal_elev);
}
float brad_phongs_model::val(float view_elev, float view_azim, float sun_elev, float sun_azim)
{

    vnl_double_3 sun_dir;
    sun_dir[0]=vcl_sin(sun_elev)*vcl_cos(sun_azim);
    sun_dir[1]=vcl_sin(sun_elev)*vcl_sin(sun_azim);
    sun_dir[2]=vcl_cos(sun_elev);

    vnl_double_3 view_dir;
    view_dir[0]=vcl_sin(view_elev)*vcl_cos(view_azim);
    view_dir[1]=vcl_sin(view_elev)*vcl_sin(view_azim);
    view_dir[2]=vcl_cos(view_elev);
    
    vnl_identity_3x3 I;
    vnl_double_3x3 normal_outer_product=outer_product<double,3,3>(normal_,normal_);
    normal_outer_product+=normal_outer_product; // multiply by 2
    vnl_double_3x3 householder_xform= I-normal_outer_product;

    vnl_double_3 reflected_light_vector =householder_xform * sun_dir;
    double diffuse_term  = ks_* vcl_fabs(dot_product<double>(normal_,sun_dir));
    double specular_term = kd_* vcl_pow(vcl_fabs(dot_product<double>(reflected_light_vector,view_dir)),(double)gamma_);

    return diffuse_term + specular_term;
}
brad_phongs_model_est::brad_phongs_model_est(double sun_elev, 
                                             double sun_azimuthal,
                                             vnl_vector<double> & camera_elevs,
                                             vnl_vector<double> & camera_azimuthals,
                                             vnl_vector<double> & obs,
                                             vnl_vector<double> & obs_weights,
                                             bool with_grad): vnl_least_squares_function(5, obs.size(), with_grad ? use_gradient : no_gradient)
                                             

{

    sun_elev_    = sun_elev;
    sun_azim_    = sun_azimuthal;
    camera_elev_ = camera_elevs;
    camera_azim_ = camera_azimuthals;
    obs_         = obs;
    obs_weights_ = obs_weights;

}

void brad_phongs_model_est::f(vnl_vector<double> const& x, vnl_vector<double>& y)
{

    
    double total_weight=0.0;

    for(unsigned i=0;i<obs_weights_.size();i++)
        total_weight += obs_weights_[i];
    for(unsigned i=0;i<obs_weights_.size();i++)
       obs_weights_[i]/=total_weight;


    vnl_double_3 normal_vector(vcl_sin(x[3])*vcl_cos(x[4]),vcl_sin(x[3])*vcl_sin(x[4]), vcl_cos(x[3]));
    vnl_double_3 light_vector(vcl_sin(sun_elev_)*vcl_cos(sun_azim_),vcl_sin(sun_elev_)*vcl_sin(sun_azim_), vcl_cos(sun_elev_));
    vnl_identity_3x3 I;
    vnl_double_3x3 normal_outer_product=outer_product<double,3,3>(normal_vector,normal_vector);
    normal_outer_product+=normal_outer_product; // multiply by 2
    vnl_double_3x3 householder_xform= I-normal_outer_product;

    vnl_double_3 reflected_light_vector =householder_xform * light_vector;
    double diffuse_term  = x[0]* vcl_fabs(dot_product<double>(normal_vector,light_vector));
    for(unsigned i=0;i<camera_elev_.size();i++)
    {
        vnl_double_3 view_vector(vcl_sin(camera_elev_[i])*vcl_cos(camera_azim_[i]),
                                 vcl_sin(camera_elev_[i])*vcl_sin(camera_azim_[i]), 
                                 vcl_cos(camera_elev_[i]));
        double dp=vcl_fabs(dot_product<double>(reflected_light_vector,view_vector));
        double test=vcl_pow(dp,x[2]);
        y[i]=(diffuse_term + x[1]*vcl_pow(dp,x[2])-obs_[i])* vcl_sqrt(obs_weights_[i]);
    }
}

void brad_phongs_model_est::gradf(vnl_vector<double> const& x, vnl_matrix<double> &J)
{
    double total_weight=0.0;

    for(unsigned i=0;i<obs_weights_.size();i++)
        total_weight += obs_weights_[i];
    for(unsigned i=0;i<obs_weights_.size();i++)
       obs_weights_[i]/=total_weight;

    vnl_double_3 nv(vcl_sin(x[3])*vcl_cos(x[4]),vcl_sin(x[3])*vcl_sin(x[4]), vcl_cos(x[3]));
    vnl_double_3 nvdt(vcl_cos(x[3])*vcl_cos(x[4]),vcl_cos(x[3])*vcl_sin(x[4]), -vcl_sin(x[3]));
    vnl_double_3 nvdp(-vcl_sin(x[3])*vcl_sin(x[4]),vcl_sin(x[3])*vcl_cos(x[4]), 0);
    vnl_double_3 lv(vcl_sin(sun_elev_)*vcl_cos(sun_azim_),vcl_sin(sun_elev_)*vcl_sin(sun_azim_), vcl_cos(sun_elev_));
    vnl_identity_3x3 I;
    vnl_double_3x3 nvop=outer_product<double,3,3>(nv,nv);
    vnl_double_3x3 nvdt_nv_op=outer_product<double,3,3>(nvdt,nv);
    vnl_double_3x3 nv_nvdt_op=outer_product<double,3,3>(nv,nvdt);
    vnl_double_3x3 nvdp_nv_op=outer_product<double,3,3>(nvdp,nv);
    vnl_double_3x3 nv_nvdp_op=outer_product<double,3,3>(nv,nvdp);
    nvop+=nvop; // multiply by 2
    // derivatives w.r.t theta
    nvdt_nv_op+=nvdt_nv_op;
    nv_nvdt_op+=nv_nvdt_op;
    // derivatives w.r.t phi
    nvdp_nv_op+=nvdp_nv_op;
    nv_nvdp_op+=nv_nvdp_op;
    vnl_double_3x3 householder_xform= I-nvop;
    vnl_double_3 householder_xform_dt_lv= (I-nvdt_nv_op-nv_nvdt_op)*lv;
    vnl_double_3 householder_xform_dp_lv= (I-nvdp_nv_op-nv_nvdp_op)*lv;

    vnl_double_3 rlv =householder_xform * lv;
    for(unsigned i=0;i<camera_elev_.size();i++)
    {
        vnl_double_3 view_vector(vcl_sin(camera_elev_[i])*vcl_cos(camera_azim_[i]),
                                 vcl_sin(camera_elev_[i])*vcl_sin(camera_azim_[i]), 
                                 vcl_cos(camera_elev_[i]));
        double dp_rlv_vv=vcl_fabs(dot_product<double>(rlv,view_vector));
        J[i][0]=0;//vcl_fabs(dot_product<double>(nv,lv))* vcl_sqrt(obs_weights_[i]);
        J[i][1]=0;//-2*x[1]/vcl_pow(1+x[1]*x[1],2)*vcl_pow(dp_rlv_vv,x[2])* vcl_sqrt(obs_weights_[i]);//2*x[1]*vcl_pow(dp_rlv_vv,x[2])* vcl_sqrt(obs_weights_[i]);
        J[i][2]=0;//x[0]*x[0]*vcl_pow(dp_rlv_vv,x[2])*vcl_log(dp_rlv_vv)* vcl_sqrt(obs_weights_[i]);
        J[i][3]=0;//x[0]*x[0]*dot_product<double>(nvdt,lv)+x[1]*x[1]*vcl_pow(dp_rlv_vv,x[2])*vcl_log(dp_rlv_vv)
                //*dot_product<double>(householder_xform_dt_lv,view_vector)* vcl_sqrt(obs_weights_[i]);
        J[i][4]=x[0]*dot_product<double>(nvdp,lv)+
                x[1]*vcl_pow(dp_rlv_vv,x[2])*vcl_log(dp_rlv_vv)
                *dot_product<double>(householder_xform_dp_lv,view_vector)* vcl_sqrt(obs_weights_[i]);
    }
}
