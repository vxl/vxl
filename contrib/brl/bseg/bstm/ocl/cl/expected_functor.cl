
float4 label_color(uchar label)
{
  if(label == 1)
    return (float4)(0.14902f,0.5451f,0.82353f,1.0f); //cyan
  else if(label == 2)
    return (float4)(0.215686f, 0.796078f,0.29411f,1.0f); //green
  else if(label == 3)
    return (float4)(0.796078f,0.29411f,0.0862f,1.0f); //orange
  else
    return (float4)(0,0,0,0);
}

float4 jet_color(uchar label)
{
  if(label == 1)
    return (float4)(0,0,1.0f,1.0f); //blue
  else if(label == 2)
    return (float4)( 0.0f ,   0.5000f,    1.00f,1.0f);
  else if(label == 3)
    return (float4)( 0, 1.0000 ,1.0000,1.0f);
  else if(label == 4)
    return (float4)(1.0f,0.5f ,0.50f,1.0f); // orange
  else if(label == 5)
    return (float4)(1.0f,1.00f ,0.0f,1.0f); //yellow
  else if(label == 6)
    return (float4)(1.0f,0.0f ,0.0f,1.0f); //red
}

#ifdef RENDER_MOG
void step_cell_render(AuxArgs aux_args, int data_ptr_tt, float d)
{
  float alpha = aux_args.alpha[data_ptr_tt];
  float diff_omega=exp(-alpha*d);
  float expected_int_cell=0.0f;
  // for rendering only
  if (diff_omega<0.995f)
  {
    CONVERT_FUNC(udata,aux_args.mog[data_ptr_tt]);
    float8  data=convert_float8(udata)/(float)NORM;
    EXPECTED_INT(expected_int_cell,data);
  }
  float omega=(*aux_args.vis) * (1.0f - diff_omega);
  (*aux_args.vis)  *= diff_omega;
  (*aux_args.expint) += expected_int_cell*omega;
}
#endif

#ifdef RENDER_VIEW_DEP
void step_cell_render(AuxArgs aux_args, int data_ptr_tt, float d)
{
  float alpha = aux_args.alpha[data_ptr_tt];
  float diff_omega=exp(-alpha*d);
  float4 expected_int_cell=0.0f;

  if (diff_omega<0.995f)
  {
    if(!aux_args.render_label || aux_args.label[data_ptr_tt] == 0)
    {

#ifdef MOG_VIEW_DEP_COLOR_COMPACT
      float8 mixture_float = aux_args.mog[data_ptr_tt];
#elif MOG_VIEW_DEP_COMPACT
      CONVERT_FUNC_FLOAT16(mixture_float,aux_args.mog[data_ptr_tt]);
#else
      float16 mixture_float = aux_args.mog[data_ptr_tt];
#endif
      float* mixture_array = (float*)(&mixture_float);


      float sum_weights = 0;
      for(short i= 0; i < 8; i++)
      {
          if(aux_args.app_model_weights[i] > 0.01f) {
#ifdef MOG_VIEW_DEP_COLOR_COMPACT
            uchar4 tmp_mu = as_uchar4(mixture_array[i]) ;
            expected_int_cell += aux_args.app_model_weights[i] * convert_float4(tmp_mu)/NORM;
#else
            expected_int_cell += aux_args.app_model_weights[i] * mixture_array[2*i];
#endif
          }
      }
    }
    else
      expected_int_cell = jet_color(aux_args.label[data_ptr_tt] );
  }

  float omega=(*aux_args.vis) * (1.0f - diff_omega);
  (*aux_args.vis) *= diff_omega;
  (*aux_args.expint) += expected_int_cell*omega;
}
#endif


#ifdef RENDER_CHANGE
void step_cell_render_change(AuxArgs aux_args, int data_ptr, int data_ptr_tt, float d)
{
  float alpha = aux_args.alpha[data_ptr_tt];
  float diff_omega=exp(-alpha*d);

/*
  if ( (*aux_args.expint).s0 < 1.0f && (*aux_args.vis) > 0.8f )
  {
    if(aux_args.mog[data_ptr] > 0.5f)
      (*aux_args.expint) = 1.0f;
  }
  */

  (*aux_args.expint) += aux_args.mog[data_ptr] * (*aux_args.vis);

  (*aux_args.vis)  *= diff_omega * (1-aux_args.mog[data_ptr]) ;
}
#endif
