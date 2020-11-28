
#pragma once

#include <stdio.h>

/// <summary>
///  dynamic memory class
/// </summary>

class dynamic_memory
{
public:
	dynamic_memory();
	~dynamic_memory();
	dynamic_memory(const dynamic_memory& obj);
	dynamic_memory& operator= (const dynamic_memory& obj);
	void release();
	void zero();
	void copy_from(void* ptr);
	template<typename T> int size() { return get_size_bytes() / sizeof(T); }
	template<typename T> void initialize(int num) { initialize_bytes(sizeof(T) * num); }
	template<typename T> T* get_array() { return (T*)m_data; };
	template<typename T> T& get(int n) { return  get_array<T>()[n]; }
	void grow();
public:
	int num_bits();
	bool get_bit(int n);
	void set_bit(int n, bool v);
public:
	int get_size_bytes() const;
	__int8* get_buffer() const;
private:
	void initialize_bytes(int numbytes);
	__int8* m_data;
	int m_size;
};