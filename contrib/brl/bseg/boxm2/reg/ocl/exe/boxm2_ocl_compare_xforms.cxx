//:
// \file
// \brief  An executable to output the distance between two R|T transforms
// \author Vishal Jain
// \date 2-july-2013


#include <vgl/vgl_vector_3d.h>
#include <vul/vul_arg.h>
#include <vul/vul_timer.h>
#include <vcl_algorithm.h>


#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_quaternion.h>
#include <vnl/vnl_math.h>

#include <vcl_iostream.h>
#include <vcl_fstream.h>

int main(int argc,  char** argv)
{
    //init vgui (should choose/determine toolkit)
    vul_arg<vcl_string> xformA_file("-xformA", "xform A filename", "");
    vul_arg<vcl_string> xformB_file("-xformB", "xform B filename", "");
    vul_arg<float> geoscale("-scale", "scale to convert in meters", 1);
    vul_arg_parse(argc, argv);

    vcl_ifstream ifileA( xformA_file().c_str() ) ;
    if(!ifileA)
    {
        vcl_cout<<"Error: Cannot open" <<xformA_file()<<vcl_endl;
        return -1;  
    }
    float scale;
    ifileA >> scale ;
    vnl_matrix<double> matA(4,4);
    ifileA >> matA;
    ifileA.close();

    matA = matA/scale;
    vnl_matrix<double> matrA(3,3);     
    matA.extract(matrA);
    vnl_quaternion<double> qA(matrA);
    vnl_vector<double> ta(3);
    ta[0] = matA[0][3];        
    ta[1] = matA[1][3]; 
    ta[2] = matA[2][3]; 


    vcl_ifstream ifileB( xformB_file().c_str() ) ;
    if(!ifileB)
    {
        vcl_cout<<"Error: Cannot open" <<xformB_file()<<vcl_endl;
        return -1;  
    }
    ifileB >> scale ;
    vnl_matrix<double> matB(4,4);
    ifileB >> matB;
    ifileB.close();

    matB = matB/scale;
    vnl_matrix<double> matrB(3,3);     
    matB.extract(matrB);
    

    vnl_vector<double> tb(3,0);
    tb[0] = matB[0][3];        
    tb[1] = matB[1][3]; 
    tb[2] = matB[2][3]; 



    vnl_quaternion<double> qB(matrB);
    double dotproduct = qA.x()*qB.x() +qA.y()*qB.y()+qA.z()*qB.z()+qA.r()*qB.r();

    vcl_cout<<"Difference in Rotation is "<<2*vcl_acos(dotproduct  )/vnl_math::pi_over_180<<" degrees "<<vcl_endl;
    vnl_vector<double> dist = (matrA.transpose()*ta - matrB.transpose()*tb);
    vcl_cout<<"Difference in Translation is "<<dist.two_norm()*scale * geoscale() <<vcl_endl;
    return 0;
}
