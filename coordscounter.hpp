#include <cmath>

#include "linalg.hpp"

class CoordsCounter
{
private:	
	const float lengthX;
	const float lengthY;
	
	const float distance;
	
	const resolution_t resolution;
public:
	CoordsCounter(const resolution_t& res, float distance = 1.f) :
		resolution(res),
		lengthX(1.f / res.w),
		lengthY(1.f / res.h),
		distance(distance)
	{
	}
	
	pixelcoords_t real2pixel(const realcoords_t& coords) const
	{
		return screen2pixel(real2screen(coords));
	}
	
	screencoords_t real2screen(const realcoords_t& coords) const
	{
		float x = float(distance) * coords.x / resolution.w / -coords.z;
		float y = float(distance) * coords.y / resolution.h / -coords.z;
		
		return {x, y}; 
	}
	
	pixelcoords_t screen2pixel(const screencoords_t& coords) const
	{
		assert(fabsf(coords.x) < 1.f);
		assert(fabsf(coords.y) < 1.f);
		
		float px = (coords.x + 1.f) * resolution.w / (2.f);
		float py = (coords.y + 1.f) * resolution.h / (2.f);
		
		return {uint16_t(std::lround(px)), uint16_t(std::lround(py))};	
	}
	
	screencoords_t pixel2screen(const pixelcoords_t& coords) const
	{
		assert(coords.x < resolution.w);
		assert(coords.y < resolution.h);
		
		float rx = 2.f * coords.x / resolution.w - 1.f;
		float ry = 2.f * coords.y / resolution.h - 1.f;
		
		return {rx + lengthX, ry + lengthY};
	}
	
	realcoords_t rotate(	const realcoords_t& coords, 
                            const float& angle, 
                            int basis) const
	{
		float sin = std::sin(angle);
		float cos = std::cos(angle);
		
		float x = coords.x;
		float y = coords.y;
		float z = coords.z;
		
		if (basis == 0) {
			y = cos * coords.y - sin * coords.z;
			z = sin * coords.y + cos * coords.z;
		}
		else if (basis == 1) {
			x = cos * coords.x + sin * coords.z;
			z = cos * coords.z - sin * coords.x;
		}
		else if (basis == 2) {
			x = cos * coords.x - sin * coords.y;
			y = sin * coords.x + cos * coords.y;
		}
		
		return {x, y, z};
	}
};
