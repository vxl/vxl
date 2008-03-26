//:
// \file
// \brief Functions to load/save lists of fixed points to text files (nb copy of par/par_fixed_pts.h)
// \author dac

#include <mbl/mbl_vector_txt_files.h>

#include <vcl_fstream.h>
#include <vcl_cstdlib.h>


//: Load boolean list indicating which points are fixed from a text file
bool mbl_load_bool_vec_vcl_list(const vcl_string& path,
                             vcl_vector<bool>& fixed_pts)
{
  vcl_ifstream ifs(path.c_str());
  if (!ifs) return false;
  unsigned np,nv,index;

  vcl_string label;
  ifs>>label;
  if (label != "n_points:")
  {
    vcl_cout<<path<<" Expected 'n_points:'. Got "<<label<<vcl_endl;
    return false;
  }
  ifs>>np;
  ifs>>label;
  if (label != "n_fixed:")
  {
    vcl_cout<<path<<" Expected 'n_fixed:'. Got "<<label<<vcl_endl;
    return false;
  }
  ifs>>nv;

  fixed_pts.resize(np,false);
  for (unsigned i=0;i<nv;++i)
  {
    ifs>>index;
    if (index>=np)
    {
      vcl_cout<<path<<" index "<<index<<" is out of range."<<vcl_endl;
      return false;
    }
    fixed_pts[index]=true;
  }

  return true;
}

//: Save boolean list indicating which points are fixed to a text file
bool mbl_save_bool_vec_vcl_list(const vcl_string& path,
                             const vcl_vector<bool>& fixed_pts)
{
  unsigned np = fixed_pts.size();
  unsigned nv = 0;
  for (unsigned i=0;i<np;++i) if (fixed_pts[i]) nv++;
  vcl_ofstream ofs(path.c_str());
  if (!ofs) return false;
  ofs<<"n_points: "<<np<<vcl_endl;
  ofs<<"n_fixed: "<<nv<<vcl_endl;
  for (unsigned i=0;i<np;++i)
    if (fixed_pts[i]) ofs<<i<<vcl_endl;
  ofs.close();
  return true;
}


//: Load vector from file with format  { n v1 v2 .. vn }
bool mbl_load_double_vec_vnl(vnl_vector<double>& v, const vcl_string& path)
{
  vcl_ifstream ifs(path.c_str());
  if (!ifs) return false;
  unsigned n;
  ifs>>n;
  if (n>9999) return false;  // Unlikely to be sensible
  v.set_size(n);
  for (unsigned i=0;i<n;++i) 
  {
    ifs>>v[i];
  }
  return true;
}

//: Save vector to file with format  { n v1 v2 .. vn }
bool mbl_save_double_vec_vnl(const vnl_vector<double>& v, const vcl_string& path)
{
  vcl_ofstream ofs(path.c_str());
  if (!ofs) return false;

  ofs<<v.size()<<vcl_endl;
  for (unsigned i=0;i<v.size();++i) 
  {
    ofs<<v[i]<<vcl_endl;
  }
  ofs.close();
  return true;
}

//: Load vector from file with format  { n v1 v2 .. vn }
void mbl_load_double_vec_vcl(vcl_vector<double>& v, const vcl_string& path)
{
  vcl_ifstream ifs(path.c_str());
  if (!ifs) 
  {
    vcl_cerr<<"ERROR mbl_load_double_vec_vcl()"<<vcl_endl;
    vcl_cerr<<"failed to load from path: "<<path<<vcl_endl;
    vcl_abort();
  }
  
  unsigned n;
  ifs>>n;
  //if (n>9999) return false;  // Unlikely to be sensible
  v.resize(n);
  for (unsigned i=0;i<n;++i) 
  {
    ifs>>v[i];
  }
  
}

//: Save vector to file with format  { n v1 v2 .. vn }
void mbl_save_double_vec_vcl(const vcl_vector<double>& v, const vcl_string& path)
{
  vcl_ofstream ofs(path.c_str());
  if (!ofs)
  {
    vcl_cerr<<"ERROR mbl_save_double_vec_vcl()"<<vcl_endl;
    vcl_cerr<<"failed to save to path: "<<path<<vcl_endl;
    vcl_abort();
  }

  ofs<<v.size()<<vcl_endl;
  for (unsigned i=0;i<v.size();++i) 
  {
    ofs<<v[i]<<vcl_endl;
  }
  ofs.close();
  
}


//: Load vector from file with format  { n v1 v2 .. vn }
void mbl_load_float_vec_vcl(vcl_vector<float>& v, const vcl_string& path)
{
  vcl_ifstream ifs(path.c_str());
  if (!ifs) 
  {
    vcl_cerr<<"ERROR mbl_load_float_vec_vcl()"<<vcl_endl;
    vcl_cerr<<"failed to load from path: "<<path<<vcl_endl;
    vcl_abort();
  }
  
  unsigned n;
  ifs>>n;
  //if (n>9999) return false;  // Unlikely to be sensible
  v.resize(n);
  for (unsigned i=0;i<n;++i) 
  {
    ifs>>v[i];
  }
  
}

