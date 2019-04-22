#include "internaldefs.h"
#include "videodecoder.h"

#include <iostream>
#include <cstdio>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main()
{
    std::cout << "Hello World!" << std::endl;
    av_register_all();
    std::string fName = "/home/mahesh/workspace/Data/vids/vid.mp4";
    EasyFFmpeg::VideoDecoder * decoder = new EasyFFmpeg::VideoDecoder(fName, 640, 480);
    //EasyFFmpeg::VideoInfo info = decoder->getVideoInfo();
    std::vector<uint8_t> frame_vec;

    int ret = decoder->getNextFrame( frame_vec );
    int i = 0;
    while( (ret == 0) && (i < 150) )
    {
        char fName[] = "/home/mahesh/workspace/Data/frame_00000000.png";
        std::sprintf(fName, "/home/mahesh/workspace/Data/frame_%08d.png", i);
        stbi_write_png(fName, 640, 480, 3, frame_vec.data(), 640*3);
        ret = decoder->getNextFrame( frame_vec );
        i++;
    }

    delete decoder;
    return 0;
}
