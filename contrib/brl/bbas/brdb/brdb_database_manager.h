#ifndef brdb_database_manager_h_
#define brdb_database_manager_h_

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


#include <iostream>
#include <set>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <brdb/brdb_database.h>
#include "brdb_export.h"

#define DATABASE brdb_database_manager::instance()

class brdb_database_manager
{
 public:
  //: the global database instance
  static brdb_database_sptr instance();

  //: a unique id
  static unsigned id() {return id_++;}

  //: clear all relations
  static bool clear_all();

  //: clear all relations except
  static bool clear_all_except(const std::set<std::string>& relation_names);

  //: remove all relations except
  static bool remove_all_except(const std::set<std::string>& relation_names);

  //: save the current database into a file
  static bool save_database(const std::string& path);

  //: load database from file
  static bool load_database(const std::string& path);

  //: load database from file and merge it with current database
  static bool merge_database(const std::string& path);

 private:
  //: Constructor
  brdb_database_manager() = default;

 private:
  //: A static instance of the database
  static brdb_EXPORT_DATA brdb_database_sptr instance_;
  //: A unique id
  static brdb_EXPORT_DATA unsigned id_;
};


#endif  // brdb_database_manager_h_
