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

#ifndef VIDEOENCODER_H
#define VIDEOENCODER_H

#include "internaldefs.h"

#include <vector>
#include <deque>
#include <string>
#include <stdexcept>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/avutil.h"
#include "libavutil/imgutils.h"
#include "libavutil/pixfmt.h"
}

namespace EasyFFmpeg
{
class VideoEncoder
{
public:
    VideoEncoder( std::string fName, int width, int height, AVPixelFormat fmt = AV_PIX_FMT_RGB24 ) :
        m_formatCtx(nullptr), m_codecCtx(nullptr),    m_codec(nullptr), m_swsCtx(nullptr),
        m_fName(fName),       m_fmt(fmt),             m_width(width),   m_height (height)
    {

    }

    ~VideoEncoder()
    {
        av_frame_free(&m_frame);
        sws_freeContext(m_swsCtx);
        avcodec_free_context(&m_codecCtx);
        avformat_close_input(&m_formatCtx);
    }

private:

    // FFmpeg file format.
    AVFormatContext* m_formatCtx;

    // FFmpeg codec context.
    AVCodecContext* m_codecCtx;

    // FFmpeg codec for video.
    AVCodec* m_codec;

    // Image to return
    AVFrame * m_frame;

    // FFmpeg context convert image.
    struct SwsContext * m_swsCtx;

    // File Name with path
    std::string m_fName;

    // Video information
    VideoInfo m_info;

    // Target image size
    AVPixelFormat m_fmt;
    int m_width;
    int m_height;
    int m_nChannels;
};


}
#endif // VIDEOENCODER_H
