#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vnl/vnl_math.h>

#include <vpgl/vpgl_basic_camera.h>
#include <vpgl/vpgl_matrix_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

#include <vcsl/vcsl_spatial.h>
#include <vcsl/vcsl_unit.h>
#include <vcsl/vcsl_graph.h>
#include <vcsl/vcsl_axis.h>
#include <vcsl/vcsl_dimension.h>
#include <vcsl/vcsl_meter.h>
#include <vcsl/vcsl_length.h>

int get_int_random ()
{
  int temp= rand();
  temp %= 200;
  temp -= 100;
  return temp;
}

double get_rnd_angle()
{
  double temp = ((double) rand())/RAND_MAX;
  return temp*vnl_math::pi;
}

int main(int argc, char ** argv)
{
  vcl_string path = "./egemen1.cm";
  if (argc > 1) path = vcl_string(argv[1]);
  vcl_cout << "writing correspondence file " << path << vcl_endl;
  vcl_ofstream output(path.c_str());

  srand(1978);
  vcl_vector<double> parameters;
  vpgl_perspective_camera pers;
  parameters = pers.get_params();
  parameters[vpgl_perspective_camera::XL] = 10;
  parameters[vpgl_perspective_camera::YL] =  5;
  parameters[vpgl_perspective_camera::ZL] =  8;

  parameters[vpgl_perspective_camera::omega] = vnl_math::pi/2;
  parameters[vpgl_perspective_camera::phi] =   0;
  parameters[vpgl_perspective_camera::kappa] = 0;

  pers.set_params(parameters);

  output << "NUMPOINTS:" << "  "<< 10 <<vcl_endl;

  for (int i=0;i<10;i++){
    double rnd_x= get_int_random();
    double rnd_y= get_int_random();
    double rnd_z= get_int_random();

    double ix,iy;
    pers.world_to_image(rnd_x,rnd_y,rnd_z,ix,iy,0);
    output << "CORRESP: 1 "<<i <<" "<< rnd_x <<" "<< rnd_y << " " << rnd_z
           << " " << (float) ix << " " << (float) iy << vcl_endl;
  }

  output.close();
  vcl_cout << "test program for vpgl libraries\n";

////////////////////////////////////////////////////
#if 0
  vcl_ifstream instr(path);
  if (!instr)
  {
    vcl_cout <<"In carmen_interface::load_correspondence_file()"
             <<" - can't open file  " <<vcl_endl;
    return false;
  }
  char buf[100];
  int npts;
  instr >> buf ;

  if (!vcl_strcmp("NUMPOINTS:",buf))
    instr >> npts;
  else
  {
    vcl_cout << "bad file " << vcl_endl;
    return false;
  }
  int view_no=0, point_id=0;
  double x=0, y=0, z=0;
  float u=0, v=0;
  for (int i=0; i<npts&&instr; i++)
  {
    instr >> buf;

    if (vcl_strcmp("CORRESP:",buf))
    {
      vcl_cout << "wrong number of correspondences " << vcl_endl;
      return false;
    }
    instr >> view_no >> point_id >> x >> y >> z >> u >> v;
    vcl_cout << "Corr: " <<  view_no << " " << point_id << " " << x << " " <<  y
             << " " << z << " " <<  u << " " << v << vcl_endl;
  }
#endif
  return 0;
}
