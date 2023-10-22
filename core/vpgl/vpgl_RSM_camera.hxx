// This is core/vpgl/vpgl_RSM_camera.hxx
#ifndef vpgl_RSM_camera_hxx_
#define vpgl_RSM_camera_hxx_
//:
// \file

#include <vector>
#include <fstream>
#include <iomanip>
#include "vpgl_RSM_camera.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
//#include <vnl/io/vnl_io_matrix_fixed.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>


//--------------------------------------
// Constructors

// Create an identity projection, i.e. (x,y) identically maps to (u,v)
template <class T>
vpgl_RSM_camera<T>::vpgl_RSM_camera()
{
  vpgl_scale_offset<T> soff;
  scale_offsets_.resize(5, soff);
}

//--------------------------------------
// Clone

// clone RSM camera
template <class T>
vpgl_RSM_camera<T> *vpgl_RSM_camera<T>::clone() const {
  return new vpgl_RSM_camera<T>(*this);
}

//--------------------------------------
// Set coefficient matrix

// set coefficients from 4 vectors
template <class T>
void vpgl_RSM_camera<T>::set_coefficients(
    std::vector<T> const& neu_u,
    std::vector<T> const& den_u,
    std::vector<T> const& neu_v,
    std::vector<T> const& den_v
    )
{
  coeffs_.clear();
  coeffs_.push_back(neu_u);
  coeffs_.push_back(den_u);
  coeffs_.push_back(neu_v);
  coeffs_.push_back(den_v);
}

// set coefficients from array encoding
template <class T>
void vpgl_RSM_camera<T>::set_coefficients(
    std::vector<std::vector<T> > const& coeffs
    )
{
  coeffs_ = coeffs;
}

// get coefficients as std vector of vectors
template <class T>
std::vector<std::vector<T> >
vpgl_RSM_camera<T>::coefficients() const
{
  return coeffs_;
  
}
template <class T>
void vpgl_RSM_camera<T>::set_powers(std::vector<int> const& neu_u_powers,
                                    std::vector<int> const& den_u_powers,
                                    std::vector<int> const& neu_v_powers,
                                    std::vector<int> const& den_v_powers)
{
  powers_.clear();
  powers_.push_back(neu_u_powers);
  powers_.push_back(den_u_powers);
  powers_.push_back(neu_v_powers);
  powers_.push_back(den_v_powers);
}

//--------------------------------------
// Set scale/offset values

// set all scale offsets from individual values
template <class T>
void vpgl_RSM_camera<T>::set_scale_offsets(
    const T x_scale, const T x_off,
    const T y_scale, const T y_off,
    const T z_scale, const T z_off,
    const T u_scale, const T u_off,
    const T v_scale, const T v_off
    )
{
  scale_offsets_.resize(5);
  scale_offsets_[X_INDX] = vpgl_scale_offset<T>(x_scale, x_off);
  scale_offsets_[Y_INDX] = vpgl_scale_offset<T>(y_scale, y_off);
  scale_offsets_[Z_INDX] = vpgl_scale_offset<T>(z_scale, z_off);
  scale_offsets_[U_INDX] = vpgl_scale_offset<T>(u_scale, u_off);
  scale_offsets_[V_INDX] = vpgl_scale_offset<T>(v_scale, v_off);
}

// set all scale offsets from vector
template <class T>
void vpgl_RSM_camera<T>::set_scale_offsets(
    std::vector<vpgl_scale_offset<T> > const& scale_offsets
    )
{
  scale_offsets_ = scale_offsets;
}

//--------------------------------------
// Project 3D world point into 2D image space

