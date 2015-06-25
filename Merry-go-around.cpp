/******************************************************************
*
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
#include "glm/vec3.hpp" // glm::vec3
#include "glm/gtc/matrix_transform.hpp" /* Provides glm::translate, glm::rotate, 
                                         * glm::scale, glm::perspective */
#include "glm/gtc/type_ptr.hpp"         /* Vector/matrix handling */

/* OpenGL includes */
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "src/texture.hpp"
#include "scene.h"

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

GLuint VAO_cube, VAO_platform, VAO_roof, VAO_floor, VAO_model, VAO_billboard;

/* Strings for loading and storing shader code */
static const char* VertexShaderString;
static const char* FragmentShaderString;

GLuint ShaderProgram;

/* Variables for texture handling */
GLuint TextureID, TextureID_2;
GLuint TextureUniform;


float ProjectionMatrix[16]; /* Perspective projection matrix */
float ViewMatrix[16]; /* Camera view matrix */
float ModelMatrixPole[6][16], ModelMatrixPlatform[16], ModelMatrixRoof[16], ModelMatrixMiddlePole[16], ModelMatrixCubes[6][16], ModelMatrixOther[6][16];
float ModelMatrixFloor[16], ModelMatrixBillboard[16];

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

/* Indices to activate either the cubes or some other model which is loaded from a .obj file */
enum {Cubes=0, Other=1};
int model = Cubes;

struct DirectionalLight
{
    glm::vec3 color;
    glm::vec3 position;
    float intensity;
};

DirectionalLight movingLight;
DirectionalLight fixedLight;

GLboolean ambientOn = GL_TRUE;
GLboolean diffuseOn = GL_TRUE;
GLboolean specularOn = GL_TRUE;
GLboolean disco = GL_FALSE;


/*----------------------------------------------------------------*/


