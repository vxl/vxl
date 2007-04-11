//:
// \file
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_fstream.h>
#include <brct/brct_algos.h>
#include <vgl/vgl_sphere_3d.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>

//: projections from S3 to xyz
static double px(const double a, const double b, const double c, const double d)
{
  double t = b-0.5;
  double s = a+b+c+d;
  double r = t/(1-s/2);
  r += 1;
  r*= 0.5;
  return r;
}

static double py(const double a, const double b, const double c, const double d)
{
  double t = c-0.5;
  double s = a+b+c+d;
  double r = t/(1-s/2);
  r += 1;
  r*= 0.5;
  return r;
}

static double pz(const double a, const double b, const double c, const double d)
{
  double t = d-0.5;
  double s = a+b+c+d;
  double r = t/(1-s/2);
  r += 1;
  r*= 0.5;
  return r;
}

//:
// define points on S3 using 4-d spherical coordinates with quaterion angle
// followed by projection theta is rotation angle, psi is elevation of rotation
// axis and phi is azimuth of rotation axis.
static double x(const double theta, const double psi, const double phi)
{
  return px(vcl_cos(theta/2),
            vcl_sin(theta/2)*vcl_sin(psi)*vcl_cos(phi),
            vcl_sin(theta/2)*vcl_sin(psi)*vcl_sin(phi),
            vcl_sin(theta/2)*vcl_cos(psi));
}

static double y(const double theta, const double psi, const double phi)
{
  return py(vcl_cos(theta/2),
            vcl_sin(theta/2)*vcl_sin(psi)*vcl_cos(phi),
            vcl_sin(theta/2)*vcl_sin(psi)*vcl_sin(phi),
            vcl_sin(theta/2)*vcl_cos(psi));
}

static double z(const double theta, const double psi, const double phi)
{
  return pz(vcl_cos(theta/2),
            vcl_sin(theta/2)*vcl_sin(psi)*vcl_cos(phi),
            vcl_sin(theta/2)*vcl_sin(psi)*vcl_sin(phi),
            vcl_sin(theta/2)*vcl_cos(psi));
}

#if 0 // static function unused
static void generate_points(vcl_vector<vgl_point_3d<double> >& pts,
                            vcl_vector<vgl_point_3d<float> >& color,
                            const double theta_min,
                            const double theta_max,
                            const double psi_min,
                            const double psi_max,
                            const double phi_min,
                            const double phi_max,
                            const double theta_inc,
                            const double psi_inc,
                            const double phi_inc)
{
  pts.clear();
  color.clear();
  for (double theta = theta_min; theta<=theta_max; theta+=theta_inc)
    for (double psi = psi_min; psi<=psi_max; psi+=psi_inc)
      for (double phi = phi_min; phi<=phi_max; phi+=phi_inc)
      {
        vgl_point_3d<double> p(x(theta, psi, phi),
                               y(theta, psi, phi),
                               z(theta, psi, phi)
                               );
        pts.push_back(p);
        double cr = (psi-psi_min)/(psi_max-psi_min)+0.5;
        double cg = (phi-phi_min)/(phi_max-phi_min)+0.5;
        //          vgl_point_3d<float> c(vcl_fabs(0.5+p.x()),vcl_fabs(0.5+p.y()),
        //                          vcl_fabs(0.5+p.z()));
        vgl_point_3d<float> c((float)cr,(float)cg,0.5f);
        color.push_back(c);
      }
}
#endif // 0

#if 0 // static function unused
static
void generate_polylines(vcl_vector<vcl_vector<vgl_point_3d<double> > >& polys,
                        vcl_vector<vgl_point_3d<float> >& polycolors,
                        const double theta_min,
                        const double theta_max,
                        const double psi_min,
                        const double psi_max,
                        const double phi_min,
                        const double phi_max,
                        const double theta_inc,
                        const double psi_inc,
                        const double phi_inc)
{
  polys.clear();
  polycolors.clear();
  for (double psi = psi_min; psi<=psi_max; psi+=psi_inc)
    for (double phi = phi_min; phi<=phi_max; phi+=phi_inc)
    {
      vcl_vector<vgl_point_3d<double> > temp;
      for (double theta = theta_min; theta<=theta_max; theta+=theta_inc)
      {
        vgl_point_3d<double> p(x(theta, psi, phi),
                               y(theta, psi, phi),
                               z(theta, psi, phi)
                               );
        temp.push_back(p);
      }
      polys.push_back(temp);
      double cr = (psi-psi_min)/(psi_max-psi_min)+0.25;
      double cg = (phi-phi_min)/(phi_max-phi_min)+0.25;
      vgl_point_3d<float> c((float)cr,(float)cg,0.25f);
      polycolors.push_back(c);
    }
}
#endif // 0

