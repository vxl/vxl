//:
// \file
// \brief A database manager
// \author Yong Zhao
// \date Tue Mar 23, 2007
//
// \verbatim
//  Modifications
// \endverbatim


#include <brdb/brdb_database_manager.h>
#include <brdb/brdb_relation.h>
#include <brdb/brdb_query.h>
#include <brdb/brdb_tuple.h>
#include <brdb/brdb_tuple_sptr.h>
#include <brdb/brdb_value.h>

brdb_database_sptr brdb_database_manager::instance_ = NULL;

//: Insure only one instance is created
brdb_database_sptr brdb_database_manager::instance()
{
  if (!instance_){
    instance_ = new brdb_database();
  }
  return brdb_database_manager::instance_;
}




//: clear all relations except
bool 
brdb_database_manager::clear_all_except(const vcl_set<vcl_string>& relation_names) 
{

  vcl_set<vcl_string> all_relation_names = instance()->get_all_relation_names();

  // check whether all those relation exist
  for(vcl_set<vcl_string>::const_iterator itr = relation_names.begin(); itr != relation_names.end(); ++itr)
  {
    if(!instance()->exists((*itr)))
    {
      vcl_cout << "Database manager warning: clear_all_except() failed, some relation does not exist." << vcl_endl;
      return false;
    }
  }

  // go through all names, if not in relation_names, remove them
  for(vcl_set<vcl_string>::iterator itr = all_relation_names.begin(); itr != all_relation_names.end(); ++itr)
  {
    if(relation_names.find((*itr)) == relation_names.end())
    {
      instance()->clear_relation((*itr));
    }
  }

  return true;
}

//: remove all relations except
bool 
brdb_database_manager::remove_all_except(const vcl_set<vcl_string>& relation_names) 
{

  vcl_set<vcl_string> all_relation_names = instance()->get_all_relation_names();

  // check whether all those relation exist
  for(vcl_set<vcl_string>::const_iterator itr = relation_names.begin(); itr != relation_names.end(); ++itr)
  {
    if(!instance()->exists((*itr)))
    {
      vcl_cout << "Database manager warning: clear_all_except() failed, some relation does not exist." << vcl_endl;
      return false;
    }
  }

  // go through all names, if not in relation_names, remove them
  for(vcl_set<vcl_string>::iterator itr = all_relation_names.begin(); itr != all_relation_names.end(); ++itr)
  {
    if(relation_names.find((*itr)) == relation_names.end())
    {
      instance()->remove_relation((*itr));
    }
  }

  return true;
}




//: save the current database into a file
bool
brdb_database_manager::save_database(const vcl_string& path) 
{
  vsl_b_ofstream out_stream(path);
  if (!out_stream){
    vcl_cerr<<"Failed to open " << path << " for binary IO output." << vcl_endl;
    return false;
  }
  vcl_cout << "Opened binary IO file "<< path << " successfully." << vcl_endl;

  instance()->b_write(out_stream);
  out_stream.close();
  return true;
}

//: load database from file
bool
brdb_database_manager::load_database(const vcl_string& path) 
{
  vsl_b_ifstream in_stream(path);
  if (!in_stream){
    vcl_cerr<<"Failed to open " << path << " for binary IO input." << vcl_endl;
    return false;
  }
  vcl_cout << "Opened binary IO file " << path << " successfully." << vcl_endl;

  instance()->b_read(in_stream);
  in_stream.close();
  
  return true;
}


//: load database from file and merge it with current database
bool
brdb_database_manager::merge_database(const vcl_string& path)
{
  vsl_b_ifstream in_stream(path);
  if (!in_stream){
    vcl_cerr<<"Failed to open " << path << " for binary IO input." << vcl_endl;
    return false;
  }
  vcl_cout << "Opened binary IO file " << path << " successfully." << vcl_endl;

  brdb_database_sptr from_file;
  from_file->b_read(in_stream);
  in_stream.close();

  return instance()->merge(from_file);
}