void sendUniformsLight() {
    GLint uniform = glGetUniformLocation(ShaderProgram, "LightColorMoving");
    if (uniform == -1)
    {
        fprintf(stderr, "Could not bind uniform LightColor[0]\n");
        exit(-1);
    }
    glUniform3fv(uniform, 1, glm::value_ptr(movingLight.color));

    uniform = glGetUniformLocation(ShaderProgram, "LightPositionMoving");
    if (uniform == -1)
    {
        fprintf(stderr, "Could not bind uniform LightPositions[0]\n");
        exit(-1);
    }
    glUniform3fv(uniform, 1, glm::value_ptr(movingLight.position));

    uniform = glGetUniformLocation(ShaderProgram, "LightPower[0]");
    if (uniform == -1)
    {
        fprintf(stderr, "Could not bind uniform LightPower[0]\n");
        exit(-1);
    }
    glUniform1f(uniform, movingLight.intensity);

    //printf("Lightsource power %f", fixedLight.intensity);
    uniform = glGetUniformLocation(ShaderProgram, "LightColorFixed");
    if (uniform == -1)
    {
        fprintf(stderr, "Could not bind uniform lightsources[1].color\n");
        exit(-1);
    }
    glUniform3fv(uniform, 1, glm::value_ptr(fixedLight.color));

    uniform = glGetUniformLocation(ShaderProgram, "LightPositionFixed");
    if (uniform == -1)
    {
        fprintf(stderr, "Could not bind uniform LightPositions[1]\n");
        exit(-1);
    }
    glUniform3fv(uniform, 1, glm::value_ptr(fixedLight.position));

    uniform = glGetUniformLocation(ShaderProgram, "LightPower[1]");
    if (uniform == -1)
    {
        fprintf(stderr, "Could not bind uniform lightsources[1].intensity\n");
        exit(-1);
    }
    glUniform1f(uniform, fixedLight.intensity);


    uniform = glGetUniformLocation(ShaderProgram, "ambientOn");
    if (uniform == -1)
    {
        fprintf(stderr, "Could not bind uniform ambientOn\n");
        exit(-1);
    }
    glUniform1i(uniform, ambientOn);

    uniform = glGetUniformLocation(ShaderProgram, "diffuseOn");
    if (uniform == -1)
    {
        fprintf(stderr, "Could not bind uniform diffuseOn\n");
        exit(-1);
    }
    glUniform1i(uniform, diffuseOn);

    uniform = glGetUniformLocation(ShaderProgram, "specularOn");
    if (uniform == -1)
    {
        fprintf(stderr, "Could not bind uniform specularOn\n");
        exit(-1);
    }
    glUniform1i(uniform, specularOn);
}
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

    GLint InverseTransposeUniform = glGetUniformLocation(ShaderProgram, "InverseTransposeModel");
    //GLint InverseTransposeUniform = glGetUniformLocation(ShaderProgram, "InverseTransposeMV");
    if (InverseTransposeUniform == -1)
    {
        fprintf(stderr, "Could not bind uniform InverseTransposeModelMatrix\n");
        exit(-1);
    }

    /* send the parameters for the lighting */
    sendUniformsLight();

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TextureID);
    // Set our "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(TextureUniform, 0);

    GLint textureEnableUniform = glGetUniformLocation(ShaderProgram, "enableTexture");
    if (textureEnableUniform == -1)
    {
        fprintf(stderr, "Could not bind uniform enableTexture\n");
        exit(-1);
    }
    glUniform1i(textureEnableUniform, true);

    GLint billBoardOn = glGetUniformLocation(ShaderProgram, "billBoard");
    if (lightingDisabled == -1)
    {
        fprintf(stderr, "Could not bind uniform billBoard\n");
        exit(-1);
    }
    glUniform1i(billBoardOn, true);
    /* Draw billboard */
    glBindVertexArray(VAO_billboard);
    //SetIdentityMatrix(ModelMatrixBillboard);
    glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrixBillboard);
    glUniformMatrix4fv(InverseTransposeUniform, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::make_mat4(ModelMatrixBillboard)))));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // enable lighting for all other geometry
    glUniform1i(billBoardOn, false);

    /* Draw platform */
    glBindVertexArray(VAO_platform);
    glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrixPlatform);
    //glUniformMatrix4fv(InverseTransposeUniform, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::make_mat4(ViewMatrix) * glm::make_mat4(ModelMatrixPlatform)))));
    glUniformMatrix4fv(InverseTransposeUniform, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::make_mat4(ModelMatrixPlatform)))));
    glDrawElements(GL_TRIANGLES, count_roof(), GL_UNSIGNED_SHORT, 0);
    
    /* Draw poles */
    int i;
    for(i = 0; i < 6; i++) {
        glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrixPole[i]);
        //glUniformMatrix4fv(InverseTransposeUniform, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::make_mat4(ViewMatrix) * glm::make_mat4(ModelMatrixPole[i])))));
        glUniformMatrix4fv(InverseTransposeUniform, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::make_mat4(ModelMatrixPole[i])))));
        glDrawElements(GL_TRIANGLES, count_roof(), GL_UNSIGNED_SHORT, 0);
    }

    /* Draw middle pole */
    glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrixMiddlePole);
    //glUniformMatrix4fv(InverseTransposeUniform, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::make_mat4(ViewMatrix) * glm::make_mat4(ModelMatrixMiddlePole)))));
    glUniformMatrix4fv(InverseTransposeUniform, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::make_mat4(ModelMatrixMiddlePole)))));
    glDrawElements(GL_TRIANGLES, count_roof(), GL_UNSIGNED_SHORT, 0);

    /* Draw roof */
    glBindVertexArray(VAO_roof);
    glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrixRoof);
    //glUniformMatrix4fv(InverseTransposeUniform, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::make_mat4(ViewMatrix) * glm::make_mat4(ModelMatrixRoof)))));
    glUniformMatrix4fv(InverseTransposeUniform, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::make_mat4(ModelMatrixRoof)))));
    glDrawElements(GL_TRIANGLES, count_roof(), GL_UNSIGNED_SHORT, 0);
 
    /* Draw floor with different texture */
    glBindTexture(GL_TEXTURE_2D, TextureID_2);
    
    glBindVertexArray(VAO_floor);
    glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrixFloor);
    //glUniformMatrix4fv(InverseTransposeUniform, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::make_mat4(ViewMatrix) * glm::make_mat4(ModelMatrixFloor)))));
    glUniformMatrix4fv(InverseTransposeUniform, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::make_mat4(ModelMatrixFloor)))));
    glDrawElements(GL_TRIANGLES, count_cube(), GL_UNSIGNED_SHORT, 0);

    glUniform1i(textureEnableUniform, false);
    
    /* Draw 6 cubes */
    if(model == Cubes) {
        glBindVertexArray(VAO_cube);
        for(i = 0; i < 6; i++) {
            glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrixCubes[i]);
            //glUniformMatrix4fv(InverseTransposeUniform, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::make_mat4(ViewMatrix) * glm::make_mat4(ModelMatrixCubes[i])))));
            glUniformMatrix4fv(InverseTransposeUniform, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::make_mat4(ModelMatrixCubes[i])))));
            glDrawElements(GL_TRIANGLES, count_cube(), GL_UNSIGNED_SHORT, 0);
        }
    }
    /* Draw 6 objects which where loaded from the .obj file */
    else if(model == Other) {
        glBindVertexArray(VAO_model);
        for(i = 0; i < 6; i++) {
            glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrixOther[i]);
            //glUniformMatrix4fv(InverseTransposeUniform, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::make_mat4(ViewMatrix) * glm::make_mat4(ModelMatrixOther[i])))));
            glUniformMatrix4fv(InverseTransposeUniform, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::make_mat4(ModelMatrixOther[i])))));
            glDrawElements(GL_TRIANGLES, count_teapots(), GL_UNSIGNED_SHORT, 0);
        }
    }

   
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
    //ModelMatrixBillboard = glm::inverse(glm::make_mat4(ViewMatrix)) * glm::inverse(glm::make_mat4(RotationMatrixAnimCamera)) * glm::make_mat4(ViewMatrix);
    MultiplyMatrix(ViewMatrix, RotationMatrixAnimCamera, ViewMatrix);

    /* Time dependent rotation for the merry-go-around*/
    SetRotationY(angle, RotationMatrixAnimRound);

    /* compute moving light */
    //float rotationLight[16];
    glm::vec3 pos = glm::vec3(0.0f, 2.0f, 4.0f);
    //SetRotationY(delta/2000.0 * 180.0/M_PI, rotationLight);
    if(disco) {
        movingLight.position = glm::vec3(glm::transpose(glm::make_mat4(RotationMatrixAnimRound)) * glm::vec4(pos, 1.0f));
    }

    /* Set Transformation for floor */
    transformationInit_floor(ModelMatrixFloor);

    /* Set Transformation for Platform */
    transformationInit_platform(ModelMatrixPlatform);
    MultiplyMatrix(RotationMatrixAnimRound, ModelMatrixPlatform, ModelMatrixPlatform);

    /* Set Transformation for the 6 outer Poles  */
    transformationInit_poles(ModelMatrixPole);
    for(int i = 0; i < 6; i++) {
        MultiplyMatrix(RotationMatrixAnimRound, ModelMatrixPole[i], ModelMatrixPole[i]);
    }

    /* Set Transformation for middle pole */
    transformationInit_middlePole(ModelMatrixMiddlePole);
    MultiplyMatrix(RotationMatrixAnimRound, ModelMatrixMiddlePole, ModelMatrixMiddlePole);

    /* Set Transformation for roof */
    transformationInit_roof(ModelMatrixRoof);
    MultiplyMatrix(RotationMatrixAnimRound, ModelMatrixRoof, ModelMatrixRoof);

    /* Set Transformation for cubes that additionally rotate around themselves */
    transformationInit_cube_model(ModelMatrixCubes, ModelMatrixOther, angle);
    SetRotationY(angle+30, RotationMatrixAnimRound);
    for(int i = 0; i < 6; i++) {
        MultiplyMatrix(RotationMatrixAnimRound, ModelMatrixCubes[i], ModelMatrixCubes[i]);
	MultiplyMatrix(RotationMatrixAnimRound, ModelMatrixOther[i], ModelMatrixOther[i]);
    }


    ModelMatrixBillboard[0] = ViewMatrix[0];
    ModelMatrixBillboard[1] = ViewMatrix[4];
    ModelMatrixBillboard[2] = ViewMatrix[8];
    ModelMatrixBillboard[3] = 0;//ViewMatrix[3];
    ModelMatrixBillboard[4] = ViewMatrix[1];
    ModelMatrixBillboard[5] = ViewMatrix[5];
    ModelMatrixBillboard[6] = ViewMatrix[9];
    ModelMatrixBillboard[7] = 0;//ViewMatrix[7];
    ModelMatrixBillboard[8] = ViewMatrix[2];
    ModelMatrixBillboard[9] = ViewMatrix[6];
    ModelMatrixBillboard[10] = ViewMatrix[10];
    ModelMatrixBillboard[11] = 0; //ViewMatrix[11];
    ModelMatrixBillboard[12] = 0;
    ModelMatrixBillboard[13] = 0;
    ModelMatrixBillboard[14] = 0;
    ModelMatrixBillboard[15] = 1;

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
        case  '3':
            ambientOn = !ambientOn;
            break;
        case '4':
            diffuseOn = !diffuseOn;
            break;
        case '5':
            specularOn = !specularOn;
            break;
        case 'y':
            disco = !disco;
            break;
        case '6':
            fixedLight.intensity += 1;
            break;
        case '7':
            fixedLight.intensity -= 1;
            break;
    }

    glutPostRedisplay();
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

