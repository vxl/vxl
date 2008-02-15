// This is brl/bprb/bprb_batch_process_manager.cxx

//:
// \File

#include "bprb_batch_process_manager.h"

#include <vcl_cassert.h>
#include <vcl_utility.h>
#include <vul/vul_arg.h>

#include <brdb/brdb_database_manager.h>
#include <brdb/brdb_relation.h>
#include <brdb/brdb_selection.h>
#include <brdb/brdb_query.h>
#include <brdb/brdb_tuple.h>
#include <bprb/bprb_process.h>
#include <bprb/bprb_null_process.h>
//: Constructor
bprb_batch_process_manager::bprb_batch_process_manager() : current_process_(0)
{

}

//: Destructor
bprb_batch_process_manager::~bprb_batch_process_manager()
{
}
bool bprb_batch_process_manager::clear()
{
  return true;
}

bool bprb_batch_process_manager::init_process(vcl_string const& process_name)
{
  bprb_process_sptr p = get_process_by_name(process_name);
  if(p){
    vcl_cout << "Process: " << p->name() << '\n';
    current_process_ = p;
    return true;
  }
  return false;
}

//: initialize the process, read the parameters from an XML file
bool bprb_batch_process_manager::set_process_params(vcl_string const& process_name, 
                                                    vcl_string const& params_XML)
{
  bprb_process_sptr p = get_process_by_name(process_name);
  if(p){
    vcl_cout << "Process: " << p->name() << '\n';
    p->parse_params_XML(params_XML);
    return true;
  }
  return false;
}

//: set primitive data type input on current process
bool bprb_batch_process_manager::
set_input(unsigned i, brdb_value_sptr const& input)
{
  if(!current_process_){
    vcl_cout << "In bprb_batch_process_manager::set_input(.) -"
             << " null process\n";
    return false;
  }
  if(!current_process_->set_input(i, input))
    return false;
  return true;
}

//: set input from the database
bool bprb_batch_process_manager::set_input_from_db(unsigned i,
                                                   unsigned id)
{
  if(!current_process_)
    return false;
  vcl_string type = current_process_->input_type(i);
  // construct the name of the relation
  vcl_string relation_name = type + "_data";

  // query to get the data 
  brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id);

  brdb_selection_sptr selec = DATABASE->select(relation_name, Q);
  if(selec->size()!=1){
    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
    << " no selections\n";
    return false;
  }
  brdb_value_sptr value;
  if(!selec->get_value(vcl_string("value"), value)){
    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
     << " didn't get value\n";
    return false;
  }
  if(!value){
    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
     << " null value \n";
    return false;
  }
  if(!current_process_->set_input(i, value)){
    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
    << " can't set input on process \n";
    return false;
  }
  return true;
}

//: put the output into the database
bool bprb_batch_process_manager::commit_output(unsigned i, unsigned& id)
{
  if(!current_process_)
    return false;
  vcl_string type = current_process_->output_type(i);
  // construct the name of the relation
  vcl_string relation_name = type + "_data";
  // construct the tuple
  id = brdb_database_manager::id();
  brdb_value_sptr vid = new brdb_value_t<unsigned>(id);
  brdb_value_sptr value = current_process_->output(i);
  if(!value){
    vcl_cout << "bprb_batch_process_manager::commit_output(.) -"
             << " null process output\n";
    return false;
  }
  brdb_tuple_sptr t = new brdb_tuple();
  t->add_value(vid); t->add_value(value);
  if(!DATABASE->add_tuple(relation_name, t)){
    vcl_cout << "bprb_batch_process_manager::commit_output(.) -"
             << " failed to add tuple to relation " << relation_name
             << '\n';
    return false;
  }
  return true;
}

bool bprb_batch_process_manager::commit_all_outputs(vcl_vector<unsigned>& ids)
{
  if(!current_process_)
    return false;
  ids.clear();
  for(unsigned i = 0; i<current_process_->n_outputs(); ++i){
    unsigned id;
    if(!this->commit_output(i, id))
      return false;
    else
      ids.push_back(id);
  }
  return true;
}

//: remove data from the database
bool bprb_batch_process_manager::remove_data(unsigned id)
{
  bool removed = false;
  vcl_set<vcl_string> names = DATABASE->get_all_relation_names();
  for(vcl_set<vcl_string>::iterator nit = names.begin();
      nit != names.end()&&!removed; ++nit){
    // query to get the data 
    brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id);

    brdb_selection_sptr selec = DATABASE->select(*nit, Q);
    if(selec->size()!=1) continue;
    selec->delete_tuples();
    removed = true;
  }
  return removed;
}
//: initilize before execution
bool bprb_batch_process_manager::process_init()
{
  bool to_return = false;
  if(!current_process_)
    return to_return;
  vcl_cout << "Initializing process: " << current_process_->name() << '\n';

  to_return = current_process_->init();

  return to_return;

}
//: Run a process on the current frame
bool bprb_batch_process_manager::run_process()
{
  bool to_return = false;
  if(!current_process_)
    return to_return;
  vcl_cout << "Running process: " << current_process_->name() << '\n';
  // EXECUTE ///////////////////////////////////////////////
  to_return = current_process_->execute();
  //////////////////////////////////////////////////////////

  // RETURN VALUE //////////////////////////////////////////
  return to_return;
  //////////////////////////////////////////////////////////
}

void bprb_batch_process_manager::print_db()
{DATABASE->print();}
