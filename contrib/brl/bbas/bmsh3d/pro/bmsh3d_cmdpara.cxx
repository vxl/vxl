// This is brl/bbas/bmsh3d/pro/bmsh3d_cmdpara.cxx
#include <iostream>
#include <limits>
#include "bmsh3d_cmdpara.h"
//:
// \file
// \author Ming-Ching Chang
// \date Feb 12, 2007

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bmsh3d/bmsh3d_utils.h>

//#####################################################################

//: bmsh3d_cmdproc command-line arguments.
//     Type     Variable     Flag      Help text        Default

        //   --- Mesh File I/O ---
vul_arg<char const*>  bmsh3d_cmd_p3d_file ("-p3d", "Point cloud file (.p3d)");
vul_arg<char const*>  bmsh3d_cmd_g3d_file ("-g3d", "Point cloud with color (.g3d)");
vul_arg<char const*>  bmsh3d_cmd_xyz_file ("-xyz", "Point cloud file (.xyz)");
vul_arg<char const*>  bmsh3d_cmd_xyzn1_file ("-xyzn1", "Oriented point cloud file (.xyzn1)");
vul_arg<char const*>  bmsh3d_cmd_xyznw_file ("-xyznw", "Oriented point cloud file (.xyznw)");
vul_arg<char const*>  bmsh3d_cmd_ply_file ("-ply", "Polygonal mesh file (.ply)");
vul_arg<char const*>  bmsh3d_cmd_ply2_file ("-ply2", "Polygonal mesh file (.ply2)");
vul_arg<char const*>  bmsh3d_cmd_off_file ("-off", "Geomview OFF mesh file (.off)");
vul_arg<char const*>  bmsh3d_cmd_obj_file ("-obj", "Wavefront OBJ mesh file (.obj)");
vul_arg<char const*>  bmsh3d_cmd_m_file ("-m", "Hugues Hoppe's mesh file (*.m)");
vul_arg<char const*>  bmsh3d_cmd_3pi_file ("-3pi", "ShapeGrabber raw scan file (*.3pi)");
vul_arg<char const*>  bmsh3d_cmd_iv_file ("-iv", "OpenInventor visualization file (.iv)");
vul_arg<char const*>  bmsh3d_cmd_wrl_file ("-wrl", "VRML mesh file (.wrl)");

//  {"cn3", _STRING, 0, &CMD_CN3, "*.cn3 contour curve file"},
//  {"c3s", _STRING, 0, &CMD_C3S, "*.c3d file for set of contour curves"},
//  {"cnt", _STRING, 0, &CMD_CNT, "Output Nuages File (xxx.cnt)."},

vul_arg<char const*>  bmsh3d_cmd_p3d_file2 ("-p3d_2", "Second P3D point cloud file (.p3d)");
vul_arg<char const*>  bmsh3d_cmd_xyz_file2 ("-xyz_2", "Second XYZ point cloud file (.xyz)");
vul_arg<char const*>  bmsh3d_cmd_xyzn1_file2 ("-xyzn1_2", "Second oriented point cloud file (.xyzn1)");
vul_arg<char const*>  bmsh3d_cmd_3pi_file2 ("-3pi_2", "Second 3PI raw scan file (.3pi)");
vul_arg<char const*>  bmsh3d_cmd_ply_file2 ("-ply_2", "Second PLY mesh file (.ply2)");
vul_arg<char const*>  bmsh3d_cmd_ply2_file2 ("-ply2_2", "Second PLY2 mesh file (.ply2)");
vul_arg<char const*>  bmsh3d_cmd_iv_file2 ("-iv_2", "Second OpenInventor IV file (.iv)");
vul_arg<char const*>  bmsh3d_cmd_iv_file3 ("-iv_3", "Third OpenInventor IV file (.iv)");

        //   --- IV/VRML parsing ---
vul_arg<char const*>  bmsh3d_cmd_iv_pfile ("-piv", "Parse reading OpenInventor file (*.iv)", nullptr);
vul_arg<char const*>  bmsh3d_cmd_wrl_pfile ("-pwrl", "Parse reading VRML ASCII mesh file (*.wrl)", nullptr);

