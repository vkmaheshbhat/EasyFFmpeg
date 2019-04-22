/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * EasyFFmpeg                                                                                  *
 *                                                                                             *
 * Copyright (C) 2019 Mahesh Venkata Krishna (mahesh@maheshvk.com)                             *
 *                                                                                             *
 * This program is free software: you can redistribute it and/or modify it under the terms     *
 * of the GNU General Public License as published by the Free Software Foundation, either      *
 * version 3 of the License, or (at your option) any later version.                            *       
 *                                                                                             *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;   *
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   *
 * See the GNU General Public License for more details.                                        *
 *                                                                                             *
 * You should have received a copy of the GNU General Public License along with this program.  *
 * If not, see <https://www.gnu.org/licenses/>.                                                *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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
