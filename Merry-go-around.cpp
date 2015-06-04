/******************************************************************
*
* RotatingCube.c
*
* Description: This example demonstrates a colored, rotating
* cube in shader-based OpenGL. The use of transformation
* matrices, perspective projection, and indexed triangle sets 
* are shown.
*
* Computer Graphics Proseminar SS 2015
* 
* Interactive Graphics and Simulation Group
* Institute of Computer Science
* University of Innsbruck
*
*******************************************************************/


/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define GLM_FORCE_RADIANS  /* Use radians in all GLM functions */

/* GLM includes - adjust path as required for local installation */
#include "glm/glm.hpp"
#include "glm/ext.hpp"
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


/*----------------------------------------------------------------*/

/* Flag for starting/stopping animation */
GLboolean anim = GL_TRUE;

/* Define handle to a vertex buffer object */
GLuint VBO_cube, VBO_platform, VBO_roof, VBO_model;

/* Define handle to a color buffer object */
GLuint CBO_cube, CBO_platform, CBO_roof, CBO_floor;

/* Define handle to an index buffer object */
GLuint IBO_cube, IBO_platform, IBO_roof, IBO_model;

/* Define handle to normal buffer objects */
GLuint NBO_roof, NBO_cube, NBO_platform;

GLuint VAO_cube, VAO_platform, VAO_roof, VAO_floor, VAO_model;

/* Indices to vertex attributes; in this case positon and color */
enum DataID {vPosition = 0, vColor = 1, vNormal = 2};

/* Strings for loading and storing shader code */
static const char* VertexShaderString;
static const char* FragmentShaderString;

GLuint ShaderProgram;

float ProjectionMatrix[16]; /* Perspective projection matrix */
float ViewMatrix[16]; /* Camera view matrix */
float ModelMatrixPole[6][16], ModelMatrixPlatform[16], ModelMatrixRoof[16], ModelMatrixMiddlePole[16], ModelMatrixCubes[6][16], ModelMatrixOther[6][16];
float ModelMatrixFloor[16];

float RotationMatrixAnimX[16];
float RotationMatrixAnimY[16];
float RotationMatrixAnimZ[16];
float RotationMatrixAnimCamera[16];
float RotationMatrixAnimRound[16];

/* Variables for storing current rotation angles */
float angleX, angleY, angleZ, angle = 0.0f;

/* Indices to active rotation axes */
enum {Xaxis=0, Yaxis=1, Zaxis=2};
int axis = 1;
int velocity = 1;

/* Reference time for animation */
int oldTime = 0;

float camera_disp = 0.0f;
float camera_up = 0.0f;

/* Buffer for loading a .obj model */
GLfloat *vertex_buffer_data1;
GLushort *index_buffer_data1;
obj_scene_data data1;

/* Indices to activate either the cubes or some other model which is loaded from a .obj file */
enum {Cubes=0, Other=1};
int model = Cubes;

/* Define normal buffers */
glm::vec3 *normal_buffer_cube;
glm::vec3 *normal_buffer_platform;
glm::vec3 *normal_buffer_roof;

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
        3, 2, 6,
        6, 7, 3,
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
        7,8,9,  // bottom
        7,9,10,
        7,10,11,
        7,11,12,
        7,12,13,
        7,13,8,
        1,2,8,  //sides
        2,8,9,
        2,3,9,
        3,9,10,
        3,4,10,
        4,10,11,
        4,5,11,
        5,11,12,
        5,6,12,
        6,12,13,
        6,1,13,
        1,13,8
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
        7,8,9,  // bottom
        7,9,10,
        7,10,11,
        7,11,12,
        7,12,13,
        7,13,8,
        1,2,8,  //sides
        2,8,9,
        2,3,9,
        3,9,10,
        3,4,10,
        4,10,11,
        4,5,11,
        5,11,12,
        5,6,12,
        6,12,13,
        6,1,13,
        1,13,8
};


/*----------------------------------------------------------------*/


