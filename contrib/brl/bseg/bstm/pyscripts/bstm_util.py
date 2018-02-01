import brl_init
import bstm_batch as batch
dbvalue = brl_init.register_batch(batch)

import vil_adaptor_bstm_batch as vil
import vpgl_adaptor_bstm_batch as vpgl

from glob import glob


def read_proj_cams_from_folder(folder, ni, nj):
    cam_names = glob(folder + "*.txt")
    cam_names.sort()
    cams = []
    for cam_name in cam_names:
        print "Processing " + cam_name
        proj_cam = vpgl.load_projective_camera(cam_name)
        cams.append(vpgl.convert_to_generic_camera(proj_cam, ni, nj, 0))
    return cams


def read_imgs_from_folder(folder):
    img_names = glob(img_folder + "*.png")
    img_names.sort()
    imgs = []
    for img_name in img_names:
        [curr_img, ni, nj] = vil.load_image(img_name)
        imgs.append(curr_img)
    return imgs


def read_img_cam_silh(img_folder, cam_folder, silh_folder, cam_id, frame_id):
    curr_img_name = img_folder + "cam_%05d_frame_%05d.png" % (cam_id, frame_id)
    curr_silh_name = silh_folder + \
        "cam_%05d_frame_%05d.png" % (cam_id, frame_id)
    curr_cam_name = cam_folder + "cam_%05d.txt" % cam_id

    [curr_img, ni, nj] = vil.load_image(curr_img_name)
    [silhouette, ni, nj] = vil.load_image(curr_silh_name)
    t_silh = vil.threshold_image(silhouette, 1)

    proj_cam = vpgl.load_projective_camera(curr_cam_name)
    curr_cam = vpgl.convert_to_generic_camera(proj_cam, ni, nj, 0)

    batch.remove_data(silhouette)
    batch.remove_data(proj_cam)

    return [curr_img, curr_cam, t_silh]


def read_img_cam_silh_persp(img_folder, cam_folder, silh_folder, cam_id, frame_id):
    curr_img_name = img_folder + "cam_%05d_frame_%05d.bmp" % (cam_id, frame_id)
    curr_silh_name = silh_folder + \
        "cam_%05d_frame_%05d.png" % (cam_id, frame_id)
    curr_cam_name = cam_folder + "cam_%05d.txt" % cam_id

    [curr_img, ni, nj] = vil.load_image(curr_img_name)
    [silhouette, ni, nj] = vil.load_image(curr_silh_name)
    t_silh = vil.threshold_image(silhouette, 1)

    curr_cam = vpgl.load_perspective_camera(curr_cam_name)

    batch.remove_data(silhouette)

    return [curr_img, curr_cam, t_silh]
