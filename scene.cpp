//
// Created by christina on 20.06.15.
//


/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define GLM_FORCE_RADIANS  /* Use radians in all GLM functions */

/* GLM includes - adjust path as required for local installation */
#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/gtc/matrix_transform.hpp" /* Provides glm::translate, glm::rotate,
                                         * glm::scale, glm::perspective */
#include "glm/gtc/type_ptr.hpp"         /* Vector/matrix handling */

/* OpenGL includes */
#include <GL/glew.h>
#include <GL/freeglut.h>

/* Local includes */
extern "C"
{
#include "src/LoadShader.h"   /* Provides loading function for shader code */
#include "src/Matrix.h"
#include "src/OBJParser.h"     /* Loading function for triangle meshes in OBJ format */
}

/* Define handle to a vertex buffer object */
GLuint VBO_cube, VBO_platform, VBO_roof, VBO_model, VBO_billboard;

/* Define handle to a color buffer object */
GLuint CBO_cube, CBO_platform, CBO_roof, CBO_floor;

/* Define handle to an index buffer object */
GLuint IBO_cube, IBO_platform, IBO_roof, IBO_model;

/* Define handle to normal buffer objects */
GLuint NBO_roof, NBO_cube, NBO_platform;

/* Define handle to uv-buffers */
GLuint UV_cube, UV_roof, UV_billboard;

GLuint VAO_c, VAO_p, VAO_r, VAO_f, VAO_m, VAO_b;

/* Indices to vertex attributes; in this case positon, color, normals and UVs */
enum DataID {vPosition = 0, vColor = 1, vNormal = 2, vUV = 3};

/* Buffer for loading a .obj model */
GLfloat *vertex_buffer_data1;
GLushort *index_buffer_data1;
obj_scene_data data1;

/* Define normal buffers */
glm::vec3 *normal_buffer_cube;
glm::vec3 *normal_buffer_platform;
glm::vec3 *normal_buffer_roof;

GLfloat vertex_buffer_billboard[] = {
        -1.0f, 3.0f, 3.0f,
        1.0f, 3.0f, 3.0f,
        -1.0f,  4.5f, 3.0f,
        1.0f,  4.5f, 3.0f,
};

GLfloat uv_buffer_billboard[] = {
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 1.0
};

glm::vec3 vertex_buffer_cube[] = { /* 8 cube vertices XYZ */
        glm::vec3(-1.0, -1.0,  1.0),
        glm::vec3(1.0, -1.0,  1.0),
        glm::vec3(1.0,  1.0,  1.0),
        glm::vec3(-1.0,  1.0,  1.0),
        glm::vec3(-1.0, -1.0, -1.0),
        glm::vec3(1.0, -1.0, -1.0),
        glm::vec3(1.0,  1.0, -1.0),
        glm::vec3(-1.0,  1.0, -1.0)
};

GLfloat uv_buffer_cube[] = {
        0.0, 0.0,
        1.0, 0.0,
        1.0, 0.0,
        0.0, 0.0,
        0.0, 1.0,
        1.0, 1.0,
        1.0, 1.0,
        0.0, 1.0
};


GLfloat color_buffer_cube[] = { /* RGB color values for 8 vertices */
        0.0, 0.0, 1.0,
        1.0, 0.0, 1.0,
        1.0, 1.0, 1.0,
        0.0, 1.0, 1.0,
        0.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
};

GLfloat color_buffer_floor[] = { /* RGB color values for 8 vertices */
        220.0/255, 245.0/255, 160.0/255,
        220.0/255, 245.0/255, 160.0/255,
        220.0/255, 245.0/255, 160.0/255,
        220.0/255, 245.0/255, 160.0/255,
        220.0/255, 245.0/255, 160.0/255,
        220.0/255, 245.0/255, 160.0/255,
        220.0/255, 245.0/255, 160.0/255,
        220.0/255, 245.0/255, 160.0/255
};

GLushort index_buffer_cube[] = { /* Indices of 6*2 triangles (6 sides) */
        3, 2, 6,
        6, 7, 3,
        0, 1, 2,
        2, 3, 0,
        1, 5, 6,
        6, 2, 1,
        7, 6, 5,
        5, 4, 7,
        4, 0, 3,
        3, 7, 4,
        4, 5, 1,
        1, 0, 4,
};

