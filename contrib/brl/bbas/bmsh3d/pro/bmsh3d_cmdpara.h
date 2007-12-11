// This is brl/bbas/bmsh3d/pro/bmsh3d_cmdpara.h
#ifndef _bmsh3d_cmdpara_h_
#define _bmsh3d_cmdpara_h_
//:
// \file
// \author Ming-Ching Chang
// \date Feb 12, 2007

#include <vul/vul_arg.h>

//: Result of application returned to the OS.
#define PRO_RESULT_SUCCESS          0
#define PRO_HAS_TASK_TO_RUN         0
#define PRO_RESULT_NO_PROCESS       1
#define PRO_RESULT_NO_FILE          2
#define PRO_RESULT_FAIL             3

//#####################################################################

//: bmsh3d_cmdproc command-line arguments.

//Mesh File I/O
extern vul_arg<char const*>   bmsh3d_cmd_p3d_file;
extern vul_arg<char const*>   bmsh3d_cmd_g3d_file;
extern vul_arg<char const*>   bmsh3d_cmd_xyz_file;
extern vul_arg<char const*>   bmsh3d_cmd_xyzn1_file;
extern vul_arg<char const*>   bmsh3d_cmd_xyznw_file;
extern vul_arg<char const*>   bmsh3d_cmd_ply_file;
extern vul_arg<char const*>   bmsh3d_cmd_ply2_file;
extern vul_arg<char const*>   bmsh3d_cmd_off_file;
extern vul_arg<char const*>   bmsh3d_cmd_obj_file;
extern vul_arg<char const*>   bmsh3d_cmd_m_file;
extern vul_arg<char const*>   bmsh3d_cmd_3pi_file;
extern vul_arg<char const*>   bmsh3d_cmd_iv_file;
extern vul_arg<char const*>   bmsh3d_cmd_wrl_file;

extern vul_arg<char const*>   bmsh3d_cmd_p3d_file2;
extern vul_arg<char const*>   bmsh3d_cmd_xyz_file2;
extern vul_arg<char const*>   bmsh3d_cmd_xyzn1_file2;
extern vul_arg<char const*>   bmsh3d_cmd_3pi_file2;
extern vul_arg<char const*>   bmsh3d_cmd_ply_file2;
extern vul_arg<char const*>   bmsh3d_cmd_ply2_file2;
extern vul_arg<char const*>   bmsh3d_cmd_iv_file2;
extern vul_arg<char const*>   bmsh3d_cmd_iv_file3;

//IV/VRML parsing
extern vul_arg<char const*>   bmsh3d_cmd_iv_pfile;
extern vul_arg<char const*>   bmsh3d_cmd_wrl_pfile;

extern vul_arg<char const*>   bmsh3d_cmd_fileprefix;
extern vul_arg<char const*>   bmsh3d_cmd_fileprefix1;
extern vul_arg<char const*>   bmsh3d_cmd_fileprefix2;
extern vul_arg<char const*>   bmsh3d_cmd_compare_pts;
extern vul_arg<char const*>   bmsh3d_cmd_align_file;
extern vul_arg<char const*>   bmsh3d_cmd_fix_af;
extern vul_arg<char const*>   bmsh3d_cmd_align_listfile;

//Output files
extern vul_arg<int>           bmsh3d_cmd_ofile;
extern vul_arg<int>           bmsh3d_cmd_fileo;
extern vul_arg<char const*>   bmsh3d_cmd_p3d_ofile;
extern vul_arg<char const*>   bmsh3d_cmd_g3d_ofile;
extern vul_arg<char const*>   bmsh3d_cmd_xyz_ofile;
extern vul_arg<char const*>   bmsh3d_cmd_xyzn1_ofile;
extern vul_arg<char const*>   bmsh3d_cmd_xyznw_ofile;
extern vul_arg<char const*>   bmsh3d_cmd_ply_ofile;
extern vul_arg<char const*>   bmsh3d_cmd_ply_ofileb;
extern vul_arg<char const*>   bmsh3d_cmd_ply2_ofile;
extern vul_arg<char const*>   bmsh3d_cmd_off_ofile;
extern vul_arg<char const*>   bmsh3d_cmd_obj_ofile;
extern vul_arg<char const*>   bmsh3d_cmd_3pi_ofile;
extern vul_arg<char const*>   bmsh3d_cmd_iv_ofile;
extern vul_arg<char const*>   bmsh3d_cmd_iv_ofilebin;
extern vul_arg<char const*>   bmsh3d_cmd_wrl_ofile;
extern vul_arg<char const*>   bmsh3d_cmd_wrl_ofile2;
extern vul_arg<char const*>   bmsh3d_cmd_align_ofile;

//Point/Mesh Processing
extern vul_arg<int>           bmsh3d_cmd_preproc;
extern vul_arg<float>         bmsh3d_cmd_ptb;
extern vul_arg<int>           bmsh3d_cmd_1stoct;
extern vul_arg<int>           bmsh3d_cmd_crop;
extern vul_arg<int>           bmsh3d_cmd_cclab;
extern vul_arg<int>           bmsh3d_cmd_rmet;
extern vul_arg<float>         bmsh3d_cmd_boxr;
extern vul_arg<int>           bmsh3d_cmd_dup;
extern vul_arg<int>           bmsh3d_cmd_sub;
extern vul_arg<int>           bmsh3d_cmd_smooth;
extern vul_arg<float>         bmsh3d_cmd_gsr;
extern vul_arg<float>         bmsh3d_cmd_grr;
extern vul_arg<float>         bmsh3d_cmd_thr;

