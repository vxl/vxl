#include <iostream>
#include "brad_phongs_model_est.h"
//
#include <vnl/vnl_identity_3x3.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_least_squares_function.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

brad_phongs_model::brad_phongs_model(float kd, float ks, float gamma, float normal_elev, float normal_azim)
{
    kd_=std::fabs(kd);
    ks_=std::fabs(ks);
    gamma_=gamma;

    normal_elev_= normal_elev;
    normal_azim_= normal_azim;
    normal_[0]=std::sin(normal_elev)*std::cos(normal_azim);
    normal_[1]=std::sin(normal_elev)*std::sin(normal_azim);
    normal_[2]=std::cos(normal_elev);
}

float brad_phongs_model::val(float view_elev, float view_azim, float sun_elev, float sun_azim)
{
    vnl_double_3 sun_dir;
    sun_dir[0]=std::sin(sun_elev)*std::cos(sun_azim);
    sun_dir[1]=std::sin(sun_elev)*std::sin(sun_azim);
    sun_dir[2]=std::cos(sun_elev);

    vnl_double_3 view_dir;
    view_dir[0]=std::sin(view_elev)*std::cos(view_azim);
    view_dir[1]=std::sin(view_elev)*std::sin(view_azim);
    view_dir[2]=std::cos(view_elev);

    vnl_identity_3x3 I;
    vnl_double_3x3 normal_outer_product=outer_product<double,3,3>(normal_,normal_);
    normal_outer_product+=normal_outer_product; // multiply by 2
    vnl_double_3x3 householder_xform= I-normal_outer_product;
    vnl_double_3 half_vector = sun_dir + view_dir;

    half_vector = half_vector / half_vector.magnitude();

    vnl_double_3 reflected_light_vector =householder_xform * sun_dir;
    double diffuse_term  = kd_* std::fabs(dot_product<double>(normal_,sun_dir));
    double specular_term = ks_* std::pow(std::fabs(dot_product<double>(reflected_light_vector,view_dir)),(double)gamma_);

    return float(diffuse_term + specular_term);
}

float brad_phongs_model::val(vnl_double_3 view_dir, float sun_elev, float sun_azim)
{
    vnl_double_3 sun_dir;
    sun_dir[0]=std::sin(sun_elev)*std::cos(sun_azim);
    sun_dir[1]=std::sin(sun_elev)*std::sin(sun_azim);
    sun_dir[2]=std::cos(sun_elev);

    vnl_identity_3x3 I;
    vnl_double_3x3 normal_outer_product=outer_product<double,3,3>(normal_,normal_);
    normal_outer_product+=normal_outer_product; // multiply by 2
    vnl_double_3x3 householder_xform= I-normal_outer_product;

    vnl_double_3 reflected_light_vector =householder_xform * sun_dir;
    vnl_double_3 half_vector = sun_dir + view_dir;

    half_vector = half_vector / half_vector.magnitude();

    double diffuse_term  = kd_* std::fabs(dot_product<double>(normal_,sun_dir));
    double specular_term = ks_* std::pow(std::fabs(dot_product<double>(reflected_light_vector,view_dir)),(double)gamma_);
    return float(diffuse_term + specular_term);
}

brad_phongs_model_approx::brad_phongs_model_approx(float kd, float ks, float gamma, float normal_elev, float normal_azim)
{
    kd_=std::fabs(kd);
    ks_=std::fabs(ks);
    gamma_=gamma;

    normal_[0]=std::sin(normal_elev)*std::cos(normal_azim);
    normal_[1]=std::sin(normal_elev)*std::sin(normal_azim);
    normal_[2]=std::cos(normal_elev);
}

