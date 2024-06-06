#define _CRT_SECURE_NO_WARNINGS
#include "obj_to_opengl.hpp"

bool parse_lighthouse(const char* path,
    std::vector < float>& out_vertices,
    std::vector < float>& out_uvs,
    std::vector < float>& out_normals,
    std::vector <int>& number_vertex)
{
    std::vector< unsigned int > vertexIndices1, uvIndices1, normalIndices1;
    std::vector< glm::vec3 > temp_vertices;
    std::vector< glm::vec2 > temp_uvs;
    std::vector< glm::vec3 > temp_normals;

    int texture_number = 0;

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("Impossible to open the file !\n");
        return false;
    }
    int linia = 0;
    while (1) {

        char lineHeader[128];
        char zmienna[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        linia++;
        if (res == EOF)
            break;
        if (strcmp(lineHeader, "usemtl") == 0) {
            fscanf(file, "%s\n", &zmienna);
            if (strcmp(zmienna, "Lighthouse") == 0) texture_number = 1;
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
                std::cout << linia << " , "<<matches<<" , "<< vertexIndex[0]<<","<<uvIndex[0]<<"," << normalIndex[0] << std::endl;
                //return false;
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
        }
    }
    number_vertex.push_back(vertexIndices1.size());
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
    return true;
}

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
    int linia = 0;
    while (1) {

        char lineHeader[128];
        char zmienna[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        linia++;
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
                std::cout << linia << " , "<<matches<<" , "<< vertexIndex[0]<<","<<uvIndex[0]<<"," << normalIndex[0] << std::endl;
                //return false;
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


bool another_parse_from_obj(const char* path,
std::vector < float>& out_vertices,
std::vector < float>& out_uvs,
std::vector < float>& out_normals,
std::vector <int>& number_vertex)
{
    std::vector< unsigned int > vertexIndices1, uvIndices1, normalIndices1, vertexIndices2, uvIndices2, normalIndices2, vertexIndices3, uvIndices3, normalIndices3, vertexIndices4, uvIndices4, normalIndices4, vertexIndices5, uvIndices5, normalIndices5, vertexIndices6, uvIndices6, normalIndices6, vertexIndices7, uvIndices7, normalIndices7, vertexIndices8, uvIndices8, normalIndices8, vertexIndices9, uvIndices9, normalIndices9, vertexIndices10, uvIndices10, normalIndices10, vertexIndices11, uvIndices11, normalIndices11, vertexIndices12, uvIndices12, normalIndices12, vertexIndices13, uvIndices13, normalIndices13, vertexIndices14, uvIndices14, normalIndices14, vertexIndices15, uvIndices15, normalIndices15;
    std::vector< glm::vec3 > temp_vertices;
    std::vector< glm::vec2 > temp_uvs;
    std::vector< glm::vec3 > temp_normals;

    int texture_number = 1;

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("Impossible to open the file !\n");
        return false;
    }
    int linia = 0;
    while (1) {

        char lineHeader[128];
        char zmienna[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        linia++;
        if (res == EOF)
            break;
        if (strcmp(lineHeader, "usemtl") == 0) {
            fscanf(file, "%s\n", &zmienna);
            if (strcmp(zmienna, "M_Ship08_WoodPlain_01") == 0)
            {
                texture_number = 1;
            }
            else if (strcmp(zmienna, "M_Ship08_Rope_02") == 0) texture_number = 2;
            else if (strcmp(zmienna, "M_Ship08_Metal") == 0) texture_number = 3;
            else if (strcmp(zmienna, "M_Ship08_WoodPlain_02") == 0) texture_number = 4;
            else if (strcmp(zmienna, "M_Ship08_Planks") == 0) texture_number = 5;
            else if (strcmp(zmienna, "M_Ship08_WoodPlain_03") == 0) texture_number = 6;
            else if (strcmp(zmienna, "M_Ship08_Rope_01") == 0) texture_number = 7;
            else if (strcmp(zmienna, "M_Ship08_Flag") == 0) texture_number = 8;
            else if (strcmp(zmienna, "M_Ship08_Cannon") == 0) texture_number = 9;
            else if (strcmp(zmienna, "M_Ship08_CannonSupport") == 0) texture_number = 10;
            else if (strcmp(zmienna, "M_Ship08_CannonAxel") == 0) texture_number = 11;
            else if (strcmp(zmienna, "M_Ship08_CannonWheels") == 0) texture_number = 12;
            else if (strcmp(zmienna, "M_Ship08_CannonSides") == 0) texture_number = 13;
            else if (strcmp(zmienna, "M_Ship08_CannonRope") == 0) texture_number = 14;
            else if (strcmp(zmienna, "M_Ship08_WoodPlain_04") == 0) texture_number = 15;
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
                std::cout << linia << " , " << matches << " , " << vertexIndex[0] << "," << uvIndex[0] << "," << normalIndex[0] << std::endl;
                //return false;
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
            else if (texture_number ==2)
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
            else if (texture_number == 3)
            {
                vertexIndices3.push_back(vertexIndex[0]);
                vertexIndices3.push_back(vertexIndex[1]);
                vertexIndices3.push_back(vertexIndex[2]);
                uvIndices3.push_back(uvIndex[0]);
                uvIndices3.push_back(uvIndex[1]);
                uvIndices3.push_back(uvIndex[2]);
                normalIndices3.push_back(normalIndex[0]);
                normalIndices3.push_back(normalIndex[1]);
                normalIndices3.push_back(normalIndex[2]);
            }
            else if (texture_number == 4)
            {
                vertexIndices4.push_back(vertexIndex[0]);
                vertexIndices4.push_back(vertexIndex[1]);
                vertexIndices4.push_back(vertexIndex[2]);
                uvIndices4.push_back(uvIndex[0]);
                uvIndices4.push_back(uvIndex[1]);
                uvIndices4.push_back(uvIndex[2]);
                normalIndices4.push_back(normalIndex[0]);
                normalIndices4.push_back(normalIndex[1]);
                normalIndices4.push_back(normalIndex[2]);
            }
            else if (texture_number == 5)
            {
                vertexIndices5.push_back(vertexIndex[0]);
                vertexIndices5.push_back(vertexIndex[1]);
                vertexIndices5.push_back(vertexIndex[2]);
                uvIndices5.push_back(uvIndex[0]);
                uvIndices5.push_back(uvIndex[1]);
                uvIndices5.push_back(uvIndex[2]);
                normalIndices5.push_back(normalIndex[0]);
                normalIndices5.push_back(normalIndex[1]);
                normalIndices5.push_back(normalIndex[2]);
            }
            else if (texture_number == 6)
            {
                vertexIndices6.push_back(vertexIndex[0]);
                vertexIndices6.push_back(vertexIndex[1]);
                vertexIndices6.push_back(vertexIndex[2]);
                uvIndices6.push_back(uvIndex[0]);
                uvIndices6.push_back(uvIndex[1]);
                uvIndices6.push_back(uvIndex[2]);
                normalIndices6.push_back(normalIndex[0]);
                normalIndices6.push_back(normalIndex[1]);
                normalIndices6.push_back(normalIndex[2]);
            }
            else if (texture_number == 7)
            {
                vertexIndices7.push_back(vertexIndex[0]);
                vertexIndices7.push_back(vertexIndex[1]);
                vertexIndices7.push_back(vertexIndex[2]);
                uvIndices7.push_back(uvIndex[0]);
                uvIndices7.push_back(uvIndex[1]);
                uvIndices7.push_back(uvIndex[2]);
                normalIndices7.push_back(normalIndex[0]);
                normalIndices7.push_back(normalIndex[1]);
                normalIndices7.push_back(normalIndex[2]);
            }
            else if (texture_number == 8)
            {
                vertexIndices8.push_back(vertexIndex[0]);
                vertexIndices8.push_back(vertexIndex[1]);
                vertexIndices8.push_back(vertexIndex[2]);
                uvIndices8.push_back(uvIndex[0]);
                uvIndices8.push_back(uvIndex[1]);
                uvIndices8.push_back(uvIndex[2]);
                normalIndices8.push_back(normalIndex[0]);
                normalIndices8.push_back(normalIndex[1]);
                normalIndices8.push_back(normalIndex[2]);
            }
            else if (texture_number == 9)
            {
                vertexIndices9.push_back(vertexIndex[0]);
                vertexIndices9.push_back(vertexIndex[1]);
                vertexIndices9.push_back(vertexIndex[2]);
                uvIndices9.push_back(uvIndex[0]);
                uvIndices9.push_back(uvIndex[1]);
                uvIndices9.push_back(uvIndex[2]);
                normalIndices9.push_back(normalIndex[0]);
                normalIndices9.push_back(normalIndex[1]);
                normalIndices9.push_back(normalIndex[2]);
            }
            else if (texture_number == 10)
            {
                vertexIndices10.push_back(vertexIndex[0]);
                vertexIndices10.push_back(vertexIndex[1]);
                vertexIndices10.push_back(vertexIndex[2]);
                uvIndices10.push_back(uvIndex[0]);
                uvIndices10.push_back(uvIndex[1]);
                uvIndices10.push_back(uvIndex[2]);
                normalIndices10.push_back(normalIndex[0]);
                normalIndices10.push_back(normalIndex[1]);
                normalIndices10.push_back(normalIndex[2]);
            }
            else if (texture_number == 11)
            {
                vertexIndices11.push_back(vertexIndex[0]);
                vertexIndices11.push_back(vertexIndex[1]);
                vertexIndices11.push_back(vertexIndex[2]);
                uvIndices11.push_back(uvIndex[0]);
                uvIndices11.push_back(uvIndex[1]);
                uvIndices11.push_back(uvIndex[2]);
                normalIndices11.push_back(normalIndex[0]);
                normalIndices11.push_back(normalIndex[1]);
                normalIndices11.push_back(normalIndex[2]);
            }
            else if (texture_number == 12)
            {
                vertexIndices12.push_back(vertexIndex[0]);
                vertexIndices12.push_back(vertexIndex[1]);
                vertexIndices12.push_back(vertexIndex[2]);
                uvIndices12.push_back(uvIndex[0]);
                uvIndices12.push_back(uvIndex[1]);
                uvIndices12.push_back(uvIndex[2]);
                normalIndices12.push_back(normalIndex[0]);
                normalIndices12.push_back(normalIndex[1]);
                normalIndices12.push_back(normalIndex[2]);
            }
            else if (texture_number == 13)
            {
                vertexIndices13.push_back(vertexIndex[0]);
                vertexIndices13.push_back(vertexIndex[1]);
                vertexIndices13.push_back(vertexIndex[2]);
                uvIndices13.push_back(uvIndex[0]);
                uvIndices13.push_back(uvIndex[1]);
                uvIndices13.push_back(uvIndex[2]);
                normalIndices13.push_back(normalIndex[0]);
                normalIndices13.push_back(normalIndex[1]);
                normalIndices13.push_back(normalIndex[2]);
            }
            else if (texture_number == 14)
            {
                vertexIndices14.push_back(vertexIndex[0]);
                vertexIndices14.push_back(vertexIndex[1]);
                vertexIndices14.push_back(vertexIndex[2]);
                uvIndices14.push_back(uvIndex[0]);
                uvIndices14.push_back(uvIndex[1]);
                uvIndices14.push_back(uvIndex[2]);
                normalIndices14.push_back(normalIndex[0]);
                normalIndices14.push_back(normalIndex[1]);
                normalIndices14.push_back(normalIndex[2]);
            }
            else if (texture_number == 15)
            {
                vertexIndices15.push_back(vertexIndex[0]);
                vertexIndices15.push_back(vertexIndex[1]);
                vertexIndices15.push_back(vertexIndex[2]);
                uvIndices15.push_back(uvIndex[0]);
                uvIndices15.push_back(uvIndex[1]);
                uvIndices15.push_back(uvIndex[2]);
                normalIndices15.push_back(normalIndex[0]);
                normalIndices15.push_back(normalIndex[1]);
                normalIndices15.push_back(normalIndex[2]);
            }
        }
    }
    number_vertex.push_back(vertexIndices1.size());
    number_vertex.push_back(vertexIndices2.size());
    number_vertex.push_back(vertexIndices3.size());
    number_vertex.push_back(vertexIndices4.size());
    number_vertex.push_back(vertexIndices5.size());
    number_vertex.push_back(vertexIndices6.size());
    number_vertex.push_back(vertexIndices7.size());
    number_vertex.push_back(vertexIndices8.size());
    number_vertex.push_back(vertexIndices9.size());
    number_vertex.push_back(vertexIndices10.size());
    number_vertex.push_back(vertexIndices11.size());
    number_vertex.push_back(vertexIndices12.size());
    number_vertex.push_back(vertexIndices13.size());
    number_vertex.push_back(vertexIndices14.size());
    number_vertex.push_back(vertexIndices15.size());

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
    for (unsigned int i = 0; i < vertexIndices3.size(); i++) {
        unsigned int vertexIndex = vertexIndices3[i];
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        out_vertices.push_back(vertex.x);
        out_vertices.push_back(vertex.y);
        out_vertices.push_back(vertex.z);
        out_vertices.push_back(1.0f);
    }
    for (unsigned int i = 0; i < uvIndices3.size(); i++) {
        unsigned int vertexIndex = uvIndices3[i];
        glm::vec2 vertex = temp_uvs[vertexIndex - 1];
        out_uvs.push_back(vertex.x);
        out_uvs.push_back(1 - vertex.y);
    }
    for (unsigned int i = 0; i < normalIndices3.size(); i++) {
        unsigned int vertexIndex = normalIndices3[i];
        glm::vec3 vertex = temp_normals[vertexIndex - 1];
        out_normals.push_back(vertex.x);
        out_normals.push_back(vertex.y);
        out_normals.push_back(vertex.z);
        out_normals.push_back(0.0f);
    }
    for (unsigned int i = 0; i < vertexIndices4.size(); i++) {
        unsigned int vertexIndex = vertexIndices4[i];
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        out_vertices.push_back(vertex.x);
        out_vertices.push_back(vertex.y);
        out_vertices.push_back(vertex.z);
        out_vertices.push_back(1.0f);
    }
    for (unsigned int i = 0; i < uvIndices4.size(); i++) {
        unsigned int vertexIndex = uvIndices4[i];
        glm::vec2 vertex = temp_uvs[vertexIndex - 1];
        out_uvs.push_back(vertex.x);
        out_uvs.push_back(1 - vertex.y);
    }
    for (unsigned int i = 0; i < normalIndices4.size(); i++) {
        unsigned int vertexIndex = normalIndices4[i];
        glm::vec3 vertex = temp_normals[vertexIndex - 1];
        out_normals.push_back(vertex.x);
        out_normals.push_back(vertex.y);
        out_normals.push_back(vertex.z);
        out_normals.push_back(0.0f);
    }
    for (unsigned int i = 0; i < vertexIndices5.size(); i++) {
        unsigned int vertexIndex = vertexIndices5[i];
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        out_vertices.push_back(vertex.x);
        out_vertices.push_back(vertex.y);
        out_vertices.push_back(vertex.z);
        out_vertices.push_back(1.0f);
    }
    for (unsigned int i = 0; i < uvIndices5.size(); i++) {
        unsigned int vertexIndex = uvIndices5[i];
        glm::vec2 vertex = temp_uvs[vertexIndex - 1];
        out_uvs.push_back(vertex.x);
        out_uvs.push_back(1 - vertex.y);
    }
    for (unsigned int i = 0; i < normalIndices5.size(); i++) {
        unsigned int vertexIndex = normalIndices5[i];
        glm::vec3 vertex = temp_normals[vertexIndex - 1];
        out_normals.push_back(vertex.x);
        out_normals.push_back(vertex.y);
        out_normals.push_back(vertex.z);
        out_normals.push_back(0.0f);
    }
    for (unsigned int i = 0; i < vertexIndices6.size(); i++) {
        unsigned int vertexIndex = vertexIndices6[i];
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        out_vertices.push_back(vertex.x);
        out_vertices.push_back(vertex.y);
        out_vertices.push_back(vertex.z);
        out_vertices.push_back(1.0f);
    }
    for (unsigned int i = 0; i < uvIndices6.size(); i++) {
        unsigned int vertexIndex = uvIndices6[i];
        glm::vec2 vertex = temp_uvs[vertexIndex - 1];
        out_uvs.push_back(vertex.x);
        out_uvs.push_back(1 - vertex.y);
    }
    for (unsigned int i = 0; i < normalIndices6.size(); i++) {
        unsigned int vertexIndex = normalIndices6[i];
        glm::vec3 vertex = temp_normals[vertexIndex - 1];
        out_normals.push_back(vertex.x);
        out_normals.push_back(vertex.y);
        out_normals.push_back(vertex.z);
        out_normals.push_back(0.0f);
    }
    for (unsigned int i = 0; i < vertexIndices7.size(); i++) {
        unsigned int vertexIndex = vertexIndices7[i];
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        out_vertices.push_back(vertex.x);
        out_vertices.push_back(vertex.y);
        out_vertices.push_back(vertex.z);
        out_vertices.push_back(1.0f);
    }
    for (unsigned int i = 0; i < uvIndices7.size(); i++) {
        unsigned int vertexIndex = uvIndices7[i];
        glm::vec2 vertex = temp_uvs[vertexIndex - 1];
        out_uvs.push_back(vertex.x);
        out_uvs.push_back(1 - vertex.y);
    }
    for (unsigned int i = 0; i < normalIndices7.size(); i++) {
        unsigned int vertexIndex = normalIndices7[i];
        glm::vec3 vertex = temp_normals[vertexIndex - 1];
        out_normals.push_back(vertex.x);
        out_normals.push_back(vertex.y);
        out_normals.push_back(vertex.z);
        out_normals.push_back(0.0f);
    }
    for (unsigned int i = 0; i < vertexIndices8.size(); i++) {
        unsigned int vertexIndex = vertexIndices8[i];
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        out_vertices.push_back(vertex.x);
        out_vertices.push_back(vertex.y);
        out_vertices.push_back(vertex.z);
        out_vertices.push_back(1.0f);
    }
    for (unsigned int i = 0; i < uvIndices8.size(); i++) {
        unsigned int vertexIndex = uvIndices8[i];
        glm::vec2 vertex = temp_uvs[vertexIndex - 1];
        out_uvs.push_back(vertex.x);
        out_uvs.push_back(1 - vertex.y);
    }
    for (unsigned int i = 0; i < normalIndices8.size(); i++) {
        unsigned int vertexIndex = normalIndices8[i];
        glm::vec3 vertex = temp_normals[vertexIndex - 1];
        out_normals.push_back(vertex.x);
        out_normals.push_back(vertex.y);
        out_normals.push_back(vertex.z);
        out_normals.push_back(0.0f);
    }
    for (unsigned int i = 0; i < vertexIndices9.size(); i++) {
        unsigned int vertexIndex = vertexIndices9[i];
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        out_vertices.push_back(vertex.x);
        out_vertices.push_back(vertex.y);
        out_vertices.push_back(vertex.z);
        out_vertices.push_back(1.0f);
    }
    for (unsigned int i = 0; i < uvIndices9.size(); i++) {
        unsigned int vertexIndex = uvIndices9[i];
        glm::vec2 vertex = temp_uvs[vertexIndex - 1];
        out_uvs.push_back(vertex.x);
        out_uvs.push_back(1 - vertex.y);
    }
    for (unsigned int i = 0; i < normalIndices9.size(); i++) {
        unsigned int vertexIndex = normalIndices9[i];
        glm::vec3 vertex = temp_normals[vertexIndex - 1];
        out_normals.push_back(vertex.x);
        out_normals.push_back(vertex.y);
        out_normals.push_back(vertex.z);
        out_normals.push_back(0.0f);
    }
    for (unsigned int i = 0; i < vertexIndices10.size(); i++) {
        unsigned int vertexIndex = vertexIndices10[i];
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        out_vertices.push_back(vertex.x);
        out_vertices.push_back(vertex.y);
        out_vertices.push_back(vertex.z);
        out_vertices.push_back(1.0f);
    }
    for (unsigned int i = 0; i < uvIndices10.size(); i++) {
        unsigned int vertexIndex = uvIndices10[i];
        glm::vec2 vertex = temp_uvs[vertexIndex - 1];
        out_uvs.push_back(vertex.x);
        out_uvs.push_back(1 - vertex.y);
    }
    for (unsigned int i = 0; i < normalIndices10.size(); i++) {
        unsigned int vertexIndex = normalIndices10[i];
        glm::vec3 vertex = temp_normals[vertexIndex - 1];
        out_normals.push_back(vertex.x);
        out_normals.push_back(vertex.y);
        out_normals.push_back(vertex.z);
        out_normals.push_back(0.0f);
    }
    for (unsigned int i = 0; i < vertexIndices11.size(); i++) {
        unsigned int vertexIndex = vertexIndices11[i];
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        out_vertices.push_back(vertex.x);
        out_vertices.push_back(vertex.y);
        out_vertices.push_back(vertex.z);
        out_vertices.push_back(1.0f);
    }
    for (unsigned int i = 0; i < uvIndices11.size(); i++) {
        unsigned int vertexIndex = uvIndices11[i];
        glm::vec2 vertex = temp_uvs[vertexIndex - 1];
        out_uvs.push_back(vertex.x);
        out_uvs.push_back(1 - vertex.y);
    }
    for (unsigned int i = 0; i < normalIndices11.size(); i++) {
        unsigned int vertexIndex = normalIndices11[i];
        glm::vec3 vertex = temp_normals[vertexIndex - 1];
        out_normals.push_back(vertex.x);
        out_normals.push_back(vertex.y);
        out_normals.push_back(vertex.z);
        out_normals.push_back(0.0f);
    }
    for (unsigned int i = 0; i < vertexIndices12.size(); i++) {
        unsigned int vertexIndex = vertexIndices12[i];
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        out_vertices.push_back(vertex.x);
        out_vertices.push_back(vertex.y);
        out_vertices.push_back(vertex.z);
        out_vertices.push_back(1.0f);
    }
    for (unsigned int i = 0; i < uvIndices12.size(); i++) {
        unsigned int vertexIndex = uvIndices12[i];
        glm::vec2 vertex = temp_uvs[vertexIndex - 1];
        out_uvs.push_back(vertex.x);
        out_uvs.push_back(1 - vertex.y);
    }
    for (unsigned int i = 0; i < normalIndices12.size(); i++) {
        unsigned int vertexIndex = normalIndices12[i];
        glm::vec3 vertex = temp_normals[vertexIndex - 1];
        out_normals.push_back(vertex.x);
        out_normals.push_back(vertex.y);
        out_normals.push_back(vertex.z);
        out_normals.push_back(0.0f);
    }
    for (unsigned int i = 0; i < vertexIndices13.size(); i++) {
        unsigned int vertexIndex = vertexIndices13[i];
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        out_vertices.push_back(vertex.x);
        out_vertices.push_back(vertex.y);
        out_vertices.push_back(vertex.z);
        out_vertices.push_back(1.0f);
    }
    for (unsigned int i = 0; i < uvIndices13.size(); i++) {
        unsigned int vertexIndex = uvIndices13[i];
        glm::vec2 vertex = temp_uvs[vertexIndex - 1];
        out_uvs.push_back(vertex.x);
        out_uvs.push_back(1 - vertex.y);
    }
    for (unsigned int i = 0; i < normalIndices13.size(); i++) {
        unsigned int vertexIndex = normalIndices13[i];
        glm::vec3 vertex = temp_normals[vertexIndex - 1];
        out_normals.push_back(vertex.x);
        out_normals.push_back(vertex.y);
        out_normals.push_back(vertex.z);
        out_normals.push_back(0.0f);
    }
    for (unsigned int i = 0; i < vertexIndices14.size(); i++) {
        unsigned int vertexIndex = vertexIndices14[i];
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        out_vertices.push_back(vertex.x);
        out_vertices.push_back(vertex.y);
        out_vertices.push_back(vertex.z);
        out_vertices.push_back(1.0f);
    }
    for (unsigned int i = 0; i < uvIndices14.size(); i++) {
        unsigned int vertexIndex = uvIndices14[i];
        glm::vec2 vertex = temp_uvs[vertexIndex - 1];
        out_uvs.push_back(vertex.x);
        out_uvs.push_back(1 - vertex.y);
    }
    for (unsigned int i = 0; i < normalIndices14.size(); i++) {
        unsigned int vertexIndex = normalIndices14[i];
        glm::vec3 vertex = temp_normals[vertexIndex - 1];
        out_normals.push_back(vertex.x);
        out_normals.push_back(vertex.y);
        out_normals.push_back(vertex.z);
        out_normals.push_back(0.0f);
    }
    for (unsigned int i = 0; i < vertexIndices15.size(); i++) {
        unsigned int vertexIndex = vertexIndices15[i];
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        out_vertices.push_back(vertex.x);
        out_vertices.push_back(vertex.y);
        out_vertices.push_back(vertex.z);
        out_vertices.push_back(1.0f);
    }
    for (unsigned int i = 0; i < uvIndices15.size(); i++) {
        unsigned int vertexIndex = uvIndices15[i];
        glm::vec2 vertex = temp_uvs[vertexIndex - 1];
        out_uvs.push_back(vertex.x);
        out_uvs.push_back(1 - vertex.y);
    }
    for (unsigned int i = 0; i < normalIndices15.size(); i++) {
        unsigned int vertexIndex = normalIndices15[i];
        glm::vec3 vertex = temp_normals[vertexIndex - 1];
        out_normals.push_back(vertex.x);
        out_normals.push_back(vertex.y);
        out_normals.push_back(vertex.z);
        out_normals.push_back(0.0f);
    }
    return true;
}
