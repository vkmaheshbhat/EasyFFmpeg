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
    std::string fName = "Data/vids/vid.mp4";
    EasyFFmpeg::VideoDecoder * decoder = new EasyFFmpeg::VideoDecoder(fName, 640, 480);
    //EasyFFmpeg::VideoInfo info = decoder->getVideoInfo();
    std::vector<uint8_t> frame_vec;

    int ret = decoder->getNextFrame( frame_vec );
    int i = 0;
    while( (ret == 0) && (i < 150) )
    {
        char fName[] = "Data/frame_00000000.png";
        std::sprintf(fName, "Data/frame_%08d.png", i);
        stbi_write_png(fName, 640, 480, 3, frame_vec.data(), 640*3);
        ret = decoder->getNextFrame( frame_vec );
        i++;
    }

    delete decoder;
    return 0;
}