float brad_phongs_model_approx::val(float  /*view_elev*/, float  /*view_azim*/, float sun_elev, float sun_azim)
{
    vnl_double_3 sun_dir;
    sun_dir[0]=std::sin(sun_elev)*std::cos(sun_azim);
    sun_dir[1]=std::sin(sun_elev)*std::sin(sun_azim);
    sun_dir[2]=std::cos(sun_elev);

#if 0 // apparently unused ...
    vnl_double_3 view_dir;
    view_dir[0]=std::sin(view_elev)*std::cos(view_azim);
    view_dir[1]=std::sin(view_elev)*std::sin(view_azim);
    view_dir[2]=std::cos(view_elev);

    vnl_identity_3x3 I;
    vnl_double_3x3 normal_outer_product=outer_product<double,3,3>(normal_,normal_);
    normal_outer_product+=normal_outer_product; // multiply by 2
    vnl_double_3x3 householder_xform= I-normal_outer_product;
    vnl_double_3 half_vector = sun_dir + view_dir;

    half_vector = half_vector / half_vector.magnitude();
    vnl_double_3 reflected_light_vector =householder_xform * sun_dir;
    double specular_term = ks_* std::pow(std::fabs(dot_product<double>(reflected_light_vector,view_dir)),(double)gamma_);
    double specular_term = ks_* std::pow(std::fabs(dot_product<double>(normal_,half_vector)),(double)gamma_);
#endif
    double diffuse_term  = kd_* std::fabs(dot_product<double>(normal_,sun_dir));
    return float(diffuse_term); // + specular_term;
}

float brad_phongs_model_approx::val(vnl_double_3 view_dir, float sun_elev, float sun_azim)
{
    vnl_double_3 sun_dir;
    sun_dir[0]=std::sin(sun_elev)*std::cos(sun_azim);
    sun_dir[1]=std::sin(sun_elev)*std::sin(sun_azim);
    sun_dir[2]=std::cos(sun_elev);

    vnl_identity_3x3 I;
    vnl_double_3x3 normal_outer_product=outer_product<double,3,3>(normal_,normal_);
    normal_outer_product+=normal_outer_product; // multiply by 2

    vnl_double_3 half_vector = sun_dir + view_dir;

    half_vector /= half_vector.magnitude();

    double dp=std::fabs(dot_product<double>(normal_,half_vector));

    double diffuse_term  = kd_* std::fabs(dot_product<double>(normal_,sun_dir));
    double specular_term = ks_* std::pow(std::fabs(dp),(double)gamma_);

    return float(diffuse_term + specular_term);
}


brad_phongs_model_est::brad_phongs_model_est(double sun_elev,
                                             double sun_azim,
                                             std::vector<vnl_double_3> & viewing_dir,
                                             vnl_vector<double> & obs,
                                             vnl_vector<double> & obs_weights,
                                             bool with_grad): vnl_least_squares_function(5, obs.size(), with_grad ? use_gradient : no_gradient)
{
    sun_elev_    = sun_elev;
    sun_azim_    = sun_azim;
    viewing_dirs_ = viewing_dir;
    obs_         = obs;
    obs_weights_ = obs_weights;
    double total_weight=0.0;

    for (double obs_weight : obs_weights_)
        total_weight += obs_weight;
    for (double & obs_weight : obs_weights_)
    {
        obs_weight/=total_weight;
        obs_weight*=obs_weights_.size();
    }
}

brad_phongs_model_est::brad_phongs_model_est(double sun_elev,
                                             double sun_azimuthal,
                                             vnl_vector<double> & camera_elevs,
                                             vnl_vector<double> & camera_azimuthals,
                                             vnl_vector<double> & obs,
                                             vnl_vector<double> & obs_weights,
                                             bool with_grad)
: vnl_least_squares_function(5, obs.size(), with_grad ? use_gradient : no_gradient)
{
    sun_elev_    = sun_elev;
    sun_azim_    = sun_azimuthal;
    camera_elev_ = camera_elevs;
    camera_azim_ = camera_azimuthals;
    obs_         = obs;
    obs_weights_ = obs_weights;
    double total_weight=0.0;

    for (double obs_weight : obs_weights_)
        total_weight += obs_weight;

    for (unsigned i=0;i<obs_weights_.size();++i)
    {
        obs_weights_[i]/=total_weight;
        obs_weights_[i]*=obs_weights_.size();

        viewing_dirs_.emplace_back(std::sin(camera_elev_[i])*std::cos(camera_azim_[i]),
                                             std::sin(camera_elev_[i])*std::sin(camera_azim_[i]),
                                             std::cos(camera_elev_[i]));
    }
}

