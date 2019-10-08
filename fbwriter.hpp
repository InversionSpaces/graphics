#include <vector>
#include <cinttypes>
#include <fstream>

#include "linalg.hpp"

typedef vec<uint8_t, 4> rgbacolor_t;
typedef vec<uint16_t, 2> resolution_t;
typedef vec<uint16_t, 2> pixelcoords_t;

class FBWriter
{
private:
	std::vector<rgbacolor_t> buffer;
    
	std::filebuf filebuf;
public:
	const resolution_t 		resolution;

	FBWriter(const resolution_t& res) :
		resolution(res),
		buffer(res.w * res.h)
	{
	}
	
	bool is_open() 
	{
		return filebuf.is_open();
	}
	
	void open(const char* filename) 
	{
		assert(filename != NULL);
		
		filebuf.open(filename, std::ios::out | std::ios::binary);
	}
	
	void flush() 
	{
		assert(is_open());
		
		size_t size = buffer.size() * sizeof(*buffer.data());
		char* ptr = reinterpret_cast<char*>(buffer.data());
		
		filebuf.sputn(ptr, size);
		filebuf.pubseekpos(0);
	}
	
	rgbacolor_t& operator[](const pixelcoords_t& coords) 
	{
		assert(coords.x < resolution.w);
		assert(coords.y < resolution.h);
		
		return buffer[coords.y * resolution.w + coords.x];
	}
	
	~FBWriter() {
		if (filebuf.is_open())
			filebuf.close();
	}
};
