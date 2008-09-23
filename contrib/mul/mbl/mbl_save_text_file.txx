#ifndef mbl_save_text_file_txx_
#define mbl_save_text_file_txx_
//:
// \file
// \brief Functions to save objects to text file
// \author dac

#include <mbl/mbl_save_text_file.h>
#include <mbl/mbl_exception.h>

#include <vcl_fstream.h>


//: Save vector to file with format  { n v1 v2 .. vn }
template <class T>
bool mbl_save_text_file(const vcl_vector<T >& v, const vcl_string& path)
{
  vcl_ofstream ofs(path.c_str());
  if (!ofs)
  {
    mbl_exception_throw_os_error( path, "mbl_save_text_file: failed to save" );
    return false;
  }

  //ofs<<v.size()<<'\n';
  for (unsigned i=0;i<v.size();++i)
  {
    ofs<<v[i]<<'\n';
  }
  ofs.close();
  return true;
}


#undef MBL_SAVE_TEXT_FILE_INSTANTIATE
#define MBL_SAVE_TEXT_FILE_INSTANTIATE(T ) \
template bool mbl_save_text_file(const vcl_vector<T >& v, const vcl_string& path)

#endif // mbl_save_text_file_txx_