/******************************************************************
*
* Display
*
* This function is called when the content of the window needs to be
* drawn/redrawn. It has been specified through 'glutDisplayFunc()';
* Enable vertex attributes, create binding between C program and 
* attribute name in shader
*
*******************************************************************/

void Display()
{
    /* Clear window; color specified in 'Initialize()' */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Associate program with shader matrices */
    GLint projectionUniform = glGetUniformLocation(ShaderProgram, "ProjectionMatrix");
    if (projectionUniform == -1)
    {
        fprintf(stderr, "Could not bind uniform ProjectionMatrix\n");
        exit(-1);
    }
    for(int i = 0; i < 16; i++) {
        printf("%f ", ProjectionMatrix[i]);
    }
    printf("\n");
    glUniformMatrix4fv(projectionUniform, 1, GL_TRUE, ProjectionMatrix);

    GLint ViewUniform = glGetUniformLocation(ShaderProgram, "ViewMatrix");
    if (ViewUniform == -1)
    {
        fprintf(stderr, "Could not bind uniform ViewMatrix\n");
        exit(-1);
    }
    for(int i = 0; i < 16; i++) {
        printf("%f ", ViewMatrix[i]);
    }
    printf("\n");
    glUniformMatrix4fv(ViewUniform, 1, GL_TRUE, ViewMatrix);

    GLint RotationUniform = glGetUniformLocation(ShaderProgram, "ModelMatrix");
    if (RotationUniform == -1)
    {
        fprintf(stderr, "Could not bind uniform ModelMatrix\n");
        exit(-1);
    }

    GLint LightUniform = glGetUniformLocation(ShaderProgram, "LightPosition_worldspace");
    if (LightUniform == -1)
    {
        fprintf(stderr, "Could not bind uniform Lightposition\n");
        exit(-1);
    }
    glUniformMatrix4fv(LightUniform, 1, GL_FALSE, glm::value_ptr(glm::vec3(0.0f, 10.0f, 0.0f)));

    /* Draw platform */
    glBindVertexArray(VAO_platform);
    glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrixPlatform);
    glDrawElements(GL_TRIANGLES, sizeof(index_buffer_platform)/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);


    /* Draw poles */
    int i;
    for(i = 0; i < 6; i++) {
        glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrixPole[i]);
        glDrawElements(GL_TRIANGLES, sizeof(index_buffer_platform)/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
    }

    /* Draw middle pole */
    glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrixMiddlePole);
    glDrawElements(GL_TRIANGLES, sizeof(index_buffer_platform)/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    /* Draw roof */
    glBindVertexArray(VAO_roof);
    glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrixRoof);
    glDrawElements(GL_TRIANGLES, sizeof(index_buffer_roof)/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    /* Draw 6 cubes */
    if(model == Cubes) {
        glBindVertexArray(VAO_cube);
        for(i = 0; i < 6; i++) {
            glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrixCubes[i]);
            glDrawElements(GL_TRIANGLES, sizeof(index_buffer_cube)/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
        }
    }
        /* Draw 6 objects which where loaded from the .obj file */
    else if(model == Other) {
        glBindVertexArray(VAO_model);
        for(i = 0; i < 6; i++) {
            glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrixOther[i]);
            glDrawElements(GL_TRIANGLES, data1.face_count * 3, GL_UNSIGNED_SHORT, 0);
        }
    }

    /* Draw floor */
    glBindVertexArray(VAO_floor);
    glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrixFloor);
    glDrawElements(GL_TRIANGLES, sizeof(index_buffer_cube)/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    glBindVertexArray(0);

    /* Swap between front and back buffer */
    glutSwapBuffers();
}



/******************************************************************
*
* OnIdle
*
*
*
*******************************************************************/

