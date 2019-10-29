#include <iostream>

#include "xwindow.hpp"
#include "rasterizer.hpp"
#include "wfobj.hpp"

Mesh::vertex get_mixed(const Mesh::vertex vs[3], const float b, const float c)
{
	const float a = 1.f - b - c;
	
	Mesh::vertex retval = {};
	
	const float* v0 = reinterpret_cast<const float *>(vs + 0);
	const float* v1 = reinterpret_cast<const float *>(vs + 1);
	const float* v2 = reinterpret_cast<const float *>(vs + 2);
	
	float* r = reinterpret_cast<float *>(&retval);
	
	for (int i = 0; i < sizeof(Mesh::vertex) / sizeof(float); ++i)
		r[i] = a * v0[i] + b * v1[i] + c * v2[i];
		
	return retval;
}

int main() {
	Mesh mesh = import_obj("cat.obj");
	
	const vec3f move = {0.f, -1.f, -2.f};
	const vec3f campos = {0.f, 0.f, 0.f};
	const vec3f light = (vec3f{0.f, 0.f, 1.f}).normalized();	
	
	XWindow xw;
	Rasterizer rast;
	
	const int w = xw.width();
	const int h = xw.height();
	
	const float ratio = static_cast<float>(w) / h;
	
	rast.set_view(0, 0, w, h);
	
	float const near = 0.2f;
    float const far  = 5.f;
    
    float const c1 = (far + near) / (far - near);
    float const c2 = 2.f * near * far / (far - near);
		
	vector<Rasterizer::rastout> rout;
	
	vector<float> depth;
	depth.assign(w * h, 1.f);
	
	for (int i = 0; i < mesh.inds.size(); i += 3) {
		Mesh::vertex vs[3] = {
			mesh.verts[mesh.inds[i]],
			mesh.verts[mesh.inds[i + 1]],
			mesh.verts[mesh.inds[i + 2]]
		};
		
		vec4f p[3];
		
		for (int j = 0; j < 3; ++j) {
			const vec3f r = vs[j].pos + move - campos;
			
			p[j].x = -r.x / ratio;
			p[j].y = -r.y;
			p[j].z = c1 * r.z + c2;
			p[j].w = r.z;
		}
		
		rast.rasterize(p, rout);
		
		//cout << rout.size() << endl;
		
		for (auto o: rout) {
			//cout << o.x << " " << o.y << endl;
			
			if (depth[w * o.y + o.x] < o.depth || o.depth < -1.f)
				continue;
			
			depth[w * o.y + o.x] = o.depth;
			
			Mesh::vertex vo = get_mixed(vs, o.b, o.c);
			
			const vec3f cat_color = {1.f, 0.f, 1.f};
			
			const float nlight = max(0.f, light * vo.norm);
			
			const vec3f color = cat_color * nlight;
			
			xw[{o.x, o.y}] = { 	
				static_cast<uint8_t>(color.x * 255u), 
				static_cast<uint8_t>(color.y * 255u),
				static_cast<uint8_t>(color.z * 255u),
				10
			};
		}
		
		rout.clear();
	}
	
	xw.update();
	
	while (1) {}
}
