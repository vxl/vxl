#ifndef mbl_vector_txt_files_h_
#define mbl_vector_txt_files_h_
//:
// \file
// \brief Functions to load/save lists of fixed points to text files (nb copy of par/par_fixed_pts.h)
// \author dac

#include <vcl_string.h>
#include <vcl_vector.h>

#include <vnl/vnl_vector.h>

//nb these functions are copies of par/par_fixed_pts.h just renamed a bit

//: Load boolean list indicating which points are fixed from a text file
// This stores true pts as a list
bool mbl_load_bool_vec_vcl_list(const vcl_string& path, vcl_vector<bool>& fixed_pts);

//: Save boolean list indicating which points are fixed to a text file
// This stores true pts as a list
bool mbl_save_bool_vec_vcl_list(const vcl_string& path, const vcl_vector<bool>& fixed_pts);

//: Load vector from file with format  { n v1 v2 .. vn }
bool mbl_load_double_vec_vnl(vnl_vector<double>& v, const vcl_string& path);

//: Save vector to file with format  { n v1 v2 .. vn }
bool mbl_save_double_vec_vnl(const vnl_vector<double>& v, const vcl_string& path);

// --- these functions are new to save vcl_vectors easily! ---

//: Load vector from file with format  { n v1 v2 .. vn }
void mbl_load_double_vec_vcl(vcl_vector<double>& v, const vcl_string& path);

//: Save vector to file with format  { n v1 v2 .. vn }
void mbl_save_double_vec_vcl(const vcl_vector<double>& v, const vcl_string& path);


//: Load vector from file with format  { n v1 v2 .. vn }
void mbl_load_float_vec_vcl(vcl_vector<float>& v, const vcl_string& path);

//: Save vector to file with format  { n v1 v2 .. vn }
void mbl_save_float_vec_vcl(const vcl_vector<float>& v, const vcl_string& path);


//: Load vector from file with format  { n v1 v2 .. vn }
void mbl_load_int_vec_vcl(vcl_vector<int>& v, const vcl_string& path);

//: Save vector to file with format  { n v1 v2 .. vn }
void mbl_save_int_vec_vcl(const vcl_vector<int>& v, const vcl_string& path);


//: Load vector from file with format  { n v1 v2 .. vn }
void mbl_load_bool_vec_vcl(vcl_vector<bool>& v, const vcl_string& path);

//: Save vector to file with format  { n v1 v2 .. vn }
void mbl_save_bool_vec_vcl(const vcl_vector<bool>& v, const vcl_string& path);


//: convert integer vector to a string
// useful when saving to a text file
// format  { n v1 v2 .. vn }
void mbl_intvec2str(vcl_string& str, const vcl_vector<int>& v );

//: convert string to a integer vector
// useful when loading from a text file
// format  { n v1 v2 .. vn }
void mbl_str2intvec(vcl_vector<int>& v, const vcl_string& str);

//: convert string vector to a string
// useful when saving to a text file
// format  { n v1 v2 .. vn }
void mbl_strvec2str(vcl_string& str, const vcl_vector<vcl_string>& v );

//: convert string to a integer vector
// useful when loading from a text file
// format  { n v1 v2 .. vn }
void mbl_str2strvec(vcl_vector<vcl_string>& v, const vcl_string& str);

#endif // mbl_vector_txt_files_h_
