#include <math.h>
#include <stdio.h>

/*

 +-----------------------
 | declarations         |
 +-----------------------

 */

struct vec3d {
	double x, y, z;

	vec3d() = default;
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

struct rgb {
	unsigned char r, g, b;
};

struct triangle3d {
	vec3d p[3];
	rgb color;
};

struct triangle {
	int i1, i2, i3;
	rgb color;
};

struct mesh {
	vec3d *vertexes;
	triangle *faces;

	/* add fromobj functionality */
};

struct actor {
	mesh &model;
	vec3d translate;
	vec3d rotate;
	double scale;

	actor(mesh &m) {
		self->model = m;
	}

	vec3d apply(vec3d); /* to be added later */
};

void matprint(mat4x4 m);
void vecprint(vec3d v);

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



/*

 +-----------------------
 | camera               |
 +-----------------------

 */

class Camera
{
	protected:

		mat4x4 project;
		mat4x4 view;

		vec3d rot, pos;
		double width, height;

		/* internal functions, invoked by Camera and updatepos/rot */
		void updateview(double x, double y, double z, double yaw, double pitch, double roll);
		void setcam(double fov, double znear, double zfar, double width, double height);

	public:
		Camera(vec3d pos, vec3d rot, double fov, double znear, double zfar, double width, double height)
		{
			this->pos = pos;
			this->rot = rot;
			setcam(fov, znear, zfar, width, height);
			updateview(pos.x, pos.y, pos.z, rot.x, rot.y, rot.z);
		}

		Camera(double x, double y, double z, double yaw, double pitch, double roll,  double fov, double znear, double zfar, double width, double height)
		{
			pos.x = x; pos.y = y; pos.z = z;
			rot.x = yaw; rot.y = pitch; rot.z = roll;
			setcam(fov, znear, zfar, width, height);
			updateview(pos.x, pos.y, pos.z, rot.x, rot.y, rot.z);
		}

		void updatepos(vec3d pos)
		{
			this->pos = pos;
			updateview(pos.x, pos.y, pos.z, rot.x, rot.y, rot.z);
		}

		void updaterot(vec3d rot) /* these two funcs update the view matrix via updateview */
		{
			this->rot = rot;
			updateview(pos.x, pos.y, pos.z, rot.x, rot.y, rot.z);
		}

		void updatepos(double x, double y, double z)
		{
			pos.x = x; pos.y = y; pos.z = z;
			updateview(pos.x, pos.y, pos.z, rot.x, rot.y, rot.z);
		}

		void updaterot(double yaw, double pitch, double roll) /* these two funcs update the view matrix via updateview */
		{
			rot.x = yaw; rot.y = pitch; rot.z = roll;
			updateview(pos.x, pos.y, pos.z, rot.x, rot.y, rot.z);
		}

		vec3d getpos() { return pos; }
		vec3d getrot() { return rot; }
		mat4x4 getproject() { return project; }
		mat4x4 getview() { return view; }

		vec3d apply(vec3d in)
		{
		    return project*(view*in);
		}
};

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

	return;
}

/*

 +----------------------+
 | shader               |
 +----------------------+

 */

vec3d subtract(const vec3d &a, const vec3d &b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

vec3d cross(const vec3d &a, const vec3d &b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

double dot(const vec3d &a, const vec3d &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3d normalize(const vec3d &v) {
    double mag = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return {v.x / mag, v.y / mag, v.z / mag};
}

vec3d calculateUnitNormal(const vec3d &p1, const vec3d &p2, const vec3d &p3) {
    vec3d edge1 = subtract(p2, p1);
    vec3d edge2 = subtract(p3, p1);
    vec3d normal = cross(edge1, edge2);
    return normalize(normal);
}

vec3d reflectedDirection(const vec3d &L, const vec3d &N) {
    double dotLN = dot(L, N);
    return {L.x - 2 * dotLN * N.x, L.y - 2 * dotLN * N.y, L.z - 2 * dotLN * N.z};
}
class Shader {

	/* vector<Lightsrc> lights; */

	/* current light: directional light source facing -y */

	public:
		rgb apply(vec3d v1, vec3d v2, vec3d v3, vec3d campos)
			/* triangle-level shading -- pixel-level to be added */
		{
			vec3d l(0.0, -100.0, 0.0);
			vec3d r = reflectedDirection(l, calculateUnitNormal(v1, v2, v3));

			rgb color;
			color.r = dot(subtract(campos, v1), r);
			color.g = dot(subtract(campos, v1), g);
			color.b = dot(subtract(campos, v1), b);
			return color;
		}
}


/*

 +------------------------+
 | scene (final assembly) |
 +------------------------+

 */

class Scene {

	public:
		actor object;
		Camera camera;
		Shader shader;

		Scene() : camera()
};


/*

 +----------------------+
 | main function        |
 +----------------------+

 */

int main()
{
	return 0;
}
