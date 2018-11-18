#include <iostream>
#include "boxm2_phongs_model_processor.h"
//
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/vnl_math.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

float boxm2_phongs_model_processor::expected_color(brad_phongs_model & pmodel,
                                                   vnl_double_3 view_dir,
                                                   float sun_elev,
                                                   float sun_azim)
{
        return pmodel.val(view_dir,sun_elev,sun_azim);
}

float boxm2_phongs_model_processor::prob_density(float obs, float expected_obs, float var)
{
    if (var>0.0f)
        return (float)std::exp(-(obs-expected_obs)*(obs-expected_obs)/(2*var))/(float)std::sqrt(vnl_math::twopi*var);
    else
        return 1.0f;
}

brad_phongs_model boxm2_phongs_model_processor::compute_phongs_model(float & var,
                                                                     float sun_elev,
                                                                     float sun_azim,
                                                                     std::vector<float>        & obs,
                                                                     std::vector<vnl_double_3> & view_dirs,
                                                                     std::vector<float>        & vis)
{
    vnl_vector<double> samples(obs.size());
    vnl_vector<double> samples_weights(obs.size());

    float sum_weights = 0.0f;
    float sum_weighted_intensities = 0.0f;
    int count  = 0;
    for (unsigned i = 0 ;i< obs.size(); i++)
    {
        samples[i]= obs[i];
        samples_weights[i] = vis[i];
        sum_weights += vis[i];
        sum_weighted_intensities += obs[i]*vis[i];
        if (vis[i] > 0.0)
            count++;
    }

    vnl_vector<double> x(5);
    if (sum_weights > 0.0) x[0] = sum_weighted_intensities/sum_weights;
    else  x[0] = 0.0;

    x[1] = 0.1;
    x[2] = 5.0;
    x[3] = 0.0;
    x[4] = 0.0;
    var = 0.09f;

    if (count>5)
    {
        brad_phongs_model_est f(sun_elev,
                                sun_azim,
                                view_dirs,
                                samples,
                                samples_weights,
                                true);

        vnl_levenberg_marquardt lm(f);
        lm.set_max_function_evals(100);
        lm.minimize(x);
        var = f.error_var(x);
    }
    return brad_phongs_model(float(x[0]),float(x[1]),float(x[2]),float(x[3]),float(x[4]));
}