glm::vec3 vertex_buffer_platform[] = {
        glm::vec3(0, 0, 0),
        glm::vec3(4, 0, 7),
        glm::vec3(8, 0, 0),
        glm::vec3(4, 0 , -7),
        glm::vec3(-4, 0, -7),
        glm::vec3(-8, 0, 0),
        glm::vec3(-4, 0, 7),
        glm::vec3(0, -1, 0),
        glm::vec3(4, -1, 7),
        glm::vec3(8, -1, 0),
        glm::vec3(4, -1 , -7),
        glm::vec3(-4, -1, -7),
        glm::vec3(-8, -1, 0),
        glm::vec3(-4, -1, 7)
};


GLfloat color_buffer_platform[] = {
        0.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0
};

GLushort index_buffer_platform[] = {
        0,1,2,  // top
        0,2,3,
        0,3,4,
        0,4,5,
        0,5,6,
        0,6,1,
        7,9,8, // bottom
        7,10,9,
        7,11,10,
        7,12,11,
        7,13,12,
        7,8,13,
        9,2,1, //sides
        8,9,1,
        10,3,2,
        9,10,2,
        11,4,3,
        10,11,3,
        12,5,4,
        11,12,4,
        13,6,5,
        12,13,5,
        8,1,6,
        13,8,6
};

glm::vec3 vertex_buffer_roof[] = {
        glm::vec3(0, 5, 0),
        glm::vec3(4, 0, 7),
        glm::vec3(8, 0, 0),
        glm::vec3(4, 0 , -7),
        glm::vec3(-4, 0, -7),
        glm::vec3(-8, 0, 0),
        glm::vec3(-4, 0, 7),
        glm::vec3(0, -1, 0),
        glm::vec3(4, -1, 7),
        glm::vec3(8, -1, 0),
        glm::vec3(4, -1 , -7),
        glm::vec3(-4, -1, -7),
        glm::vec3(-8, -1, 0),
        glm::vec3(-4, -1, 7)
};

GLfloat uv_buffer_roof[] = {
        0.5, 0.5,
        0.75, 1,
        1.0, 0.5,
        0.75, 0.0,
        0.25, 0.0,
        0.0, 0.5,
        0.25, 1.0,
        0.0, 0.0,
        0.0, 1.0,
        1.0, 1.0,
        1.0, 1.0,
        0.0, 1.0,
        1.0, 1.0,
        0.0, 1.0
};

GLfloat color_buffer_roof[] = {
        0.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0
};

GLushort index_buffer_roof[] = {
        0,1,2,  // top
        0,2,3,
        0,3,4,
        0,4,5,
        0,5,6,
        0,6,1,
        7,9,8, // bottom
        7,10,9,
        7,11,10,
        7,12,11,
        7,13,12,
        7,8,13,
        9,2,1, //sides
        8,9,1,
        10,3,2,
        9,10,2,
        11,4,3,
        10,11,3,
        12,5,4,
        11,12,4,
        13,6,5,
        12,13,5,
        8,1,6,
        13,8,6
};


/******************************************************************
*
* SetupDataBuffers
*
* Create buffer objects and load data into buffers
*
*******************************************************************/