// generic interface
template <class T>
void vpgl_RSM_camera<T>::project(
    const T x, const T y, const T z,
    T& u, T& v) const
{
  // scale, offset the world point before projection
  T sx = scale_offsets_[X_INDX].normalize(x);
  T sy = scale_offsets_[Y_INDX].normalize(y);
  T sz = scale_offsets_[Z_INDX].normalize(z);
  
  // projection
  // ==== u neumerator ====
  double num_u = 0.0;
  int c = 0;
  for(int k = 0; k<=powers_[P_NEU_U][Z_INDX]; ++k)
    for(int j = 0; j<=powers_[P_NEU_U][Y_INDX]; ++j)
      for(int i = 0; i<=powers_[P_NEU_U][X_INDX]; ++i)
          num_u += coeffs_[P_NEU_U][c++] * pow(sx, i) * pow(sy, j) * pow(sz, k);

  double den_u = 0.0;
  c = 0;
  // ==== u denominator ====
  for(int k = 0; k<=powers_[P_DEN_U][Z_INDX]; ++k)
    for(int j = 0; j<=powers_[P_DEN_U][Y_INDX]; ++j)
      for(int i = 0; i<=powers_[P_DEN_U][X_INDX]; ++i)
          den_u += coeffs_[P_DEN_U][c++] * pow(sx, i) * pow(sy, j) * pow(sz, k);

  // ==== v neumerator ====
  double num_v = 0.0;
  c = 0;
 for(int k = 0; k<= powers_[P_NEU_V][Z_INDX]; ++k)
    for(int j = 0; j<= powers_[P_NEU_V][Y_INDX]; ++j)
        for (int i = 0; i <= powers_[P_NEU_V][X_INDX]; ++i) 
            num_v += coeffs_[P_NEU_V][c++] * pow(sx, i) * pow(sy, j) * pow(sz, k);

  // ==== v denominator ====
  double den_v = 0.0;
  c = 0;
  for(int k = 0; k<=powers_[P_DEN_V][Z_INDX]; ++k)
    for(int j = 0; j<=powers_[P_DEN_V][Y_INDX]; ++j)
      for(int i = 0; i<=powers_[P_DEN_V][X_INDX]; ++i) 
          den_v += coeffs_[P_DEN_V][c++] * pow(sx, i) * pow(sy, j) * pow(sz, k);

  // ratios
  double su = num_u/den_u;
  double sv = num_v/den_v;
  // unscale the resulting image coordinates
  u = scale_offsets_[U_INDX].un_normalize(su);
  v = scale_offsets_[V_INDX].un_normalize(sv);
}

// vnl interface
template <class T>
vnl_vector_fixed<T, 2>
vpgl_RSM_camera<T>::project(vnl_vector_fixed<T, 3> const& world_point) const
{
  vnl_vector_fixed<T, 2> image_point;
  this->project(world_point[0], world_point[1], world_point[2],
                         image_point[0], image_point[1]);
  return image_point;
}

// vgl interface
template <class T>
vgl_point_2d<T> vpgl_RSM_camera<T>::project(vgl_point_3d<T> world_point) const
{
  T u = 0, v = 0;
  this->project(world_point.x(), world_point.y(), world_point.z(), u, v);
  return vgl_point_2d<T>(u, v);
}


//--------------------------------------
// Output

// write camera parameters to output stream as PVL (paramter value language)
template <class T>
void vpgl_RSM_camera<T>::write_pvl(
    std::ostream& ostr
    ) const
{
  /*
  // current ostream settings (restore at end of function)
  auto ostr_flags = ostr.flags();
  auto ostr_precision = ostr.precision();

  // print header & scale/offset values
  ostr << "satId = \"????\";\n"
       << "bandId = \"RGB\";\n"
       << "SpecId = \"" << vpgl_RSM_order_func::to_string(output_order)
          << "\";" << std::endl;

  // begin image group
  ostr << "BEGIN_GROUP = IMAGE" << std::endl;

  // print scale/offset values
  ostr << std::fixed;
  ostr.precision(6);

  ostr << std::endl // skip errBias
       << std::endl // skip errRand
       << "\tlineOffset = "   << offset(V_INDX) << std::endl
       << "\tsampOffset = "   << offset(U_INDX) << std::endl
       << "\tlatOffset = "    << offset(Y_INDX) << std::endl
       << "\tlongOffset = "   << offset(X_INDX) << std::endl
       << "\theightOffset = " << offset(Z_INDX) << std::endl
       << "\tlineScale = "    <<  scale(V_INDX) << std::endl
       << "\tsampScale = "    <<  scale(U_INDX) << std::endl
       << "\tlatScale = "     <<  scale(Y_INDX) << std::endl
       << "\tlongScale = "    <<  scale(X_INDX) << std::endl
       << "\theightScale = "  <<  scale(Z_INDX) << std::endl
       ;

  // print coefficients in specified order
  auto coeffs = this->coefficient_matrix(output_order);

  std::vector<std::pair<std::string, poly_index> > items;
  items.push_back(std::make_pair("lineNumCoef",NEU_V));
  items.push_back(std::make_pair("lineDenCoef",DEN_V));
  items.push_back(std::make_pair("sampNumCoef",NEU_U));
  items.push_back(std::make_pair("sampDenCoef",DEN_U));

  ostr << std::scientific << std::showpos;
  ostr.precision(12);

  for (auto const& item : items) {
    ostr << "\t" << item.first << " = (" << std::endl;
    for (int i=0; i<20; i++) {
      ostr << "\t\t" << coeffs[item.second][i];
      if (i < 19)
        ostr << "," << std::endl;
      else
        ostr << ");" << std::endl;
    }
  }

  // end image group
  ostr << "END_GROUP = IMAGE" << std::endl;

  // footer
  ostr << "END;" << std::endl;

  // restore ostream settings
  ostr.flags(ostr_flags);
  ostr.precision(ostr_precision);
  */
}

