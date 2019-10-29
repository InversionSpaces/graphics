#pragma once

#include <cmath>
#include <cassert>
#include <vector>

#include "linalg.hpp"

using namespace std;

class Rasterizer
{
private:
	float sx, sy, wover2, hover2;

	inline int x_s2p(const float& coord)
	{
		//assert(fabsf(coord) < 1.f);
		
		return lround((coord + 1.f) * wover2 + sx);
	}
	
	inline int y_s2p(const float& coord)
	{
		//assert(fabsf(coord) < 1.f);
		
		return lround((coord + 1.f) * hover2 + sy);
	}
	
	inline float x_p2s(const int& coord)
	{
		//assert(coord - sx > 0.5f);
		//assert(coord - sx < wover2 * 2 + 0.5f);
		
		return (coord - sx) / wover2 - 1.f;
	}
	
	inline float y_p2s(const int& coord)
	{
		//assert(coord - sy > 0.5f);
		//assert(coord - sy < hover2 * 2 + 0.5f);
		
		return (coord - sy) / hover2 - 1.f;
	}
public:
	
	inline void set_view(int x, int y, int w, int h)
	{
		sx = x - 0.5f;
		sy = y - 0.5f;
		
		wover2 = w * 0.5f;
		hover2 = h * 0.5f;
	}
	
	struct rastout {
		int x, y;
		float depth, b, c;
	};
	
	inline vec<float, 3> vec4to3(const vec<float, 4> v)
	{
		return {v.x / v.w, v.y / v.w, v.z / v.w};
	}
	
	inline void rasterize(vec4f vs[3], vector<rastout>& rout)
	{
		vec3f const v[3] = {vec4to3(vs[0]), 
									vec4to3(vs[1]),
									vec4to3(vs[2])};
		
		const float ax = v[1].x - v[0].x;
		const float ay = v[1].y - v[0].y;
		
		const float bx = v[2].x - v[0].x;
		const float by = v[2].y - v[0].y;
		
		const float det = ax * by - bx * ay;
		
		auto const clamp = [] (float const x)
		{
			float const eps = 1e-6;
			return 	x >= 1.f ? 1.f - eps : (x <= -1.f ? -1.f + eps : x);
		};
		
		const int xmin = x_s2p(clamp(min(min(v[0].x, v[1].x), v[2].x)));
        const int xmax = x_s2p(clamp(max(max(v[0].x, v[1].x), v[2].x)));
        
        const int ymin = y_s2p(clamp(min(min(v[0].y, v[1].y), v[2].y)));
        const int ymax = y_s2p(clamp(max(max(v[0].y, v[1].y), v[2].y)));
        
        for (int x = xmin; x <= xmax; ++x) {
			const float cx = x_p2s(x) - v[0].x;
			for (int y = ymin; y <= ymax; ++y) {
				const float cy = y_p2s(y) - v[0].y;
				
				const float det1 = cx * by - cy * bx;
				const float det2 = ax * cy - ay * cx;
				
				const float b0 = det1 / det;
				const float c0 = det2 / det;
				const float a0 = 1.f - b0 - c0;
				
				if (a0 < 0 || b0 < 0 || c0 < 0) 
					continue;
					
				const float depth = v[0].z * a0 + 
									v[1].z * b0 + 
									v[2].z * c0;
				
				const float a = a0 / vs[0].w;
				const float b = b0 / vs[1].w;
				const float c = c0 / vs[2].w;
				
				const float sum = a + b + c;
				
				rout.push_back({x, y, depth, b / sum, c / sum});
			}
		}
	}
};
