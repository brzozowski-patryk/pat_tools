
#pragma once

#include <stdio.h>


#define SERIALIZATION( nam ) \
	void write_##nam( FILE* f ); \
	void read_##nam( FILE* f ); \
	void write_file_##nam( const char* path ) \
	{ \
		FILE* f = NULL; fopen_s( &f, path, "wb" ); \
		if( f ) \
		{ \
			write_##nam( f ); \
			fclose( f ); \
		} \
	} \
	void read_file_##nam( const char* path ) \
	{ \
		FILE* f = NULL; fopen_s( &f, path, "rb" ); \
		if( f ) \
		{ \
			read_##nam( f ); \
			fclose( f ); \
		} \
	}

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
	void allocate_bytes(int num);

	template<typename T> int size() { return get_size_bytes() / sizeof(T); }
	template<typename T> void initialize(int num) { initialize_bytes(sizeof(T) * num); }
	template<typename T> T* get_array() { return (T*)m_data; };
	template<typename T> T& get(int n) { return  get_array<T>()[n]; }


	SERIALIZATION(raw)
		SERIALIZATION(huffman)
		SERIALIZATION(zip)
public:
	int num_bits();
	bool get_bit(int n);
	void set_bit(int n, bool v);

public:
	int get_size_bytes() const;
	__int8* get_buffer() const;
private:
	void grow(); // m_size*2 copy
	void initialize_bytes(int numbytes);
	__int8* m_data;
	int m_size;
};

/////////////////////////////////////////////////////////