/******************************************************************
*
* SetupTexture
*
* This function is called to load the texture and initialize
* texturing parameters
*
*******************************************************************/

void SetupTexture(void)
{
    // Load the texture from bmp
    TextureID = loadBMP_custom("data/floor.bmp");
    
    TextureID_2 = loadBMP_custom("data/grass.bmp");

    // Get a handle for our "myTextureSampler" uniform
    TextureUniform  = glGetUniformLocation(ShaderProgram, "myTextureSampler");

}


/******************************************************************
*
* initLights
*
* This function is called to initialize the two lightsources
* with appropriate parameters
*
*******************************************************************/
void initLights() {

    /* the outside light, it should look kind of like a sun */
    fixedLight.color = glm::vec3(1.0f, 1.0f, 0.0f); //yellow
    fixedLight.position = glm::vec3(0.0f, 5.0f, 1.0f);
    fixedLight.intensity = 10;

    /* the moving light */
    movingLight.color = glm::vec3(1.0f, 1.0f, 1.0f); //white
    movingLight.position = glm::vec3(0.0f, 2.0f, 4.0f);
    movingLight.intensity = 5;
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

    initVAOs(&VAO_cube, &VAO_roof, &VAO_platform, &VAO_floor, &VAO_model, &VAO_billboard);
    /* Set background (clear) color to dark blue */
    glClearColor(0.0, 0.0, 0.4, 0.0);

    /* Enable depth testing */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Cull triangles which normal is not towards the camera
    //glEnable(GL_CULL_FACE);

    /* Setup shaders and shader program */
    CreateShaderProgram();

    /* Setup texture */
    SetupTexture();

    /* Setup Lights */
    initLights();

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