void SetupDataBuffers()
{
        /* billboard */
        glGenBuffers(1, &VBO_billboard);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_billboard);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_billboard), vertex_buffer_billboard, GL_STATIC_DRAW);

        glGenBuffers(1, &UV_billboard);
        glBindBuffer(GL_ARRAY_BUFFER, UV_billboard);
        glBufferData(GL_ARRAY_BUFFER, sizeof(uv_buffer_billboard), uv_buffer_billboard, GL_STATIC_DRAW);

        /* cube */
        glGenBuffers(1, &VBO_cube);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_cube), vertex_buffer_cube, GL_STATIC_DRAW);

        glGenBuffers(1, &IBO_cube);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_cube);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_cube), index_buffer_cube, GL_STATIC_DRAW);

        glGenBuffers(1, &CBO_cube);
        glBindBuffer(GL_ARRAY_BUFFER, CBO_cube);
        glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_cube), color_buffer_cube, GL_STATIC_DRAW);

        glGenBuffers(1, &NBO_cube);
        glBindBuffer(GL_ARRAY_BUFFER, NBO_cube);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*8, normal_buffer_cube, GL_STATIC_DRAW);

        glGenBuffers(1, &UV_cube);
        glBindBuffer(GL_ARRAY_BUFFER, UV_cube);
        glBufferData(GL_ARRAY_BUFFER, sizeof(uv_buffer_cube), uv_buffer_cube, GL_STATIC_DRAW);

        /* platform */
        glGenBuffers(1, &VBO_platform);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_platform);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_platform), vertex_buffer_platform, GL_STATIC_DRAW);

        glGenBuffers(1, &IBO_platform);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_platform);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_platform), index_buffer_platform, GL_STATIC_DRAW);

        glGenBuffers(1, &CBO_platform);
        glBindBuffer(GL_ARRAY_BUFFER, CBO_platform);
        glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_platform), color_buffer_platform, GL_STATIC_DRAW);

        glGenBuffers(1, &NBO_platform);
        glBindBuffer(GL_ARRAY_BUFFER, NBO_platform);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*14, normal_buffer_platform, GL_STATIC_DRAW);

        /* roof */
        glGenBuffers(1, &VBO_roof);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_roof);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_roof), vertex_buffer_roof, GL_STATIC_DRAW);

        glGenBuffers(1, &IBO_roof);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_roof);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_roof), index_buffer_roof, GL_STATIC_DRAW);

        glGenBuffers(1, &CBO_roof);
        glBindBuffer(GL_ARRAY_BUFFER, CBO_roof);
        glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_roof), color_buffer_roof, GL_STATIC_DRAW);

        glGenBuffers(1, &UV_roof);
        glBindBuffer(GL_ARRAY_BUFFER, UV_roof);
        glBufferData(GL_ARRAY_BUFFER, sizeof(uv_buffer_roof), uv_buffer_roof, GL_STATIC_DRAW);

        glGenBuffers(1, &NBO_roof);
        glBindBuffer(GL_ARRAY_BUFFER, NBO_roof);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*14, normal_buffer_roof, GL_STATIC_DRAW);

        /* floor */
        glGenBuffers(1, &CBO_floor);
        glBindBuffer(GL_ARRAY_BUFFER, CBO_floor);
        glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_floor), color_buffer_floor, GL_STATIC_DRAW);

        /* Model */
        glGenBuffers(1, &VBO_model);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_model);
        glBufferData(GL_ARRAY_BUFFER, data1.vertex_count*3*sizeof(GLfloat), vertex_buffer_data1, GL_STATIC_DRAW);

        glGenBuffers(1, &IBO_model);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_model);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, data1.face_count*3*sizeof(GLushort), index_buffer_data1, GL_STATIC_DRAW);
}

void SetupVertexArrayObjects() {
        glGenVertexArrays(1, &VAO_b);
        glGenVertexArrays(1, &VAO_p);
        glGenVertexArrays(1, &VAO_c);
        glGenVertexArrays(1, &VAO_r);
        glGenVertexArrays(1, &VAO_f);
        glGenVertexArrays(1, &VAO_m);
        GLint size; // don't really need this anymore, maybe change it to passing 0 in the glGetBufferParameteriv

        /* billboard */
        glBindVertexArray(VAO_b);

        glEnableVertexAttribArray(vPosition);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_billboard);
        glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(vUV);
        glBindBuffer(GL_ARRAY_BUFFER, UV_billboard);
        glVertexAttribPointer(vUV, 2, GL_FLOAT, GL_FALSE, 0, 0);

        /* platform */
        glBindVertexArray(VAO_p);

        glEnableVertexAttribArray(vPosition);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_platform);
        glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(vColor);
        glBindBuffer(GL_ARRAY_BUFFER, CBO_platform);
        glVertexAttribPointer(vColor, 3, GL_FLOAT,GL_FALSE, 0, 0);

        glEnableVertexAttribArray(vNormal);
        glBindBuffer(GL_ARRAY_BUFFER, NBO_platform);
        glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(vUV);
        glBindBuffer(GL_ARRAY_BUFFER, UV_roof);
        glVertexAttribPointer(vUV, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_platform);
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

        /* cube */
        glBindVertexArray(VAO_c);

        glEnableVertexAttribArray(vPosition);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
        glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(vColor);
        glBindBuffer(GL_ARRAY_BUFFER, CBO_cube);
        glVertexAttribPointer(vColor, 3, GL_FLOAT,GL_FALSE, 0, 0);

        glEnableVertexAttribArray(vNormal);
        glBindBuffer(GL_ARRAY_BUFFER, NBO_cube);
        glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_cube);
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

        /* roof */
        glBindVertexArray(VAO_r);

        glEnableVertexAttribArray(vPosition);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_roof);
        glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(vColor);
        glBindBuffer(GL_ARRAY_BUFFER, CBO_roof);
        glVertexAttribPointer(vColor, 3, GL_FLOAT,GL_FALSE, 0, 0);

        glEnableVertexAttribArray(vNormal);
        glBindBuffer(GL_ARRAY_BUFFER, NBO_roof);
        glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(vUV);
        glBindBuffer(GL_ARRAY_BUFFER, UV_roof);
        glVertexAttribPointer(vUV, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_roof);
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

        /* floor */
        glBindVertexArray(VAO_f);

        glEnableVertexAttribArray(vPosition);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
        glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(vColor);
        glBindBuffer(GL_ARRAY_BUFFER, CBO_floor);
        glVertexAttribPointer(vColor, 3, GL_FLOAT,GL_FALSE, 0, 0);

        glEnableVertexAttribArray(vNormal);
        glBindBuffer(GL_ARRAY_BUFFER, NBO_cube);
        glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(vUV);
        glBindBuffer(GL_ARRAY_BUFFER, UV_cube);
        glVertexAttribPointer(vUV, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_cube);
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

        /* model */
        glBindVertexArray(VAO_m);

        glEnableVertexAttribArray(vPosition);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_model);
        glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(vColor);
        glBindBuffer(GL_ARRAY_BUFFER, CBO_cube);
        glVertexAttribPointer(vColor, 3, GL_FLOAT,GL_FALSE, 0, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_model);
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

}

