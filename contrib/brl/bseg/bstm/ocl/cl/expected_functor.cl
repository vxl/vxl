
float4 label_color(uchar label)
{
  if(label == 1)
    return (float4)(0.14902f,0.5451f,0.82353f,1.0f); //cyan
  else
    return (float4)(0,0,0,0);
}

#ifdef RENDER
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
      CONVERT_FUNC_FLOAT16(mixture_float,aux_args.mog[data_ptr_tt])/NORM;
      
      float* mixture_array = (float*)(&mixture_float);
      float sum_weights = 0;
      for(short i= 0; i < 8; i++)
      {
          if(aux_args.app_model_weights[i] > 0.01f)
              expected_int_cell += aux_args.app_model_weights[i] * mixture_array[2*i];
      }
    }
    else
      expected_int_cell = label_color(aux_args.label[data_ptr_tt] );
    
    
  }  

  float omega=(*aux_args.vis) * (1.0f - diff_omega);
  (*aux_args.vis) *= diff_omega;
  (*aux_args.expint) += expected_int_cell*omega; 
}
#endif
