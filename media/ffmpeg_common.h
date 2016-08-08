#ifndef MEDIA_FFMPEG_COMMON_H_
#define MEDIA_FFMPEG_COMMON_H_
#include <stdint.h>
#include <cerrno>
#include "media/ffmpeg_deleters.h"

extern "C" {

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/internal.h>
#include <libavformat/avio.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/log.h>
#include <libavutil/mathematics.h>
#include <libavutil/opt.h>

} // extern "C"

namespace media {

inline void ScopedPtrAVFree::operator()(void* x) const {
  av_free(x);
}

inline void ScopedPtrAVFreePacket::operator()(void* x) const {
  AVPacket* packet = static_cast<AVPacket*>(x);
  av_packet_unref(packet);
  delete packet;
}

inline void ScopedPtrAVFreeContext::operator()(void* x) const {
  AVCodecContext* codec_context = static_cast<AVCodecContext*>(x);
  avcodec_free_context(&codec_context);
}

inline void ScopedPtrAVFreeFrame::operator()(void* x) const {
  AVFrame* frame = static_cast<AVFrame*>(x);
  av_frame_free(&frame);
}

} // namespace media
#endif // MEDIA_FFMPEG_COMMON_H_