/******************************************************************
*
* computeNormals
*
* This function is called to compute the normals for a
* specified vertexBuffer and indexBuffer
*
*******************************************************************/
void computeNormals(glm::vec3* vertexBuffer, int vertexBufferSize, GLushort* indexBuffer, int indexSize, glm::vec3** normalBuffer) {
        int i;
        // initialize normalbuffer with zeros
        glm::vec3 *localBuffer = (glm::vec3*) calloc(vertexBufferSize, sizeof(glm::vec3));

        for(i = 0; i < indexSize; i += 3) {
                // get the three vertices that make the faces
                glm::vec3 p1 = glm::vec3(vertexBuffer[indexBuffer[i+0]]);
                glm::vec3 p2 = glm::vec3(vertexBuffer[indexBuffer[i+1]]);
                glm::vec3 p3 = glm::vec3(vertexBuffer[indexBuffer[i+2]]);

                glm::vec3 v1 = p2 - p1;
                glm::vec3 v2 = p3 - p1;
                glm::vec3 normal = glm::cross(v1, v2);
                normal = glm::normalize(normal);

                // Store the face's normal for each of the vertices that make up the face.
                localBuffer[indexBuffer[i+0]] += normal;
                localBuffer[indexBuffer[i+1]] += normal;
                localBuffer[indexBuffer[i+2]] += normal;
        }

        // Normalize vertex normals
        for(i = 0; i < vertexBufferSize; i++) {
                localBuffer[i] = glm::normalize(localBuffer[i]);
                //printf("[%f, %f, %f] \n", (localBuffer[i]).x, (localBuffer[i]).y, (localBuffer[i]).z);
        }
        printf("\n");
        *normalBuffer = localBuffer;
}

/* Set Transformation for floor */
void transformationInit_floor(float* result)
{
        float scaling[16];
        float translation[16];
        SetScaling(3, 0.1, 3, scaling);
        SetTranslation(0, -0.5, 0, translation);
        MultiplyMatrix(translation, scaling, result);
}

void transformationInit_roof(float* result) {
        float scaling[16];
        float translation[16];
        SetScaling(0.25, 0.25, 0.25, scaling);
        SetTranslation(0,2,0, translation);
        MultiplyMatrix(translation, scaling, result);
}

void transformationInit_platform(float* result) {
        SetScaling(0.25, 0.25, 0.25, result);
}

void transformationInit_poles(float result[6][16]) {
        float scaling[16];
        float translation[16];
        SetScaling(0.005, 2, 0.005, scaling);
        int i;
        float transX, transZ;
        float transY = 2;
        for(i = 0; i < 6; i++) {
                transX = vertex_buffer_platform[(i+1)].x/4;
                transZ = vertex_buffer_platform[(i+1)].z/4;
                SetTranslation(transX, transY, transZ, translation);
                MultiplyMatrix(translation, scaling, result[i]);
        }
}

void transformationInit_middlePole(float *result) {
        float scaling[16];
        float translation[16];
        SetScaling(0.01, 2, 0.01, scaling);
        SetTranslation(0, 2, 0, translation);
        MultiplyMatrix(translation, scaling, result);
}

