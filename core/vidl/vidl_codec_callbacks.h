#ifndef vidl_codec_callbacks_h_
#define vidl_codec_callbacks_h_

/*
 *   7-28-02
 *   l.e.galup
 *   these callbacks gather user info for a player.
 *   these should be defined by the impl of the player itself.
 *   that way, we don't have to introduce a vgui dependence
 *   to this library.
 *
 */

class vidl_codec;

// implemented in player impl
void (* load_mpegcodec_callback)( vidl_codec*);



#endif //vidl_codec_callbacks_h_
