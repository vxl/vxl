// This is core/vsl/vsl_quick_file.h
#ifndef vsl_quick_file_h_
#define vsl_quick_file_h_
//:
// \file
// \brief Functions for quickly loading and saving binary files.
// \author Ian Scott
//
// All the functions return true if successful.
// The functions will also output a success or failure message to stderr by
// default, although you may substitute any std::ostream, or \c (std::ostream*)0 to avoid the
// message.
//
// For these templated functions to work, the object must have vsl_b_read and
// vsl_b_write functions defined for them

#include <string>
#include <iostream>
#include <cerrno>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>


//: Load something from a file
template <class T>
inline bool vsl_quick_file_load(T &data,
                                const std::string& path,
                                std::ostream* errorStream = &std::cerr)
{
  vsl_b_ifstream bfs(path);
  if ( !(!bfs))
  {
    vsl_b_read(bfs,data);
    if (!(!bfs))
    {
      // Check that we have reached the end of the file.
      char dummy;
      vsl_b_read(bfs,dummy);
      if (bfs.is().eof())
      {
        bfs.close();
        if (errorStream)
          *errorStream << "Successfully loaded: " << path << '\n';
        return true;
      }
    }
  }
  bfs.close();
  if (errorStream)
    *errorStream << "Unable to load: "<< path <<'\n';
  return false;
}


//: Save something to a file
template <class T>
inline bool vsl_quick_file_save(const T &data,
                                const std::string& path,
                                std::ostream* errorStream = &std::cerr)
{
  vsl_b_ofstream bfs(path);
  if (!(!bfs))
  {
    vsl_b_write(bfs,data);
    if (!(!bfs))
    {
      bfs.close();
      if (errorStream)
        *errorStream << "Successfully saved: "<< path <<'\n';
      return true;
    }
  }
  bfs.close();
  if (errorStream)
    *errorStream << "Unable to save: " << path << '\n';
  return false;
}

// Load two objects from a file
template <class T, class S>
inline bool vsl_quick_file_load(T &data1,
                                S &data2,
                                const std::string& path,
                                std::ostream* errorStream = &std::cerr)
{
  vsl_b_ifstream bfs(path);
  int reason = errno;
  if ( !(!bfs))
  {
    vsl_b_read(bfs,data1);
    vsl_b_read(bfs,data2);
    if (!(!bfs))
    {
      // Check that we have reached the end of the file.
      char dummy;
      vsl_b_read(bfs,dummy);
      if (bfs.is().eof())
      {
        bfs.close();
        if (errorStream)
          *errorStream << "Successfully loaded: " << path << '\n';
        return true;
      }
    }
  }
  reason = errno;
  bfs.close();
  if (errorStream)
    *errorStream << "Unable to load: "<< path <<'\n';
  return false;
}

// Save two objects to a file
template <class T, class S>
inline bool vsl_quick_file_save(const T &data1,
                                const S &data2,
                                const std::string& path,
                                std::ostream* errorStream = &std::cerr)
{
  vsl_b_ofstream bfs(path);
  if (!(!bfs))
  {
    vsl_b_write(bfs,data1);
    vsl_b_write(bfs,data2);
    if (!(!bfs))
    {
      bfs.close();
      if (errorStream)
        *errorStream << "Successfully saved: "<< path <<'\n';
      return true;
    }
  }
  bfs.close();
  if (errorStream)
    *errorStream << "Unable to save: " << path << '\n';
  return false;
}

// Load three objects from a file
template <class T, class S, class U>
inline bool vsl_quick_file_load(T &data1,
                                S &data2, U &data3,
                                const std::string& path,
                                std::ostream* errorStream = &std::cerr)
{
  vsl_b_ifstream bfs(path);
  if ( !(!bfs))
  {
    vsl_b_read(bfs,data1);
    vsl_b_read(bfs,data2);
    vsl_b_read(bfs,data3);
    if (!(!bfs))
    {
      // Check that we have reached the end of the file.
      char dummy;
      vsl_b_read(bfs,dummy);
      if (bfs.is().eof())
      {
        bfs.close();
        if (errorStream)
          *errorStream << "Successfully loaded: " << path << '\n';
        return true;
      }
    }
  }
  bfs.close();
  if (errorStream)
    *errorStream << "Unable to load: "<< path <<'\n';
  return false;
}

// Save three objects to a file
template <class T, class S, class U>
inline bool vsl_quick_file_save(const T &data1,
                                const S &data2, const U &data3,
                                const std::string& path,
                                std::ostream* errorStream = &std::cerr)
{
  vsl_b_ofstream bfs(path);
  if (!(!bfs))
  {
    vsl_b_write(bfs,data1);
    vsl_b_write(bfs,data2);
    vsl_b_write(bfs,data3);
    if (!(!bfs))
    {
      bfs.close();
      if (errorStream)
        *errorStream << "Successfully saved: "<< path <<'\n';
      return true;
    }
  }
  bfs.close();
  if (errorStream)
    *errorStream << "Unable to save: " << path << '\n';
  return false;
}

