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

struct ScreenCoords
{
	float x, y;
};

struct RealCoords
{
	float x, y, z;
};

enum XYZ
{
	X_B,
	Y_B,
	Z_B
};

class CoordsCounter
{
private:
	const float rangeX = 1;
	const float rangeY = 1;
	
	const float lengthX;
	const float lengthY;
	
	const float distance;
	
	const uint16_t width;
	const uint16_t height;
public:
	CoordsCounter(	const Resolution& res, float distance = 1,
					float rangeX = 1, float rangeY = 1) :
		width(res.w),
		height(res.h),
		rangeX(rangeX),
		rangeY(rangeY),
		lengthX(rangeX / res.w),
		lengthY(rangeY / res.h),
		distance(distance)
	{
	}
	
	PixelCoords Real2Pixel(const RealCoords& coords) const
	{
		return Screen2Pixel(Real2Screen(coords));
	}
	
	ScreenCoords Real2Screen(const RealCoords& coords) const
	{
		float x = distance * coords.x / -coords.z;
		float y = distance * coords.y / -coords.z;
		
		return {x, y}; 
	}
	
	PixelCoords Screen2Pixel(const ScreenCoords& coords) const
	{
		assert(fabsf(coords.x) <= rangeX);
		assert(fabsf(coords.y) <= rangeY);
		
		float px = (coords.x + rangeX) * width  / (2 * rangeX);
		float py = (coords.y + rangeY) * height / (2 * rangeY);
		
		return {uint16_t(std::lround(px)), uint16_t(std::lround(py))};	
	}
	
	ScreenCoords Pixel2Screen(const PixelCoords& coords) const
	{
		assert(coords.x < width);
		assert(coords.y < height);
		
		float rx = 2 * rangeX * coords.x / width  - rangeX;
		float ry = 2 * rangeY * coords.y / height - rangeY;
		
		return {rx + lengthX, ry + lengthY};
	}
	
	RealCoords Rotate(	const RealCoords& coords, const float& angle, 
						XYZ basis) const
	{
		float sin = std::sin(angle);
		float cos = std::cos(angle);
		
		float x = coords.x;
		float y = coords.y;
		float z = coords.z;
		
		if (basis == X_B) {
			y = cos * coords.y - sin * coords.z;
			z = sin * coords.y + cos * coords.z;
		}
		else if (basis == Y_B) {
			x = cos * coords.x + sin * coords.z;
			z = cos * coords.z - sin * coords.x;
		}
		else if (basis == Z_B) {
			x = cos * coords.x - sin * coords.y;
			y = sin * coords.x + cos * coords.y;
		}
		
		return {x, y, z};
	}
};

class FBWriter
{
private:
	std::vector<RGBAColor> 	buffer;
	std::filebuf 			filebuf;
public:
	const Resolution 		resolution;

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
	RealCoords dots[2];
};

struct Cube
{
	RealCoords dots[8];
};

void Fill(FBWriter& fb, RGBAColor color) {
	for (uint16_t x = 0; x < fb.resolution.w; ++x)
		for (uint16_t y = 0; y < fb.resolution.h; ++y)
			fb[{x, y}] = color;
}

void Draw(	FBWriter& fb, const CoordsCounter& cc, 
			const Line& line, const RGBAColor& color)
{	
	PixelCoords start 	= cc.Real2Pixel(line.dots[0]);
	PixelCoords end 	= cc.Real2Pixel(line.dots[1]);
	
	bool xORy = fabs(start.x - end.x) > fabs(start.y - end.y);
	
	if (( xORy && (start.x > end.x)) ||
		(!xORy && (start.y > end.y)))
		std::swap(start, end);
		
	//std::cout << "Start: x[" << start.x << "] y[" << start.y << "]" << std::endl;
	//std::cout << "End: x[" << end.x << "] y[" << end.y << "]" << std::endl;
	
	uint16_t iterStart 	= 0;
	uint16_t iterEnd 	= 0;
	uint16_t initial	= 0;
	
	float tg = 0;
	
	if (xORy) {
		initial 	= start.y;
		
		iterStart 	= start.x;
		iterEnd 	= end.x;
		
		tg = float(end.y - start.y) / float(end.x - start.x);
	}
	else {
		initial 	= start.x;
		
		iterStart 	= start.y;
		iterEnd 	= end.y;
		
		tg = float(end.x - start.x) / float(end.y - start.y);
	}
	
	//std::cout << "Initial: " << initial << std::endl;
	//std::cout << "IterStart: " << iterStart << " IterEnd: " << iterEnd << std::endl;
	//std::cout << "TG: " << tg << std::endl;
	
	for (uint16_t i = iterStart; i <= iterEnd; ++i) {
		uint16_t j = initial + std::lround(tg * (i - iterStart));
		//std::cout << i << " " << j << std::endl;
		fb[xORy ? PixelCoords({i, j}) : PixelCoords({j, i})] = color;
	}
}

void Draw(	FBWriter& fb, const CoordsCounter& cc, 
			const Cube& cube, const RGBAColor& color)
{		
	for (int i = 0; i < 4; ++i) {
		Draw(fb, cc, Line({{cube.dots[i], cube.dots[i + 4]}}), color);
		Draw(fb, cc, Line({{cube.dots[i], cube.dots[(i + 1) % 4]}}), color);
		Draw(fb, cc, Line({{cube.dots[i + 4], cube.dots[(i + 1) % 4 + 4]}}), color);
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
	
	float z = -2.5;
	Cube cube = {{	{1, 1, 1}, {1, 1, -1}, {-1, 1, -1}, {-1, 1, 1}, 
					{1, -1, 1}, {1, -1, -1}, {-1, -1, -1}, {-1, -1, 1}  }};
					
	while (1) {
		for (float ang = 0; ang < 2 * M_PI; ang += 0.05) {
			Fill(fb, {0, 0, 0, 0xff});
			
			Cube tmpcube = cube;
			
			for (int i = 0; i < 8; ++i) {
				tmpcube.dots[i] = cc.Rotate(tmpcube.dots[i], ang, Y_B);
				tmpcube.dots[i] = cc.Rotate(tmpcube.dots[i], ang, X_B);
			}

			for (int i = 0; i < 8; ++i)
				tmpcube.dots[i].z += z;
					
			Draw(fb, cc, tmpcube, {0x55, 0x44, 0x33, 0xff});
			
			fb.flush();
			
			//std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
	
	/*
	float halflen = 0.5;
	float z = -5;
	
	while (1) {
		for (float i = 0; i < 2 * M_PI; i += 0.01) {
			Fill(fb, {0, 0, 0, 0xff});
			
			float x = std::sin(i) * halflen;
			float y = std::cos(i) * halflen;
			
			Draw(fb, cc, {{{-x, -y, z}, {x, y, z}}}, {0, 0xff, 0, 0xff});
			Draw(fb, cc, {{{-x, y, z}, {x, -y, z}}}, {0, 0xff, 0, 0xff});
			Draw(fb, cc, {{{-x, -y, z}, {-x, y, z}}}, {0, 0xff, 0, 0xff});
			Draw(fb, cc, {{{x, y, z}, {x, -y, z}}}, {0, 0xff, 0, 0xff});
			
			fb.flush();
			
			//std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
	*/
}
