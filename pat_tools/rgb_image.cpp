
#include "pat_tools.h"
#include "pat_image.h"

void save_rgb_image(int resx, int resy, const char* data, FILE* f)
{
	pat_image img;
	img.initialize(resx, resy, 3);
	memcpy(img.data(), data, resx * resy * 3);
	img.write_dct16(f);
}

char* load_rgb_image(int& resx, int& resy, FILE* f)
{
	pat_image img;
	img.read_dct16(f);
	resx = img.resx();
	resy = img.resy();
	char* data = new char[resx * resy * 3];
	memcpy(data,img.data(), resx * resy * 3);
	return data;
}