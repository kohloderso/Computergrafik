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

/* OpenGL includes */
#include <GL/glew.h>
#include <GL/freeglut.h>


/* Local includes */
#include "LoadShader.h"   /* Provides loading function for shader code */
#include "Matrix.h"  


/*----------------------------------------------------------------*/

/* Define handle to a vertex buffer object */
GLuint VBO_cube, VBO_platform, VBO_roof;

/* Define handle to a color buffer object */
GLuint CBO_cube, CBO_platform, CBO_roof;

/* Define handle to an index buffer object */
GLuint IBO_cube, IBO_platform, IBO_roof;

GLuint VAO_cube, VAO_platform, VAO_roof;

/* Indices to vertex attributes; in this case positon and color */ 
enum DataID {vPosition = 0, vColor = 1}; 

/* Strings for loading and storing shader code */
static const char* VertexShaderString;
static const char* FragmentShaderString;

GLuint ShaderProgram;

float ProjectionMatrix[16]; /* Perspective projection matrix */
float ViewMatrix[16]; /* Camera view matrix */ 
float ModelMatrixPole[6][16], ModelMatrixPlatform[16], ModelMatrixCube[16], ModelMatrixRoof[16], ModelMatrixMiddlePole[16], ModelMatrixCubes[6][16]; /* Model matrix */


/* Transformation matrices for initial position */
float TranslateOrigin[16];
float TranslateDown[16];
float RotateX[16];
float RotateZ[16];
float InitialTransform[16];