void brad_phongs_model_est::f(vnl_vector<double> const& x, vnl_vector<double>& y)
{
    vnl_double_3 normal_vector(std::sin(x[3])*std::cos(x[4]),std::sin(x[3])*std::sin(x[4]), std::cos(x[3]));
    vnl_double_3 light_vector(std::sin(sun_elev_)*std::cos(sun_azim_),std::sin(sun_elev_)*std::sin(sun_azim_), std::cos(sun_elev_));
    vnl_identity_3x3 I;
    vnl_double_3x3 normal_outer_product=outer_product<double,3,3>(normal_vector,normal_vector);
    normal_outer_product+=normal_outer_product; // multiply by 2
    vnl_double_3x3 householder_xform= I-normal_outer_product;

    vnl_double_3 reflected_light_vector =householder_xform * light_vector;
    double diffuse_term  = std::fabs(x[0])* std::fabs(dot_product<double>(normal_vector,light_vector));
    for (unsigned i=0;i<viewing_dirs_.size();++i)
    {
        vnl_double_3 view_vector= viewing_dirs_[i];
        double dp=std::fabs(dot_product<double>(reflected_light_vector,view_vector));
        y[i]=(diffuse_term + std::fabs(x[1])*std::pow(dp,x[2])-obs_[i])* std::sqrt(obs_weights_[i]);
    }
}

float brad_phongs_model_est::error_var(vnl_vector<double> const& x)
{
    double var =0.0;
    double sum_weights = 0.0;
    vnl_double_3 normal_vector(std::sin(x[3])*std::cos(x[4]),std::sin(x[3])*std::sin(x[4]), std::cos(x[3]));
    vnl_double_3 light_vector(std::sin(sun_elev_)*std::cos(sun_azim_),std::sin(sun_elev_)*std::sin(sun_azim_), std::cos(sun_elev_));
    vnl_identity_3x3 I;
    vnl_double_3x3 normal_outer_product=outer_product<double,3,3>(normal_vector,normal_vector);
    normal_outer_product+=normal_outer_product; // multiply by 2
    vnl_double_3x3 householder_xform= I-normal_outer_product;

    vnl_double_3 reflected_light_vector =householder_xform * light_vector;
    double diffuse_term  = std::fabs(x[0])* std::fabs(dot_product<double>(normal_vector,light_vector));
    vnl_vector<double> y(viewing_dirs_.size());
    for (unsigned i=0;i<viewing_dirs_.size();++i)
    {
        vnl_double_3 view_vector= viewing_dirs_[i];
        double dp=std::fabs(dot_product<double>(reflected_light_vector,view_vector));
        y[i]=(diffuse_term + std::fabs(x[1])*std::pow(dp,x[2])-obs_[i])* std::sqrt(obs_weights_[i]);
        var         += y[i]*y[i];
        sum_weights += obs_weights_[i];
    }

    return (sum_weights > 0.0) ? float(var/sum_weights) : 0.0f;
}

void brad_phongs_model_est::gradf(vnl_vector<double> const& x, vnl_matrix<double> &J)
{
    vnl_double_3 nv(std::sin(x[3])*std::cos(x[4]),std::sin(x[3])*std::sin(x[4]), std::cos(x[3]));
    vnl_double_3 nvdt(std::cos(x[3])*std::cos(x[4]),std::cos(x[3])*std::sin(x[4]), -std::sin(x[3]));
    vnl_double_3 nvdp(-std::sin(x[3])*std::sin(x[4]),std::sin(x[3])*std::cos(x[4]), 0);
    vnl_double_3 lv(std::sin(sun_elev_)*std::cos(sun_azim_),std::sin(sun_elev_)*std::sin(sun_azim_), std::cos(sun_elev_));
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
    for (unsigned i=0;i<viewing_dirs_.size();++i)
    {
        vnl_double_3 view_vector=viewing_dirs_[i];
        double dp_rlv_vv=std::fabs(dot_product<double>(rlv,view_vector));

        double sign_of_x0 = (x[0] >= 0.0) ? 1.0 : -1.0;
        double sign_of_x1 = (x[1] >= 0.0) ? 1.0 : -1.0;

        J[i][0]=sign_of_x0*std::fabs(dot_product<double>(nv,lv))* std::sqrt(obs_weights_[i]);
        J[i][1]=sign_of_x1*std::pow(dp_rlv_vv,x[2])*std::sqrt(obs_weights_[i]);

        J[i][2]=std::fabs(x[1])*std::pow(dp_rlv_vv,x[2])*std::log(dp_rlv_vv)*std::sqrt(obs_weights_[i]);

        J[i][3]=std::fabs(x[0])*dot_product<double>(nvdt,lv)*std::sqrt(obs_weights_[i])+
                std::fabs(x[1])*x[2]*std::pow(dp_rlv_vv,x[2]-1)*dot_product<double>(householder_xform_dt_lv,view_vector)*std::sqrt(obs_weights_[i]);

        J[i][4]=std::fabs(x[0])*dot_product<double>(nvdp,lv)*std::sqrt(obs_weights_[i])+
                std::fabs(x[1])*x[2]*std::pow(dp_rlv_vv,x[2]-1)*dot_product<double>(householder_xform_dp_lv,view_vector)*std::sqrt(obs_weights_[i]);
    }
}

