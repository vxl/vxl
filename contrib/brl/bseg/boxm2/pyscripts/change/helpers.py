import brl_init
import vpgl_adaptor_boxm2_batch as vpgl
import vil_adaptor_boxm2_batch as vil
import bbas_adaptor_boxm2_batch as bbas
import boxm2_adaptor
import os
import shutil
from os.path import basename, splitext
from glob import glob

# renders a collection of images/cameras into outdir


def render_changes(scene, img_glob, cam_glob, outdir, n=1, raybelief="", max_mode=False):

    # make sure imgglob and camglob are string lists and same size
    if isinstance(img_glob, str) and isinstance(cam_glob, str):
        img_glob = [img_glob]
        cam_glob = [cam_glob]
    assert len(img_glob) == len(cam_glob)

    # make sure outdir exists
    if not os.path.exists(outdir):
        os.makedirs(outdir)

    print "Rendering images: ", img_glob, " into ", outdir

    # load change for each image
    for idx, img in enumerate(img_glob):

        # grab image number of ground truth image
        imgnum, ext = os.path.splitext(basename(img))
        pcam = vpgl.load_perspective_camera(cam_glob[idx])
        rimg, ni, nj = vil.load_image(img)

        # render exp
        expimg = scene.render(pcam, ni, nj)

        # render change detection
        cd_fname = outdir + "/cd_" + imgnum + ".tiff"
        cd_img = scene.change_detect(
            pcam, rimg, expimg, n, raybelief, max_mode)
        vil.save_image(cd_img, cd_fname)

        # clean up
        bbas.remove_from_db([rimg, expimg, cd_img])


# renders a collection (or single) of thresholded images
def render_vis_changes(change_glob, img_glob, thresh, outdir):

    # make sure imgglob and camglob are string lists and same size
    if isinstance(change_glob, str) and isinstance(img_glob, str):
        img_glob = [img_glob]
        change_glob = [change_glob]
    assert len(img_glob) == len(change_glob)

    # make sure outdir exists
    if not os.path.exists(outdir):
        os.makedirs(outdir)

    print "Rendering vis change imgs: ", img_glob, " into ", outdir

    # load change for each image
    for idx, img in enumerate(img_glob):

        # grab image number of ground truth image
        imgnum, ext = os.path.splitext(basename(img))
        cimg, ni, nj = vil.load_image(change_glob[idx])
        rimg, ni, nj = vil.load_image(img)

        # render visualize image
        vis_img = boxm2_adaptor.visualize_change(cimg, rimg, thresh)
        vis_name = outdir + "/thresh_" + str(thresh) + "_" + imgnum + ".png"
        vil.save_image(vis_img, vis_name)


# helper function grabs the *mog* .bin files and hides them in a
# separate directory.  return moves em back in
def move_appearance(scene_dir):
    # make temp dir
    if os.path.exists(scene_dir + "/tmp"):
        shutil.rmtree(scene_dir + "/tmp")
    os.mkdir(scene_dir + "/tmp")

    # move mog and numobs
    mogs = glob(scene_dir + "/*mog*")
    nobs = glob(scene_dir + "/*num*")
    toMove = mogs + nobs
    for f in toMove:
        shutil.move(f, scene_dir + "/tmp/")


def return_appearance(scene_dir):
    # move mog and numobs back
    mogs = glob(scene_dir + "/tmp/*mog*")
    nobs = glob(scene_dir + "/tmp/*num*")
    toMove = mogs + nobs
    for f in toMove:
        shutil.move(f, scene_dir)
    # remove tempdir
    os.rmdir(scene_dir + "/tmp/")
