
#include "pat_image.h"
#include <math.h>
#define PAT_PI 3.1415926535897932384626433f


/////////////////////////////////////////////////////////////////////////////////////////

void transform_type_cosine_function(float* data, int nums, float* out)
{
	float sqn1 = 1.0f / sqrtf(float(nums));
	float sqn2 = sqrtf(2.0f / float(nums));
	out[0] = 0.0f;
	for (int m = 0; m < nums; m++)
	{
		out[0] += data[m];
	}
	out[0] *= sqn1;
	for (int k = 1; k < nums; k++)
	{
		out[k] = 0;
		for (int m = 0; m < nums; m++)
		{
			out[k] += data[m] * cosf((PAT_PI * float(k) * (2.0f * float(m) + 1.0f)) / (2.0f * float(nums)));
		}
		out[k] *= sqn2;
	}
}

void transform_type_cosine_inverse_function(float* data, int nums, float* out)
{
	float sqn1 = 1.0f / sqrtf(float(nums));
	float sqn2 = sqrtf(2.0f / float(nums));
	for (int m = 0; m < nums; m++)
	{
		out[m] = 0;
		for (int k = 1; k < nums; k++)
		{
			out[m] += data[k] * cosf((PAT_PI * float(k) * (2.0f * float(m) + 1.0f)) / (2.0f * float(nums)));
		}
		out[m] *= sqn2;
		out[m] += sqn1 * data[0];
	}
}








///////////////////


pat_image_channel16::pat_image_channel16()
{
	m_resx = 0;
	m_resy = 0;
}
pat_image_channel16::~pat_image_channel16()
{
	release();
}
pat_image_channel16::pat_image_channel16(const pat_image_channel16& obj)
{
	m_resx = obj.m_resx;
	m_resy = obj.m_resy;
	m_data = obj.m_data;
}
pat_image_channel16& pat_image_channel16::operator= (const pat_image_channel16& obj)
{
	if (&obj != this)
	{
		m_resx = obj.m_resx;
		m_resy = obj.m_resy;
		m_data = obj.m_data;
	}
	return *this;
}
void pat_image_channel16::initialize(int rx, int ry)
{
	release();
	m_resx = rx;
	m_resy = ry;
	m_data.initialize<__int16>(m_resx * m_resy);
}
void pat_image_channel16::release()
{
	m_resx = 0;
	m_resy = 0;
	m_data.release();
}

void pat_image_channel16::write_raw(FILE* f)
{
	fwrite(&m_resx, sizeof(int), 1, f);
	fwrite(&m_resy, sizeof(int), 1, f);
	m_data.write_raw(f);
}
void pat_image_channel16::read_raw(FILE* f)
{
	int rx = 0;
	int ry = 0;
	fread(&rx, sizeof(int), 1, f);
	fread(&ry, sizeof(int), 1, f);
	initialize(rx, ry);
	m_data.read_raw(f);
}

void pat_image_channel16::write_zip(FILE* f)
{
	fwrite(&m_resx, sizeof(int), 1, f);
	fwrite(&m_resy, sizeof(int), 1, f);
	m_data.write_zip(f);
}
void pat_image_channel16::read_zip(FILE* f)
{
	int rx = 0;
	int ry = 0;
	fread(&rx, sizeof(int), 1, f);
	fread(&ry, sizeof(int), 1, f);
	initialize(rx, ry);
	m_data.read_zip(f);
}