void OnIdle()
{
    float rotationX[16], rotationY[16], rotationZ[16];
    float scaling[16];
    float translation[16];

    /* Determine delta time between two frames to ensure constant animation */
    int newTime = glutGet(GLUT_ELAPSED_TIME);
    int delta = newTime - oldTime;
    oldTime = newTime;
    float angle = (glutGet(GLUT_ELAPSED_TIME) / 2000.0) * (180.0/M_PI);

    /* Determine the angles for the rotation of the camera around the model */
    if(anim)
    {
        /* Increment rotation angles and update matrix */
        if(axis == Xaxis) {
            angleX = fmod(angleX + delta*velocity/40.0, 360.0);
        } else if(axis == Yaxis) {
            angleY = fmod(angleY + delta*velocity/40.0, 360.0);
        } else if(axis == Zaxis) {
            angleZ = fmod(angleZ + delta*velocity/40.0, 360.0);
        }
    }
    /* set the rotations for the camera in RotationMatrixAnimCamera */
    SetRotationX(angleX, RotationMatrixAnimX);
    SetRotationY(angleY, RotationMatrixAnimY);
    SetRotationZ(angleZ, RotationMatrixAnimZ);
    MultiplyMatrix(RotationMatrixAnimX, RotationMatrixAnimY, RotationMatrixAnimCamera);
    MultiplyMatrix(RotationMatrixAnimCamera, RotationMatrixAnimZ, RotationMatrixAnimCamera);

    /* Set viewing transform */
    SetTranslation(0.0, camera_up, camera_disp, ViewMatrix);
    MultiplyMatrix(ViewMatrix, RotationMatrixAnimCamera, ViewMatrix);


    /* Time dependent rotation for the merry-go-around*/
    SetRotationY(angle, RotationMatrixAnimRound);

    /* Set Transformation for floor */
    SetScaling(3, 0.1, 3, scaling);
    SetTranslation(0, -0.5, 0, translation);
    MultiplyMatrix(translation, scaling, ModelMatrixFloor);

    /* Set Transformation for Platform */
    SetScaling(0.25, 0.25, 0.25, scaling);
    MultiplyMatrix(RotationMatrixAnimRound, scaling, ModelMatrixPlatform);

    /* Set Transformation for the 6 outer Poles  */
    SetScaling(0.005, 2, 0.005, scaling);
    int i;
    float transX, transZ;
    float transY = 2;
    for(i = 0; i < 6; i++) {
        transX = vertex_buffer_platform[(i+1)].x/4;
        transZ = vertex_buffer_platform[(i+1)].z/4;
        SetTranslation(transX, transY, transZ, translation);
        MultiplyMatrix(translation, scaling, ModelMatrixPole[i]);
        MultiplyMatrix(RotationMatrixAnimRound, ModelMatrixPole[i], ModelMatrixPole[i]);
    }

    /* Set Transformation for middle pole */
    SetScaling(0.01, 2, 0.01, scaling);
    SetTranslation(0, 2, 0, translation);
    MultiplyMatrix(translation, scaling, ModelMatrixMiddlePole);
    MultiplyMatrix(RotationMatrixAnimRound, ModelMatrixMiddlePole, ModelMatrixMiddlePole);
    //MultiplyMatrix(RotationMatrixAnimCamera, ModelMatrixMiddlePole, ModelMatrixMiddlePole);

    /* Set Transformation for roof */
    SetScaling(0.25, 0.25, 0.25, scaling);
    SetTranslation(0,2,0, translation);
    MultiplyMatrix(translation, scaling, ModelMatrixRoof);

    MultiplyMatrix(RotationMatrixAnimRound, ModelMatrixRoof, ModelMatrixRoof);
    printf("ModelmatrixRoof ");
    for(int i = 0; i < 16; i++) {
        printf("%f ", ModelMatrixRoof[i]);
    }
    printf("\n");
    /* Set Transformation for cubes that additionally rotate around themselves */
    SetRotationX(-45, rotationX);
    SetRotationZ(35, rotationZ);

    SetRotationY(angle+30, RotationMatrixAnimRound);

    transY = 0.4;
    for(i = 0; i < 6; i++) {
        /* translate the points to the inner platform */
        transX = vertex_buffer_platform[(i+1)].x/6;
        transZ = vertex_buffer_platform[(i+1)].z/6;
        SetTranslation(transX, transY, transZ, translation);

        /* set the rotation for each cube, they will rotate in the opposite direction of the platform and each cube rotates at a different speed
         * (1. and 4. cube twice the speed of the platform, 2. and 5. four times the speed of the platform and the 3. and 6. cube eight times the speed)
         */
        SetScaling(0.25, 0.25, 0.25, scaling);
        SetRotationY(-2*angle * (i%3 +1), rotationY);
        MultiplyMatrix(rotationX, scaling, ModelMatrixCubes[i]);
        MultiplyMatrix(rotationZ, ModelMatrixCubes[i], ModelMatrixCubes[i]);
        MultiplyMatrix(rotationY, ModelMatrixCubes[i], ModelMatrixCubes[i]);
        MultiplyMatrix(translation, ModelMatrixCubes[i], ModelMatrixCubes[i]);
        MultiplyMatrix(RotationMatrixAnimRound, ModelMatrixCubes[i], ModelMatrixCubes[i]);

        /* Set ModelMatrices for alternative Objects */
        SetScaling(0.2, 0.2, 0.2, scaling);
        MultiplyMatrix(rotationY, scaling, ModelMatrixOther[i]);
        MultiplyMatrix(translation, ModelMatrixOther[i], ModelMatrixOther[i]);
        MultiplyMatrix(RotationMatrixAnimRound, ModelMatrixOther[i], ModelMatrixOther[i]);

    }


    /* Request redrawing of window content */
    glutPostRedisplay();
}

