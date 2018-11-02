#include <iostream>
#include <fstream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include "../boxm2_vecf_fit_orbit.h"
#include "../boxm2_vecf_fit_margins.h"
int main(int argc, char ** argv)
{

  vul_arg_info_list arglist;
  vul_arg<std::string> base_dir_path(arglist, "-bdir", "Base patient directory", "");
  vul_arg<std::string> patient_idstr(arglist, "-bid", "Patient id string", "");
  vul_arg<std::string> show_model_arg(arglist, "-smod", "Show model", "true");
  vul_arg<std::string> verbose_arg(arglist, "-verbose", "print out fitting results", "true");
  vul_arg<std::string> estimate_t_arg(arglist,"-estimate_t", "Estimate lid opening amount rather than s_y" , "false");
  vul_arg<float> mm_scale_arg(arglist,"-mm_scale", "mm per pixel coversion scale" , 0.0f);
  vul_arg<std::string> non_lin_arg(arglist, "-nlin", "non-linear refine orbit parameters", "true");
  vul_arg<std::string> dlib_arg(arglist, "-dlib", "2-d data from dlib", "false");
  arglist.parse(argc, argv, false);
  std::string show_model_str = show_model_arg();
  bool show_model = show_model_str == "true";

  std::string non_lin_str = non_lin_arg();
  bool do_non_lin = non_lin_str == "true";

  float mm_per_pix = mm_scale_arg();
  std::string dlib_str = dlib_arg();
  bool from_dlib = dlib_str == "true";

  std::string base_dir = base_dir_path();

  if(base_dir == ""){
    std::cout << "Must have a patient directory - fatal!\n";
    return -1;
  }
  std::string patient_id = patient_idstr();
  if(patient_idstr() == ""){
    std::cout << "Must have a patient id string - fatal!\n";
    return -1;
  }

  bool estimate_t = estimate_t_arg() == "true";
  bool verbose = verbose_arg() == "true";
  //  patient sub-directory named by patient_id
  base_dir =  base_dir+ patient_id + "/";

  std::string left_dlib_path = base_dir + patient_id + "_left_orbit_parts.txt";
  std::string left_anchor_path = base_dir + patient_id + "_left_anchors.txt";
  std::string left_sclera_path = base_dir + patient_id + "_left_sclera.txt";
  std::string left_inferior_margin_path = base_dir + patient_id + "_left_inferior_margin.txt";
  std::string left_inferior_surface_path = base_dir + patient_id + "_left_inferior_surface.txt";
  std::string left_superior_margin_path = base_dir + patient_id + "_left_superior_margin.txt";
  std::string left_superior_crease_path = base_dir + patient_id + "_left_superior_crease.txt";
  std::string left_vrml_path = base_dir + patient_id + "_left_orbit_plot.wrl";
  std::string left_param_path = base_dir + patient_id + "_left_orbit_params.txt";

  std::string right_dlib_path = base_dir + patient_id + "_right_orbit_parts.txt";
  std::string right_anchor_path = base_dir + patient_id + "_right_anchors.txt";
  std::string right_sclera_path = base_dir + patient_id + "_right_sclera.txt";
  std::string right_inferior_margin_path = base_dir + patient_id + "_right_inferior_margin.txt";
  std::string right_inferior_surface_path = base_dir + patient_id + "_right_inferior_surface.txt";
  std::string right_superior_margin_path = base_dir + patient_id + "_right_superior_margin.txt";
  std::string right_superior_crease_path = base_dir + patient_id + "_right_superior_crease.txt";
  std::string right_vrml_path = base_dir + patient_id + "_right_orbit_plot.wrl";
  std::string right_param_path = base_dir + patient_id + "_right_orbit_params.txt";

  std::string left_right_merge_path = base_dir + patient_id + "_left_right_merge_plot.wrl";

  std::ostream* output_s = &std::cout;
  boxm2_vecf_fit_orbit fo;
  bool good = true;
  /// left eye processing
  if(!from_dlib){
  good = fo.read_anchor_file(left_anchor_path);
  if(!good){
    std::cout << "failed to read left anchor file\n";
    return -1;
  }
  good = fo.load_orbit_data("left_eye_sclera", left_sclera_path);
  if(!good){
        std::cout << "failed to read left sclera file\n";
    return -1;
  }
  good = fo.load_orbit_data("left_eye_inferior_lid_surface", left_inferior_surface_path, false);
  if(!good){
    good = fo.load_orbit_data("left_eye_inferior_lid_surface", left_inferior_margin_path);
    if(!good){
      std::cout << "failed to read left inferior lid surface file  or next best choice the left_inferior_margin file\n";
      return -1;
    }else{
      fo.set_has_inferior_surface(false);
    }
  }
  }else{
    good = fo.read_dlib_part_file(left_dlib_path);
    if(!good)
      return -1;
  }

/// right eye processing
  if(!from_dlib){
  good = fo.read_anchor_file(right_anchor_path);
  if(!good){
    std::cout << "failed to read right anchor file\n";
    return -1;
  }
  good = fo.load_orbit_data("right_eye_sclera", right_sclera_path);
  if(!good){
    std::cout << "failed to read right sclera file\n";
    return -1;
  }
  good = fo.load_orbit_data("right_eye_inferior_lid_surface", right_inferior_surface_path, false);
  if(!good){
    std::cout << "failed to read right inferior lid surface file\n";
    good = fo.load_orbit_data("right_eye_inferior_lid_surface", right_inferior_margin_path);
    if(!good){
      std::cout << "failed to read right inferior lid surface file  or next best choice, the right_inferior_margin file\n";
      return -1;
    }else{
      fo.set_has_inferior_surface(false);
    }
  }
  }else{
    good = fo.read_dlib_part_file(right_dlib_path);
    if(!good)
      return -1;
  }

  good= fo.set_left_iris_radius();
  if(!good)
    return -1;
  good= fo.set_right_iris_radius();
  if(!good)
    return -1;

  if(fo.from_image_data())
    fo.normalize_eye_data(mm_per_pix);


  good = fo.fit_left();
  if(!good){
    std::cout << "left orbit parameter fit failed\n";
    return -1;
  }

  good = fo.fit_right();
  if(!good){
    std::cout << "right orbit parameter fit failed\n";
    return -1;
  }


  if(!from_dlib){
    fo.load_orbit_data("right_eye_inferior_margin", right_inferior_margin_path);
    fo.load_orbit_data("right_eye_superior_margin", right_superior_margin_path);
    fo.load_orbit_data("right_eye_superior_crease", right_superior_crease_path);
  }
  std::cout << "\n>>>Processing "<< patient_id << " right eye <<<\n";
  // non-linear fit
  if(do_non_lin){
    vgl_point_3d<double> rlat, rmed;
    good = fo.lab_point("right_eye_lateral_canthus", rlat);
    if(!good){
      std::cout << "right_eye_lateral_canthus missing\n";
      return -1;
    }
    good = fo.lab_point("right_eye_medial_canthus", rmed);
    if(!good){
      std::cout << "right_eye_medial_canthus missing\n";
      return -1;
    }
    boxm2_vecf_fit_margins right_fmargs(fo.orbit_data("right_eye_inferior_margin"), fo.orbit_data("right_eye_superior_margin"),
                                        fo.orbit_data("right_eye_superior_crease"), rlat, rmed,true,estimate_t);
    right_fmargs.set_initial_guess(fo.right_params());
    double rer = right_fmargs.fit(&std::cout, verbose);
    if(rer == -1){
      std::cout << "non-linear fit for right eye failed \n";
      return -1;
    }
    boxm2_vecf_orbit_params rprm = right_fmargs.orbit_parameters();
    if(estimate_t){
      std::cout<<"right T was estimated to be "<<rprm.superior_margin_t<<std::endl;
      rprm.eyelid_dt_ = rprm.eyelid_tmax_ - rprm.superior_margin_t;
    }

    fo.set_right_params(rprm);
    std::string right_marg_path = base_dir + patient_id +"_right_margin_fit.txt";
    std::ofstream rmstr(right_marg_path.c_str());
    good = right_fmargs.plot_orbit(rmstr);
    if(!good){
      std::cout << "ERROR: Invalid parameter to plot_orbit: " << __FILE__ << __LINE__ << std::endl;
      return -1;
    }
    rmstr.close();
  }
  // if image data set z values for margin and crease curve points
  if(fo.from_image_data()){
    good = fo.set_right_z_values();
    if(!good)
      return -1;
  }


  std::ofstream rostr(right_vrml_path.c_str());
  good = fo.display_orbit_vrml(rostr, true, show_model);
  if(!good){
    std::cout << "ERROR: Invalid parameter to display_orbit: " << __FILE__ << __LINE__ << std::endl;
    return -1;
  }
  rostr.close();

  fo.fitting_error("right_eye_inferior_margin");
  fo.fitting_error("right_eye_superior_margin");
  fo.fitting_error("right_eye_superior_crease");

  std::ofstream rpstr(right_param_path.c_str());
  rpstr << fo.right_params() << '\n';
  rpstr.close();

  if(verbose){
    std::cout<<"---Right parameters---"<<std::endl;
    std::cout<<fo.right_params()<<std::endl;
    }

  if(!from_dlib){
    fo.load_orbit_data("left_eye_inferior_margin", left_inferior_margin_path);
    fo.load_orbit_data("left_eye_superior_margin", left_superior_margin_path);
    fo.load_orbit_data("left_eye_superior_crease", left_superior_crease_path);
  }


  std::cout << "\n>>>Processing "<< patient_id << " left eye <<<\n";
  // non-linear fit
  if(do_non_lin){
    vgl_point_3d<double> llat, lmed;
    good = fo.lab_point("left_eye_lateral_canthus", llat);
    if(!good){
      std::cout << "left_eye_lateral_canthus missing\n";
      return -1;
    }
    good = fo.lab_point("left_eye_medial_canthus", lmed);
    if(!good){
      std::cout << "left_eye_medial_canthus missing\n";
      return -1;
    }
    boxm2_vecf_fit_margins left_fmargs(fo.orbit_data("left_eye_inferior_margin"), fo.orbit_data("left_eye_superior_margin"),
                                       fo.orbit_data("left_eye_superior_crease"),  llat, lmed,false,estimate_t);
    left_fmargs.set_initial_guess(fo.left_params());
    double ler = left_fmargs.fit(&std::cout, verbose);
    if(ler == -1){
      std::cout << "non-linear fit for left eye failed \n";
      return -1;
    }
    boxm2_vecf_orbit_params lprm = left_fmargs.orbit_parameters();
    if(estimate_t){
      std::cout<<"left T was estimated to be "<<lprm.superior_margin_t<<std::endl;
      lprm.eyelid_dt_ = lprm.eyelid_tmax_ - lprm.superior_margin_t;
    }

    fo.set_left_params(lprm);
    std::string left_marg_path = base_dir + patient_id +"_left_margin_fit.txt";
    std::ofstream lmstr(left_marg_path.c_str());
    good = left_fmargs.plot_orbit(lmstr);
    if(!good){
      std::cout << "ERROR: Invalid parameter to plot_orbit: " << __FILE__ << __LINE__ << std::endl;
      return -1;
    }
    lmstr.close();
  }
  // if image data set z values for margin and crease curve points
  if(fo.from_image_data()){
    good = fo.set_left_z_values();
     if(!good)
      return -1;
  }
  std::ofstream lostr(left_vrml_path.c_str());
  good = fo.display_orbit_vrml(lostr, false, show_model);
  if(!good){
    std::cout << "ERROR: Invalid parameter to display_orbit: " << __FILE__ << __LINE__ << std::endl;
    return -1;
  }
  lostr.close();

  // find canthus angles
  fo.set_canthus_angle(false);
  fo.set_canthus_angle(true);

  // test the fitting accuracy
  fo.fitting_error("left_eye_inferior_margin");
  fo.fitting_error("left_eye_superior_margin");
  fo.fitting_error("left_eye_superior_crease");

   std::ofstream lpstr(left_param_path.c_str());
  lpstr << fo.left_params() << '\n';
  lpstr.close();

  if(verbose){
    std::cout<<"---Left parameters---"<<std::endl;
    std::cout<<fo.left_params()<<std::endl;
    }

  std::ofstream lrmstr(left_right_merge_path.c_str());
  good = fo.display_left_right_orbit_model_vrml(lrmstr);
  if(!good){
    std::cout << "ERROR: Invalid parameter to display_orbit: " << __FILE__ << __LINE__ << std::endl;
    return -1;
  }
  lrmstr.close();

  return 0;
}