//: Save vector to file with format  { n v1 v2 .. vn }
void mbl_save_float_vec_vcl(const vcl_vector<float>& v, const vcl_string& path)
{
  vcl_ofstream ofs(path.c_str());
  if (!ofs)
  {
    vcl_cerr<<"ERROR mbl_save_float_vec_vcl()"<<vcl_endl;
    vcl_cerr<<"failed to save to path: "<<path<<vcl_endl;
    vcl_abort();
  }

  ofs<<v.size()<<vcl_endl;
  for (unsigned i=0;i<v.size();++i) 
  {
    ofs<<v[i]<<vcl_endl;
  }
  ofs.close();
  
}

//: Load vector from file with format  { n v1 v2 .. vn }
void mbl_load_int_vec_vcl(vcl_vector<int>& v, const vcl_string& path)
{
  vcl_ifstream ifs(path.c_str());
  if (!ifs) 
  {
    vcl_cerr<<"ERROR mbl_load_int_vec_vcl()"<<vcl_endl;
    vcl_cerr<<"failed to load from path: "<<path<<vcl_endl;
    vcl_abort();
  }
  
  unsigned n;
  ifs>>n;
  //if (n>9999) return false;  // Unlikely to be sensible
  v.resize(n);
  for (unsigned i=0;i<n;++i) 
  {
    ifs>>v[i];
  }
  
}

//: Save vector to file with format  { n v1 v2 .. vn }
void mbl_save_int_vec_vcl(const vcl_vector<int>& v, const vcl_string& path)
{
  vcl_ofstream ofs(path.c_str());
  if (!ofs)
  {
    vcl_cerr<<"ERROR mbl_save_int_vec_vcl()"<<vcl_endl;
    vcl_cerr<<"failed to save to path: "<<path<<vcl_endl;
    vcl_abort();
  }

  ofs<<v.size()<<vcl_endl;
  for (unsigned i=0;i<v.size();++i) 
  {
    ofs<<v[i]<<vcl_endl;
  }
  ofs.close();
  
}



//: Load vector from file with format  { n v1 v2 .. vn }
void mbl_load_bool_vec_vcl(vcl_vector<bool>& v, const vcl_string& path)
{
  vcl_ifstream ifs(path.c_str());
  if (!ifs) 
  {
    vcl_cerr<<"ERROR mbl_load_bool_vec_vcl()"<<vcl_endl;
    vcl_cerr<<"failed to load from path: "<<path<<vcl_endl;
    vcl_abort();
  }
  
  unsigned n;
  ifs>>n;
  //if (n>9999) return false;  // Unlikely to be sensible
  v.resize(n);
  for (unsigned i=0;i<n;++i) 
  {
    int x;
    ifs>>x;
    if (x==1) 
    {
      v[i]=true;
    }
    else
    {
      v[i]=false;
    }
  }
  
}

//: Save vector to file with format  { n v1 v2 .. vn }
void mbl_save_bool_vec_vcl(const vcl_vector<bool>& v, const vcl_string& path)
{
  vcl_ofstream ofs(path.c_str());
  if (!ofs)
  {
    vcl_cerr<<"ERROR mbl_save_bool_vec_vcl()"<<vcl_endl;
    vcl_cerr<<"failed to save to path: "<<path<<vcl_endl;
    vcl_abort();
  }

  ofs<<v.size()<<vcl_endl;
  for (unsigned i=0;i<v.size();++i) 
  {
    ofs<<v[i]<<vcl_endl;
  }
  ofs.close();
  
}


//: convert integer vector to a string
// useful when saving to a text file
// format  { n v1 v2 .. vn }
void mbl_intvec2str(vcl_string& str, const vcl_vector<int>& v )
{
  vcl_stringstream stream;
  stream<<"{ "<<v.size()<<" ";
  for (unsigned i=0;i<v.size();++i) 
  {
    stream<<v[i]<<" ";
  }
  stream<<"}";
  
  str= stream.str();
  
  //ofs.close();
}


//: convert string to a integer vector
// useful when loading from a text file
// format  { n v1 v2 .. vn }
void mbl_str2intvec(vcl_vector<int>& v, const vcl_string& str)
{
  vcl_stringstream stream;
  stream<<str;
  
  // get rid of opening bracket
  vcl_string b;
  stream>>b;
  
  unsigned n;
  stream>>n;
  //if (n>9999) return false;  // Unlikely to be sensible
  v.resize(n);
  for (unsigned i=0;i<n;++i) 
  {
    stream>>v[i];
  }
  
  // get rid of final bracket
  stream>>b;
  
  // some checks??
  
}



//: convert string vector to a string
// useful when saving to a text file
// format  { n v1 v2 .. vn }
void mbl_strvec2str(vcl_string& str, const vcl_vector<vcl_string>& v )
{
  vcl_stringstream stream;
  stream<<"{ "<<v.size()<<" ";
  for (unsigned i=0;i<v.size();++i) 
  {
    stream<<v[i]<<" ";
  }
  stream<<"}";
  
  str= stream.str();
  
  //ofs.close();
}


//: convert string to a string vector
// useful when loading from a text file
// format  { n v1 v2 .. vn }
void mbl_str2strvec(vcl_vector<vcl_string>& v, const vcl_string& str)
{
  vcl_stringstream stream;
  stream<<str;
  
  // get rid of opening bracket
  vcl_string b;
  stream>>b;
  
  unsigned n;
  stream>>n;
  //if (n>9999) return false;  // Unlikely to be sensible
  v.resize(n);
  for (unsigned i=0;i<n;++i) 
  {
    stream>>v[i];
  }
  
  // get rid of final bracket
  stream>>b;
  
  // some checks??
  
}