#if 0 // static function unused
static void
generate_algebra_polylines(vcl_vector<vcl_vector<vgl_point_3d<double> > >& pys,
                           vcl_vector<vgl_point_3d<float> >& polycolors,
                           const double i_min,
                           const double i_max,
                           const double j_min,
                           const double j_max,
                           const double k_min,
                           const double k_max,
                           const double i_inc,
                           const double j_inc,
                           const double k_inc)
{
  pys.clear();
  polycolors.clear();
  for (double i = i_min; i<=i_max; i+=i_inc)
    for (double j = j_min; j<=j_max; j+=j_inc)
      for (double k = k_min; k<=k_max; k+=k_inc)
      {
        vcl_vector<vgl_point_3d<double> > vts;
        for (double t = 0.0; t<=1.0; t+=0.05)
        {
          if (vcl_fabs(vcl_fabs(t*i + t*j + t*k)-1)<0.01)
            continue;
          vgl_point_3d<double> p(px(1,-t*i,-t*j,-t*k),
                                 py(1,-t*i,-t*j,-t*k),
                                 pz(1,-t*i,-t*j,-t*k));
          vts.push_back(p);
        }
        vgl_point_3d<float> c((float)(0.25+i), (float)(0.25+j),
                              (float)(0.25+k));
        polycolors.push_back(c);
        pys.push_back(vts);
      }
}
#endif // 0

//generate surfaces of constant theta in the group
static
void generate_d_polylines(vcl_vector<vcl_vector<vgl_point_3d<double> > >& polys,
                          vcl_vector<vgl_point_3d<float> >& polycolors,
                          const double theta_min,
                          const double theta_max,
                          const double psi_min,
                          const double psi_max,
                          const double phi_min,
                          const double phi_max,
                          const double theta_inc,
                          const double psi_inc,
                          const double phi_inc)
{
  polys.clear();
  polycolors.clear();
  for (double theta =theta_min; theta<=theta_max; theta += theta_inc)
  {
    vcl_cout << "d group theta = " << theta << '\n';
    for (double psi = psi_min; psi<=psi_max; psi+=psi_inc)
    {
      vcl_vector<vgl_point_3d<double> > temp;
      for (double phi = phi_min; phi<=phi_max; phi+=phi_inc)
      {
        vgl_point_3d<double> p(x(theta, psi, phi),
                               y(theta, psi, phi),
                               z(theta, psi, phi)
                              );
        temp.push_back(p);
      }
      polys.push_back(temp);
      double cr = (theta-theta_min)/(theta_max-theta_min);
      double cg = 1-cr;
      vgl_point_3d<float> c((float)cr,(float)cg,0.25f);
      polycolors.push_back(c);
    }
  }
}

//generate surfaces of constant theta in the algebra
static
void generate_da_polylines(vcl_vector<vcl_vector<vgl_point_3d<double> > >& polys,
                          vcl_vector<vgl_point_3d<float> >& polycolors,
                          const double theta_min,
                          const double theta_max,
                          const double psi_min,
                          const double psi_max,
                          const double phi_min,
                          const double phi_max,
                          const double theta_inc,
                          const double psi_inc,
                          const double phi_inc)
{
  polys.clear();
  polycolors.clear();
  for (double theta =theta_min; theta<=theta_max; theta += theta_inc)
  {
    vcl_cout << "da algebra theta = " << theta << '\n';
    for (double psi = psi_min; psi<=psi_max; psi+=psi_inc)
    {
      vcl_vector<vgl_point_3d<double> > temp;
      for (double phi = phi_min; phi<=phi_max; phi+=phi_inc)
      {
        double i = vcl_sin(psi)*vcl_cos(phi);
        double j = vcl_sin(psi)*vcl_sin(phi);
        double k = vcl_cos(psi);
        double b = 0.5*i*theta, c = 0.5*j*theta, d = 0.5*k*theta;
        vgl_point_3d<double> p(px(1, b, c, d),
                               py(1, b, c, d),
                               pz(1, b, c, d)
                              );
        temp.push_back(p);
      }
      polys.push_back(temp);
      //        double cr = (theta-theta_min)/(theta_max-theta_min);
      //double cg = 1-cr;
      //        vgl_point_3d<float> c((float)cr,(float)cg,0.25f);
      vgl_point_3d<float> c(1.0f,0.0f,1.0f);
      polycolors.push_back(c);
    }
  }
}

