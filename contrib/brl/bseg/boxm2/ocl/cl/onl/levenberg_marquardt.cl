// x : nx1
// y : mx1
// J : mxn
// A : nxn

void multiplymatrix(__local float * A,
                    __local float * B,
                    __local float * C,
                    int mA, int nA,
                    int mB, int nB)
{
    int grpsize = get_local_size(0);
    int lid = get_local_id(0);
    for (int i = lid ; i < mA * nB ; i+=grpsize)
    {
        int r = i/mA;
        int c = i - r*mA;
        C[i] = 0.0f;
        for (unsigned k = 0 ; k < nA; k++)
            C[i] = C[i] + A[r*nA+k] * B[k*nA+c];
    }
}

void matrixvector(__local float * J,
                  __local float * h,
                  __local float * output,
                  int  m,
                  int  n) // J = mxn and vector is n by 1
{
    int grpsize = get_local_size(0);
    int lid = get_local_id(0);
    for (int i = lid ; i < m ; i+=grpsize)
    {
        output[i] =0.0f;
        for (unsigned k = 0 ; k < n; k++)
            output[i] = output[i] + J[i*m+k]*h[k];
    }
}

void dot_product(__local float * x1, __local float * x2, __local float * dotproduct,  int  n )
{
    int grpsize = get_local_size(0);
    int lid = get_local_id(0);
    if (lid == 0)
    {
        dotproduct[0] = 0.0f;
        for (unsigned k = 0 ; k < n; k++)
            dotproduct[0] = dotproduct[0] + x1[k]*x2[k];
    }
    barrier(CLK_LOCAL_MEM_FENCE);
}

void computeJtJ(__local float *J,
                __local float * A,
                int  m,
                int  n)
{
    int grpsize = get_local_size(0);
    int lid = get_local_id(0);
    for (int i = lid ; i < n*n ; i+=grpsize)
    {
        int r = i/n;
        int c = i - r*n;

        // r becomes column and c becomes row
        A[i] = 0.0f;
        for (unsigned k = 0 ; k < m; k++)
            A[i] = A[i] + J[k*n+r] * J[k*n+c];
    }
}

void calulateJtf(__local float * J,
                 __local float * f,
                 __local float * g,
                 int  m,
                 int  n)
{
    int grpsize = get_local_size(0);
    int lid = get_local_id(0);
    for (int i = lid ; i < n ; i+=grpsize)
    {
        g[i] =0.0f;
        for (unsigned k = 0 ; k < m; k++)
            g[i] = g[i] + J[k*n+i]*f[k];
        g[i] = -g[i];
    }
}

float maxg(__local float * g,  int  n,__local float * temp)
{
    int grpsize = get_local_size(0);
    int lid = get_local_id(0);
    float maxg = 0.0;
    if (lid == 0)
    {
        for (int i = lid ; i < n ; i++)
        {
            if ( fabs(g[i]) > maxg )
                maxg = fabs(g[i]);
        }
        temp[0] = maxg;
    }
    barrier(CLK_LOCAL_MEM_FENCE);
    return temp[0];
}

float maxA(__local float * A,  int  n, __local float * temp)
{
    int grpsize = get_local_size(0);
    int lid = get_local_id(0);
    float maxA = 0.0;
    if (lid == 0)
    {
        for (int i = lid ; i < n ; i+=grpsize)
        {
            if ( fabs(A[i*n+i]) > maxA )
                maxA = fabs(A[i*n+i]);
        }
        temp[0] = maxA;
    }
    barrier(CLK_LOCAL_MEM_FENCE);
    return temp[0];
}

float compute_eta(__local float * x,
                  __local float * y,
                  __local float * h,
                  __local float * g,
                  __local float * J,
                  __local float * A,
                  int  m,
                  int  n,
                  float  mu,
                  __local float * temp,  // 1x1
                  __local float * tempm, // mx1
                  __local float * tempn,
                  AuxArgs args) // nx1
{
    int grpsize = get_local_size(0);
    int lid = get_local_id(0);

    float denom = 0.0f;
    for (int i = lid ; i < n ; i+=grpsize)
        tempn[i] = (mu*h[i]+g[i]);  // note that g is negated in the above function

    dot_product(tempn,h,temp,n);
    if (lid == 0)
        denom =0.5f *temp[0];
    barrier(CLK_LOCAL_MEM_FENCE);

    // compute F(x) = f'f
    f(x,y,m,n,args);
    dot_product(y,y,temp,m);
    float Fx = temp[0];
    // xnew
    for (int i = lid ; i < n ; i+=grpsize)
        tempn[i] = x[i]+h[i];
    // compute F(xnew) = f'(xnew)f(xnew)
    f(tempn,y,m,n,args);
    dot_product(y,y,temp,m);
    float Fxnew = temp[0];
    if (lid == 0)
        temp[lid] = (Fx-Fxnew)/denom;
    barrier(CLK_LOCAL_MEM_FENCE);
    return temp[0];
}

float levenberg_marquadt_solve(__constant int * max_iter,    // x : nx1
                               int  m,                       // y : mx1
                               int  n,
                               __global float * output,
                               __local float * x,
                               __local float * h,            // J : mxn
                               __local float * y,            // A : nxn
                               __local float * g,
                               __local float * J,
                               __local float * A,
                               __local float * tempm,
                               __local float * tempn,
                               AuxArgs  args)
{
    int grpsize = get_local_size(0);
    int lid = get_local_id(0);
    float tau = 1e-4 ;
    float nu = 2;
    float epsilon1 = 1e-5;
    int k =0 ;
    __local float temp[1];
    // computing J, A
    Jacobian(x,J,m,n,args);
    computeJtJ(J,A,m,n);
    f(x,y,m,n,args);
    calulateJtf(J,y,g,m,n);
    // loop through different iterations
    float mu = tau * maxA(A,n,temp);
    bool found = maxg(g,n,temp) < epsilon1;
    while ( k < (*max_iter) && !found )
    {
        // compute A+\mu I
        for (int i = lid ; i <n ; i+=grpsize)
            A[i*n+i] += mu;

        cholesky_solve(A,n,g,h);
        float eta = compute_eta(x,y,h,g,J,A,m,n,mu,temp,tempm,tempn, args);
        if ( eta > 0)
        {
            //===========================
            // update x for next iteration
            for (int i = lid ; i < n ; i+=grpsize)
                x[i] = x[i]+h[i];
            Jacobian(x,J,m,n,args);
            computeJtJ(J,A,m,n);
            f(x,y,m,n,args);
            calulateJtf(J,y,g,m,n);
            found = maxg(g,n,temp) < epsilon1;
            nu = 2;
            mu = mu* max(0.3333f,1-(2*eta -1)*(2*eta -1)*(2*eta -1));
        }
        else
        {
            mu = mu * nu;
            nu = 2*nu;
        }
        //================================
        k++;
    }
    // compute final error;
    //f(x,y,m,n,args);
    //dot_product(y,y,temp,m);

    return 0.0;
}
