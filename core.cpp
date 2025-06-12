#include <math.h>
#include <stdio.h>

#define RAD(a)	((a)*3.141592/180)
#define WIDTH	1400.0f
#define HEIGHT	800.0f

struct vec3d {
	double x, y, z;
};

struct mat4x4 {
	double m[4][4];
};

struct triangle {
	vec3d p[3];
	vec3d color;
};

struct mesh {
	triangle *list;
};

struct actor {
	mesh *model;
	vec3d translate;
	vec3d rotate;
	float scale;
};

void matprint(mat4x4 m);
void vecprint(vec3d v);

class camera
{
	protected:

		mat4x4 project;
		mat4x4 rotx, roty, rotz;
		mat4x4 transx, transy, transz;

		vec3d rot, pos;
		
		float width, height;

		void mul(vec3d *in, mat4x4 *m, vec3d *result);
	
	public:
		void setcam(double fov, double znear, double zfar);

		void apply(vec3d *in, vec3d *out);
};

void camera::mul(vec3d *in, mat4x4 *m, vec3d *result)
{
	result->x = in->x * m->m[0][0] + in->y * m->m[1][0] + in->z * m->m[2][0] + m->m[3][0];
	result->y = in->x * m->m[0][1] + in->y * m->m[1][1] + in->z * m->m[2][1] + m->m[3][1];
	result->z = in->x * m->m[0][2] + in->y * m->m[1][2] + in->z * m->m[2][2] + m->m[3][2];
	double w = in->x * m->m[0][3] + in->y * m->m[1][3] + in->z * m->m[2][3] + m->m[3][3];

	if (w != 0.0f)
	{
		result->x /= w; result->y /= w; result->z /= w;
	}

	return;
}

void camera::setcam(double fov, double znear, double zfar) 
{
	double v1 = 1.0f/tan(RAD(fov*0.5f));
	project.m[0][0] = (HEIGHT/WIDTH) * v1;
	project.m[1][1] = v1;
	project.m[2][2] = zfar/(zfar-znear);
	project.m[3][2] = -(znear*zfar)/(zfar-znear);
	project.m[2][3] = 1.0f;
	
	return;
}

void camera::setrotation(double x, double y, double z)
{
    double cx = cos(RAD(x)), sx = -sin(RAD(x));  // cos and sin of roll (x-axis)
    double cy = cos(RAD(y)), sy = -sin(RAD(y));  // cos and sin of pitch (y-axis)
    double cz = cos(RAD(z)), sz = -sin(RAD(z));  // cos and sin of yaw (z-axis)

    rotx.m[0][0] = 1; rotx.m[0][1] = 0; rotx.m[0][2] = 0; rotx.m[0][3] = 0;
    rotx.m[1][0] = 0; rotx.m[1][1] = cx; rotx.m[1][2] = sx; rotx.m[1][3] = 0;
    rotx.m[2][0] = 0; rotx.m[2][1] = -sx; rotx.m[2][2] = cx; rotx.m[2][3] = 0;
    rotx.m[3][0] = 0; rotx.m[3][1] = 0; rotx.m[3][2] = 0; rotx.m[3][3] = 1;

    roty.m[0][0] = cy; roty.m[0][1] = 0; roty.m[0][2] = -sy; roty.m[0][3] = 0;
    roty.m[1][0] = 0; roty.m[1][1] = 1; roty.m[1][2] = 0; roty.m[1][3] = 0;
    roty.m[2][0] = sy; roty.m[2][1] = 0; roty.m[2][2] = cy; roty.m[2][3] = 0;
    roty.m[3][0] = 0; roty.m[3][1] = 0; roty.m[3][2] = 0; roty.m[3][3] = 1;

    rotz.m[0][0] = cz; rotz.m[0][1] = sz; rotz.m[0][2] = 0; rotz.m[0][3] = 0;
    rotz.m[1][0] = -sz; rotz.m[1][1] = cz; rotz.m[1][2] = 0; rotz.m[1][3] = 0;
    rotz.m[2][0] = 0; rotz.m[2][1] = 0; rotz.m[2][2] = 1; rotz.m[2][3] = 0;
    rotz.m[3][0] = 0; rotz.m[3][1] = 0; rotz.m[3][2] = 0; rotz.m[3][3] = 1;
	
	return;
}

void camera::apply(vec3d *in, vec3d *out)
{
	vec3d t1; vec3d t2; vec3d t3;

	mul(in, &roty, &t1);
	mul(&t1, &rotz, &t2);
	mul(&t2, &rotx, &t3);
	mul(&t3, &project, out);

	return;
}

void matprint(mat4x4 m)
{
	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			printf("%2f\t", m.m[i][j]);
		} printf("\n");
	}
	return;
}

void vecprint(vec3d v)
{
	printf("(%2f, %2f, %2f)\n", v.x, v.y, v.z);
	return;
}

int main()
{
	vec3d vec = {200.3f, 311.1f, 808.8f};
	vec3d applied;

	camera test;

	test.setcam(150.0f, 0.1f, 1000.0f);
	test.setrotation(60.0f, -80.1f, 45.0f);
	
	test.apply(&vec, &applied);

	vecprint(vec);
	vecprint(applied);

	return 0;
}
