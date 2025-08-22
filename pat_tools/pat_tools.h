#pragma once

#include <stdio.h>


extern "C"
{
    void save_rgb_image(int resx, int resy, const char* data, FILE* f);
    char* load_rgb_image(int & resx, int & resy, FILE* f);
}