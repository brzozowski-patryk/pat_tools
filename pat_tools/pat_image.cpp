

#include <string>
#include <memory.h>
#include "pat_image.h"
#include <wincodecsdk.h>
#include <wincodec.h>

///////////////

int pat_image::get_raw_version() { return 'raw_'; }
int pat_image::get_jpg16_version() { return 'jp16'; }
int pat_image::get_wavelet_version() { return 'wave'; }




std::wstring widen(const char* str)
{
	const size_t cSize = strlen(str) + 1;
	wchar_t* wc = new wchar_t[cSize + 1];
	wc[cSize] = 0;
	size_t conv = 0;
	mbstowcs_s(&conv, wc, cSize, str, cSize);
	std::wstring s(wc);
	delete[] wc;
	return s;
}

__int8 clamp_pat_image(float val)
{
	float v = val < 0.0f ? 0.0f : val;
	return v > 255.0f ? 255 : (__int8)v;
}

void pat_image::rgb_yuv(unsigned __int8 r, unsigned __int8 g, unsigned __int8 b, unsigned __int8& y, unsigned __int8& u, unsigned __int8& v)
{
	float yv = 0.257f * (float)r + 0.504f * (float)g + 0.098f * (float)b + 16.0f;
	float uv = -0.148f * (float)r - 0.291f * (float)g + 0.439f * (float)b + 128.0f;
	float vv = 0.439f * (float)r - 0.368f * (float)g - 0.071f * (float)b + 128.0f;
	y = clamp_pat_image(yv);
	u = clamp_pat_image(uv);
	v = clamp_pat_image(vv);
}
void pat_image::yuv_rgb(unsigned __int8 y, unsigned __int8 u, unsigned __int8 v, unsigned __int8& r, unsigned __int8& g, unsigned __int8& b)
{
	float yv = (float)y - 16.0f;
	float uv = (float)u - 128.0f;
	float vv = (float)v - 128.0f;
	float rv = 1.164f * yv + 1.596f * vv;
	float gv = 1.164f * yv - 0.392f * uv - 0.813f * vv;
	float bv = 1.164f * yv + 2.017f * uv;
	r = clamp_pat_image(rv);
	g = clamp_pat_image(gv);
	b = clamp_pat_image(bv);
}



pat_image::pat_image()
{
	m_resx = 0;
	m_resy = 0;
	m_resz = 0;
}
pat_image::~pat_image()
{
	release();
}
pat_image::pat_image(const pat_image& obj)
{
	m_resx = obj.m_resx;
	m_resy = obj.m_resy;
	m_resz = obj.m_resz;
	m_data = obj.m_data;
}
pat_image& pat_image::operator= (const pat_image& obj)
{
	if (&obj != this)
	{
		m_resx = obj.m_resx;
		m_resy = obj.m_resy;
		m_resz = obj.m_resz;
		m_data = obj.m_data;
	}
	return *this;
}
void pat_image::initialize(int rx, int ry, int rz)
{
	release();
	m_resx = rx;
	m_resy = ry;
	m_resz = rz;
	m_data.initialize<char>(m_resx * m_resy * m_resz);
}
void pat_image::release()
{
	m_resx = 0;
	m_resy = 0;
	m_resz = 0;
	m_data.release();
}
void pat_image::load_from_file(const char* path)
{
	release();
	char* buff = 0;	UINT rx;	UINT ry;	int d;
	IWICImagingFactory* piImagingFactory = NULL;
	IWICBitmapDecoder* piDecoder = NULL;
	IWICBitmapFrameDecode* piBitmapFrame = NULL;
	UINT uiFrameCount = 0;
	HRESULT hr = S_OK;
	std::wstring str = widen(path);
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	hr = CoCreateInstance(CLSID_WICImagingFactory1, NULL, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), (LPVOID*)&piImagingFactory);
	hr = piImagingFactory->CreateDecoderFromFilename(str.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &piDecoder);
	hr = piDecoder->GetFrameCount(&uiFrameCount);
	if (uiFrameCount > 0)
	{
		piDecoder->GetFrame(0, &piBitmapFrame);
		if (SUCCEEDED(hr))
		{
			piBitmapFrame->GetSize(&rx, &ry);
			WICPixelFormatGUID frm;
			piBitmapFrame->GetPixelFormat(&frm);
			if (frm == GUID_WICPixelFormat24bppBGR) { d = 3; }
			if (frm == GUID_WICPixelFormat24bppRGB) { d = 3; }
			if (frm == GUID_WICPixelFormat32bppBGR) { d = 4; }
			if (frm == GUID_WICPixelFormat32bppBGRA) { d = 4; }
			if (frm == GUID_WICPixelFormat32bppPBGRA) { d = 4; }
			if (frm == GUID_WICPixelFormat32bppGrayFloat) { d = 4; }
			if (frm == GUID_WICPixelFormat32bppRGBA) { d = 4; }
			int rowPitch = rx * d;
			rowPitch += rx % 4;
			int fsiz = rowPitch * ry;
			buff = new char[fsiz];
			piBitmapFrame->CopyPixels(0, rowPitch, fsiz, (BYTE*)buff);
			piBitmapFrame->Release();
			piBitmapFrame = NULL;
		}
	}
	(piDecoder->Release());
	(piImagingFactory->Release());
	CoUninitialize();
	//////////////////////////////////////

	int rowPitch = rx * d + rx % 4;
	initialize(rx, ry, d);
	for (unsigned int i = 0; i < ry; i++)
	{
		memcpy(m_data.get_array<char>() + rx * d * i, buff + rowPitch * i, rx * d * sizeof(char));
	}
	delete[] buff;
}
void pat_image::capture_screen()
{
	HDC hScreenDC = CreateDC("DISPLAY", NULL, NULL, NULL);

	int width = GetDeviceCaps(hScreenDC, HORZRES);
	int height = GetDeviceCaps(hScreenDC, VERTRES);

	capture_hdc(hScreenDC, width, height);
	DeleteDC(hScreenDC);
}