vul_arg<char const*>  bmsh3d_cmd_fileprefix ("-f", "Input file prefix");
vul_arg<char const*>  bmsh3d_cmd_fileprefix1 ("-f1", "Input file prefix1");
vul_arg<char const*>  bmsh3d_cmd_fileprefix2 ("-f2", "Input file prefix2");
vul_arg<char const*>  bmsh3d_cmd_align_file ("-af", "Affine (R+T) xform file (align 2nd to 1st).");
vul_arg<char const*>  bmsh3d_cmd_fix_af ("-fixaf", "Fix the orientation of alignment file (of Polywork).");
vul_arg<char const*>  bmsh3d_cmd_align_listfile ("-aflist", "Affine (R+T) xform list file.");
vul_arg<char const*>  bmsh3d_cmd_compare_pts ("-cnpt", "Compare corresponding point sets");

        //   --- Output files ---
vul_arg<int>          bmsh3d_cmd_ofile ("-ofile", "Produce output file", 1);
vul_arg<int>          bmsh3d_cmd_fileo ("-fileo", "Output file option", 1);
vul_arg<char const*>  bmsh3d_cmd_p3d_ofile ("-op3d", "Output point cloud file (.p3d)");
vul_arg<char const*>  bmsh3d_cmd_g3d_ofile ("-og3d", "Output point cloud file with color (.g3d)");
vul_arg<char const*>  bmsh3d_cmd_xyz_ofile ("-oxyz", "Output point cloud file (.xyz)");
vul_arg<char const*>  bmsh3d_cmd_xyzn1_ofile ("-oxyzn1", "Output oriented point cloud file (.xyzn1)");
vul_arg<char const*>  bmsh3d_cmd_xyznw_ofile ("-oxyznw", "Output oriented point cloud file (.xyznw)");
vul_arg<char const*>  bmsh3d_cmd_ply_ofile ("-oply", "Output PLY ASCII mesh file (.ply)");
vul_arg<char const*>  bmsh3d_cmd_ply_ofileb ("-oplyb", "Output PLY bindary mesh file (.ply)");
vul_arg<char const*>  bmsh3d_cmd_ply2_ofile ("-oply2", "Output polygonal mesh file (.ply2)");
vul_arg<char const*>  bmsh3d_cmd_off_ofile ("-ooff", "Output Geomview OFF mesh file (.off)");
vul_arg<char const*>  bmsh3d_cmd_obj_ofile ("-oobj", "Output Wavefront OBJ mesh file (.obj)");
vul_arg<char const*>  bmsh3d_cmd_3pi_ofile ("-o3pi", "Output ShapeGrabber raw scan file (.3pi)");
vul_arg<char const*>  bmsh3d_cmd_iv_ofile ("-oiv", "Output OpenInventor file (.iv)");
vul_arg<char const*>  bmsh3d_cmd_iv_ofilebin ("-oivbin", "Output OpenInventor binary file (.iv)");
vul_arg<char const*>  bmsh3d_cmd_wrl_ofile ("-owrl", "Output VRML mesh file (.wrl)");
vul_arg<char const*>  bmsh3d_cmd_wrl_ofile2 ("-owrl2", "Output VRML 2.0 (VRML97) mesh file (.wrl)");
vul_arg<char const*>  bmsh3d_cmd_align_ofile ("-oaf", "Output affine (R+T) xform file (align 2nd to 1st).");

        //   --- Point/Mesh Pre-Processing ---
vul_arg<int>          bmsh3d_cmd_preproc ("-pp", "Preprocess the input.\n\
                                          -tx -ty -tz, -rx -ry -rz, -scale, -crop, -1stoct.", 0);
vul_arg<float>        bmsh3d_cmd_ptb ("-ptb", "Perturb by %% of avg. sampling dist.", 0.0f);
vul_arg<int>          bmsh3d_cmd_1stoct ("-1stoct", "Move data to 1st octant", 0);
vul_arg<int>          bmsh3d_cmd_crop ("-crop", "Crop by -minx -miny -minz -maxx -maxy -maxz", 0);
vul_arg<int>          bmsh3d_cmd_cclab ("-cclab", "Mesh connected component labelling", 0);
vul_arg<int>          bmsh3d_cmd_rmet ("-rmet", "Remove extraneous non-2-manifold faces (1,1,3), (1,3,3).", 0);
vul_arg<float>        bmsh3d_cmd_boxr ("-boxr", "Box ratio", 0.1f);
vul_arg<int>          bmsh3d_cmd_dup ("-dup", "Check/remove duplicate points", 0);
vul_arg<int>          bmsh3d_cmd_sub ("-sub", "Sub-sample the data", 0);
vul_arg<int>          bmsh3d_cmd_smooth ("-sm", "Apply smoothing", 0);
vul_arg<float>        bmsh3d_cmd_gsr ("-gsr", "Gaussian sigma ratio", 1.0f);
vul_arg<float>        bmsh3d_cmd_grr ("-grr", "Gaussian radius ratio", 1.0f);
vul_arg<float>        bmsh3d_cmd_thr ("-thr", "Threshold ratio", 5);

        //   --- General options ---
