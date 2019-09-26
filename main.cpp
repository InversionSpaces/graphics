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
	const float rangeX = 1;
	const float rangeY = 1;
	
	const float lengthX;
	const float lengthY;
	
	const uint16_t width;
	const uint16_t height;
public:
	CoordsCounter(	const Resolution& res, 
					float rangeX = 1, float rangeY = 1) :
		width(res.w),
		height(res.h),
		rangeX(rangeX),
		rangeY(rangeY),
		lengthX(rangeX / res.w),
		lengthY(rangeY / res.h)
	{
	}
	
	PixelCoords Real2Pixel(const RealCoords& coords) const
	{
		assert(fabsf(coords.x) <= rangeX);
		assert(fabsf(coords.y) <= rangeY);
		
		float px = (coords.x + rangeX) * width  / (2 * rangeX);
		float py = (coords.y + rangeY) * height / (2 * rangeY);
		
		return {std::lround(px), std::lround(py)};	
	}
	
	RealCoords Pixel2Real(const PixelCoords& coords) const
	{
		assert(coords.x < width);
		assert(coords.y < height);
		
		float rx = 2 * rangeX * coords.x / width  - rangeX;
		float ry = 2 * rangeY * coords.y / height - rangeY;
		
		return {rx + lengthX, ry + lengthY};
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
		buffer(res.h * res.w)
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
		assert(coords.x < resolution.w);
		assert(coords.y < resolution.h);
		
		return buffer[coords.y * resolution.w + coords.x];
	}
	
	~FBWriter() {
		if (filebuf.is_open())
			filebuf.close();
	}
};

struct Line 
{
	RealCoords p1, p2;
};

void Draw(	FBWriter& fb, const CoordsCounter& cc, 
			const Line& line, const RGBAColor& color)
{	
	PixelCoords p1 = cc.Real2Pixel(line.p1);
	PixelCoords p2 = cc.Real2Pixel(line.p2);
	
	uint16_t xmin = std::min(p1.x, p2.x);
	uint16_t xmax = std::max(p1.x, p2.x);
	 
	uint16_t ymin = std::min(p1.y, p2.y);
	uint16_t ymax = std::max(p1.y, p2.y);
	
	std::cout << xmin << " -> " << xmax << std::endl;
	std::cout << ymin << " -> " << ymax << std::endl;
	
	if (xmax - xmin > ymax - ymin) {
		for (uint16_t x = xmin; x <= xmax; ++x) {
			 uint16_t y = uint16_t(std::lround(
							ymin + float(ymax - ymin) / 
							float(xmax - xmin) * (x - xmin)
							));
			fb[{x, y}] = color;
		}
	}
	else {
		for (uint16_t y = ymin; y <= ymax; ++y) {
			uint16_t x = uint16_t(std::lround(
							xmin + float(xmax - xmin) / 
							float(ymax - ymin) * (y - ymin)
							));
			fb[{x, y}] = color;
		}
	}
}



const uint32_t height 	= 1080;
const uint32_t width	= 1920;

int main() {
	CoordsCounter cc({width, height});
	FBWriter fb({width, height});
	
	fb.open("/dev/fb0");
	
	if (!fb.is_open()) {
		std::cout << "Can't open\n";
		return 1;
	}
	
	Line line = {{0.9, 0.9}, {-0.9, -0.9}};
	Draw(fb, cc, line, {0, 0xff, 0, 0xff});
	
	fb.flush();
	
	/*
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
	*/
}
