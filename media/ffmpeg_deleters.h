#ifndef MEDIA_FFMPEG_DELETERS_H_
#define MEDIA_FFMPEG_DELETERS_H_
namespace media {

struct ScopedPtrAVFree {
  void operator()(void* x) const;
};

struct ScopedPtrAVFreePacket {
  void operator()(void* x) const;
};

struct ScopedPtrAVFreeContext {
  void operator()(void* x) const;
};

struct ScopedPtrAVFreeFrame {
  void operator()(void* x) const;
};


} // namespace media
#endif // MEDIA_FFMPEG_DELETERS_H_