//General options
extern vul_arg<int>           bmsh3d_cmd_o;
extern vul_arg<int>           bmsh3d_cmd_gui;
extern vul_arg<int>           bmsh3d_cmd_verbose;
extern vul_arg<int>           bmsh3d_cmd_valid;
extern vul_arg<int>           bmsh3d_cmd_mem;
extern vul_arg<int>           bmsh3d_cmd_help;

//Processing options
extern vul_arg<int>           bmsh3d_cmd_bkt;
extern vul_arg<int>           bmsh3d_cmd_npbkt;
extern vul_arg<float>         bmsh3d_cmd_bktbr;

extern vul_arg<float>         bmsh3d_cmd_msr;
extern vul_arg<int>           bmsh3d_cmd_smi;
extern vul_arg<int>           bmsh3d_cmd_rmip;
extern vul_arg<int>           bmsh3d_cmd_normal;
extern vul_arg<int>           bmsh3d_cmd_meshtri;
extern vul_arg<int>           bmsh3d_cmd_ssim;
extern vul_arg<int>           bmsh3d_cmd_orient;
extern vul_arg<int>           bmsh3d_cmd_del;
extern vul_arg<int>           bmsh3d_cmd_test;

extern vul_arg<float>         bmsh3d_cmd_dx;
extern vul_arg<float>         bmsh3d_cmd_dy;
extern vul_arg<float>         bmsh3d_cmd_dz;
extern vul_arg<float>         bmsh3d_cmd_tx;
extern vul_arg<float>         bmsh3d_cmd_ty;
extern vul_arg<float>         bmsh3d_cmd_tz;
extern vul_arg<float>         bmsh3d_cmd_rx;
extern vul_arg<float>         bmsh3d_cmd_ry;
extern vul_arg<float>         bmsh3d_cmd_rz;
extern vul_arg<float>         bmsh3d_cmd_scale;
extern vul_arg<float>         bmsh3d_cmd_minx;
extern vul_arg<float>         bmsh3d_cmd_miny;
extern vul_arg<float>         bmsh3d_cmd_minz;
extern vul_arg<float>         bmsh3d_cmd_maxx;
extern vul_arg<float>         bmsh3d_cmd_maxy;
extern vul_arg<float>         bmsh3d_cmd_maxz;
extern vul_arg<int>           bmsh3d_cmd_n;
extern vul_arg<int>           bmsh3d_cmd_n1;
extern vul_arg<int>           bmsh3d_cmd_n2;
extern vul_arg<int>           bmsh3d_cmd_n3;
extern vul_arg<float>         bmsh3d_cmd_len;
extern vul_arg<float>         bmsh3d_cmd_len_tiny;
extern vul_arg<float>         bmsh3d_cmd_percent;
extern vul_arg<float>         bmsh3d_cmd_percent2;

//Visualization options
extern vul_arg<int>           bmsh3d_cmd_v;
extern vul_arg<int>           bmsh3d_cmd_bgcol;
extern vul_arg<bool>          bmsh3d_cmd_shapehints;
extern vul_arg<float>         bmsh3d_cmd_mesh_transp;
extern vul_arg<int>           bmsh3d_cmd_light;
extern vul_arg<int>           bmsh3d_cmd_idv;
extern vul_arg<int>           bmsh3d_cmd_draw_inf;
extern vul_arg<int>           bmsh3d_cmd_showid;
extern vul_arg<float>         bmsh3d_cmd_psz;
extern vul_arg<float>         bmsh3d_cmd_cw;
extern vul_arg<float>         bmsh3d_cmd_r;
extern vul_arg<float>         bmsh3d_cmd_cube_size;
extern vul_arg<int>           bmsh3d_cmd_colorcode;
extern vul_arg<int>           bmsh3d_cmd_colorcode2;
extern vul_arg<int>           bmsh3d_cmd_colorcodebnd;

//Shortest path (graph) on mesh.
extern vul_arg<int>           bmsh3d_cmd_shp;

//Geodesic distance transform options
extern vul_arg<int>           bmsh3d_cmd_gdt;
extern vul_arg<int>           bmsh3d_cmd_fmm;
extern vul_arg<int>           bmsh3d_cmd_ns;
extern vul_arg<int>           bmsh3d_cmd_ne;
extern vul_arg<int>           bmsh3d_cmd_s;
extern vul_arg<int>           bmsh3d_cmd_e;
extern vul_arg<int>           bmsh3d_cmd_ntestquery;
extern vul_arg<int>           bmsh3d_cmd_ntestcontour;

//Generate data file
extern vul_arg<char const*>   bmsh3d_cmd_gen_pgutter;
extern vul_arg<char const*>   bmsh3d_cmd_gen_box;
extern vul_arg<char const*>   bmsh3d_cmd_gen_rand_box;


//#####################################################################

//: bmsh3d_cmdproc process parameters.
extern vcl_string             bmsh3d_app_window_title;
extern bool                   bmsh3d_pro_contiune;

#endif // _bmsh3d_cmdpara_h_
