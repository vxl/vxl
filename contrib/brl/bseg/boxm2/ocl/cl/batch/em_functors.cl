#ifdef SEGLENNOBS
//Update step cell functor::seg_len
void step_cell_seglen(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    int seg_int = convert_int_rte(d * SEGLEN_FACTOR);
    atom_add(&aux_args.seg_len[data_ptr], seg_int);
    atom_inc(&aux_args.nobs[data_ptr]);

}
#endif // SEGLENNOBS

#ifdef EXPSUM
void step_cell_expsum(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{

    float  alpha    = aux_args.alpha[data_ptr];
    CONVERT_FUNC_FLOAT8(mixture,aux_args.mog[data_ptr])/NORM;
    float  weight3  = (1.0f-mixture.s2-mixture.s5);


    int cum_int = aux_args.seg_len[data_ptr];
    //int mean_int = aux_args.mean_obs[data_ptr];
    //float mean_obs = convert_float(mean_int) / convert_float(cum_int);
    float cum_len = (convert_float(cum_int) / SEGLEN_FACTOR ) * aux_args.linfo->block_len;

    float mean_obs = aux_args.obs; //use the actual pixel, not the averaged mean obs

    if (cum_len > 1.0e-10f) //if the cumulative seglen is greater than epsilon
    {
        float PI = gauss_3_mixture_prob_density( mean_obs,
                                                   mixture.s0,
                                                   mixture.s1,
                                                   mixture.s2,
                                                   mixture.s3,
                                                   mixture.s4,
                                                   mixture.s5,
                                                   mixture.s6,
                                                   mixture.s7,
                                                   weight3 //(1.0f-mixture.s2-mixture.s5)
                                                  );


        float diff_omega = exp(-alpha * d* aux_args.linfo->block_len);
        float vis_prob_end = (*aux_args.vis_inf) * diff_omega;

        (*aux_args.exp_denom) += ((*aux_args.vis_inf) - vis_prob_end) *  PI; //expectation numerator sum along ray

        (*aux_args.vis_inf) = vis_prob_end; //visibility
    }
}

#endif // EXPSUM



#ifdef EXPECTATION
void step_cell_expectation(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    float  alpha    = aux_args.alpha[data_ptr];
    CONVERT_FUNC_FLOAT8(mixture,aux_args.mog[data_ptr])/NORM;
    float  weight3  = (1.0f-mixture.s2-mixture.s5);

    float cum_len = (convert_float(aux_args.seg_len[data_ptr]) / SEGLEN_FACTOR ) * aux_args.linfo->block_len;

    if (cum_len > 1.0e-10f)
    {
        float PI = gauss_3_mixture_prob_density( aux_args.obs,
                                               mixture.s0,
                                               mixture.s1,
                                               mixture.s2,
                                               mixture.s3,
                                               mixture.s4,
                                               mixture.s5,
                                               mixture.s6,
                                               mixture.s7,
                                               weight3 //(1.0f-mixture.s2-mixture.s5)
                                              );


        float diff_omega = exp(-alpha* d * aux_args.linfo->block_len);
        float vis_prob_end = (* (aux_args.vis_inf)) * diff_omega;

        (*aux_args.pre_exp_num) += ((*aux_args.vis_inf) - vis_prob_end) *  PI; //expectation num sum along ray

        //compute expectation
        float expectation = (((*aux_args.vis_inf) - vis_prob_end) *  PI) / aux_args.exp_denom; //expectation numerator / exp_denom
        //store it in aux[2]]
        int exp_int = convert_int_rte( expectation * SEGLEN_FACTOR );
        //atom_add(&aux_args.exp[data_ptr], exp_int);

        //compute pre-expectation
        float pre_exp = (*aux_args.pre_exp_num) / aux_args.exp_denom; // sum_{k<=m} expectation numerator / exp_denom
        //store it in aux[3]
        //int pre_exp_int = convert_int_rte( (1-pre_exp) * SEGLEN_FACTOR * d ); //STORE 1-pre_exp which is  sum_{k>m} expectation numerator / exp_denom
        //atom_add(&aux_args.pre_exp[data_ptr], pre_exp_int);

        (*aux_args.vis_inf) = vis_prob_end;

        /////////////////////////////////////
        //add exp_int and (uchar)(aux_args.obs * 255) to all_exp and all_obs
        uint ptr_exp = aux_args.currIdx[data_ptr]; //start
        uint total_num_rays = aux_args.nobs[data_ptr];
        uint i = 0;


        for(; i < total_num_rays; i++)
        {
            if(atomic_cmpxchg(&( aux_args.all_exp[ptr_exp+i] ), -1, exp_int) == -1) {
                aux_args.all_obs[ptr_exp+i] = (uchar)(aux_args.obs * 255);
                aux_args.all_seglen[ptr_exp+i] = d * aux_args.linfo->block_len;
                aux_args.all_preexp[ptr_exp+i] = 1-pre_exp- aux_args.pi_inf;
                break;
            }
        }
        if(i == total_num_rays) //this should NOT happen
            aux_args.all_exp[ptr_exp] = -2;
    }
}

#endif // EXPECTATION
