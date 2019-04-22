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

#ifndef VIDEODECODER_H
#define VIDEODECODER_H

#include "internaldefs.h"

#include <vector>
#include <deque>
#include <string>
#include <memory>
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

class VideoDecoder
{
public:
    VideoDecoder( std::string fName, int width, int height, AVPixelFormat fmt = AV_PIX_FMT_RGB24 ) :
        m_fName(fName), m_fmt(fmt), m_width(width), m_height (height)
    {
        av_register_all();

        // get format from video file
        if (avformat_open_input(&m_formatCtx, m_fName.c_str(), nullptr, nullptr) != 0)
            throw std::runtime_error("EasyFFmpeg::VideoDecoder: Can't open input/allocate format context");
        if (avformat_find_stream_info(m_formatCtx, nullptr) < 0)
            throw std::runtime_error("EasyFFmpeg::VideoDecoder: Can't find stream info");

        // Find the index of the first video stream
        m_streamIndex = av_find_best_stream(m_formatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &m_codec, 0);
        if (m_streamIndex == AVERROR_STREAM_NOT_FOUND)
            throw std::runtime_error("EasyFFmpeg::VideoDecoder: Can't find video stream");

        AVStream* stream = m_formatCtx->streams[m_streamIndex];

        // find & open codec
        //m_codec = avcodec_find_decoder( stream->codecpar->codec_id );
        m_codecCtx = avcodec_alloc_context3( m_codec );
        if (avcodec_open2(m_codecCtx, m_codec, nullptr) < 0)
            throw std::runtime_error("EasyFFmpeg::VideoDecoder: Can't open codec/allocate codec context");
        // Transfer stream parameters to codec context
        if( avcodec_parameters_to_context(m_codecCtx, stream->codecpar) < 0 )
                throw std::runtime_error("EasyFFmpeg::VideoDecoder: Can't transfer params to codec context");

        // Set the duration in the info struct
        m_info.duration = static_cast<float>(m_formatCtx->duration)/1000000.0f;
        if( m_info.duration <= 0.001f )
            throw std::runtime_error("EasyFFmpeg::VideoDecoder: Video duration is smaller than 1ms");

        m_info.framerate = ( m_codecCtx->framerate.den ?
                                 (m_codecCtx->framerate.num/m_codecCtx->framerate.den) : 0 );
        m_info.width = m_codecCtx->width;
        m_info.height = m_codecCtx->height;
        m_info.pix_fmt = m_codecCtx->pix_fmt;

        // Init swscale context
        m_swsCtx = sws_getContext( m_codecCtx->width, m_codecCtx->height, m_codecCtx->pix_fmt,
                               m_width, m_height, m_fmt,
                               SWS_FAST_BILINEAR, nullptr, nullptr, nullptr );

        // prepare to read data
        AVPacket packet;
        av_init_packet(&packet);
        m_frame = av_frame_alloc();
        if (!m_frame)
           throw std::runtime_error("EasyFFmpeg::VideoDecoder: Can't allocate frame");

        m_nChannels = fmt2nChannels.find(m_fmt)->second;

        m_info.valid = true;
    }

    ~VideoDecoder()
    {
        av_frame_free(&m_frame);
        sws_freeContext(m_swsCtx);
        avcodec_free_context(&m_codecCtx);
        avformat_close_input(&m_formatCtx);
    }

    // Skip one frame
    void skipNextFrame()
    {
        while( !m_frameBuf.size() )
            m_decodeNextPacket();

        m_frameBuf.pop_front();
    }

    // Return next frame
    int getNextFrame( std::vector<uint8_t>& img )
    {
        int c = 0;
        while( !m_frameBuf.size() )
        {
            m_decodeNextPacket();
            if(++c > 10)
                return -1;
        }

        if( !m_frameBuf.size() )
            std::fill(img.begin(), img.end(), 0);
        else
        {
            img = std::move( m_frameBuf[0] );
            m_frameBuf.pop_front();
        }

        return 0;
    }

    // Return vide info
    VideoInfo getVideoInfo()
    {
        return m_info;
    }

private:

    void m_decodeNextPacket()
    {
        AVPacket packet;
        av_init_packet(&packet);
        while (av_read_frame(m_formatCtx, &packet) >= 0)
        {
            // decode one frame
            int ret = avcodec_send_packet(m_codecCtx, &packet);
            if(ret < 0)
                break;
            if(packet.stream_index != m_streamIndex)
            {
                av_packet_unref(&packet);
                continue;
            }

            while( ret >= 0)
            {
                ret = avcodec_receive_frame(m_codecCtx, m_frame);
                if( (ret == AVERROR(EAGAIN)) && (ret != AVERROR_EOF) )
                    break;

                // Scale frames
                uint8_t * dst_data[4];
                int dst_linesize[4];
                av_image_alloc(dst_data, dst_linesize, m_width, m_height, m_fmt, align);
                sws_scale(m_swsCtx, m_frame->data, m_frame->linesize,
                          0, m_frame->height, dst_data, dst_linesize);

                /*std::vector<uint8_t> img( dst_data[0], dst_data[0]+static_cast<size_t>(dst_linesize[0] * m_height) );
                uint8_t * data = img.data();
                for (int i = 0; i < m_height; ++i)
                    for(int j = 0; j < dst_linesize[0]; ++j)
                        data[(i*dst_linesize[0]) + j] = dst_data[0][(i*dst_linesize[0]) + j];
                */
                m_frameBuf.emplace_back(std::vector<uint8_t>(dst_data[0],
                                        dst_data[0]+static_cast<size_t>(dst_linesize[0] * m_height)));

                av_freep(&dst_data[0]);
                av_packet_unref(&packet);
                av_frame_unref(m_frame);
            }
            if( (ret < 0) && (ret != AVERROR(EAGAIN)) && (ret != AVERROR_EOF) )
                break;
        }
        return;
    }

    // FFmpeg file format.
    AVFormatContext* m_formatCtx = nullptr;

    // FFmpeg codec context.
    AVCodecContext* m_codecCtx = nullptr;

    // FFmpeg codec for video.
    AVCodec* m_codec = nullptr;

    // Image to return
    AVFrame * m_frame = nullptr;

    // FFmpeg context convert image.
    struct SwsContext * m_swsCtx = nullptr;

    int m_streamIndex = -1;

    // File Name with path
    std::string m_fName;

    // Video information
    VideoInfo m_info;

    // Target image size
    AVPixelFormat m_fmt;
    int m_width;
    int m_height;
    int m_nChannels;

    // Buffer for frames
    std::deque<std::vector<uint8_t>> m_frameBuf;
};
}
#endif // VIDEODECODER_H