/******************************************************************
*
* Mouse
*
* Function is called on mouse button press; has been seta
* with glutMouseFunc(), x and y specify mouse coordinates,
* but are not used here.
*
*******************************************************************/

void Mouse(int button, int state, int x, int y)
{
    if(state == GLUT_DOWN)
    {
        /* Depending on button pressed, set rotation axis,
         * turn on animation */
        switch(button)
        {
            case GLUT_LEFT_BUTTON:
                axis = Xaxis;
                break;

            case GLUT_MIDDLE_BUTTON:
                axis = Yaxis;
                break;

            case GLUT_RIGHT_BUTTON:
                axis = Zaxis;
                break;
        }
        anim = GL_TRUE;
    }
}

/******************************************************************
*
* Keyboard
*
* Function to be called on key press in window; set by
* glutKeyboardFunc(); x and y specify mouse position on keypress;
* not used in this example
*
*******************************************************************/

void Keyboard(unsigned char key, int x, int y)
{
    switch( key )
    {
        /* Activate the cubes or some other object */
        case '1':
            model = Cubes;
            printf("pressed 1\n");
            break;

        case '2':
            model = Other;
            printf("pressed 2\n");
            break;

            /* Toggle animation */
        case '0':
            if (anim)
                anim = GL_FALSE;
            else
                anim = GL_TRUE;
            break;

            /* Reset initial rotation of object */
        case 'r':
            angleX = 0.0;
            angleY = 0.0;
            angleZ = 0.0;
            camera_disp = -10.0;
            camera_up = -2;
            anim = GL_FALSE;
            break;

        case 'c': case 'C':
            exit(0);
            break;
        case 'q':
            angleY = fmod(angleY-5.0, 360.0);
            break;
        case 'e':
            angleY = fmod(angleY+5.0, 360.0);
            break;
        case 'w':
            angleX = fmod(angleX-5.0, 360.0);
            break;
        case 's':
            angleX = fmod(angleX+5.0, 360.0);
            break;
        case 'a':
            angleZ = fmod(angleZ+5.0, 360.0);
            break;
        case 'd':
            angleZ = fmod(angleZ-5.0, 360.0);
            break;
        case '+':
            velocity++;
            break;
        case '-':
            if(velocity > 0) {
                velocity--;
            }
            break;
        case 'k':
            camera_disp = camera_disp + 0.5;
            break;
        case 'i':
            camera_disp = camera_disp - 0.5;
            break;
        case 'o':
            camera_up = camera_up + 0.5;
            break;
        case 'l':
            camera_up = camera_up - 0.5;
            break;
    }

    glutPostRedisplay();
}