brad_phongs_model_approx_est::brad_phongs_model_approx_est(double sun_elev,
                                                           double sun_azim,
                                                           std::vector<vnl_double_3> & viewing_dir,
                                                           vnl_vector<double> & obs,
                                                           vnl_vector<double> & obs_weights,
                                                           bool with_grad)
: vnl_least_squares_function(5, obs.size(), with_grad ? use_gradient : no_gradient)
{
    sun_elev_    = sun_elev;
    sun_azim_    = sun_azim;
    viewing_dirs_ = viewing_dir;
    obs_         = obs;
    obs_weights_ = obs_weights;
    double total_weight=0.0;

    for (double obs_weight : obs_weights_)
        total_weight += obs_weight;
    for (double & obs_weight : obs_weights_)
    {
        obs_weight/=total_weight;
        obs_weight*=obs_weights_.size();
    }
}

brad_phongs_model_approx_est::brad_phongs_model_approx_est(double sun_elev,
                                                           double sun_azimuthal,
                                                           vnl_vector<double> & camera_elevs,
                                                           vnl_vector<double> & camera_azimuthals,
                                                           vnl_vector<double> & obs,
                                                           vnl_vector<double> & obs_weights,
                                                           bool with_grad)
: vnl_least_squares_function(5, obs.size(), with_grad ? use_gradient : no_gradient)
{
    sun_elev_    = sun_elev;
    sun_azim_    = sun_azimuthal;
    camera_elev_ = camera_elevs;
    camera_azim_ = camera_azimuthals;
    obs_         = obs;
    obs_weights_ = obs_weights;
    double total_weight=0.0;

    for (double obs_weight : obs_weights_)
        total_weight += obs_weight;

    for (unsigned i=0;i<obs_weights_.size();++i)
    {
        obs_weights_[i]/=total_weight;
        obs_weights_[i]*=obs_weights_.size();

        viewing_dirs_.emplace_back(std::sin(camera_elev_[i])*std::cos(camera_azim_[i]),
                                             std::sin(camera_elev_[i])*std::sin(camera_azim_[i]),
                                             std::cos(camera_elev_[i]));
    }
}

void brad_phongs_model_approx_est::f(vnl_vector<double> const& x, vnl_vector<double>& y)
{
    vnl_double_3 normal_vector(std::sin(x[3])*std::cos(x[4]),std::sin(x[3])*std::sin(x[4]), std::cos(x[3]));
    vnl_double_3 light_vector(std::sin(sun_elev_)*std::cos(sun_azim_),std::sin(sun_elev_)*std::sin(sun_azim_), std::cos(sun_elev_));
    double diffuse_term  = std::fabs(x[0])* std::fabs(dot_product<double>(normal_vector,light_vector));
    for (unsigned i=0;i<viewing_dirs_.size();++i)
    {
        vnl_double_3 view_vector= viewing_dirs_[i];
        vnl_double_3 half_vector = light_vector + view_vector;
        half_vector = half_vector / half_vector.magnitude();
        double dp=std::fabs(dot_product<double>(normal_vector,half_vector));
        y[i]=(diffuse_term + std::fabs(x[1])*std::pow(dp,x[2])-obs_[i])* std::sqrt(obs_weights_[i]);
    }
}

