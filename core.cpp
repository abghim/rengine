#include <math.h>
#include <stdio.h>

#define RAD(a)	((a)*3.141592/180)
#define WIDTH	1400.0f
#define HEIGHT	800.0f


struct vec3d {
	double x, y, z;
	
	vec3d(double xval, double yval, double zval) {
		x = xval;
		y = yval;
		z = zval;
	}
};

struct mat4x4 {
	double m[4][4];

	vec3d operator*(const vec3d &in) const
	{
		vec3d result;
		result.x = in.x * m[0][0] + in.y * m[0][1] + in.z * m[0][2] + m[0][3];
		result.y = in.x * m[1][0] + in.y * m[1][1] + in.z * m[1][2] + m[1][3];
		result.z = in.x * m[2][0] + in.y * m[2][1] + in.z * m[2][2] + m[2][3];
		double w = in.x * m[3][0] + in.y * m[3][1] + in.z * m[3][2] + m[3][3];

		if (w != 0.0)
		{
			result.x /= w; result.y /= w; result.z /= w;
		}

		return result;
	}
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

class Camera
{
	protected:

		mat4x4 project;
		mat4x4 view;

		vec3d rot, pos;
		vec3d screendim;

		/* internal functions, invoked by Camera and updatepos/rot */
		void updateview(double x, double y, double z, double yaw, double pitch, double roll);
		void setcam(double fov, double znear, double zfar);
	
	public:
		Camera(vec3d pos, vec3d rot, double fov, double znear, double zfar);

		void updatepos(vec3d pos);
		void updaterot(vec3d rot); /* these two funcs update the view matrix via updateview */
		
		vec3d getpos()
		{
			return pos;
		}

		vec3d getrot()
		{
			return rot;
		}

		vec3d apply(vec3d in);
};

void Camera::setcam(double fov, double znear, double zfar) 
{
	double v1 = 1.0f/tan(RAD(fov*0.5f));
	project.m[0][0] = (screendim.x/screendim.y) * v1;
	project.m[1][1] = v1;
	project.m[2][2] = -(zfar+z_near)/(zfar-znear);
	project.m[3][2] = -1.0f;
	project.m[2][3] = -2*z_far*z_near/(z_far-z_near);
	
	return;
}
vec3d Camera::apply(vec3d in)
{
	return project*(view*in);
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
		return 0;
}
