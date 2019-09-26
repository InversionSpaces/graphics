#include <iostream>
#include <fstream>
#include <inttypes.h>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <cmath>

struct RGBAColor
{
	uint8_t b, g, r, a;
};

struct Resolution
{
	uint32_t w, h;
};

struct PixelCoords
{
	int32_t x, y;
};

struct Coords
{
	double x, y;
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
	
	PixelCoords convert_c2p(const Coords& coords) {
		return { std::lround(-0.5 + resolution.w / 2.0 * 
												(coords.x + 1.0)),
				 std::lround(-0.5 + resolution.h / 2.0 * 
												(coords.y + 1.0)) };	
	}
	
	Coords convert_p2c(const PixelCoords& coords) {
		return { -1.0 + (2.0 * coords.x + 1.0) / resolution.w,
				 -1.0 + (2.0 * coords.y + 1.0) / resolution.h };
	}
};

class FBWriter
{
private:
	const Resolution resolution;
	std::vector<RGBAColor> buffer;
	std::ofstream stream;
public:
	FBWriter(const Resolution& res) :
		resolution(res),
		buffer(res.h * res.w, {0xff}),
		stream("/dev/fb0", std::ofstream::binary) {
	}
	
	void flush() {
		size_t size = buffer.size() * sizeof(RGBAColor);
		const char* data = reinterpret_cast<const char*>(buffer.data());
		
		stream.write(data, size);
		stream.flush();
		stream.seekp(0);
	}
	
	RGBAColor& operator[](const PixelCoords& coords) {
		return buffer[coords.y * resolution.w + coords.x];
	}
	
	~FBWriter() {
		stream.close();
	}
};



const uint32_t height 	= 1080;
const uint32_t width	= 1920;

int main() {
	Resolution res = {width, height};
	
	FBWriter fb(res);
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
