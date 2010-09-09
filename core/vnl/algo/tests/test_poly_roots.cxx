#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_rnpoly_solve.h>
#include <vnl/vnl_real_npolynomial.h>
#include <vnl/algo/vnl_svd.h>
#include <vcl_iostream.h>

void solvePolnomial(double* );

int main( int argc, char * argv[] )
{
    double coeffsBUG[5], coeffsTEST[5];

    /* Coefficients got from generating co-variance matrix for the following set of points

    9 -253 -1187
    -45 -222 -740
    -98 -223 -750

    */
    vcl_cout<<"=========================\n"
            <<"Bug points\n"
            <<"========================="<<vcl_endl;
    coeffsBUG[0]=1;
    coeffsBUG[1]=0;
    coeffsBUG[2]=-4061462907.703703;
    coeffsBUG[3]=0.000004;
    coeffsBUG[4]=4121959965347122688.000000;
    solvePolnomial(coeffsBUG);
    vcl_cout<<"Actual roots should be\n"
            "\t-45545.999131 \t+i\t0.000000\n"
            "\t-44576.056260 \t+i\t0.000000\n"
            "\t44575.974608 \t+i\t0.000000\n"
            "\t45546.080783 \t+i\t0.000000 "<<vcl_endl;


    /* Coefficients got from generating co-variance matrix for the following set of points

    .9 -25.3 -118.7
    -4.5 -22.2 -74.0
    -9.8 -22.3 -75.0

    */
    vcl_cout<<"=========================\n"
            <<"      Test points\n"
            <<"========================="<<vcl_endl;
    coeffsTEST[0]=1;
    coeffsTEST[1]=0;
    coeffsTEST[2]=-40614629.077037;
    coeffsTEST[3]=0.000000;
    coeffsTEST[4]=412195996534712.562500;
    solvePolnomial(coeffsTEST);
    vcl_cout<<"Actual roots should be\n"
            "\t-4554.599913 \t+i\t0.000000\n"
            "\t-4457.605626 \t+i\t0.000000\n"
            "\t4457.597461 \t+i\t0.000000\n"
            "\t4554.608078 \t+i\t0.000000\n"<<vcl_endl;

    return EXIT_SUCCESS;
}


void solvePolnomial(double* coeffs)
{
    vnl_vector<double> rcoeffsV(5,0);
    int i;
    for (i=0; i<5; i++) rcoeffsV[i] = coeffs[i];

    vnl_matrix<unsigned int> pol(5,1, 0);
    pol(0,0) = 4; pol(1,0) = 3;  pol(2,0) = 2;  pol(3,0) = 1;  pol(4,0) = 0;
    vnl_real_npolynomial monom1(rcoeffsV,pol);
    vcl_vector<vnl_real_npolynomial*> l(1, &monom1);
    vnl_rnpoly_solve solver(l);
    vcl_vector<vnl_vector<double>*> realVal = solver.real();
    vcl_vector<vnl_vector<double>*> imagVal = solver.imag();

    vcl_cout<< "Real part of roots has size " << realVal.size() << '\n'
            << "Imag part of roots has size " << imagVal.size() << vcl_endl;

    if (realVal.size()==0 && imagVal.size()==0)
      vcl_cout<<"No VNL roots "<<vcl_endl;
    else
    {
        vcl_cout<<"Number of roots are  "<<imagVal.size()<<vcl_endl;

        vcl_vector<vnl_vector<double>*>::iterator rp, ip;
        vcl_cout<<"VNL roots are"<<vcl_endl;
        rp = realVal.begin();
        ip=imagVal.begin();
        for (i=0; i<realVal.size(); i++) {
            vnl_vector<double>& rootr = *(*rp);
            vnl_vector<double>& rooti = *(*ip);
            vcl_cout<<"\t"<<rootr[0] <<" \t+i\t "<<rooti[0]<<vcl_endl;
            ++rp;++ip;
        }
    }
}