// print camera parameters to output stream
template <class T>
void vpgl_RSM_camera<T>::print(
    std::ostream& ostr = std::cout
    ) const
{
  this->write_pvl(ostr);
}

// save camera parameters to file
template <class T>
bool vpgl_RSM_camera<T>::save(
    std::string cam_path
    ) const
{
  // open file
  std::ofstream file_out;
  file_out.open(cam_path.c_str());
  if (!file_out.good()) {
    std::cerr << "error: bad filename: " << cam_path << std::endl;
    return false;
  }

  // print to file
  this->write_pvl(file_out);

  // cleanup
  file_out.close();
  return true;
}


//--------------------------------------
// Input

// read from PVL (parameter value language) file
template <class T>
bool vpgl_RSM_camera<T>::read_pvl(std::string cam_path)
{
  // open file
  std::ifstream file_inp;
  file_inp.open(cam_path.c_str());
  if (!file_inp.good()) {
    std::cout << "error: bad filename: " << cam_path << std::endl;
    return false;
  }

  // read from file stream
  bool success = read_pvl(file_inp);

  // cleanup
  file_inp.close();
  return success;
}

// read from PVL input stream
template <class T>
bool vpgl_RSM_camera<T>::read_pvl(std::istream& istr)
{
  /*
  std::vector<T> neu_u;
  std::vector<T> den_u;
  std::vector<T> neu_v;
  std::vector<T> den_v;
  T x_scale,x_off,y_scale,y_off,z_scale,z_off,u_scale,u_off,v_scale,v_off;

  bool has_xs = false, has_xo = false, has_ys = false, has_yo = false,
       has_zs = false, has_zo = false;
  bool has_us = false, has_uo = false, has_vs = false, has_vo = false;

  // assume RPC00B ordering as default
  auto input_order = vpgl_RSM_order::RPC00B;

  std::string input;
  char bulk[100];
  T temp_dbl;

  while (!istr.eof()) {
    istr >> input;

    if (input=="SpecId") {
      istr >> input;
      istr >> input;
      try {
        input_order = vpgl_RSM_order_func::from_string(input);
      } catch (const std::exception& err) {
        std::cerr << "<" << input << "> unrecognized as vpgl_RSM_order" << std::endl;
        std::cerr << err.what() << std::endl;
        return false;
      }
    }

    else if (input=="sampScale") {
      istr >> input;
      istr >> u_scale;
      has_us = true;
    }
    else if (input=="sampOffset") {
      istr >> input;
      istr >> u_off;
      has_uo = true;
    }

    else if (input=="lineScale") {
      istr >> input;
      istr >> v_scale;
      has_vs = true;
    }
    else if (input=="lineOffset") {
      istr >> input;
      istr >> v_off;
      has_vo = true;
    }

    else if (input=="longScale") {
      istr >> input;
      istr >> x_scale;
      has_xs = true;
    }
    else if (input=="longOffset") {
      istr >> input;
      istr >> x_off;
      has_xo = true;
    }

    else if (input=="latScale") {
      istr >> input;
      istr >> y_scale;
      has_ys = true;
    }
    else if (input=="latOffset") {
      istr >> input;
      istr >> y_off;
      has_yo = true;
    }

    else if (input=="heightScale") {
      istr >> input;
      istr >> z_scale;
      has_zs = true;
    }
    else if (input=="heightOffset") {
      istr >> input;
      istr >> z_off;
      has_zo = true;
    }

    else if (input=="lineNumCoef") {
      istr >> input;
      istr >> input;
      for (int i=0; i<20; i++) {
        istr >> temp_dbl;
        neu_v.push_back(temp_dbl);
        istr.getline(bulk,200);
      }
    }

    else if (input=="lineDenCoef") {
      istr >> input;
      istr >> input;
      for (int i=0; i<20; i++) {
        istr >> temp_dbl;
        den_v.push_back(temp_dbl);
        istr.getline(bulk,200);
      }
    }

    else if (input=="sampNumCoef") {
      istr >> input;
      istr >> input;
      for (int i=0; i<20; i++) {
        istr >> temp_dbl;
        neu_u.push_back(temp_dbl);
        istr.getline(bulk,200);
      }
    }

    else if (input=="sampDenCoef") {
      istr >> input;
      istr >> input;
      for (int i=0; i<20; i++) {
        istr >> temp_dbl;
        den_u.push_back(temp_dbl);
        istr.getline(bulk,200);
      }
      break;
    }
  }

  istr >> input;
  if (input!="END_GROUP")
    return false;
  istr >> input;
  if (input!="=")
    return false;
  istr >> input;
  if (input!="IMAGE")
    return false;
  istr >> input;
  if (input!="END;")
    return false;

  if ( (!has_xs) || (!has_xo) || (!has_ys) || (!has_yo) || (!has_zs) || (!has_zo) ||
       (!has_us) || (!has_uo) || (!has_vs) || (!has_vo) )
  {
    std::cerr << "RSM camera missing scale/offset values" << std::endl;
    return false;
  }

  if ((neu_u.size() != 20) || (den_u.size() != 20) ||
      (neu_v.size() != 20) || (den_v.size() != 20))
  {
    std::cerr << "the input is not a valid RSM camera" << std::endl;
    return false;
  }

  // set values & cleanup
  this->set_coefficients(neu_u, den_u, neu_v, den_v, input_order);
  this->set_scale_offsets(x_scale, x_off, y_scale, y_off, z_scale, z_off,
                          u_scale, u_off, v_scale, v_off);
  */
  return true;
}


