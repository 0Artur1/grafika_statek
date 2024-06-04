#define _CRT_SECURE_NO_WARNINGS
#include "obj_to_opengl.hpp"

bool parse_from_obj(const char* path,
    std::vector < float>& out_vertices,
    std::vector < float>& out_uvs,
    std::vector < float>& out_normals,
    std::vector <int>& number_vertex)
{
    std::vector< unsigned int > vertexIndices1, uvIndices1, normalIndices1, vertexIndices2, uvIndices2, normalIndices2;
    std::vector< glm::vec3 > temp_vertices;
    std::vector< glm::vec2 > temp_uvs;
    std::vector< glm::vec3 > temp_normals;

    int texture_number = 0;

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("Impossible to open the file !\n");
        return false;
    }

    while (1) {

        char lineHeader[128];
        char zmienna[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break;
        if (strcmp(lineHeader, "usemtl") == 0) {
            fscanf(file, "%s\n", &zmienna);
            if (strcmp(zmienna, "VikingBoat_Mat1") == 0)
            {
                texture_number = 1;
            }
            else texture_number = 2;
        }
        else if (strcmp(lineHeader, "v") == 0) {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader, "vt") == 0) {
            glm::vec3 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            temp_uvs.push_back(uv);
        }
        else if (strcmp(lineHeader, "vn") == 0) {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        else if (strcmp(lineHeader, "f") == 0) {
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            if (matches != 9) {
                printf("File can't be read by our simple parser : ( Try exporting with other options\n");
                return false;
            }
            if (texture_number == 1)
            {
                vertexIndices1.push_back(vertexIndex[0]);
                vertexIndices1.push_back(vertexIndex[1]);
                vertexIndices1.push_back(vertexIndex[2]);
                uvIndices1.push_back(uvIndex[0]);
                uvIndices1.push_back(uvIndex[1]);
                uvIndices1.push_back(uvIndex[2]);
                normalIndices1.push_back(normalIndex[0]);
                normalIndices1.push_back(normalIndex[1]);
                normalIndices1.push_back(normalIndex[2]);
            }
            else
            {
                vertexIndices2.push_back(vertexIndex[0]);
                vertexIndices2.push_back(vertexIndex[1]);
                vertexIndices2.push_back(vertexIndex[2]);
                uvIndices2.push_back(uvIndex[0]);
                uvIndices2.push_back(uvIndex[1]);
                uvIndices2.push_back(uvIndex[2]);
                normalIndices2.push_back(normalIndex[0]);
                normalIndices2.push_back(normalIndex[1]);
                normalIndices2.push_back(normalIndex[2]);
            }
        }
    }
    number_vertex.push_back(vertexIndices1.size());
    number_vertex.push_back(vertexIndices2.size());
    for (unsigned int i = 0; i < vertexIndices1.size(); i++) {
        unsigned int vertexIndex = vertexIndices1[i];
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        out_vertices.push_back(vertex.x);
        out_vertices.push_back(vertex.y);
        out_vertices.push_back(vertex.z);
        out_vertices.push_back(1.0f);
    }
    for (unsigned int i = 0; i < uvIndices1.size(); i++) {
        unsigned int vertexIndex = uvIndices1[i];
        glm::vec2 vertex = temp_uvs[vertexIndex - 1];
        out_uvs.push_back(vertex.x);
        out_uvs.push_back(1 - vertex.y);
    }
    for (unsigned int i = 0; i < normalIndices1.size(); i++) {
        unsigned int vertexIndex = normalIndices1[i];
        glm::vec3 vertex = temp_normals[vertexIndex - 1];
        out_normals.push_back(vertex.x);
        out_normals.push_back(vertex.y);
        out_normals.push_back(vertex.z);
        out_normals.push_back(0.0f);
    }
    for (unsigned int i = 0; i < vertexIndices2.size(); i++) {
        unsigned int vertexIndex = vertexIndices2[i];
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        out_vertices.push_back(vertex.x);
        out_vertices.push_back(vertex.y);
        out_vertices.push_back(vertex.z);
        out_vertices.push_back(1.0f);
    }
    for (unsigned int i = 0; i < uvIndices2.size(); i++) {
        unsigned int vertexIndex = uvIndices2[i];
        glm::vec2 vertex = temp_uvs[vertexIndex - 1];
        out_uvs.push_back(vertex.x);
        out_uvs.push_back(1 - vertex.y);
    }
    for (unsigned int i = 0; i < normalIndices2.size(); i++) {
        unsigned int vertexIndex = normalIndices2[i];
        glm::vec3 vertex = temp_normals[vertexIndex - 1];
        out_normals.push_back(vertex.x);
        out_normals.push_back(vertex.y);
        out_normals.push_back(vertex.z);
        out_normals.push_back(0.0f);
    }
    return true;
}