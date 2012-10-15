bool fit_intensity_cubic(__local float * obs,       // dim n
                         __local float * vis,       // dim n
                         __local float * s,         // dim n
                         __local float * temp,
                         __local float * XtWX,      // dim 16
                         __local float * cofactor,  // dim 16
                         __local float * invXtWX,   // dim 16
                         __local float * XtY,       // dim 4
                         __local float * outerprodl,// dim 16
                         __local float * l,         // dim 4
                         __global float * coeffs,   // dim 8
                         __constant int * nobs)
{
    // construct the matrix XtWX
    int gid = get_group_id(0);
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
    // find the min s and the max s;
    if (llid == 0)
    {
        int min_index = 0;
        int max_index = (*nobs)-1;
        float stemp =10000;
        for (unsigned int i = 0 ; i< (*nobs) ; i++)
        {
            if (s[i]<stemp)
            {
                min_index = i;
                stemp = s[i];
            }
        }
        stemp = -10000;
        for (unsigned int i = 0 ; i< (*nobs) ; i++)
        {
            if (s[i]>stemp)
            {
                max_index = i;
                stemp = s[i];
            }
        }
        l[0] = 0.0;
        l[1] = s[min_index]-s[max_index];
        l[2] = s[min_index]*s[min_index]  - s[max_index]*s[max_index];
        l[3] = s[min_index]*s[min_index]*s[min_index]  - s[max_index]*s[max_index]*s[max_index];
    }
    barrier(CLK_LOCAL_MEM_FENCE);
    if (llid < (*nobs) )
    {
        temp[llid] = vis[llid];
    }
    barrier(CLK_LOCAL_MEM_FENCE);
    for (unsigned int k = 0 ; k< 7; k++)
    {
        float sum = 0;
        for (unsigned int i = 0 ; i< (*nobs) ; i++)
            sum+= temp[i];
        // writing at appropriate places in the matrix.
        if (get_local_id(0) + get_local_id(1) == k && get_local_id(0) < 4 && get_local_id(1) < 4)
            XtWX[get_local_id(0) + 4*get_local_id(1)] = sum;
        barrier(CLK_LOCAL_MEM_FENCE);

        if (llid < (*nobs) )
            temp[llid] = temp[llid] * s[llid];
        barrier(CLK_LOCAL_MEM_FENCE);
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    onl_outerproduct_4x4(l,l,outerprodl);
    if (llid < 16)
        XtWX[ llid] = XtWX[ llid] + outerprodl[llid];
    barrier(CLK_LOCAL_MEM_FENCE);
    // RHS XtWY
    if (llid < (*nobs) )
        temp[llid] = obs[llid]*vis[llid];

    barrier(CLK_LOCAL_MEM_FENCE);

    if (llid== 0)
    {
        for (unsigned int k = 0 ; k< 4; k++)
        {
            float sum = 0;
            for (unsigned int i = 0 ; i< *nobs; i++)
            {
                sum+= temp[i];
                temp[i] = temp[i] * s[i];
            }
            XtY[k] = sum;
        }
    }
    barrier(CLK_LOCAL_MEM_FENCE);
    //// Inverse of XtWX
    onl_inverse_4x4(XtWX,cofactor, invXtWX);
    barrier(CLK_LOCAL_MEM_FENCE);
    // inv(XtWX) * XtY
    for ( unsigned int k = 0 ; k < 4; k++)
    {
        if (llid == k)
        {
            float sum = 0;
            for (unsigned int i = 0 ; i< 4; i++)
                sum+= invXtWX[k*4+i] * XtY[i];
            coeffs[gid*8+k] = sum;
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }
    barrier(CLK_LOCAL_MEM_FENCE);
}


float variance_multiplier(float n_obs)
{
    if (n_obs<2)
        return 100.0;//shouldn't happen
    if (n_obs==2)
        return 79.79;
    float noff = n_obs-1.5;
    float npow = pow(noff, 1.55f);
    return 1.28 + 20.0/(1.0+npow);
}

void cubic_fit_error(__local float  * obs,       // dim n
                     __local float  * vis,       // dim n
                     __local float  * s,         // dim n
                     __local float  * temp,
                     __global float * coeffs,   // dim 8
                     __constant float * internal_sigma,
                     __constant int * nobs)
{
    // Compute Variance or Error
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
    int gid = get_group_id(0);

    float a = coeffs[gid*8+0];
    float b = coeffs[gid*8+1];
    float c = coeffs[gid*8+2];
    float d = coeffs[gid*8+3];

    if (llid < *nobs)
    {
        temp[llid] = vis[llid]*(a+b*s[llid]+c*s[llid]*s[llid]+d*s[llid]*s[llid]*s[llid] - obs[llid]);
        temp[llid] = temp[llid]*temp[llid];
    }
    barrier(CLK_LOCAL_MEM_FENCE);
    if (llid == 0)
    {
        float var = 0.0f;
        float e_nobs = 0.0f;
        for (unsigned i = 0; i < *nobs; i++)
        {
            var += temp[i];
            e_nobs +=vis[i];
        }
        if (e_nobs <2)
            var = 1.0;
        else
        {
            var = var * variance_multiplier(e_nobs);
            var = var / (e_nobs-1);
        }
        if (var > 0.0f)
            coeffs[gid*8+4] =sqrt(var);
        else
            coeffs[gid*8+4] = 1.0;
        // Computing Density
        float denom = sqrt(2*M_PI)*(*internal_sigma);
        float numer = exp(-var/(2*(*internal_sigma)*(*internal_sigma)));
        coeffs[gid*8+5] = numer/denom;

        if (var < 0 )
            coeffs[gid*8+5] = 1.0;
    }
    barrier(CLK_LOCAL_MEM_FENCE);
}

#if 0
// Entropy based empty model
void compute_empty(__local float * obs,       // dim n
                   __local float * vis,       // dim n
                   __local float * hist,      // dim 16
                   __global float * coeffs,       // dim 8
                   __constant int * nobs)
{
    // construct the matrix XtWX
    int gid = get_group_id(0);
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
    if (llid < 8)
        hist[llid] = 0.125;
    if (llid == 0)
    {
        // initialize the histogram with uniform density.
        float sum = 1.0f;
        for (unsigned i = 0; i< (*nobs) ; i++)
        {
            int next = i+1;
            int prev = i-1;
            if (i == (*nobs)-1)
                next =0;
            if ( i == 0)
                prev = (*nobs)-1;

            float gradI=fabs(obs[i]-obs[next]);
            float gradgradI=fabs(2*obs[i]-obs[next]-obs[prev]);
            hist[(int)ceil(gradI*8/2)] += (vis[i]+vis[next])/2;
            sum +=(vis[i]+vis[next])/2;
        }

        for (unsigned i = 0; i< 8; i++)
            hist[i] /= sum;
    }

    barrier(CLK_LOCAL_MEM_FENCE);
    if (llid == 0)
    {
        float entropy_histo = 0.0f;
        for (unsigned int i = 0; i<8; ++i)
        {
            entropy_histo += hist[i]*log(hist[i]);
        }
        entropy_histo  = exp(-entropy_histo);
        coeffs[gid*8+6]= entropy_histo;
    }
    barrier(CLK_LOCAL_MEM_FENCE);
}
#endif // 0

// empty function based on posts
void compute_empty(__local float * posts,       // dim n
                   __local float * vis,       // dim n
                   __global float * coeffs,       // dim 8
                   __constant int * nobs)
{
    // construct the matrix XtWX
    int gid = get_group_id(0);
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
    float sigma2 = 0.007;
    if (llid == 0)
    {
        // initialize the histogram with uniform density.
        float sum = 0.0f;
        float sum_p = 0.0f;
        for (unsigned i = 0; i< (*nobs) ; i++)
        {
            if (vis[i]!=1.0)
            {
            sum  += vis[i];
            //sum_p+= -0.5*vis[i]*(1-posts[i])*(1-posts[i])/sigma2;
            sum_p+= vis[i]*posts[i];
            }
        }
        if (sum > 0.0)
            sum_p=sum_p/sum;

        //float p = exp(sum_p) ;
        //p= p/sqrt(2*M_PI*sigma2);
        coeffs[gid*8+6]= sum_p;
    }
    barrier(CLK_LOCAL_MEM_FENCE);
}

#if 0
// Zigzag based empty model
void compute_empty(__local float * obs,       // dim n
                   __local float * vis,       // dim n
                   __local float * hist,      // dim 16
                   __global float * coeffs,       // dim 8
                   __constant int * nobs)
{
    // construct the matrix XtWX
    int gid = get_group_id(0);
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
    if (llid == 0)
    {
        // initialize the histogram with uniform density.
        float sum_weights = 0.0f;
        float avg_variance = 0.0f;
        for (int i = 0; i< (*nobs) ; i++)
        {
            int next = i+1;
            int prev = i-1;
            if (i == (*nobs)-1)
                next =0;
            if ( i == 0)
                prev = (*nobs)-1;
            float w= min(vis[prev],min(vis[i],vis[next]));
            avg_variance+=w*fabs((obs[i]+obs[next]+obs[prev])/3- obs[i])*fabs((obs[i]+obs[next]+obs[prev])/3- obs[i]);
            sum_weights =sum_weights +w ;
        }
        avg_variance= avg_variance / sum_weights;
        avg_variance= avg_variance/(0.025*0.025);

        coeffs[gid*8+6]= avg_variance;//exp(avg_variance);//numer/denom;
    }
    barrier(CLK_LOCAL_MEM_FENCE);
}
#endif // 0