// Load four objects from a file
template <class T, class S, class U, class V>
inline bool vsl_quick_file_load(T &data1,
                                S &data2, U &data3, V &data4,
                                const std::string& path,
                                std::ostream* errorStream = &std::cerr)
{
  vsl_b_ifstream bfs(path);
  if ( !(!bfs))
  {
    vsl_b_read(bfs,data1);
    vsl_b_read(bfs,data2);
    vsl_b_read(bfs,data3);
    vsl_b_read(bfs,data4);
    if (!(!bfs))
    {
      // Check that we have reached the end of the file.
      char dummy;
      vsl_b_read(bfs,dummy);
      if (bfs.is().eof())
      {
        bfs.close();
        if (errorStream)
          *errorStream << "Successfully loaded: " << path << '\n';
        return true;
      }
    }
  }
  bfs.close();
  if (errorStream)
    *errorStream << "Unable to load: "<< path <<'\n';
  return false;
}

// Save four objects to a file
template <class T, class S, class U, class V>
inline bool vsl_quick_file_save(const T &data1, const S &data2,
                                const U &data3, const V &data4,
                                const std::string& path,
                                std::ostream* errorStream = &std::cerr)
{
  vsl_b_ofstream bfs(path);
  if (!(!bfs))
  {
    vsl_b_write(bfs,data1);
    vsl_b_write(bfs,data2);
    vsl_b_write(bfs,data3);
    vsl_b_write(bfs,data4);
    if (!(!bfs))
    {
      bfs.close();
      if (errorStream)
        *errorStream << "Successfully saved: "<< path <<'\n';
      return true;
    }
  }
  bfs.close();
  if (errorStream)
    *errorStream << "Unable to save: " << path << '\n';
  return false;
}

// Load five objects from a file
template <class T, class S, class U, class V, class W>
inline bool vsl_quick_file_load(T &data1,
                                S &data2, U &data3, V &data4, W &data5,
                                const std::string& path,
                                std::ostream* errorStream = &std::cerr)
{
  vsl_b_ifstream bfs(path);
  if ( !(!bfs))
  {
    vsl_b_read(bfs,data1);
    vsl_b_read(bfs,data2);
    vsl_b_read(bfs,data3);
    vsl_b_read(bfs,data4);
    vsl_b_read(bfs,data5);
    if (!(!bfs))
    {
      // Check that we have reached the end of the file.
      char dummy;
      vsl_b_read(bfs,dummy);
      if (bfs.is().eof())
      {
        bfs.close();
        if (errorStream)
          *errorStream << "Successfully loaded: " << path << '\n';
        return true;
      }
    }
  }
  bfs.close();
  if (errorStream)
    *errorStream << "Unable to load: "<< path <<'\n';
  return false;
}

// Save five objects to a file
template <class T, class S, class U, class V, class W>
inline bool vsl_quick_file_save(const T &data1, const S &data2, const U &data3,
                                const V &data4, const W &data5,
                                const std::string& path,
                                std::ostream* errorStream = &std::cerr)
{
  vsl_b_ofstream bfs(path);
  if (!(!bfs))
  {
    vsl_b_write(bfs,data1);
    vsl_b_write(bfs,data2);
    vsl_b_write(bfs,data3);
    vsl_b_write(bfs,data4);
    vsl_b_write(bfs,data5);
    if (!(!bfs))
    {
      bfs.close();
      if (errorStream)
        *errorStream << "Successfully saved: "<< path <<'\n';
      return true;
    }
  }
  bfs.close();
  if (errorStream)
    *errorStream << "Unable to save: " << path << '\n';
  return false;
}


// Load six objects from a file
template <class T, class S, class U, class V, class W, class X>
inline bool vsl_quick_file_load(T &data1, S &data2, U &data3,
                                V &data4, W &data5, X &data6,
                                const std::string& path,
                                std::ostream* errorStream = &std::cerr)
{
  vsl_b_ifstream bfs(path);
  if ( !(!bfs))
  {
    vsl_b_read(bfs,data1);
    vsl_b_read(bfs,data2);
    vsl_b_read(bfs,data3);
    vsl_b_read(bfs,data4);
    vsl_b_read(bfs,data5);
    vsl_b_read(bfs,data6);
    if (!(!bfs))
    {
      // Check that we have reached the end of the file.
      char dummy;
      vsl_b_read(bfs,dummy);
      if (bfs.is().eof())
      {
        bfs.close();
        if (errorStream)
          *errorStream << "Successfully loaded: " << path << '\n';
        return true;
      }
    }
  }
  bfs.close();
  if (errorStream)
    *errorStream << "Unable to load: "<< path <<'\n';
  return false;
}

// Save six objects to a file
template <class T, class S, class U, class V, class W, class X>
inline bool vsl_quick_file_save(const T &data1, const S &data2, const U &data3,
                                const V &data4, const W &data5, const X &data6,
                                const std::string& path,
                                std::ostream* errorStream = &std::cerr)
{
  vsl_b_ofstream bfs(path);
  if (!(!bfs))
  {
    vsl_b_write(bfs,data1);
    vsl_b_write(bfs,data2);
    vsl_b_write(bfs,data3);
    vsl_b_write(bfs,data4);
    vsl_b_write(bfs,data5);
    vsl_b_write(bfs,data6);
    if (!(!bfs))
    {
      bfs.close();
      if (errorStream)
        *errorStream << "Successfully saved: "<< path <<'\n';
      return true;
    }
  }
  bfs.close();
  if (errorStream)
    *errorStream << "Unable to save: " << path << '\n';
  return false;
}
#endif // vsl_quick_file_h_