float  brad_phongs_model_approx_est::error_var(vnl_vector<double> const& x)
{
    double var =0.0;
    double sum_weights = 0.0;
    vnl_double_3 normal_vector(std::sin(x[3])*std::cos(x[4]),std::sin(x[3])*std::sin(x[4]), std::cos(x[3]));
    vnl_double_3 light_vector(std::sin(sun_elev_)*std::cos(sun_azim_),std::sin(sun_elev_)*std::sin(sun_azim_), std::cos(sun_elev_));

    double diffuse_term  = std::fabs(x[0])* std::fabs(dot_product<double>(normal_vector,light_vector));
    vnl_vector<double> y(viewing_dirs_.size());
    for (unsigned i=0;i<viewing_dirs_.size();++i)
    {
        vnl_double_3 view_vector = viewing_dirs_[i];
        vnl_double_3 half_vector = light_vector + view_vector;

        half_vector = half_vector / half_vector.magnitude();

        double dp=std::fabs(dot_product<double>(normal_vector,half_vector));
        y[i]=(diffuse_term + std::fabs(x[1])*std::pow(dp,x[2])-obs_[i])* std::sqrt(obs_weights_[i]);
        var         += y[i]*y[i];
        sum_weights += obs_weights_[i];
    }

    return (sum_weights > 0.0) ? float(var/sum_weights) : 0.0f;
}

void brad_phongs_model_approx_est::gradf(vnl_vector<double> const& x, vnl_matrix<double> &J)
{
    vnl_double_3 nv(std::sin(x[3])*std::cos(x[4]),std::sin(x[3])*std::sin(x[4]), std::cos(x[3]));
    vnl_double_3 nvdt(std::cos(x[3])*std::cos(x[4]),std::cos(x[3])*std::sin(x[4]), -std::sin(x[3]));
    vnl_double_3 nvdp(-std::sin(x[3])*std::sin(x[4]),std::sin(x[3])*std::cos(x[4]), 0);
    vnl_double_3 lv(std::sin(sun_elev_)*std::cos(sun_azim_),
                    std::sin(sun_elev_)*std::sin(sun_azim_),
                    std::cos(sun_elev_));
    for (unsigned i=0;i<viewing_dirs_.size();++i)
    {
        vnl_double_3 view_vector=viewing_dirs_[i];
        vnl_double_3 half_vector = lv + view_vector;
        half_vector = half_vector / half_vector.magnitude();

        double dp=std::fabs(dot_product<double>(nv,half_vector));

        double sign_of_x0 = (x[0] >= 0.0) ? 1.0 : -1.0;
        double sign_of_x1 = (x[1] >= 0.0) ? 1.0 : -1.0;

        J[i][0]=sign_of_x0*std::fabs(dot_product<double>(nv,lv))* std::sqrt(obs_weights_[i]);//+std::pow(dp_rlv_vv,x[2])* /*std::sqrt*/(obs_weights_[i]);
        J[i][1]=sign_of_x1*std::pow(dp,x[2])*std::sqrt(obs_weights_[i]);
        J[i][2]=std::fabs(x[1])*std::pow(dp,x[2])*std::log(dp)*std::sqrt(obs_weights_[i]);
#if 0
        J[i][3]=std::fabs(x[0])*dot_product<double>(nvdt,lv)*std::sqrt(obs_weights_[i])+
                std::fabs(x[1])*std::pow(dp_rlv_vv,x[2])*std::log(dp_rlv_vv)*dot_product<double>(householder_xform_dt_lv,view_vector)*std::sqrt(obs_weights_[i]);

        J[i][4]=std::fabs(x[0])*dot_product<double>(nvdp,lv)*std::sqrt(obs_weights_[i])+
                std::fabs(x[1])*std::pow(dp_rlv_vv,x[2])*std::log(dp_rlv_vv)
                *dot_product<double>(householder_xform_dp_lv,view_vector)*std::sqrt(obs_weights_[i]);
#else // 0
        J[i][3]=std::fabs(x[0])*dot_product<double>(nvdt,lv)*std::sqrt(obs_weights_[i])+
                std::fabs(x[1])*x[2]*std::pow(dp,x[2]-1)*dot_product<double>(nvdt,half_vector)*std::sqrt(obs_weights_[i]);

        J[i][4]=std::fabs(x[0])*dot_product<double>(nvdp,lv)*std::sqrt(obs_weights_[i])+
                std::fabs(x[1])*x[2]*std::pow(dp,x[2]-1)*dot_product<double>(nvdp,half_vector)*std::sqrt(obs_weights_[i]);
#endif // 0
    }
}
