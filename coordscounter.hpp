#include <cmath>

#include "linalg.hpp"

class CoordsCounter
{
private:
	const float rangeX;
	const float rangeY;
	
	const float lengthX;
	const float lengthY;
	
	const float distance;
	
	const resolution_t resolution;
public:
	CoordsCounter(	const resolution_t& res, float distance = 1,
					float rangeX = 1, float rangeY = 1) :
		resolution(res),
		rangeX(rangeX),
		rangeY(rangeY),
		lengthX(rangeX / res.w),
		lengthY(rangeY / res.h),
		distance(distance)
	{
	}
	
	pixelcoords_t real2pixel(const realcoords_t& coords) const
	{
		return screen2pixel(real2screen(coords));
	}
	
	screencoords_t real2screen(const realcoords_t& coords) const
	{
		float x =   rangeX 
                    * distance / resolution.w 
                    * coords.x / -coords.z;
		float y =   rangeY 
                    * distance / resolution.h 
                    * coords.y / -coords.z;
		
		return {x, y}; 
	}
	
	pixelcoords_t screen2pixel(const screencoords_t& coords) const
	{
		assert(fabsf(coords.x) <= rangeX);
		assert(fabsf(coords.y) <= rangeY);
		
		float px = (coords.x + rangeX) * resolution.w / (2 * rangeX);
		float py = (coords.y + rangeY) * resolution.h / (2 * rangeY);
		
		return {uint16_t(std::lround(px)), uint16_t(std::lround(py))};	
	}
	
	screencoords_t pixel2screen(const pixelcoords_t& coords) const
	{
		assert(coords.x < resolution.w);
		assert(coords.y < resolution.h);
		
		float rx = 2 * rangeX * coords.x / resolution.w - rangeX;
		float ry = 2 * rangeY * coords.y / resolution.h - rangeY;
		
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