void transformationInit_cube_model(float result_cubes[6][16], float result_teapots[6][16], float angle) {
	 /* Set Transformation for cubes that additionally rotate around themselves */
	 float rotationX[16];
	 float rotationZ[16];
	 float rotationY[16];
	 float translation[16];
	 float scaling[16];
    SetRotationX(-45, rotationX);
    SetRotationZ(35, rotationZ);

    float transX, transZ;
    float transY = 0.4;
    for(int i = 0; i < 6; i++) {
        /* translate the points to the inner platform */
        transX = vertex_buffer_platform[(i+1)].x/6;
        transZ = vertex_buffer_platform[(i+1)].z/6;
        SetTranslation(transX, transY, transZ, translation);

        /* set the rotation for each cube, they will rotate in the opposite direction of the platform and each cube rotates at a different speed
         * (1. and 4. cube twice the speed of the platform, 2. and 5. four times the speed of the platform and the 3. and 6. cube eight times the speed)
         */
        SetScaling(0.25, 0.25, 0.25, scaling);
        SetRotationY(-2*angle * (i%3 +1), rotationY);
        MultiplyMatrix(rotationX, scaling, result_cubes[i]);
        MultiplyMatrix(rotationZ, result_cubes[i], result_cubes[i]);
        MultiplyMatrix(rotationY, result_cubes[i], result_cubes[i]);
        MultiplyMatrix(translation, result_cubes[i], result_cubes[i]);
        
        /* Set ModelMatrices for alternative Objects */
        SetScaling(0.2, 0.2, 0.2, scaling);
        MultiplyMatrix(rotationY, scaling, result_teapots[i]);
        MultiplyMatrix(translation, result_teapots[i], result_teapots[i]);

    }
}

int count_cube() {
        return sizeof(index_buffer_cube)/sizeof(GLushort);
}

int count_roof() {
        return sizeof(index_buffer_roof)/sizeof(GLushort);
}

int count_teapots() {
	return data1.face_count * 3;
}

void initVAOs(GLuint *VAO_cube_ptr, GLuint *VAO_roof_ptr, GLuint *VAO_platform_ptr, GLuint *VAO_floor_ptr, GLuint *VAO_model_ptr, GLuint *VAO_billboard_ptr) {
        int i;
        int success;

        /* Load first OBJ model */
        char* filename1 = "models/teapot.obj";
        success = parse_obj_scene(&data1, filename1);

        if(!success)
                printf("Could not load file. Exiting.\n");

        /*  Copy mesh data from structs into appropriate arrays */
        int vert = data1.vertex_count;
        int indx = data1.face_count;

        vertex_buffer_data1 = (GLfloat*) calloc (vert*3, sizeof(GLfloat));
        index_buffer_data1 = (GLushort*) calloc (indx*3, sizeof(GLushort));

        /* Vertices */
        for(i=0; i<vert; i++)
        {
                vertex_buffer_data1[i*3] = (GLfloat)(*data1.vertex_list[i]).e[0];
                vertex_buffer_data1[i*3+1] = (GLfloat)(*data1.vertex_list[i]).e[1];
                vertex_buffer_data1[i*3+2] = (GLfloat)(*data1.vertex_list[i]).e[2];
        }

        /* Indices */
        for(i=0; i<indx; i++)
        {
                index_buffer_data1[i*3] = (GLushort)(*data1.face_list[i]).vertex_index[0];
                index_buffer_data1[i*3+1] = (GLushort)(*data1.face_list[i]).vertex_index[1];
                index_buffer_data1[i*3+2] = (GLushort)(*data1.face_list[i]).vertex_index[2];
        }

        /* Compute normals */
        computeNormals(vertex_buffer_cube, sizeof(vertex_buffer_cube)/sizeof(glm::vec3), index_buffer_cube, sizeof(index_buffer_cube)/sizeof(GLushort), &normal_buffer_cube);
        computeNormals(vertex_buffer_roof, sizeof(vertex_buffer_roof)/sizeof(glm::vec3), index_buffer_roof, sizeof(index_buffer_roof)/sizeof(GLushort), &normal_buffer_roof);
        computeNormals(vertex_buffer_platform, sizeof(vertex_buffer_platform)/sizeof(glm::vec3), index_buffer_platform, sizeof(index_buffer_platform)/sizeof(GLushort), &normal_buffer_platform);

        /* Setup vertex, color, and index buffer objects */
        SetupDataBuffers();
        SetupVertexArrayObjects();

        *VAO_model_ptr = VAO_m;
        *VAO_platform_ptr = VAO_p;
        *VAO_cube_ptr = VAO_c;
        *VAO_floor_ptr = VAO_f;
        *VAO_roof_ptr = VAO_r;
        *VAO_billboard_ptr = VAO_b;
}


