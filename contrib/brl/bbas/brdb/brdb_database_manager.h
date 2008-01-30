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
// \endverbatim



#include <vcl_set.h>
#include <vcl_string.h>
#include <brdb/brdb_database.h>

#define DATABASE brdb_database_manager::instance()

class brdb_database_manager
{
public:
  //: the global database instance
  static brdb_database_sptr instance();


  //: clear all relations except
  static bool clear_all_except(const vcl_set<vcl_string>& relation_names);

  //: remove all relations except
  static bool remove_all_except(const vcl_set<vcl_string>& relation_names);



  //: save the current database into a file
  static bool save_database(const vcl_string& path);

  //: load database from file
  static bool load_database(const vcl_string& path);

  //: load database from file and merge it with current database
  static bool merge_database(const vcl_string& path);


private:
  //: Constructor
  brdb_database_manager() {};



private:
  //: A static instance of the database
  static brdb_database_sptr instance_;

};


#endif  // brdb_database_manager_h_