GLfloat vertex_buffer_cube[] = { /* 8 cube vertices XYZ */
    -1.0, -1.0,  1.0,
     1.0, -1.0,  1.0,
     1.0,  1.0,  1.0,
    -1.0,  1.0,  1.0,
    -1.0, -1.0, -1.0,
     1.0, -1.0, -1.0,
     1.0,  1.0, -1.0,
    -1.0,  1.0, -1.0,
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

GLfloat vertex_buffer_platform[] = {
        0, 0, 0,
        4, 0, 7,
        8, 0, 0,
        4, 0 , -7,
        -4, 0, -7,
        -8, 0, 0,
        -4, 0, 7,
        0, -1, 0,
        4, -1, 7,
        8, -1, 0,
        4, -1 , -7,
        -4, -1, -7,
        -8, -1, 0,
        -4, -1, 7,
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

GLfloat vertex_buffer_roof[] = {
        0, 5, 0,
        4, 0, 7,
        8, 0, 0,
        4, 0 , -7,
        -4, 0, -7,
        -8, 0, 0,
        -4, 0, 7,
        0, -1, 0,
        4, -1, 7,
        8, -1, 0,
        4, -1 , -7,
        -4, -1, -7,
        -8, -1, 0,
        -4, -1, 7,
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
    glUniformMatrix4fv(projectionUniform, 1, GL_TRUE, ProjectionMatrix);
    
    GLint ViewUniform = glGetUniformLocation(ShaderProgram, "ViewMatrix");
    if (ViewUniform == -1) 
    {
        fprintf(stderr, "Could not bind uniform ViewMatrix\n");
        exit(-1);
    }
    glUniformMatrix4fv(ViewUniform, 1, GL_TRUE, ViewMatrix);
   
    GLint RotationUniform = glGetUniformLocation(ShaderProgram, "ModelMatrix");
    if (RotationUniform == -1) 
    {
        fprintf(stderr, "Could not bind uniform ModelMatrix\n");
        exit(-1);
    }

    /* draw cube *//*
    glBindVertexArray(VAO_cube);
    SetIdentityMatrix(ModelMatrixCube);
    glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrixCube);
    glPointSize(3);
    glDrawElements(GL_POINTS, sizeof(index_buffer_cube)/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);*/

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
    float angle = (glutGet(GLUT_ELAPSED_TIME) / 1000.0) * (180.0/M_PI);
    float RotationMatrixAnim[16];
    float scaling[16];
    float translation[16];

    /* Time dependent rotation */
    SetRotationY(angle, RotationMatrixAnim);

    /* Set Transformation for Platform */
    SetScaling(0.25, 0.25, 0.25, scaling);
    MultiplyMatrix(RotationMatrixAnim, scaling, ModelMatrixPlatform);

    /* Set Transformation for the 6 outer Poles  */
    SetScaling(0.005, 2, 0.005, scaling);
    int i;
    float transX, transZ;
    float transY = 2;
    for(i = 0; i < 6; i++) {
        transX = vertex_buffer_platform[(i+1)*3]/4;
        transZ = vertex_buffer_platform[(i+1)*3+2]/4;
        SetTranslation(transX, transY, transZ, translation);
        MultiplyMatrix(translation, scaling, ModelMatrixPole[i]);
        MultiplyMatrix(RotationMatrixAnim, ModelMatrixPole[i], ModelMatrixPole[i]);
    }

    /* Set Transformation for middle pole */
    SetScaling(0.01, 2, 0.01, scaling);
    SetTranslation(0, 2, 0, translation);
    MultiplyMatrix(translation, scaling, ModelMatrixMiddlePole);
    MultiplyMatrix(RotationMatrixAnim, ModelMatrixMiddlePole, ModelMatrixMiddlePole);

    /* Set Transformation for roof */
    SetScaling(0.25, 0.25, 0.25, scaling);
    SetTranslation(0,2,0, translation);
    MultiplyMatrix(translation, scaling, ModelMatrixRoof);
    MultiplyMatrix(RotationMatrixAnim, ModelMatrixRoof, ModelMatrixRoof);

    

    /* Request redrawing of window content */
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
}

void SetupVertexArrayObjects() {
    glGenVertexArrays(1, &VAO_platform);
    glGenVertexArrays(1, &VAO_cube);
    glGenVertexArrays(1, &VAO_roof);
    GLint size; // don't really need this anymore, maybe change it to passing 0 in the glGetBufferParameteriv

    /* platform */
    glBindVertexArray(VAO_platform);

    glEnableVertexAttribArray(vPosition);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_platform);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(vColor);
    glBindBuffer(GL_ARRAY_BUFFER, CBO_platform);
    glVertexAttribPointer(vColor, 3, GL_FLOAT,GL_FALSE, 0, 0);

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

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_roof);
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
    VertexShaderString = LoadShader("vertexshader.vs");
    FragmentShaderString = LoadShader("fragmentshader.fs");

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
    /* Set background (clear) color to dark blue */ 
    glClearColor(0.0, 0.0, 0.4, 0.0);

    /* Enable depth testing */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);    

    /* Setup vertex, color, and index buffer objects */
    SetupDataBuffers();

    SetupVertexArrayObjects();

    /* Setup shaders and shader program */
    CreateShaderProgram();  

    /* Initialize matrices */
    SetIdentityMatrix(ProjectionMatrix);
    SetIdentityMatrix(ViewMatrix);
    SetIdentityMatrix(ModelMatrixPlatform);
    SetIdentityMatrix(ModelMatrixRoof);
    int i;
    for(i = 0; i < 6; i++) {
        SetIdentityMatrix(ModelMatrixPole[i]);
    }

    /* Set projection transform */
    float fovy = 45.0;
    float aspect = 1.0; 
    float nearPlane = 1.0; 
    float farPlane = 50.0;
    SetPerspectiveMatrix(fovy, aspect, nearPlane, farPlane, ProjectionMatrix);

    /* Set viewing transform */
    float camera_disp = -10.0;
    float camera_up = -2;
    SetTranslation(0.0, camera_up, camera_disp, ViewMatrix);
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
    glutCreateWindow("CG Proseminar - Rotating Cube");

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
    glutMainLoop();

    /* ISO C requires main to return int */
    return 0;
}
