#ifndef obj_to_opengl_hpp
#define obj_to_opengl_hpp


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>


bool parse_from_obj(const char* path,
    std::vector < float >& out_vertices,
    std::vector < float >& out_uvs,
    std::vector < float >& out_normals,
    std::vector <int>& number_vertex);

bool another_parse_from_obj(const char* path,
    std::vector < float >& out_vertices,
    std::vector < float >& out_uvs,
    std::vector < float >& out_normals,
    std::vector <int>& number_vertex);

bool parse_lighthouse(const char* path,
    std::vector < float >& out_vertices,
    std::vector < float >& out_uvs,
    std::vector < float >& out_normals,
    std::vector <int>& number_vertex);

#endif