/******************************************************************
*
* SetupDataBuffers
*
* Create buffer objects and load data into buffers
*
*******************************************************************/

void SetupDataBuffers()
{
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(normal_buffer_cube), normal_buffer_cube, GL_STATIC_DRAW);

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(normal_buffer_platform), normal_buffer_platform, GL_STATIC_DRAW);

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

    glGenBuffers(1, &NBO_roof);
    glBindBuffer(GL_ARRAY_BUFFER, NBO_roof);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normal_buffer_roof), normal_buffer_roof, GL_STATIC_DRAW);

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
    glGenVertexArrays(1, &VAO_platform);
    glGenVertexArrays(1, &VAO_cube);
    glGenVertexArrays(1, &VAO_roof);
    glGenVertexArrays(1, &VAO_floor);
    glGenVertexArrays(1, &VAO_model);
    GLint size; // don't really need this anymore, maybe change it to passing 0 in the glGetBufferParameteriv

    /* platform */
    glBindVertexArray(VAO_platform);

    glEnableVertexAttribArray(vPosition);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_platform);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(vColor);
    glBindBuffer(GL_ARRAY_BUFFER, CBO_platform);
    glVertexAttribPointer(vColor, 3, GL_FLOAT,GL_FALSE, 0, 0);

    glEnableVertexAttribArray(vNormal);
    glBindBuffer(GL_ARRAY_BUFFER, NBO_platform);
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_platform);
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

    /* cube */
    glBindVertexArray(VAO_cube);

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
    glBindVertexArray(VAO_roof);

    glEnableVertexAttribArray(vPosition);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_roof);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(vColor);
    glBindBuffer(GL_ARRAY_BUFFER, CBO_roof);
    glVertexAttribPointer(vColor, 3, GL_FLOAT,GL_FALSE, 0, 0);

    glEnableVertexAttribArray(vNormal);
    glBindBuffer(GL_ARRAY_BUFFER, NBO_roof);
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_roof);
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

    /* floor */
    glBindVertexArray(VAO_floor);

    glEnableVertexAttribArray(vPosition);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(vColor);
    glBindBuffer(GL_ARRAY_BUFFER, CBO_floor);
    glVertexAttribPointer(vColor, 3, GL_FLOAT,GL_FALSE, 0, 0);

    glEnableVertexAttribArray(vNormal);
    glBindBuffer(GL_ARRAY_BUFFER, NBO_cube);
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_cube);
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

    /* model */
    glBindVertexArray(VAO_model);

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
* AddShader
*
* This function creates and adds individual shaders
*
*******************************************************************/

void AddShader(GLuint ShaderProgram, const char* ShaderCode, GLenum ShaderType)
{
    /* Create shader object */
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0)
    {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }

    /* Associate shader source code string with shader object */
    glShaderSource(ShaderObj, 1, &ShaderCode, NULL);

    GLint success = 0;
    GLchar InfoLog[1024];

    /* Compile shader source code */
    glCompileShader(ShaderObj);
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }

    /* Associate shader with shader program */
    glAttachShader(ShaderProgram, ShaderObj);
}


/******************************************************************
*
* CreateShaderProgram
*
* This function creates the shader program; vertex and fragment
* shaders are loaded and linked into program; final shader program
* is put into the rendering pipeline 
*
*******************************************************************/

