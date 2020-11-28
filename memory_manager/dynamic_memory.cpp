
#include "dynamic_memory.h"
#include <memory.h>

///////////////////////////////////////////////////////////////////////

dynamic_memory::dynamic_memory()
{
	m_data = NULL;
	m_size = 0;
}
dynamic_memory::~dynamic_memory()
{
	release();
}
dynamic_memory::dynamic_memory(const dynamic_memory& obj)
{
	initialize_bytes(obj.get_size_bytes());
	memcpy(m_data, obj.get_buffer(), obj.get_size_bytes());
}
dynamic_memory& dynamic_memory::operator= (const dynamic_memory& obj)
{
	if (&obj != this)
	{
		initialize_bytes(obj.get_size_bytes());
		memcpy(m_data, obj.get_buffer(), obj.get_size_bytes());
	}
	return *this;
}
void dynamic_memory::release()
{
	if (m_data) { delete[] m_data; m_data = NULL; m_size = 0; }
}
void dynamic_memory::initialize_bytes(int num)
{
	release();
	m_size = num;
	m_data = new __int8[num];
}

int dynamic_memory::num_bits()
{
	return m_size * 8;
}
bool dynamic_memory::get_bit(int n)
{
	int i = n / 8;
	int j = n % 8;
	while (i >= m_size) { grow(); }
	return (m_data[i] & (1 << j)) ? 1 : 0;
}
void dynamic_memory::set_bit(int n, bool v)
{
	int i = n / 8;
	int j = n % 8;
	while (i >= m_size) { grow(); }
	if (v)
	{
		m_data[i] |= (1 << j);
	}
	else
	{
		m_data[i] &= ~(1 << j);
	}
}

void dynamic_memory::grow()
{
	char* temp = m_data;
	int siz = m_size;
	m_size = siz * 2;
	m_data = new char[m_size];
	memcpy(m_data, temp, siz);
	memset(m_data + siz, 0, siz);
	delete[] temp;
}

void dynamic_memory::zero()
{
	memset(m_data, 0, m_size);
}

int dynamic_memory::get_size_bytes() const { return m_size; }
__int8* dynamic_memory::get_buffer() const { return m_data; }

void dynamic_memory::copy_from(void* ptr)
{
	memcpy(m_data, ptr, get_size_bytes());
}
