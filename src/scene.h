//
// Created by christina on 20.06.15.
//
#ifndef ROTATING_CUBE_SCENE_H
#define ROTATING_CUBE_SCENE_H

void initVAOs(GLuint *VAO_cube_ptr, GLuint *VAO_roof_ptr, GLuint *VAO_platform_ptr, GLuint *VAO_floor_ptr, GLuint *VAO_model_ptr);
int count_cube();
int count_roof();
void transformationInit_floor(float* result);
void transformationInit_poles(float result[6][16]);
void transformationInit_platform(float* result);
void transformationInit_roof(float* result);
void transformationInit_middlePole(float *result);

#endif //ROTATING_CUBE_SCENE_H