void pat_image::capture_HWND(HWND okno)
{
	HDC hScreenDC = GetDC(okno);
	RECT rc;
	GetClientRect(okno, &rc);
	capture_hdc(hScreenDC, rc.right, rc.bottom);
	DeleteDC(hScreenDC);
}

void pat_image::capture_hdc(HDC hdc, int rx, int ry)
{
	HDC hMemoryDC = CreateCompatibleDC(hdc);

	int width = rx;
	int height = ry;

	if (width != m_resx || height != m_resy)
	{
		release();
		initialize(width, height, 3);
	}

	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, width, height);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap);
	BitBlt(hMemoryDC, 0, 0, width, height, hdc, 0, 0, SRCCOPY);



	BITMAPINFO bmpInfo;
	memset(&bmpInfo, 0, sizeof(BITMAPINFOHEADER));
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	GetDIBits(hdc, hBitmap, 0, 0, NULL, &bmpInfo, DIB_RGB_COLORS);
	bmpInfo.bmiHeader.biBitCount = 24;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	dynamic_memory temp;
	temp.initialize<char>(bmpInfo.bmiHeader.biSizeImage);
	GetDIBits(hdc, hBitmap, 0, bmpInfo.bmiHeader.biHeight, temp.get_buffer(), &bmpInfo, DIB_RGB_COLORS);
	SelectObject(hMemoryDC, hOldBitmap);

	int stride = width * 3 + width % 4;

	for (int y = 0; y < height; y++)
	{
		memcpy(m_data.get_buffer() + (width) * 3 * (height - (y + 1)), temp.get_buffer() + stride * y, width * 3);
	}

	DeleteDC(hMemoryDC);
}

/////////////////////////////////////////////////////////////////////////

void pat_image::sample(int x, int y, char* out, bool warp)
{
	int xx = x;
	int yy = y;

	if (warp)
	{
		xx = xx % m_resx;
		yy = yy % m_resx;
		xx = xx < 0 ? xx + m_resx : xx;
		yy = yy < 0 ? yy + m_resy : yy;
	}
	else
	{
		xx = xx < 0 ? 0 : xx;
		yy = yy < 0 ? 0 : yy;
		xx = xx >= m_resx ? m_resx - 1 : xx;
		yy = yy >= m_resy ? m_resy - 1 : yy;
	}

	int ind = m_resx * yy + xx;
	char* src = m_data.get_array<char>() + ind * m_resz;
	memcpy(out, src, m_resz);
}

