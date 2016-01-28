#ifndef bhdfs_processes_h_
#define bhdfs_processes_h_

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_macros.h>

// the init functions
DECLARE_FUNC_CONS(bhdfs_create_fs_manager_process);
DECLARE_FUNC_CONS(bhdfs_fs_create_dir_process);
DECLARE_FUNC_CONS(bhdfs_load_image_view_process);
DECLARE_FUNC_CONS(bhdfs_save_image_view_process);
DECLARE_FUNC_CONS(bhdfs_fs_copy_file_process);
DECLARE_FUNC_CONS(bhdfs_fs_get_working_dir_process);
DECLARE_FUNC_CONS(bhdfs_generate_stdin_file_process);
DECLARE_FUNC_CONS(bhdfs_generate_file_process);
DECLARE_FUNC_CONS(bhdfs_fs_copy_files_to_local_process);
DECLARE_FUNC_CONS(bhdfs_boxm2_load_scene_process);
DECLARE_FUNC_CONS(bhdfs_load_perspective_camera_process);
DECLARE_FUNC_CONS(bhdfs_load_txt_file_process);
DECLARE_FUNC_CONS(bhdfs_save_txt_file_process);
#endif
