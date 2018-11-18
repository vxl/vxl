#include "bhdfs_manager.h"
//:
// \file

//: global initialization for singleton instance_
bhdfs_manager_sptr bhdfs_manager::instance_ = 0;

//: global initialization for singleton destroyer instance
bhdfs_manager_destroyer bhdfs_manager::destroyer_;

//: Only one instance should be created (by a derived class)
bhdfs_manager_sptr bhdfs_manager::instance()
{
  if (!instance_)
    std::cerr<<"warning: bhdfs_manager:: instance has not been created\n";
  return instance_;
}
//: hidden constructor
bhdfs_manager::bhdfs_manager(std::string host_name, int port)
{
  fs_ = hdfsConnect(host_name.c_str(), port);
  if (!fs_) {
    std::cerr << "Cannot connect to the file system on: " << host_name << " using port: " << port << "!\n";
    throw 0;
  }
}
bhdfs_manager::bhdfs_manager(std::string host_name, int port, std::string user_name)
{
  fs_ = hdfsConnectAsUser(host_name.c_str(), port, user_name.c_str());
  if (!fs_) {
    std::cerr << "Cannot connect to the file system on: " << host_name << " using port: " << port << " user_name: " << user_name << "!\n";
    throw 0;
  }
}

//: create function used instead of constructor
void bhdfs_manager::create(std::string host_name, int port)
{
  instance_ = new bhdfs_manager(host_name, port);
  destroyer_.set_singleton(instance_);
}
void bhdfs_manager::create(std::string host_name, int port, std::string user_name)
{
  instance_ = new bhdfs_manager(host_name, port, user_name.c_str());
  destroyer_.set_singleton(instance_);
}

//: get the current working directory on hdfs
std::string bhdfs_manager::get_working_dir()
{
  char buffer[256];
  hdfsGetWorkingDirectory(fs_, buffer, sizeof(buffer));
  return std::string (buffer);
}

//: delete the file, returns true on success, false on error.
bool bhdfs_manager::rm(std::string path)
{
  return hdfsDelete(fs_, path.c_str()) != -1;
}

//: create a directory, returns true on success, false on error.
bool bhdfs_manager::create_dir(std::string path, short mode)
{
  int val = hdfsCreateDirectory(fs_, path.c_str());
  if (val == -1)
    return false;
  return hdfsChmod(fs_, path.c_str(), mode) != -1;
}

//: check existence of a file or path, returns true on success, false on error.
bool bhdfs_manager::exists(std::string path)
{
  return hdfsExists(fs_, path.c_str()) != -1;
}

//: copy file from local dir to hdfs folder
bool bhdfs_manager::copy_to_hdfs(std::string local_file, std::string hdfs_folder)
{
  hdfsFS lfs = hdfsConnect(NULL, 0);
  if (!lfs) return false;
  return hdfsCopy(lfs, local_file.c_str(), fs_, hdfs_folder.c_str()) != -1;
}

//: copy file from hdfs to local dir, hdfs_file is the full path of the file on hdfs
bool bhdfs_manager::copy_from_hdfs(std::string hdfs_file, std::string local_dir)
{
  hdfsFS lfs = hdfsConnect(NULL, 0);
  if (!lfs) return false;
  return hdfsCopy(fs_, hdfs_file.c_str(), lfs, local_dir.c_str()) != -1;
}

//: get a list of filenames in the given directory
bool bhdfs_manager::get_dir_list(std::string dir, std::vector<std::string>& fnames)
{
  int cnt;
  hdfsFileInfo* inf = hdfsListDirectory(fs_, dir.c_str(), &cnt);
  if (!inf || !cnt)
    return false;

  fnames.clear();
  for (int i = 0; i < cnt; i++) {
    fnames.push_back(std::string(inf[i].mName));
  }

  return true;
}



//: Binary write bhdfs_manager  to stream
void vsl_b_write(vsl_b_ostream& os, bhdfs_manager const& scene){}
void vsl_b_write(vsl_b_ostream& os, const bhdfs_manager* &p){}
void vsl_b_write(vsl_b_ostream& os, bhdfs_manager_sptr& sptr){}
void vsl_b_write(vsl_b_ostream& os, bhdfs_manager_sptr const& sptr){}

//: Binary load bhdfs_manager  from stream.
void vsl_b_read(vsl_b_istream& is, bhdfs_manager &scene){}
void vsl_b_read(vsl_b_istream& is, bhdfs_manager* p){}
void vsl_b_read(vsl_b_istream& is, bhdfs_manager_sptr& sptr){}
void vsl_b_read(vsl_b_istream& is, bhdfs_manager_sptr const& sptr){}


bhdfs_manager_destroyer::bhdfs_manager_destroyer(bhdfs_manager_sptr s)
{
  s_ = s;
}

//: the destructor deletes the instance
bhdfs_manager_destroyer::~bhdfs_manager_destroyer()
{
  if (s_ != 0)
    s_->unref();  // smart pointer is deleted when ref cnt is zero
}

void bhdfs_manager_destroyer::set_singleton(bhdfs_manager_sptr s)
{
  s_ = s;
}
