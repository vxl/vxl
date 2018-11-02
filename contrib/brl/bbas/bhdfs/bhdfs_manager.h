// This is brl/bbas/bhdfs/bhdfs_manager.h
#ifndef bhdfs_manager_h_
#define bhdfs_manager_h_
//:
// \file
// \brief A parent class for singleton hadoop file system managers
// \author Ozge C. Ozcanli
// \date November 30, 2011
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <vector>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include "hdfs.h"

class bhdfs_manager;
typedef vbl_smart_ptr<bhdfs_manager> bhdfs_manager_sptr;

class bhdfs_manager_destroyer;

class bhdfs_manager: public vbl_ref_count
{
 public:

  //: Use this to access singleton instance
  static bhdfs_manager_sptr instance();

  //: create function used instead of constructor
  //  Pass 'default' as host (and port as 0) to use the 'configured' filesystem in ${HADOOP_HOME}/conf/core-default.xml
  //  Pass NULL as host (and port as 0) to use the filesystem on localhost
  static void create(std::string host_name, int port);
  static void create(std::string host_name, int port, std::string user_name);

  static bool exists() { return bhdfs_manager::instance_!=0; }

  //: the destructor instance to make sure memory is deallocated when the program exits
  static bhdfs_manager_destroyer destroyer_;  // it's not a pointer so C++ will make sure that its destructor will be called
  friend class bhdfs_manager_destroyer;

  //: check if FS is opened fine
  bool ok() { return fs_ != 0; }

  //: get the current working directory on hdfs
  std::string get_working_dir();

  //: delete the file, returns true on success, false on error.
  bool rm(std::string path);

  //: create a directory, returns true on success, false on error. create permission is rwx rwx --- by default
  bool create_dir(std::string path, short mode =  S_IRWXU | S_IRWXG);

  //: remove a directory, returns true on success, false on error.
  bool remove_dir(std::string path) { return rm(path); }

  //: check existence of a file or path,  returns true on success, false on error.
  bool exists(std::string path);

  //: copy file from local dir to hdfs folder
  bool copy_to_hdfs(std::string local_file, std::string hdfs_folder);

  //: copy file from hdfs to local dir, hdfs_file is the full path of the file on hdfs
  bool copy_from_hdfs(std::string hdfs_file, std::string local_dir);

  //: get a list of filenames in the given directory
  bool get_dir_list(std::string dir, std::vector<std::string>& fnames);

  friend class bhdfs_fstream;

 protected:

  //: hidden constructor
  bhdfs_manager(std::string host_name, int port);
  bhdfs_manager(std::string host_name, int port, std::string user_name);

  //: hidden destructor
  virtual ~bhdfs_manager() {}

  //: singleton instance of the manager
  static bhdfs_manager_sptr instance_;

  //: the file system handle for hdfs (its type is void*)
  hdfsFS fs_;
};

//: Binary write bhdfs_manager to stream
void vsl_b_write(vsl_b_ostream& os, bhdfs_manager const& scene);
//: Binary write bhdfs_manager to stream
void vsl_b_write(vsl_b_ostream& os, const bhdfs_manager* &p);
//: Binary write bhdfs_manager to stream
void vsl_b_write(vsl_b_ostream& os, bhdfs_manager_sptr& sptr);
//: Binary write bhdfs_manager to stream
void vsl_b_write(vsl_b_ostream& os, bhdfs_manager_sptr const& sptr);

//: Binary load boxm2_scene scene from stream.
void vsl_b_read(vsl_b_istream& is, bhdfs_manager &scene);
//: Binary load boxm2_scene scene from stream.
void vsl_b_read(vsl_b_istream& is, bhdfs_manager* p);
//: Binary load boxm2_scene scene from stream.
void vsl_b_read(vsl_b_istream& is, bhdfs_manager_sptr& sptr);
//: Binary load boxm2_scene scene from stream.
void vsl_b_read(vsl_b_istream& is, bhdfs_manager_sptr const& sptr);


//: create another class whose sole purpose is to destroy the singleton instance
class bhdfs_manager_destroyer
{
 public:
  bhdfs_manager_destroyer(bhdfs_manager_sptr s = 0);
  ~bhdfs_manager_destroyer();

  void set_singleton(bhdfs_manager_sptr s);
 private:
  bhdfs_manager_sptr s_;
};

#endif // bhdfs_manager_h_
