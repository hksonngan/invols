#ifndef FIGURES_INC
#define FIGURES_INC

#include "Geometry.h"

//¬ геометрию можно добавл€ть разные фигуры:

void AddBox(Geometry*g, vec3 a,vec3 b);
void AddBox(Geometry*g, vec3 a,vec3 b,ivec3 br);
void AddCylinder(Geometry*g,float h,float r, int sides);
void AddSphere(Geometry*g,float r, int sides_a, int sides_b);
void AddArmDetail(Geometry*g,float w,float r, int sides);

#endif