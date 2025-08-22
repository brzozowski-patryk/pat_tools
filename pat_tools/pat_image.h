
#pragma once

#include "../memory_manager/dynamic_memory.h"
#include <Windows.h>
#include <string>

class pat_image_viewer;

static __int16 mask_y[64] = {
						2,2,3,4,5,5,5,5,
						2,3,4,5,5,5,5,4,
						3,4,5,5,5,5,4,4,
						4,5,5,5,5,4,4,3,
						5,5,5,5,4,4,3,3,
						5,5,5,4,4,3,3,2,
						5,5,4,4,3,3,2,2,
						5,4,4,3,3,2,2,1,
};

static __int16 mask_uv[64] = {
						24,19,19,18,18,17,17,16,
						19,19,18,18,17,17,16,16,
						19,18,18,17,17,16,16,15,
						18,18,17,17,16,16,15,15,
						18,17,17,16,16,15,15,14,
						17,17,16,16,15,15,14,14,
						17,16,16,15,15,14,14,13,
						16,16,15,15,14,14,13,13,
};

static __int16 mask_y_wavelet[64] = {
						10,10,40,40,40,40,40,90,
						10,40,40,40,40,40,90,90,
						40,40,140,140,140,190,190,190,
						40,40,140,140,190,190,190,190,
						40,40,140,190,190,190,190,190,
						40,40,190,190,190,190,190,190,
						40,90,190,190,190,190,190,190,
						90,90,190,190,190,190,190,190,
};

static __int16 mask_uv_wavelet[64] = {
						200,300,300,400,500,600,700,900,
						300,300,400,500,600,700,900,900,
						300,400,500,600,700,900,900,900,
						400,500,600,700,900,900,900,900,
						500,600,700,900,900,900,900,900,
						600,700,900,900,900,900,900,900,
						700,900,900,900,900,900,900,900,
						900,900,900,900,900,900,900,900,
};

class pat_image
{
public:
	pat_image();
	~pat_image();
	pat_image(const pat_image& obj);
	pat_image& operator= (const pat_image& obj);
	void initialize(int rx, int ry, int rz);
	void release();
	void load_from_file(const char* path);
	void capture_screen();
	void capture_HWND(HWND okno);
	void capture_hdc(HDC hdc, int rx, int ry);
public:
	void sample(int x, int y, char* out, bool warp);
	void sample_nearest(float x, float y, char* out, bool warp);
public:
	int resx() { return m_resx; }
	int resy() { return m_resy; }
	int resz() { return m_resz; }
	unsigned char* data() { return (unsigned char*)m_data.get_buffer(); }

	unsigned char* pixel(int i) { return data() + (i * m_resz); }
	unsigned char* pixel(int x, int y) { return data() + ((m_resx * y + x) * m_resz); }

public:
	void write_pat_image(const char* path, int method);
	void read_pat_image(const char* path);

	void write_raw(FILE* f);
	void read_raw(FILE* f);

	void write_dct16(FILE* f);
	void read_dct16(FILE* f);

	void write_wavelet(FILE* f);
	void read_wavelet(FILE* f);

public:
	static int round_to_eight(int val);
	static void rgb_yuv(unsigned __int8 r, unsigned __int8 g, unsigned __int8 b, unsigned __int8& y, unsigned __int8& u, unsigned __int8& v);
	static void yuv_rgb(unsigned __int8 y, unsigned __int8 u, unsigned __int8 v, unsigned __int8& r, unsigned __int8& g, unsigned __int8& b);

	static int get_raw_version();
	static int get_jpg16_version();
	static int get_wavelet_version();
	static int get_current_version() { return get_jpg16_version(); }
	///////////////////////////////////////////

	std::string m_compression;
private:
	int m_resx;
	int m_resy;
	int m_resz;
	dynamic_memory m_data;

};

class pat_image_channel16
{
public:
	pat_image_channel16();
	~pat_image_channel16();
	pat_image_channel16(const pat_image_channel16& obj);
	pat_image_channel16& operator= (const pat_image_channel16& obj);
	void initialize(int rx, int ry);
	void release();

	inline dynamic_memory& memory() { return m_data; }
	__int16* data() { return (__int16*)m_data.get_buffer(); }
	__int16* pixel(int i) { return data() + (i); }
	__int16* pixel(int x, int y) { return data() + (m_resx * y + x); }

	void write_raw(FILE* f);
	void read_raw(FILE* f);

	void write_zip(FILE* f);
	void read_zip(FILE* f);

	void calculate_dct();
	void calculate_dct_inv();
	void calculate_wavelet();
	void calculate_wavelet_inv();
	void transpose();
	void apply_mask_forward(__int16* mask);
	void apply_mask_backwards(__int16* mask);
	void clamp();

	void downsample();
	void upsample();
private:
	int m_resx;
	int m_resy;
	dynamic_memory m_data;
};