void pat_image_channel16::calculate_dct()
{
	float inp[8];
	float out[8];
	int allpix = m_resx * m_resy;

	for (int i = 0; i < allpix / 8; i++)
	{
		inp[0] = (float)pixel(i * 8)[0];
		inp[1] = (float)pixel(i * 8)[1];
		inp[2] = (float)pixel(i * 8)[2];
		inp[3] = (float)pixel(i * 8)[3];

		inp[4] = (float)pixel(i * 8)[4];
		inp[5] = (float)pixel(i * 8)[5];
		inp[6] = (float)pixel(i * 8)[6];
		inp[7] = (float)pixel(i * 8)[7];

		memset(out, 0, sizeof(out));

		transform_type_cosine_function(inp, 8, out);

		pixel(i * 8)[0] = __int16(out[0]);
		pixel(i * 8)[1] = __int16(out[1]);
		pixel(i * 8)[2] = __int16(out[2]);
		pixel(i * 8)[3] = __int16(out[3]);
		pixel(i * 8)[4] = __int16(out[4]);
		pixel(i * 8)[5] = __int16(out[5]);
		pixel(i * 8)[6] = __int16(out[6]);
		pixel(i * 8)[7] = __int16(out[7]);
	}
}
void pat_image_channel16::calculate_dct_inv()
{
	float inp[8];
	float out[8];
	int allpix = m_resx * m_resy;

	for (int i = 0; i < allpix / 8; i++)
	{
		inp[0] = (float)pixel(i * 8)[0];
		inp[1] = (float)pixel(i * 8)[1];
		inp[2] = (float)pixel(i * 8)[2];
		inp[3] = (float)pixel(i * 8)[3];
		inp[4] = (float)pixel(i * 8)[4];
		inp[5] = (float)pixel(i * 8)[5];
		inp[6] = (float)pixel(i * 8)[6];
		inp[7] = (float)pixel(i * 8)[7];

		memset(out, 0, sizeof(out));

		transform_type_cosine_inverse_function(inp, 8, out);

		pixel(i * 8)[0] = __int16(out[0]);
		pixel(i * 8)[1] = __int16(out[1]);
		pixel(i * 8)[2] = __int16(out[2]);
		pixel(i * 8)[3] = __int16(out[3]);
		pixel(i * 8)[4] = __int16(out[4]);
		pixel(i * 8)[5] = __int16(out[5]);
		pixel(i * 8)[6] = __int16(out[6]);
		pixel(i * 8)[7] = __int16(out[7]);
	}
}


void pat_image_channel16::calculate_wavelet()
{
	float inp[8];
	float out[8];
	int allpix = m_resx * m_resy;

	for (int i = 0; i < allpix / 8; i++)
	{
		inp[0] = (float)pixel(i * 8)[0];
		inp[1] = (float)pixel(i * 8)[1];
		inp[2] = (float)pixel(i * 8)[2];
		inp[3] = (float)pixel(i * 8)[3];
		inp[4] = (float)pixel(i * 8)[4];
		inp[5] = (float)pixel(i * 8)[5];
		inp[6] = (float)pixel(i * 8)[6];
		inp[7] = (float)pixel(i * 8)[7];
		memcpy(out, inp, sizeof(inp));
		{
			out[0] = (inp[0] + inp[1]) / 1;
			out[1] = (inp[2] + inp[3]) / 1;
			out[2] = (inp[4] + inp[5]) / 1;
			out[3] = (inp[6] + inp[7]) / 1;
			out[4] = (inp[0] - inp[1]) / 1;
			out[5] = (inp[2] - inp[3]) / 1;
			out[6] = (inp[4] - inp[5]) / 1;
			out[7] = (inp[6] - inp[7]) / 1;

			memcpy(inp, out, sizeof(inp));
			out[0] = (inp[0] + inp[1]) / 1;
			out[1] = (inp[2] + inp[3]) / 1;
			out[2] = (inp[0] - inp[1]) / 1;
			out[3] = (inp[2] - inp[3]) / 1;

			memcpy(inp, out, sizeof(inp));
			out[0] = (inp[0] + inp[1]) / 1;
			out[1] = (inp[0] - inp[1]) / 1;
		}

		pixel(i * 8)[0] = __int16(out[0]);
		pixel(i * 8)[1] = __int16(out[1]);
		pixel(i * 8)[2] = __int16(out[2]);
		pixel(i * 8)[3] = __int16(out[3]);
		pixel(i * 8)[4] = __int16(out[4]);
		pixel(i * 8)[5] = __int16(out[5]);
		pixel(i * 8)[6] = __int16(out[6]);
		pixel(i * 8)[7] = __int16(out[7]);
	}
}
void pat_image_channel16::calculate_wavelet_inv()
{
	float inp[8];
	float out[8];
	int allpix = m_resx * m_resy;

	for (int i = 0; i < allpix / 8; i++)
	{
		inp[0] = (float)pixel(i * 8)[0];
		inp[1] = (float)pixel(i * 8)[1];
		inp[2] = (float)pixel(i * 8)[2];
		inp[3] = (float)pixel(i * 8)[3];
		inp[4] = (float)pixel(i * 8)[4];
		inp[5] = (float)pixel(i * 8)[5];
		inp[6] = (float)pixel(i * 8)[6];
		inp[7] = (float)pixel(i * 8)[7];
		memcpy(out, inp, sizeof(inp));
		{
			memcpy(inp, out, sizeof(inp));
			out[0] = (inp[0] + inp[1]) / 2;
			out[1] = (inp[0] - inp[1]) / 2;

			memcpy(inp, out, sizeof(inp));
			out[0] = (inp[0] + inp[2]) / 2;
			out[1] = (inp[0] - inp[2]) / 2;
			out[2] = (inp[1] + inp[3]) / 2;
			out[3] = (inp[1] - inp[3]) / 2;

			memcpy(inp, out, sizeof(inp));
			out[0] = (inp[0] + inp[4]) / 2;
			out[1] = (inp[0] - inp[4]) / 2;
			out[2] = (inp[1] + inp[5]) / 2;
			out[3] = (inp[1] - inp[5]) / 2;
			out[4] = (inp[2] + inp[6]) / 2;
			out[5] = (inp[2] - inp[6]) / 2;
			out[6] = (inp[3] + inp[7]) / 2;
			out[7] = (inp[3] - inp[7]) / 2;
		}

		pixel(i * 8)[0] = __int16(out[0]);
		pixel(i * 8)[1] = __int16(out[1]);
		pixel(i * 8)[2] = __int16(out[2]);
		pixel(i * 8)[3] = __int16(out[3]);
		pixel(i * 8)[4] = __int16(out[4]);
		pixel(i * 8)[5] = __int16(out[5]);
		pixel(i * 8)[6] = __int16(out[6]);
		pixel(i * 8)[7] = __int16(out[7]);
	}
}