// read from TXT file
template <class T>
bool vpgl_RSM_camera<T>::read_txt(std::string cam_path)
{
  // open file
  std::ifstream file_inp;
  file_inp.open(cam_path.c_str());
  if (!file_inp.good()) {
    std::cout << "error: bad filename: " << cam_path << std::endl;
    return false;
  }

  // read from file stream
  bool success = read_txt(file_inp);

  // cleanup
  file_inp.close();
  return success;
}

// read from TXT input stream
template <class T>
bool vpgl_RSM_camera<T>::read_txt(std::istream& istr)
{
  /*
  std::vector<T> neu_u;
  std::vector<T> den_u;
  std::vector<T> neu_v;
  std::vector<T> den_v;
  T x_scale,x_off,y_scale,y_off,z_scale,z_off,u_scale,u_off,v_scale,v_off;

  bool has_xs = false, has_xo = false, has_ys = false, has_yo = false,
       has_zs = false, has_zo = false;
  bool has_us = false, has_uo = false, has_vs = false, has_vo = false;

  // assume RPC00B ordering as default
  auto input_order = vpgl_RSM_order::RPC00B;

  std::string input;
  char bulk[100];
  T temp_dbl;

  while (!istr.eof()) {
    istr >> input;

    if (input=="SAMP_SCALE:") {
      //istr >> input;
      istr >> u_scale;
      has_us = true;
    }
    else if (input=="SAMP_OFF:") {
      //istr >> input;
      istr >> u_off;
      has_uo = true;
    }

    else if (input=="LINE_SCALE:") {
      //istr >> input;
      istr >> v_scale;
      has_vs = true;
    }
    else if (input=="LINE_OFF:") {
      //istr >> input;
      istr >> v_off;
      has_vo = true;
    }

    else if (input=="LONG_SCALE:") {
      //istr >> input;
      istr >> x_scale;
      has_xs = true;
    }
    else if (input=="LONG_OFF:") {
      //istr >> input;
      istr >> x_off;
      has_xo = true;
    }

    else if (input=="LAT_SCALE:") {
      //istr >> input;
      istr >> y_scale;
      has_ys = true;
    }
    else if (input=="LAT_OFF:") {
      //istr >> input;
      istr >> y_off;
      has_yo = true;
    }

    else if (input=="HEIGHT_SCALE:") {
      //istr >> input;
      istr >> z_scale;
      has_zs = true;
    }
    else if (input=="HEIGHT_OFF:") {
      //istr >> input;
      istr >> z_off;
      has_zo = true;
    }

    else if (input=="LINE_NUM_COEFF_1:") {
      //istr >> input;
      //istr >> input;
      istr >> temp_dbl;
      neu_v.push_back(temp_dbl);
      for (int i=1; i<20; i++) {
        istr >> input;
        istr >> temp_dbl;
        neu_v.push_back(temp_dbl);
        istr.getline(bulk,200);
      }
    }

    else if (input=="LINE_DEN_COEFF_1:") {
      //istr >> input;
      //istr >> input;
      istr >> temp_dbl;
      den_v.push_back(temp_dbl);
      for (int i=1; i<20; i++) {
        istr >> input;
        istr >> temp_dbl;
        den_v.push_back(temp_dbl);
        istr.getline(bulk,200);
      }
    }

    else if (input=="SAMP_NUM_COEFF_1:") {
      //istr >> input;
      //istr >> input;
      istr >> temp_dbl;
      neu_u.push_back(temp_dbl);
      for (int i=1; i<20; i++) {
        istr >> input;
        istr >> temp_dbl;
        neu_u.push_back(temp_dbl);
        istr.getline(bulk,200);
      }
    }

    else if (input=="SAMP_DEN_COEFF_1:") {
      //istr >> input;
      //istr >> input;
      istr >> temp_dbl;
      den_u.push_back(temp_dbl);
      for (int i=1; i<20; i++) {
        istr >> input;
        istr >> temp_dbl;
        den_u.push_back(temp_dbl);
        istr.getline(bulk,200);
      }
      break;
    }
  }

  if ( (!has_xs) || (!has_xo) || (!has_ys) || (!has_yo) || (!has_zs) || (!has_zo) ||
       (!has_us) || (!has_uo) || (!has_vs) || (!has_vo) )
  {
    std::cerr << "RSM camera missing scale/offset values" << std::endl;
    return false;
  }

  if ((neu_u.size() != 20) || (den_u.size() != 20) ||
      (neu_v.size() != 20) || (den_v.size() != 20))
  {
    std::cerr << "the input is not a valid RSM camera" << std::endl;
    return false;
  }

  // set values & cleanup
  this->set_coefficients(neu_u, den_u, neu_v, den_v, input_order);
  this->set_scale_offsets(x_scale, x_off, y_scale, y_off, z_scale, z_off,
                          u_scale, u_off, v_scale, v_off);
  */
  return true;
}