void pat_image::sample_nearest(float x, float y, char* out, bool warp)
{
	int xx = int(x * float(m_resx));
	int yy = int(y * float(m_resy));
	sample(xx, yy, out, warp);
}

/////////////////////////////////////////////////////////////////////////

void pat_image::write_pat_image(const char* path, int method)
{
	FILE* f = NULL;
	fopen_s(&f, path, "wb");
	if (f)
	{
		fwrite(&method, sizeof(int), 1, f);
		fwrite(&m_resx, sizeof(int), 1, f);
		fwrite(&m_resy, sizeof(int), 1, f);
		fwrite(&m_resz, sizeof(int), 1, f);
		if (method == get_raw_version()) { write_raw(f); }
		if (method == get_jpg16_version()) { write_dct16(f); }
		if (method == get_wavelet_version()) { write_wavelet(f); }
		fclose(f);
	}
}
void pat_image::read_pat_image(const char* path)
{
	FILE* f = NULL;
	fopen_s(&f, path, "rb");
	if (f)
	{
		int method = -1;
		int rx = 0;
		int ry = 0;
		int rz = 0;
		fread(&method, sizeof(int), 1, f);
		fread(&rx, sizeof(int), 1, f);
		fread(&ry, sizeof(int), 1, f);
		fread(&rz, sizeof(int), 1, f);
		initialize(rx, ry, rz);
		if (method == get_raw_version()) { read_raw(f); m_compression = "raw"; }
		if (method == get_jpg16_version()) { read_dct16(f); m_compression = "jpg16"; }
		if (method == get_wavelet_version()) { read_wavelet(f); m_compression = "wavelet"; }
		fclose(f);
	}
}

void pat_image::write_raw(FILE* f)
{
	m_data.write_raw(f);
}
void pat_image::read_raw(FILE* f)
{
	m_data.read_raw(f);
}

int pat_image::round_to_eight(int val)
{
	int num = val / 8;
	int res = val % 8;
	return res == 0 ? (num * 8) : ((num + 1) * 8);
}

//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

void pat_image::write_dct16(FILE* f)
{
	int rx = pat_image::round_to_eight(m_resx);
	int ry = pat_image::round_to_eight(m_resy);

	pat_image_channel16 r;
	pat_image_channel16 g;
	pat_image_channel16 b;

	r.initialize(rx, ry);
	g.initialize(rx, ry);
	b.initialize(rx, ry);

	unsigned __int8 pix2[3];

	for (int i = 0; i < m_resx * m_resy; i++)
	{
		unsigned char* pix = pixel(i);
		rgb_yuv(pix[0], pix[1], pix[2], pix2[0], pix2[1], pix2[2]);
		r.pixel(i)[0] = pix2[0];
		g.pixel(i)[0] = pix2[1];
		b.pixel(i)[0] = pix2[2];
	}

	r.calculate_dct();
	g.calculate_dct();
	b.calculate_dct();

	r.transpose();
	g.transpose();
	b.transpose();

	r.calculate_dct();
	g.calculate_dct();
	b.calculate_dct();

	r.apply_mask_forward(mask_y);
	g.apply_mask_forward(mask_uv);
	b.apply_mask_forward(mask_uv);

	fwrite(mask_y, sizeof(__int16) * 64, 1, f);
	fwrite(mask_uv, sizeof(__int16) * 64, 1, f);

	r.write_zip(f);
	g.write_zip(f);
	b.write_zip(f);
}
void pat_image::read_dct16(FILE* f)
{
	int rx = pat_image::round_to_eight(m_resx);
	int ry = pat_image::round_to_eight(m_resy);

	pat_image_channel16 r;
	pat_image_channel16 g;
	pat_image_channel16 b;

	__int16 mask_loaded_y[64] = { 0 };
	__int16 mask_loaded_uv[64] = { 0 };
	fread(mask_loaded_y, sizeof(mask_loaded_y), 1, f);
	fread(mask_loaded_uv, sizeof(mask_loaded_uv), 1, f);

	r.read_zip(f);
	g.read_zip(f);
	b.read_zip(f);

	r.apply_mask_backwards(mask_loaded_y);
	g.apply_mask_backwards(mask_loaded_uv);
	b.apply_mask_backwards(mask_loaded_uv);

	r.calculate_dct_inv();
	g.calculate_dct_inv();
	b.calculate_dct_inv();

	r.transpose();
	g.transpose();
	b.transpose();

	r.calculate_dct_inv();
	g.calculate_dct_inv();
	b.calculate_dct_inv();

	r.clamp();
	g.clamp();
	b.clamp();

	unsigned __int8 pix2[3];

	for (int i = 0; i < m_resx * m_resy; i++)
	{
		unsigned char* pix = pixel(i);
		yuv_rgb((unsigned char)r.pixel(i)[0], (unsigned char)g.pixel(i)[0], (unsigned char)b.pixel(i)[0], pix2[0], pix2[1], pix2[2]);
		pix[0] = pix2[0];
		pix[1] = pix2[1];
		pix[2] = pix2[2];
	}
}


