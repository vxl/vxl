//step_cell functors for various render functions

#ifdef RENDER_VIEW_DEP
void step_cell_render_nobs(AuxArgs aux_args,
                           int        data_ptr,
                           float      d)
{
  float alpha = aux_args.alpha[data_ptr];
  float diff_omega=exp(-alpha*d);
  float expected_nobs_cell=0.0f;
  // for rendering only

  if (diff_omega<0.995f)
  {
    float8 nobs = aux_args.nobs[data_ptr];
    float* nobs_array = (float*)(&nobs);
    float * view_weights = aux_args.view_weights;

    for(short i= 0; i < 8; i++)
    {
        if(view_weights[i] > 0.01f) {
            expected_nobs_cell += view_weights[i] * nobs_array[i];
        }
    }
  }

  float omega=(*aux_args.vis) * (1.0f - diff_omega);
  (*aux_args.vis) *= diff_omega;
  (*aux_args.exp_nobs) += expected_nobs_cell*omega;
}
#endif //RENDER_VEW_DEP
