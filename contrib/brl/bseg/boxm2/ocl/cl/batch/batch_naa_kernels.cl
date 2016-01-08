//THIS IS batch_naa_kernels.cl
//Created March 3 2012
//Implements batch-related algorithms for hte normal-albedo array appearance model
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable
#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif


#ifdef COMPUTE_NAA
__kernel
void batch_fit_normal_albedo_array(__global float * aux0,
                                   __global float * aux1,
                                   __global float * aux2,
                                   __global float * aux3,
                                   __global float * naa_apm_array,
                                   __global float16 * radiance_scales,
                                   __global float16 * radiance_offsets,
                                   __global float16 * radiance_var_scales,
                                   __global float16 * radiance_var_offsets,
                                   __global int *num_cells,
                                   __global int *num_obs,
                                   __local float * obs, // one per observation
                                   __local float * vis, // one per observation
                                   __local float * pre, // one per observation
                                   __local float16 * log_prob_sums // one per local thread
                                  )
{
   unsigned gid = get_group_id(0);
   uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
   if (gid<(*num_cells))
   {
      // one thread per observation
      if ( llid < (*num_obs) )
      {
         // OBTAIN obs, vis
         float seg_len = aux0[(*num_cells)*llid+gid];
         obs[llid] = 0.0;
         vis[llid] = 0.0;
         pre[llid] = 0.0;
         if (seg_len > 1e-10f)
         {
            obs[llid]   = aux1[(*num_cells)*llid+gid]/seg_len;
            vis[llid]   = aux2[(*num_cells)*llid+gid]/seg_len;
            pre[llid]   = aux3[(*num_cells)*llid+gid]/seg_len;
         }
      }
      barrier(CLK_LOCAL_MEM_FENCE);
      int n_local_threads = get_local_size(0) * get_local_size(1);
      // one thread per test albedo
      float albedo = ((float)llid) / (float)n_local_threads;
      log_prob_sums[llid] = (float16)0;
      for (int m=0; m < *num_obs; ++m)
      {
         float pre_val = pre[m];
         float16 predicted = radiance_scales[m]*albedo + radiance_offsets[m];
         float16 radiance_var = radiance_var_scales[m]*albedo*albedo + radiance_var_offsets[m];
         // normal directions pointing away from viewer are invalid
         int16 invalid = islessequal(radiance_var,(float16)0);
         float16 radiance_sigmas = sqrt(radiance_var);
         float16 prediction_densities = vis[m] * gauss_prob_density_f16(&predicted, obs[m], &radiance_sigmas);
         // add in probability of observation based on occluding cells
         prediction_densities += pre_val;
         // set prob. density to pre for invalid normal directions
         prediction_densities = select(prediction_densities,pre_val,invalid);
         // set minimum density to avoid log(0)
         prediction_densities = max(prediction_densities,1e-4);
         // multiply by scale factor to prevent underflow
         prediction_densities *= 100.0;
         log_prob_sums[llid] += log(prediction_densities);
      }

      barrier(CLK_LOCAL_MEM_FENCE);

      // single thread: determine best albedo per normal and normal probability dist.
      if (llid == 0)
      {
         __global float16 *albedos = (__global float16 *)&(naa_apm_array[32*gid]);
         __global float16 *probs = (__global float16 *)&(naa_apm_array[32*gid + 16]);

         // search for best probability per normal
         float16 max_log_prob_sum = log_prob_sums[0];
         for (int i=0; i<n_local_threads; ++i)
         {
            float albedo = ((float)i) / (float)n_local_threads;
            int16 is_best = log_prob_sums[i] > max_log_prob_sum;
            *albedos = select(*albedos, (float16)albedo, is_best);
            max_log_prob_sum = select(max_log_prob_sum, log_prob_sums[i], is_best);
         }
         // compute probability distribution of normal directions
         float16 normal_probs = exp(max_log_prob_sum);
         float prob_sum = dot(normal_probs,(float16)1.0);
         if (prob_sum > 1e-10)
         {
            *probs = normal_probs / prob_sum;
         }
         else
         {
            *probs = (float16)(1.0/16.0);
         }
      }
   }
}
#endif // COMPUTE_NAA

