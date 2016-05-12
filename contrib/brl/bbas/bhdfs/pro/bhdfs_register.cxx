#include "bhdfs_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include <bhdfs/bhdfs_manager.h>

#include "bhdfs_processes.h"

void bhdfs_register::register_datatype()
{
  REGISTER_DATATYPE( bhdfs_manager_sptr );
}

void bhdfs_register::register_process()
{
  std::cout << "registering bhdfs!\n";
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bhdfs_create_fs_manager_process, "bhdfsCreateFSManagerProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bhdfs_fs_create_dir_process, "bhdfsCreateDirProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bhdfs_load_image_view_process, "bhdfsLoadImageViewProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bhdfs_save_image_view_process, "bhdfsSaveImageViewProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bhdfs_fs_copy_file_process, "bhdfsCopyFileProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bhdfs_fs_get_working_dir_process, "bhdfsGetWorkingDirProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bhdfs_generate_stdin_file_process, "bhdfsGenerateStdInFileProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bhdfs_generate_file_process, "bhdfsGenerateFileProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bhdfs_fs_copy_files_to_local_process, "bhdfsCopyFilesToLocalProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bhdfs_boxm2_load_scene_process, "bhdfsBOXM2LoadSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bhdfs_load_perspective_camera_process, "bhdfsLoadPerspectiveCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bhdfs_load_txt_file_process, "bhdfsLoadTxtFileProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bhdfs_save_txt_file_process, "bhdfsSaveTxtFileProcess");
}
