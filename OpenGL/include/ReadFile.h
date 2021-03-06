//
//  ReadFile.h
//  OpenGL
//
//  Created by xt on 16/10/25
//  Copyright (c) 2016�� xt. All rights reserved.
//
#ifndef __OpenGL_READFILE__
#define __OpenGL_READFILE__

#include <vector>
#include <glm/glm.hpp>

//////////////////////////////////////////////////////////////////////////
//			load obj
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* load obj f: v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3                          */
/************************************************************************/

extern class Mesh;

bool loadOBJ(const char * path,
	std::vector < glm::vec3 > & out_vertices,
	std::vector < glm::vec2 > & out_uvs,
	std::vector < glm::vec3 > & out_normals);

/************************************************************************/
/* load obj f:   // (v v/t v//n v/t/n) = (1 2 3 4) four type is ok,but no other info     */
/************************************************************************/
bool loadOBJ(const char*path, unsigned int &vao, unsigned int& vbo, unsigned int &ibo,
	unsigned int &num_indices);
///////////////////////////////////////////////////////////////////
//          load BMP
////////////////////////////////////////////////////////////////////////// 

unsigned int loadBMP_custom(const char*imagePath);

///////////////////////////////////////////////////////////////////
//          load DDS
////////////////////////////////////////////////////////////////////////// 
unsigned int loadDDS(const char * imagepath);

///////////////////////////////////////////////////////////////////
//          SOIL2
/////////////////////////////////////////////////////////////////////////// 
unsigned int soil_load_DDS(const char* imagePath);


///////////////////////////////////////////////////////////////////
//          Assimp
/////////////////////////////////////////////////////////////////////////// 
bool loadObjByAssimp(const char*fileName,std::vector<Mesh>&meshs, unsigned int steps);
#endif