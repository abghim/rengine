#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <strstream>
#include <vector>
#include <stdlib.h>
#include "core.hpp"

#define DEV_SCALE_MESH 1

using std::ifstream;
using std::strstream;
using std::vector;

void matprint(mat4x4 m)
{
	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			printf("%4f\t", m.m[i][j]);
		} printf("\n");
	}
	return;
}

void vecprint(vec3d v)
{
	printf("(%4f, %4f, %4f)\n", v.x, v.y, v.z);
	return;
}


void Camera::updateview(double x, double y, double z, double yaw, double pitch, double roll)
{
	double cy = cos(yaw),   sy = sin(yaw);
    double cp = cos(pitch), sp = sin(pitch);
    double cr = cos(roll),  sr = sin(roll);

    double Rx =  cy*cr + sy*sp*sr;
    double Ry =       cp*sr;
    double Rz = -sy*cr + cy*sp*sr;

    double Ux = -cy*sr + sy*sp*cr;
    double Uy =        cp*cr;
    double Uz =  sy*sr + cy*sp*cr;

    double Fx =  sy*cp;
    double Fy = -sp;
    double Fz =  cy*cp;

	view.m[0][0] = Rx;  view.m[0][1] = Ry;  view.m[0][2] = Rz;
    view.m[1][0] = Ux;  view.m[1][1] = Uy;  view.m[1][2] = Uz;
    view.m[2][0] = Fx;  view.m[2][1] = Fy;  view.m[2][2] = Fz;

    view.m[0][3] = -(Rx*x + Ry*y + Rz*z);
    view.m[1][3] = -(Ux*x + Uy*y + Uz*z);
    view.m[2][3] = -(Fx*x + Fy*y + Fz*z);

    view.m[3][0] = 0.0;  view.m[3][1] = 0.0;  view.m[3][2] = 0.0;  view.m[3][3] = 1.0;

	return;
}

void Camera::setcam(double fov, double znear, double zfar, double width, double height)
{
	project = {};
	double v1 = 1.0/tan(fov*0.5);
	project.m[0][0] = (height/width) * v1;
	project.m[1][1] = v1;
	project.m[2][2] = -(zfar+znear)/(zfar-znear);
	project.m[3][2] = -1.0;
	project.m[2][3] = -2*zfar*znear/(zfar-znear);

	this->width = width; this->height = height;

	return;
}

/*

 +----------------------+
 | shader               |
 +----------------------+

 */

vec3d operator+(const vec3d &a, const vec3d &b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

vec3d operator-(const vec3d &a, const vec3d &b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

vec3d cross(const vec3d &a, const vec3d &b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

double dot(const vec3d &a, const vec3d &b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}



vec3d calculateUnitNormal(const vec3d &p1, const vec3d &p2, const vec3d &p3)
{
    vec3d edge1 = p2 - p1;
    vec3d edge2 = p3 - p1;
    vec3d normal = cross(edge1, edge2);
    return normal.normalize();
}

vec3d reflectedDirection(const vec3d &L, const vec3d &N)
{
    double dotLN = dot(L, N);
    return {L.x - 2 * dotLN * N.x, L.y - 2 * dotLN * N.y, L.z - 2 * dotLN * N.z};
}

/*

 +----------------------+
 | main function        |
 +----------------------+

 */
