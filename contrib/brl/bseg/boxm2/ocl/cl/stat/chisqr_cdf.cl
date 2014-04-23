float chisqr_cdf(__global float  * chisqrlookup, int k, float chi2)
{
    // Code to produce this data
    /*
    from scipy import stats
    import numpy as np;
    from matplotlib import pyplot as plt;
    x = np.arange(0.0,50,0.1);
    x[0] = 0.0001;
    f= open("e:/data/4D/NTC/chi2.txt",'w');
    for k in range(1, 20):
    y = stats.chi2.cdf(x , k)
    print >>f,','.join([str(yval) for yval in y]), ","
    f.close();
    */

    int length = 500;
    if( k > 20 )
        k = 20;
    if(k <= 20 && k > 0 )
    {
        int indexx = 0;
        if(chi2 >= 50 )
            indexx  = length - 1;
        else
            indexx = (int) floor(chi2/0.1) ;
        return chisqrlookup[(k-1)*length+indexx];
    }
    else
        return 0.0f;
}