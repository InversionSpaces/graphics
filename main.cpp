#include <iostream>
#include <fstream>
#include <inttypes.h>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <cmath>
#include <cassert>

struct RGBAColor
{
	uint8_t b, g, r, a;
};

struct Resolution
{
	uint16_t w, h;
};

struct PixelCoords
{
	uint16_t x, y;
};

struct RealCoords
{
	float x, y;
};

class CoordsCounter
{
private:
/*
	const double rangeX = 1;
	const double rangeY = 1;
	
	const double lX;
	const double lY;
*/
	const Resolution resolution;
public:
	CoordsCounter(const Resolution& res) :
		resolution(resolution) {
	}
	
	PixelCoords convert_c2p(const RealCoords& coords) {
		return { std::lround(-0.5 + resolution.w / 2.0 * 
												(coords.x + 1.0)),
				 std::lround(-0.5 + resolution.h / 2.0 * 
												(coords.y + 1.0)) };	
	}
	
	RealCoords convert_p2c(const PixelCoords& coords) {
		return { -1.0 + (2.0 * coords.x + 1.0) / resolution.w,
				 -1.0 + (2.0 * coords.y + 1.0) / resolution.h };
	}
};

class FBWriter
{
private:
	const Resolution 		resolution;
	std::vector<RGBAColor> 	buffer;
	std::filebuf 			filebuf;
public:
	FBWriter(const Resolution& res) :
		resolution(res),
		buffer(res.h * res.w, {0xff})
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
		
		size_t buffersize = buffer.size() * sizeof(RGBAColor);
		char* bufferp = reinterpret_cast<char*>(buffer.data());
		
		filebuf.sputn(bufferp, buffersize);
		filebuf.pubseekpos(0);
	}
	
	RGBAColor& operator[](const PixelCoords& coords) 
	{
		assert(coords.x <= resolution.w);
		assert(coords.y <= resolution.h);
		
		return buffer[coords.y * resolution.w + coords.x];
	}
	
	~FBWriter() {
		if (filebuf.is_open())
			filebuf.close();
	}
};



const uint32_t height 	= 1080;
const uint32_t width	= 1920;

int main() {	
	FBWriter fb({width, height});
	fb.open("/dev/fb0");
	
	if (!fb.is_open()) {
		std::cout << "Can't open\n";
		return 1;
	}
	
	/*
	
	for (int32_t x = 0; x < width; ++x)
			for (int32_t y = 0; y < height; y++)
				fb.setPixel({x, y}, {0xff, 0xff, 0xff, 0xff});
				
	fb.flush();
	
	CoordsCounter cc(res);
	
	Coords p1 = {-0.7, -0.5};
	Coords p2 = {0.7, 0.5};
	
	PixelCoords p1p = cc.convert_c2p(p1);
	PixelCoords p2p = cc.convert_c2p(p2);
	
	int32_t xmin = std::min(p1p.x, p2p.x);
	int32_t xmax = std::max(p1p.x, p2p.x);
	 
	int32_t ymin = std::min(p1p.y, p2p.y);
	int32_t ymax = std::max(p1p.y, p2p.y);
	
	std::cout << xmin << " -> " << xmax << std::endl;
	std::cout << ymin << " -> " << ymax << std::endl;
	
	if (xmax - xmin > ymax - ymin) {
		for (uint32_t x = xmin; x <= xmax; ++x) {
			uint32_t y = std::lround(
							ymin + double(ymax - ymin) / 
							double(xmax - xmin) * (x - xmin)
							);
			fb.setPixel({x, y}, {0, 0, 0, 0xff});
		}
	}
	else {
		
	}
	*/
	
	while (1) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		for (int x = 0; x < width; ++x)
			for (int y = 0; y < height; y++)
				fb[{x, y}] =  {x, y, 0, 0xff};
				
		fb.flush();
		
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		for (int x = 0; x < width; ++x)
			for (int y = 0; y < height; y++)
				fb[{x, y}] =  {0, x, y, 0xff};
				
		fb.flush();
	}
}