void CreateShaderProgram()
{
    /* Allocate shader object */
    ShaderProgram = glCreateProgram();

    if (ShaderProgram == 0)
    {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    /* Load shader code from file */
    VertexShaderString = LoadShader("shaders/vertexshader.vs");
    FragmentShaderString = LoadShader("shaders/fragmentshader.fs");

    /* Separately add vertex and fragment shader to program */
    AddShader(ShaderProgram, VertexShaderString, GL_VERTEX_SHADER);
    AddShader(ShaderProgram, FragmentShaderString, GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024];

    /* Link shader code into executable shader program */
    glLinkProgram(ShaderProgram);

    /* Check results of linking step */
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

    if (Success == 0)
    {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    /* Check if shader program can be executed */
    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);

    if (!Success)
    {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    /* Put linked shader program into drawing pipeline */
    glUseProgram(ShaderProgram);
}

void computeNormals(glm::vec3* vertexBuffer, int vertexBufferSize, GLushort* indexBuffer, int indexSize, glm::vec3* normalBuffer) {
    int i;
    // initialize normalbuffer with zeros
    normalBuffer = (glm::vec3*) calloc(vertexBufferSize, sizeof(glm::vec3));
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
        normalBuffer[indexBuffer[i+0]] += normal;
        normalBuffer[indexBuffer[i+1]] += normal;
        normalBuffer[indexBuffer[i+2]] += normal;
    }

    // Normalize vertex normals
    for(i = 0; i < vertexBufferSize; i++) {
        normalBuffer[i] = glm::normalize(normalBuffer[i]);
        //printf("[%f, %f, %f] ", normalBuffer[i].x, normalBuffer[i].y, normalBuffer[i].z);
    }
}

/******************************************************************
*
* Initialize
*
* This function is called to initialize rendering elements, setup
* vertex buffer objects, and to setup the vertex and fragment shader
*
*******************************************************************/

void Initialize(void)
{
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

    /* Set background (clear) color to dark blue */
    glClearColor(0.0, 0.0, 0.4, 0.0);

    /* Enable depth testing */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    /* Compute normals */
    computeNormals(vertex_buffer_cube, sizeof(vertex_buffer_cube)/sizeof(glm::vec3), index_buffer_cube, sizeof(index_buffer_cube)/sizeof(GLushort), normal_buffer_cube);
    computeNormals(vertex_buffer_roof, sizeof(vertex_buffer_roof)/sizeof(glm::vec3), index_buffer_roof, sizeof(index_buffer_roof)/sizeof(GLushort), normal_buffer_roof);
    computeNormals(vertex_buffer_platform, sizeof(vertex_buffer_platform)/sizeof(glm::vec3), index_buffer_platform, sizeof(index_buffer_platform)/sizeof(GLushort), normal_buffer_platform);

    /* Setup vertex, color, and index buffer objects */
    SetupDataBuffers();
    SetupVertexArrayObjects();

    /* Setup shaders and shader program */
    CreateShaderProgram();


    /* Set projection transform */
    float fovy = 45.0;   //*M_PI/180.0;
    float aspect = 1.0;
    float nearPlane = 1.0;
    float farPlane = 50.0;
    //ProjectionMatrix = glm::perspective(fovy, aspect, nearPlane, farPlane);
    SetPerspectiveMatrix(fovy, aspect, nearPlane, farPlane, ProjectionMatrix);

    /* Set viewing transform */
    camera_disp = -10.0;
    camera_up = -2;
    SetTranslation(0.0, camera_up, camera_disp, ViewMatrix);
    //ViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, camera_up, camera_disp));

    anim = GL_FALSE;
}



/******************************************************************
*
* main
*
* Main function to setup GLUT, GLEW, and enter rendering loop
*
*******************************************************************/

int main(int argc, char** argv)
{
    /* Initialize GLUT; set double buffered window and RGBA color model */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(400, 400);
    glutCreateWindow("CG Proseminar - Campregher-Kohl");

    /* Initialize GL extension wrangler */
    GLenum res = glewInit();
    if (res != GLEW_OK)
    {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }

    /* Setup scene and rendering parameters */
    Initialize();


    /* Specify callback functions;enter GLUT event processing loop, 
     * handing control over to GLUT */
    glutIdleFunc(OnIdle);
    glutDisplayFunc(Display);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);

    glutMainLoop();

    /* ISO C requires main to return int */
    return 0;
}