#if 0 // static function unused
static void write_points(vcl_ofstream& str,
                         vcl_vector<vgl_point_3d<double> > const& points,
                         vcl_vector<vgl_point_3d<float> > const& color)
{
  brct_algos::write_vrml_header(str);
  brct_algos::write_vrml_points(str, points, color);
  double r = 0.01;
  // identity white
  vgl_sphere_3d<double> sid(0, 0, 0, r);
  brct_algos::write_vrml_sphere(str, sid, 1,1,1,0);

  // i red
  vgl_sphere_3d<double> si(1, 0, 0, r);
  brct_algos::write_vrml_sphere(str, si, 1,0,0,0);

  // j green
  vgl_sphere_3d<double> sj(0, 1, 0, r);
  brct_algos::write_vrml_sphere(str, sj, 0,1,0,0);

  // k blue
  vgl_sphere_3d<double> sk(0, 0, 1, r);
  brct_algos::write_vrml_sphere(str, sk, 0,0,1,0);

  // anti-pode purple
  vgl_sphere_3d<double> santi(0.33,0.33,0.33,r);
  brct_algos::write_vrml_sphere(str, santi, 1,0,1,0);

  //brct_algos::write_vrml_trailer(str);
}
#endif // 0

static void write_spheres(vcl_ofstream& str)
{
  double r = 0.02;
  // identity white
  vgl_sphere_3d<double> sid(0, 0, 0, r);
  brct_algos::write_vrml_sphere(str, sid, 1,1,1,0);

  // i red
  vgl_sphere_3d<double> si(1, 0, 0, r);
  brct_algos::write_vrml_sphere(str, si, 1,0,0,0);

  // j green
  vgl_sphere_3d<double> sj(0, 1, 0, r);
  brct_algos::write_vrml_sphere(str, sj, 0,1,0,0);

  // k blue
  vgl_sphere_3d<double> sk(0, 0, 1, r);
  brct_algos::write_vrml_sphere(str, sk, 0,0,1,0);

  // anti-pode purple
  vgl_sphere_3d<double> santi(0.33,0.33,0.33,r);
  brct_algos::write_vrml_sphere(str, santi, 1,0,1,0);
}


static void
write_polylines(vcl_ofstream& str,
                vcl_vector<vcl_vector<vgl_point_3d<double> > > const& polys,
                vcl_vector<vgl_point_3d<float> > const& polycolors)
{
  brct_algos::write_vrml_header(str);
  unsigned n = polys.size();
  for (unsigned i = 0; i<n; ++i)
  {
    vcl_vector<vgl_point_3d<double> > poly = polys[i];
    vgl_point_3d<float> c = polycolors[i];
    brct_algos::write_vrml_polyline(str, poly, c.x(),c.y(),c.z());
  }
}

int main(int argc,char * argv[])
{
  if (argc<8)
  {
    vcl_cout<<"Usage : generate_S3.exe file theta_min theta_max\n"
            << "psi_min psi_max phi_min phi_max theta_inc psi_inc phi_inc\n";
    return -1;
  }
  vcl_string point_file(argv[1]);
  vcl_ofstream point_ostr(point_file.c_str());
  double theta_min = static_cast<double>(atof(argv[2]));
  double theta_max = static_cast<double>(atof(argv[3]));
  double psi_min = static_cast<double>(atof(argv[4]));
  double psi_max = static_cast<double>(atof(argv[5]));
  double phi_min = static_cast<double>(atof(argv[6]));
  double phi_max = static_cast<double>(atof(argv[7]));
  double theta_inc = 0.05;
  double psi_inc = 0.05;
  double phi_inc = 0.05;
  if (argc >8 )
  {
    theta_inc = static_cast<double>(atof(argv[8]));
    psi_inc = static_cast<double>(atof(argv[9]));
    phi_inc = static_cast<double>(atof(argv[10]));
  }
  vcl_vector<vcl_vector<vgl_point_3d<double> > > polylines;
  vcl_vector<vgl_point_3d<float> > polycolors;
  generate_d_polylines(polylines, polycolors, theta_min, theta_max,
                       psi_min, psi_max, phi_min, phi_max,
                       theta_inc, psi_inc, phi_inc);
  write_polylines(point_ostr, polylines, polycolors);
  generate_da_polylines(polylines, polycolors, theta_min, theta_max,
                       psi_min, psi_max, phi_min, phi_max,
                       theta_inc, psi_inc, phi_inc);
  write_polylines(point_ostr, polylines, polycolors);
  write_spheres(point_ostr);

  return 0;
}
