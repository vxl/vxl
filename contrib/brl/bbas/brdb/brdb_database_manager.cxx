#include "brdb_database_manager.h"
//:
// \file
// \brief A database manager
// \author Yong Zhao
// \date Tue Mar 23, 2007
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <brdb/brdb_relation.h>
#include <brdb/brdb_query.h>
#include <brdb/brdb_tuple.h>
#include <brdb/brdb_tuple_sptr.h>
#include <brdb/brdb_value.h>

brdb_database_sptr brdb_database_manager::instance_ = nullptr;

unsigned brdb_database_manager::id_ = 0;
//: Insure only one instance is created
brdb_database_sptr brdb_database_manager::instance()
{
  if (!instance_){
    instance_ = new brdb_database();
  }
  return brdb_database_manager::instance_;
}

//: clear all relations
bool
brdb_database_manager::clear_all()
{
  std::set<std::string> all_relation_names = instance()->get_all_relation_names();

  // go through all names, if not in relation_names, remove them
  for (const auto & all_relation_name : all_relation_names)
  {
      if (instance()->exists(all_relation_name))
        instance()->clear_relation(all_relation_name);
  }

  return true;
}


//: clear all relations except
bool
brdb_database_manager::clear_all_except(const std::set<std::string>& relation_names)
{
  std::set<std::string> all_relation_names = instance()->get_all_relation_names();

  // check whether all those relation exist
  for (const auto & relation_name : relation_names)
  {
    if (!instance()->exists(relation_name))
    {
      std::cout << "Database manager warning: clear_all_except() failed, some relation does not exist." << std::endl;
      return false;
    }
  }

  // go through all names, if not in relation_names, remove them
  for (const auto & all_relation_name : all_relation_names)
  {
    if (relation_names.find(all_relation_name) == relation_names.end())
    {
      instance()->clear_relation(all_relation_name);
    }
  }

  return true;
}

//: remove all relations except
bool
brdb_database_manager::remove_all_except(const std::set<std::string>& relation_names)
{
  std::set<std::string> all_relation_names = instance()->get_all_relation_names();

  // check whether all those relation exist
  for (const auto & relation_name : relation_names)
  {
    if (!instance()->exists(relation_name))
    {
      std::cout << "Database manager warning: clear_all_except() failed, some relation does not exist." << std::endl;
      return false;
    }
  }

  // go through all names, if not in relation_names, remove them
  for (const auto & all_relation_name : all_relation_names)
  {
    if (relation_names.find(all_relation_name) == relation_names.end())
    {
      instance()->remove_relation(all_relation_name);
    }
  }

  return true;
}


//: save the current database into a file
bool
brdb_database_manager::save_database(const std::string& path)
{
  vsl_b_ofstream out_stream(path);
  if (!out_stream){
    std::cerr<<"Failed to open " << path << " for binary IO output.\n";
    return false;
  }
  std::cout << "Opened binary IO file "<< path << " successfully." << std::endl;

  instance()->b_write(out_stream);
  out_stream.close();
  return true;
}

//: load database from file
bool
brdb_database_manager::load_database(const std::string& path)
{
  vsl_b_ifstream in_stream(path);
  if (!in_stream){
    std::cerr<<"Failed to open " << path << " for binary IO input.\n";
    return false;
  }
  std::cout << "Opened binary IO file " << path << " successfully." << std::endl;

  instance()->b_read(in_stream);
  in_stream.close();

  return true;
}


//: load database from file and merge it with current database
bool
brdb_database_manager::merge_database(const std::string& path)
{
  vsl_b_ifstream in_stream(path);
  if (!in_stream){
    std::cerr<<"Failed to open " << path << " for binary IO input.\n";
    return false;
  }
  std::cout << "Opened binary IO file " << path << " successfully." << std::endl;

  brdb_database_sptr from_file;
  from_file->b_read(in_stream);
  in_stream.close();

  return instance()->merge(from_file);
}
