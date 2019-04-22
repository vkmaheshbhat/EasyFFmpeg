#ifndef INTERNALDEFS_H
#define INTERNALDEFS_H
#include <cinttypes>
#include <cstdlib>
#include <unordered_map>

extern "C"
{
#include "libavformat/avformat.h"
}

// A small float value for comaprison
#define EPSILON 0.00001f
// A large value implying some sort of infinity
#define LARGE 10000

namespace EasyFFmpeg
{
// Holder for information of video stream
struct VideoInfo
{
    bool valid = false;
    int height = 0;
    int width = 0;
    float framerate = 0.0f;
    float duration = 0.0f;
    AVPixelFormat pix_fmt = AV_PIX_FMT_RGB24;
};

// map from AVPixelFormat to number of channels. Not all formats are supported. So need to see how to map.
const std::unordered_map<AVPixelFormat, int> fmt2nChannels = {
    {AV_PIX_FMT_GRAY8, 1},
    {AV_PIX_FMT_YA8, 2},
    {AV_PIX_FMT_RGB24, 3},
    {AV_PIX_FMT_BGR24, 3},
    {AV_PIX_FMT_RGBA, 4},
    {AV_PIX_FMT_ARGB, 4},
    {AV_PIX_FMT_BGRA, 4},
    {AV_PIX_FMT_ABGR, 4}
};

const int align = 1;
// Structure to hold some info of audio stream
struct AudioInfo
{
    bool valid;
    uint64_t duration;
};
}
#endif // INTERNALDEFS_H