//--------------------------------------
// Convenience functions

// write to stream
template <class T>
std::ostream& operator<<(std::ostream& s, const vpgl_RSM_camera<T >& c )
{
  c.print(s);
  return s;
}

// read from stream
template <class T>
std::istream& operator >>(std::istream& s, vpgl_RSM_camera<T >& c )
{
  c.read_pvl(s);
  return s;
}

// read from a PVL file/stream
template <class T>
vpgl_RSM_camera<T>* read_RSM_camera(std::string cam_path)
{
  vpgl_RSM_camera<T> cam;
  if (!cam.read_pvl(cam_path))
    return nullptr;
  else
    return cam.clone();
}

template <class T>
vpgl_RSM_camera<T>* read_RSM_camera(std::istream& istr)
{
  vpgl_RSM_camera<T> cam;
  if (!cam.read_pvl(istr))
    return nullptr;
  else
    return cam.clone();
}

// read from a TXT file/stream
template <class T>
vpgl_RSM_camera<T>* read_RSM_camera_from_txt(std::string cam_path)
{
  vpgl_RSM_camera<T> cam;
  if (!cam.read_txt(cam_path))
    return nullptr;
  else
    return cam.clone();
}

template <class T>
vpgl_RSM_camera<T>* read_RSM_camera_from_txt(std::istream& istr)
{
  vpgl_RSM_camera<T> cam;
  if (!cam.read_txt(istr))
    return nullptr;
  else
    return cam.clone();
}


// Code for easy instantiation.
#undef vpgl_RSM_CAMERA_INSTANTIATE
#define vpgl_RSM_CAMERA_INSTANTIATE(T) \
template class vpgl_scale_offset<T >; \
template class vpgl_RSM_camera<T >; \
template std::ostream& operator<<(std::ostream&, const vpgl_RSM_camera<T >&); \
template std::istream& operator>>(std::istream&, vpgl_RSM_camera<T >&); \
template vpgl_RSM_camera<T > * read_RSM_camera(std::string); \
template vpgl_RSM_camera<T > * read_RSM_camera(std::istream&); \
template vpgl_RSM_camera<T > * read_RSM_camera_from_txt(std::string); \
template vpgl_RSM_camera<T > * read_RSM_camera_from_txt(std::istream&)


#endif // vpgl_RSM_camera_hxx_