///////////////////////////////////////////////////

void pat_image::write_wavelet(FILE* f)
{
	int rx = pat_image::round_to_eight(m_resx);
	int ry = pat_image::round_to_eight(m_resy);

	pat_image_channel16 r;
	pat_image_channel16 g;
	pat_image_channel16 b;

	r.initialize(rx, ry);
	g.initialize(rx, ry);
	b.initialize(rx, ry);

	unsigned __int8 pix2[3];

	for (int i = 0; i < m_resx * m_resy; i++)
	{
		unsigned char* pix = pixel(i);
		rgb_yuv(pix[0], pix[1], pix[2], pix2[0], pix2[1], pix2[2]);
		r.pixel(i)[0] = pix2[0];
		g.pixel(i)[0] = pix2[1];
		b.pixel(i)[0] = pix2[2];
	}

	r.calculate_wavelet();
	g.calculate_wavelet();
	b.calculate_wavelet();

	r.transpose();
	g.transpose();
	b.transpose();

	r.calculate_wavelet();
	g.calculate_wavelet();
	b.calculate_wavelet();

	r.apply_mask_forward(mask_y_wavelet);
	g.apply_mask_forward(mask_uv_wavelet);
	b.apply_mask_forward(mask_uv_wavelet);

	fwrite(mask_y_wavelet, sizeof(__int16) * 64, 1, f);
	fwrite(mask_uv_wavelet, sizeof(__int16) * 64, 1, f);

	r.write_zip(f);
	g.write_zip(f);
	b.write_zip(f);
}

void pat_image::read_wavelet(FILE* f)
{
	int rx = pat_image::round_to_eight(m_resx);
	int ry = pat_image::round_to_eight(m_resy);

	pat_image_channel16 r;
	pat_image_channel16 g;
	pat_image_channel16 b;

	__int16 mask_loaded_y[64] = { 0 };
	__int16 mask_loaded_uv[64] = { 0 };
	fread(mask_loaded_y, sizeof(mask_loaded_y), 1, f);
	fread(mask_loaded_uv, sizeof(mask_loaded_uv), 1, f);

	r.read_zip(f);
	g.read_zip(f);
	b.read_zip(f);

	r.apply_mask_backwards(mask_loaded_y);
	g.apply_mask_backwards(mask_loaded_uv);
	b.apply_mask_backwards(mask_loaded_uv);

	r.calculate_wavelet_inv();
	g.calculate_wavelet_inv();
	b.calculate_wavelet_inv();

	r.transpose();
	g.transpose();
	b.transpose();

	r.calculate_wavelet_inv();
	g.calculate_wavelet_inv();
	b.calculate_wavelet_inv();

	r.clamp();
	g.clamp();
	b.clamp();

	unsigned __int8 pix2[3];

	for (int i = 0; i < m_resx * m_resy; i++)
	{
		unsigned char* pix = pixel(i);
		yuv_rgb((unsigned char)r.pixel(i)[0], (unsigned char)g.pixel(i)[0], (unsigned char)b.pixel(i)[0], pix2[0], pix2[1], pix2[2]);
		pix[0] = pix2[0];
		pix[1] = pix2[1];
		pix[2] = pix2[2];
	}
}