void pat_image_channel16::transpose()
{
	dynamic_memory temp;
	temp.initialize<__int16>(m_resx * m_resy);

	for (int y = 0; y < m_resy; y++)
	{
		for (int x = 0; x < m_resx; x++)
		{
			{
				__int16* pix1 = (__int16*)m_data.get_buffer() + (m_resx * y + x);
				__int16* pix2 = (__int16*)temp.get_buffer() + (m_resy * x + y);
				pix2[0] = pix1[0];
			}
		}
	}
	m_data = temp;
	int rx = m_resx;
	m_resx = m_resy;
	m_resy = rx;
}


void pat_image_channel16::apply_mask_forward(__int16* mask)
{
	for (int y = 0; y < m_resy; y++)
	{
		for (int x = 0; x < m_resx; x++)
		{
			int mx = x % 8;
			int my = y % 8;
			int m = my * 8 + mx; // index na masce
			int ind = y * m_resx + x; // index na obrazku

			m_data.get<__int16>(ind) /= mask[m];
		}
	}
}

void pat_image_channel16::apply_mask_backwards(__int16* mask)
{
	for (int y = 0; y < m_resy; y++)
	{
		for (int x = 0; x < m_resx; x++)
		{
			int mx = x % 8;
			int my = y % 8;
			int m = my * 8 + mx; // index na masce
			int ind = y * m_resx + x; // index na obrazku

			m_data.get<__int16>(ind) *= mask[m];
		}
	}
}

void pat_image_channel16::clamp()
{
	int nump = m_resx * m_resy;
	for (int i = 0; i < nump; i++)
	{
		__int16 val = m_data.get<__int16>(i);
		val = val > 255 ? 255 : val;
		val = val < 0 ? 0 : val;
		m_data.get<__int16>(i) = val;
	}
}



void pat_image_channel16::downsample()
{
	dynamic_memory buf;

	int rx = m_resx / 2;
	int ry = m_resy / 2;

	buf.initialize<__int16>(rx * ry);

	int ind = 0;
	for (int y = 0; y < ry; y++)
	{
		for (int x = 0; x < rx; x++)
		{
			__int16* pix = pixel(x * 2, y * 2);
			buf.get<__int16>(ind) = pix[0];
			ind++;
		}
	}
	m_resx = rx;
	m_resy = ry;
	m_data = buf;
}
void pat_image_channel16::upsample()
{
	dynamic_memory buf;

	int rx = m_resx * 2;
	int ry = m_resy * 2;

	buf.initialize<__int16>(rx * ry);

	int ind = 0;
	for (int y = 0; y < ry; y++)
	{
		for (int x = 0; x < rx; x++)
		{
			__int16* pix = pixel(x / 2, y / 2);
			buf.get<__int16>(ind) = pix[0];
			ind++;
		}
	}
	m_resx = rx;
	m_resy = ry;
	m_data = buf;
}
/////////////////////////////////////////////////////////////////////////////////////////