vul_arg<int>          bmsh3d_cmd_o ("-o", "Option", 1);
vul_arg<int>          bmsh3d_cmd_gui ("-gui", "Show GUI window (option)", 1);
vul_arg<int>          bmsh3d_cmd_verbose ("-vb", "Verbose output (0:off, 1:critial, 2:few, 3:normal, 4+:verb.)", 1);
vul_arg<int>          bmsh3d_cmd_valid ("-valid", "Do validation", 0);
vul_arg<int>          bmsh3d_cmd_mem ("-mem", "Print object size and memory usage", 0);
vul_arg<int>          bmsh3d_cmd_help ("-hh", "Detailed help.", -1);

        //   --- Bucketing options ---
vul_arg<int>          bmsh3d_cmd_bkt ("-bkt", "Bucketing the input points -f prefix", 0);
vul_arg<int>          bmsh3d_cmd_npbkt ("-npbkt", "Number of points in each bucket", 200000); //150000
vul_arg<float>        bmsh3d_cmd_bktbr ("-bktbr", "Bucketing extended box by ratio of max box size", 0.01f); //0.05

        //   --- Processing options ---
vul_arg<float>        bmsh3d_cmd_msr ("-msr", "Max sampling ratio", 10.0f);
vul_arg<int>          bmsh3d_cmd_smi ("-smi", "Smoothing iterations", -1);
vul_arg<int>          bmsh3d_cmd_rmip ("-rmip", "Remove isolated points", 0);
vul_arg<int>          bmsh3d_cmd_normal ("-normal", "Estimate point normal", 0);
vul_arg<int>          bmsh3d_cmd_meshtri ("-tri", "Triangulate input mesh", 0);
vul_arg<int>          bmsh3d_cmd_ssim ("-ssim", "3D scanning simulation", 0);
vul_arg<int>          bmsh3d_cmd_orient ("-orient", "Fix mesh manifold orientation", 0);
vul_arg<int>          bmsh3d_cmd_del ("-del", "Click to delete", 0);
vul_arg<int>          bmsh3d_cmd_test ("-test", "Run experimental test code", 0);

vul_arg<float>        bmsh3d_cmd_dx ("-dx", "Displacement in X", -1.0f); //-1.0f
vul_arg<float>        bmsh3d_cmd_dy ("-dy", "Displacement in Y", -1.0f);
vul_arg<float>        bmsh3d_cmd_dz ("-dz", "Displacement in Z", -1.0f);
vul_arg<float>        bmsh3d_cmd_tx ("-tx", "Translation in X", 0.0f);
vul_arg<float>        bmsh3d_cmd_ty ("-ty", "Translation in Y", 0.0f);
vul_arg<float>        bmsh3d_cmd_tz ("-tz", "Translation in Z", 0.0f);
vul_arg<float>        bmsh3d_cmd_rx ("-rx", "Rotation in X", 0.0f);
vul_arg<float>        bmsh3d_cmd_ry ("-ry", "Rotation in Y", 0.0f);
vul_arg<float>        bmsh3d_cmd_rz ("-rz", "Rotation in Z", 0.0f);
vul_arg<float>        bmsh3d_cmd_scale ("-scale", "Scaling factor", 1.0f);
vul_arg<float>        bmsh3d_cmd_minx ("-minx", "Min X", std::numeric_limits<float>::max());
vul_arg<float>        bmsh3d_cmd_miny ("-miny", "Min Y", std::numeric_limits<float>::max());
vul_arg<float>        bmsh3d_cmd_minz ("-minz", "Min Z", std::numeric_limits<float>::max());
vul_arg<float>        bmsh3d_cmd_maxx ("-maxx", "Max X", std::numeric_limits<float>::min());
vul_arg<float>        bmsh3d_cmd_maxy ("-maxy", "Max Y", std::numeric_limits<float>::min());
vul_arg<float>        bmsh3d_cmd_maxz ("-maxz", "Max Z", std::numeric_limits<float>::min());
vul_arg<int>          bmsh3d_cmd_n ("-n", "An integer number", -1);
vul_arg<int>          bmsh3d_cmd_n1 ("-n1", "n1", 1);
vul_arg<int>          bmsh3d_cmd_n2 ("-n2", "n2", 1);
vul_arg<int>          bmsh3d_cmd_n3 ("-n3", "n3", 1);
vul_arg<float>        bmsh3d_cmd_len ("-l", "A length", 1.0f);
vul_arg<float>        bmsh3d_cmd_len_tiny ("-tl", "A tiny length", 0.01f);
vul_arg<float>        bmsh3d_cmd_percent ("-pc", "Percentage", 0.0f);
vul_arg<float>        bmsh3d_cmd_percent2 ("-pc2", "Second percentage", 100.0f);

        //   --- Visualization options ---
