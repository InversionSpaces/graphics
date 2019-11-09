#pragma once
#include <cstdio>
#include <fstream>
#include <vector>
#include "linalg.hpp"

struct Mesh
{
    struct vertex
    {
        vec3f pos;
        vec2f tex;
        vec3f norm;
    };

    using uint = unsigned;
    std::vector<vertex> verts;
    std::vector<uint> inds;
};

inline Mesh import_obj(char const *filename)
{
    std::ifstream in(filename);
    Mesh out;
    if(!in.is_open())
        throw std::invalid_argument("can not find file " + std::string(filename));

    std::vector<vec3f> pos;
    std::vector<vec2f> tex;
    std::vector<vec3f> norm;

    std::string line;
    while(std::getline(in, line))
    {
        char const * const cstr = line.c_str();
        
        if(cstr[0] == 'v')
        {
            vec3f v;
            vec2f vt;
            if(sscanf(cstr + 1, " %f %f %f", &v.x, &v.y, &v.z) == 3)
                pos.push_back(v);
            else if(sscanf(cstr + 1, "t %f %f", &vt.x, &vt.y) == 2)
                tex.push_back(vt);
            else if(sscanf(cstr + 1, "n %f %f %f", &v.x, &v.y, &v.z) == 3)
                norm.push_back(v);
        }
        else if(cstr[0] == 'f')
        {	
            Mesh::uint idx[3];
            auto const vsize = out.verts.size();

            char const *cptr = cstr + 2;
            int eaten;
            while(sscanf(cptr, "%u/%u/%u%n", idx, idx + 1, idx + 2, &eaten) == 3)
            {
                out.verts.push_back({pos[idx[0] - 1], tex[idx[1] - 1], norm[idx[2] - 1]});
                cptr += eaten;
            }

            auto const vcount = out.verts.size() - vsize;
            
            if (!vcount) continue;
            
            for(auto i = 1u; i < vcount - 1; i++)
            {
                out.inds.push_back(vsize);
                out.inds.push_back(vsize + i);
                out.inds.push_back(vsize + i + 1);
            }
        }
    }
    
    //std::cout << "here 2" << std::endl;
    
    return out;
}
