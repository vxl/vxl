#include <vsrl/vsrl_parameters.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cstring.h> // for strcmp()

// initialize the _instance variable
vsrl_parameters* vsrl_parameters::_instance = 0;

// -- Returns the one instance of a vsrl_parameters.  One
// is created on the first call to Instance

vsrl_parameters* vsrl_parameters::instance()
{
  if(vsrl_parameters::_instance == 0){
    vsrl_parameters::_instance = new vsrl_parameters;
  }
  
  return vsrl_parameters::_instance;
}


// constructor

vsrl_parameters::vsrl_parameters(){
  
  // set default values 
  
  // ***** used in many classes ******
  
  // the largest amount of allowable disparaty -  default 10
  correlation_range =10;

  // ****** used in dynamic_program ******
  
  // the cost of not assigning an inner pixel - default 1.0
  inner_cost = 1.0; 
  
  // the cost of not assigning an inner pixel - default 0.5
  outer_cost =0.5; 
  
  // the cost for discontinuous pixel assignments  - default 0.1
  continuity_cost =0.1; 
  
  // **** used in image correlation *****
  
  // the correlation window width - default 20
  
  correlation_window_width = 20;

  // the correlation window height - default 20 
  
  correlation_window_height = 20;
  
  // *** used in the raster dp setup *****
  
  // the cost of a bias towards the previous assignment - default 0.2 
  
  bias_cost=0.2;

  // the difference in intensity which defines a common region - default 0.0
  
  common_intensity_diff=0.0;
    
}

// destructor 
vsrl_parameters::~vsrl_parameters(){

}

void vsrl_parameters::load(char *filename)
{
  // read in the parameters

  ifstream file(filename);
  
  // read in the parameters 
  
  char tag[512];
  float val;

  while(!file.eof()){
    
    file >> tag;
    file >> val;
     
    if(!strcmp(tag,"correlation_range")){
      cout << "setting correlation_range to " << val << endl;
      this->correlation_range=(int)val;
    }

    if(!strcmp(tag,"inner_cost")){
      cout << "setting inner_cost to " << val << endl;
      this->inner_cost=(double)val;
    }
    
    
    if(!strcmp(tag,"outer_cost")){
      cout << "setting outer_cost to " << val << endl;
      this->outer_cost=(double)val;
    }
    
    
    if(!strcmp(tag,"continuity_cost")){
      cout << "setting continuity_cost to " << val << endl;
      this->continuity_cost=(double)val;
    }
    
    
    if(!strcmp(tag,"correlation_window_width")){
      cout << "setting correlation_window_width to " << val << endl;
      this->correlation_window_width=(int)val;
    }

    
    if(!strcmp(tag,"correlation_window_height")){
      cout << "setting correlation_window_height to " << val << endl;
      this->correlation_window_height=(int)val;
    }
    
    
    if(!strcmp(tag,"bias_cost")){
      cout << "setting bias_cost to " << val << endl;
      this->bias_cost=(double)val;
    }
    

    if(!strcmp(tag,"common_intensity_diff")){
      cout << "setting common_intensity_diff to " << val << endl;
      this->common_intensity_diff=(double)val;
    }
    
  }
}