vul_arg<int>          bmsh3d_cmd_v ("-v", "Visualization option 0:fastest - 2:slowest", 0);
vul_arg<int>          bmsh3d_cmd_bgcol ("-bg", "Background color, 0:black, 1:white", 1);
vul_arg<bool>         bmsh3d_cmd_shapehints ("-sh", "SoShapeHints in drawing a mesh", true);
vul_arg<float>        bmsh3d_cmd_mesh_transp ("-ts", "Surface mesh transparency", 0.0f);
vul_arg<int>          bmsh3d_cmd_light ("-light", "Lighting, 0:off, 1:RGB.", 0);
vul_arg<int>          bmsh3d_cmd_idv ("-idv", "Draw objects individually", 0);
vul_arg<int>          bmsh3d_cmd_draw_inf ("-inf", "Draw unbounded objects (approaching infinity)", 0);
vul_arg<int>          bmsh3d_cmd_showid ("-id", "Show object id, 1:v, 2:e, 3:f, 4:v+e, 5:all", 0);
vul_arg<float>        bmsh3d_cmd_psz ("-gsize", "Point (generator) drawing size", 1.0f);
vul_arg<float>        bmsh3d_cmd_cw ("-cw", "Curve drawing width", 1.0f);
vul_arg<float>        bmsh3d_cmd_r ("-r", "Vertex drawing radius", 0.03f);
vul_arg<float>        bmsh3d_cmd_cube_size ("-rc", "Cube drawing size", 0.01f);
vul_arg<int>          bmsh3d_cmd_colorcode ("-col", "Colorcode for drawing object", COLOR_SILVER);
vul_arg<int>          bmsh3d_cmd_colorcode2 ("-col2", "Colorcode 2 for drawing object", COLOR_GREEN);
vul_arg<int>          bmsh3d_cmd_colorcodebnd ("-colb", "Colorcode for boundary", COLOR_BLUE);

        //   --- Shortest path (graph) on mesh. ---
vul_arg<int>          bmsh3d_cmd_shp ("-shp", "Compute (graph) shortest path on mesh.", 0);

        //   --- Geodesic distance transform options ---
vul_arg<int>          bmsh3d_cmd_gdt ("-gdt", "Compute exact geodesic distance transform\n\
                                      1: Surazhsky & Kirsanov's interval based method,\n\
                                      2: Our face-based method, 3: Our face-based + shock propagation,\n\
                                      4: Our simutaneous discrete wavefront and shock propagation,\n\
                                      -f (prefix) -s (source index) -n (iter)", 0);
vul_arg<int>          bmsh3d_cmd_fmm ("-fmm", "FMM to compute geodesic distance transform\n\
                                      -s -e for source and link indices, -ns -ne for their numbers.", 0);
vul_arg<int>          bmsh3d_cmd_ns ("-ns", "Number of source vertices", 1);
vul_arg<int>          bmsh3d_cmd_ne ("-ne", "Number of ending vertices", 1);
vul_arg<int>          bmsh3d_cmd_s ("-s", "Source vertex id", -1);
vul_arg<int>          bmsh3d_cmd_e ("-e", "Ending vertex id", -1);
vul_arg<int>          bmsh3d_cmd_ntestquery ("-ti", "Number of test queries per interval", 5);
vul_arg<int>          bmsh3d_cmd_ntestcontour ("-tc", "Number of test iso-contour queires", 10);

        //   --- Generate data file ---
vul_arg<char const*>  bmsh3d_cmd_gen_pgutter ("-genpg", "Generate a parabolic gutter shape");
vul_arg<char const*>  bmsh3d_cmd_gen_box ("-genbox", "Generate a box shape");
vul_arg<char const*>  bmsh3d_cmd_gen_rand_box ("-grbox", "Generate random points -n in a box");

//#####################################################################

//: bmsh3d_cmdproc process parameters.
std::string            bmsh3d_app_window_title = "LEMS 3D Mesh App";

//: continuing the processing from the last step on existing data
bool                  bmsh3d_pro_continue = false;

//#####################################################################
