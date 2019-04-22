#ifndef AUDIODECODER_H
#define AUDIODECODER_H

#include "internaldefs.h"

#include <vector>
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
class AudioDecoder
{
public:
    AudioDecoder() {}
    ~AudioDecoder() {}

    // Get Audio sequence
    int vGetNextAudioSequence( AVFrame * frame )
    {
        int res = 0;
        if ( ( m_audioStreamIndex != -1 ) && m_isOpen )
        {
            AVFrame *pCurrFrame = av_frame_alloc();
            res = avcodec_receive_frame( m_pAudioCodecCtx, pCurrFrame );
            if( res != 0)
            {
                AVPacket packet;
                av_init_packet( &packet );
                packet.data = nullptrptr;
                packet.size = 0;

                // Read packet.
                av_read_frame( m_pFormatCtx, &packet );
                if( packet.stream_index == m_audioStreamIndex )
                {
                    avcodec_send_packet( m_pAudioCodecCtx, &packet );
                    res = avcodec_receive_frame( m_pAudioCodecCtx, pCurrFrame );
                    m_frameTS = pCurrFrame->pts;
                }
                av_packet_unref( &packet );
            }
            if( res == 0 )
                p_vGetU8PFrame( pCurrFrame, frame );

            av_frame_free( &pCurrFrame );
        } // End if m_videoStreamIndex

        return res;
    }

    // Get the Audio Info
    AudioInfo stGetAudioInfo()
    {
        return m_aInfo;
    }

private:

    // open audio stream.
    bool p_boOpenAudio()
    {
        bool res = false;

        if( m_pFormatCtx )
        {
            m_audioStreamIndex = -1;

            for( unsigned int i = 0; i < m_pFormatCtx->nb_streams; i++ )
            {
                if( m_pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO )
                {
                    m_audioStreamIndex = static_cast<int>(i);
                    m_pAudioCodec = avcodec_find_decoder( m_pFormatCtx->streams[i]->codecpar->codec_id );
                    m_pAudioCodecCtx = avcodec_alloc_context3( m_pAudioCodec );

                    if( m_pAudioCodec )
                    {
                        res = !( avcodec_open2( m_pAudioCodecCtx, m_pAudioCodec, nullptr ) < 0 );
                    }
                    break;
                }
            }

            if( !res )
            {
                p_vCloseAudio();
            }
            else
            {
                m_audioBaseTime = av_q2d( m_pFormatCtx->streams[m_audioStreamIndex]->time_base );

                m_aInfo.valid = true;
                m_aInfo.duration = static_cast<uint64_t>( m_audioBaseTime * m_pFormatCtx->duration);

                m_pAudioConvertCtx = swr_alloc_set_opts( nullptr, AV_CH_LAYOUT_MONO, AV_SAMPLE_FMT_U8P,
                                                         m_pAudioCodecCtx->sample_rate,
                                                         static_cast<int64_t>(m_pAudioCodecCtx->channel_layout),
                                                         m_pAudioCodecCtx->sample_fmt, m_pAudioCodecCtx->sample_rate,
                                                         0, nullptr );
                swr_init( m_pAudioConvertCtx );
            }
        }

        return res;
    }

    // Convert Audio frame to U8P format
    void p_vGetU8PFrame( const AVFrame * frame, AVFrame * u8pFrame )
    {
        if( u8pFrame )
        {
            swr_convert_frame( m_pAudioConvertCtx, u8pFrame, frame );
        }
    }

    // close audio stream.
    void p_vCloseAudio()
    {
        if( m_pAudioCodecCtx )
        {
            avcodec_close( m_pAudioCodecCtx );
            m_pAudioCodecCtx   = nullptr;
            m_pAudioCodec      = nullptr;
            m_audioStreamIndex = 0;
        }

        if( m_pAudioConvertCtx )
        {
            swr_close( m_pAudioConvertCtx );
            swr_free( &m_pAudioConvertCtx );
        }

    }

    // FFmpeg file format.
    AVFormatContext* m_pFormatCtx;

    // FFmpeg codec context for audio.
    AVCodecContext* m_pAudioCodecCtx;

    // FFmpeg codec for audio.
    AVCodec* m_pAudioCodec;

    // Audio stream number in file.
    int m_audioStreamIndex;

    // FFmpeg timebase for audio.
    double m_audioBaseTime;

    // Context for Audio conversion
    SwrContext * m_pAudioConvertCtx;

    // Aggregated Audio vector
    std::vector<uint8_t> m_audioSamples;

    // File is open or not.
    bool m_isOpen;

    // Audio Information
    AudioInfo m_aInfo;

    // File Name with path
    std::string m_fName;
};
}
#endif // AUDIODECODER_H
