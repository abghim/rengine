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

#define DEV_SCALE_MESH 1

using std::ifstream;
using std::strstream;
using std::vector;

/*

 +-----------------------
 | declarations         |
 +-----------------------

 */

struct vec3d {
	double x, y, z;

	vec3d() {
	    x = 0; y = 0; z = 0;
	}

	vec3d(double xval, double yval, double zval)
	{
		x = xval;
		y = yval;
		z = zval;
	}

	void print()
	{
	    printf("%f, %f, %f\n", this->x, this->y, this->z);
	}

	vec3d normalize()
	{
        double mag = sqrt(x * x + y * y + z * z);
        return {x / mag, y / mag, z / mag};
    }

    void print2d()
    {
        printf("%f, %f\n", x, y);
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

	rgb()
	{
	    r = 0; g = 0; b = 0;
	}

	rgb(unsigned char r, unsigned char g, unsigned char b)
	{
	    this->r = r; this->g = g; this->b = b;
	}
};

struct triangle3d {
	vec3d p[3];
	rgb color;

	triangle3d(vec3d p1, vec3d p2, vec3d p3) : color(255, 255, 255)
	{
        p[0] = p1;
        p[1] = p2;
        p[2] = p3;
	}
};

struct triangle {
	int i1, i2, i3;
	rgb color;

	triangle() : color(0, 0, 0) {
        i1 = 0; i2 = 0; i3 = 0;
	}
};

struct mesh {
	vector<vec3d> vertexes;
	vector<triangle> faces;

	/* modified from javidx9's console 3d graphics engine (https://github.com/OneLoneCoder/Javidx9/blob/master/ConsoleGameEngine/BiggerProjects/Engine3D/OneLoneCoder_olcEngine3D_Part2.cpp) */
	mesh(const char *filename)
	{
		ifstream f(filename);

		while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);

			strstream s;
			s << line;
			char junk;

			if (line[0] == 'v')
			{
				vec3d v;
				s >> junk >> v.x >> v.y >> v.z;
				vertexes.push_back(v);
			}

			if (line[0] == 'f')
			{
                triangle face;

				s >> junk >> face.i1 >> face.i2 >> face.i3;

				faces.push_back(face);
			}
		}

	}

	void print() {
	    int i = 1;
        for (triangle face : faces) {
            printf("\nface #%d: \n", i++);
            printf("\t"); vertexes[face.i1].print();
            printf("\t"); vertexes[face.i2].print();
            printf("\t"); vertexes[face.i3].print();
        } return;
	}

};

struct actor {
	mesh *model;
	vec3d translate;
	vec3d rotate;
	double scale;

	actor(mesh *m) {
		this->model = m;
	}

	vec3d apply(vec3d); /* to be added later */
};

void matprint(mat4x4 m);
void vecprint(vec3d v);


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
		double getwidth() { return width; }
		double getheight() { return height; }
		mat4x4 getproject() { return project; }
		mat4x4 getview() { return view; }

		vec3d apply(vec3d &in)
		{
		    // updateview(pos.x, pos.y, pos.z, rot.x, rot.y, rot.z);

			vec3d ndc = (project*(view*vec3d(in.x*DEV_SCALE_MESH, in.y*DEV_SCALE_MESH, in.z*DEV_SCALE_MESH)));
			// vec3d ndc = (project*(view*in));
			return vec3d((ndc.x+1)*width/2, (-ndc.y+1)*height/2, 0.0);
			// return ndc;
		}
};

/*

 +----------------------+
 | shader               |
 +----------------------+

 */

vec3d operator+(const vec3d &a, const vec3d &b);

vec3d operator-(const vec3d &a, const vec3d &b);

vec3d cross(const vec3d &a, const vec3d &b);

double dot(const vec3d &a, const vec3d &b);

vec3d calculateUnitNormal(const vec3d &p1, const vec3d &p2, const vec3d &p3);

vec3d reflectedDirection(const vec3d &L, const vec3d &N);

class Shader {

	/* add later: vector<Lightsrc> lights; */

	/* current light: directional light source facing -y axis */

	public:
		rgb apply(vec3d v1, vec3d v2, vec3d v3, vec3d campos)
			/* triangle-level shading -- pixel-level to be added */
		{
			vec3d l(0.0, -1.0, 0.0);
			vec3d r = reflectedDirection(l, calculateUnitNormal(v1, v2, v3));

			rgb color(0, 0, 0);
			color.r = (int) dot(campos -v1, r)*255;
			color.g = (int) dot(campos-v1, r)*255;
			color.b = (int) dot(campos-v1, r)*255;
			return color;
		}
};


/*

 +------------------------+
 | scene (final assembly) |
 +------------------------+

 */

struct pixel {
    rgb color;
    double invz;

    pixel() : color(), invz(0.0) {}

    void putcolor(unsigned char r, unsigned char g, unsigned char b)
    {
        color = rgb(r, g, b);
        return;
    }

    void setdepth(double invz)
    {
        this->invz = invz;
        return;
    }
};

struct screen {
    int width;
    int height;

    pixel *data;

    screen(int w, int h) : width(w), height(h)
    {
        data = (pixel *) malloc(sizeof(pixel)*w*h);
        if (data == NULL) fprintf(stderr, "Screen allocation failed.\n");
    }

    ~screen() {
        free(data);
    }

    bool putcolor(int x, int y, unsigned char r, unsigned char g, unsigned char b)
    {
        if (r>255 || r<0 || g>255 || g<0 || b>255 || b<0) return 1;
        data[x+height*y].putcolor(r, g, b);
        return 0;
    }

    bool putzbuf(int x, int y, double invz)
    {
        if (invz<0 || invz<this->data[x+height*y].invz) return 1;
        data[x+height*y].setdepth(invz);
        return 0;
    }

    pixel get(int x, int y)
    {
        return data[x+height*y];
    }

};



class Scene {

	public:
		actor object;
		Camera camera;
		Shader shader;

		/* simple obj viewer (no complex scene, no actor transforms) */
		Scene(mesh *meshobj, /* cameara */ double x, double y, double z, double yaw, double pitch, double roll,  double fov, double znear, double zfar, double width, double height /* no shader settings (yet) */) : camera(x, y, z, yaw, pitch, roll, fov, znear, zfar, width, height), object(meshobj), shader(){
            // printf("Scene object initialized at <%p>: actor <%p>, camera <%p>, shader <%p>\n", this, &(this->object), &(this->camera), &(this->shader));
		}

		Scene(mesh *meshobj, /* cameara */ vec3d pos, vec3d rot,  double fov, double znear, double zfar, double width, double height /* no shader settings (yet) */) : camera(pos, rot, fov, znear, zfar, width, height), object(meshobj), shader(){
		    // printf("Scene initialized at <%p>: actor <%p>, camera <%p>, shader <%p>\n", this, &(this->object), &(this->camera), &(this->shader));
		}

		/* query scene */
		bool query()
		{
		    return 0;
		}

		void frame(screen &viewport)
		{
            int i = 1;
            for (triangle face : object.model->faces) {
                printf("\nface #%d: \n", i++);
                printf("\t"); camera.apply((object.model->vertexes)[face.i1]).print2d();
                printf("\t"); camera.apply((object.model->vertexes)[face.i2]).print2d();
                printf("\t"); camera.apply((object.model->vertexes)[face.i3]).print2d();
            } return;
		}

		void _showscene /* dev option: don't have SDL visualizer yet, output to file instead */ ()
		{
		    std::cout << camera.getwidth() << '\n';
			std::cout << camera.getheight() << '\n';
            for (triangle face : object.model->faces) {
                camera.apply((object.model->vertexes)[face.i1]).print2d();
                camera.apply((object.model->vertexes)[face.i2]).print2d();
                camera.apply((object.model->vertexes)[face.i3]).print2d();
            } return;
		}

		vector<triangle3d> tris()
		{
		    /* prints all triangles */
			vector<triangle3d> ret;
			for (triangle face : object.model->faces) {
			    vec3d vs[3];
                vs[0] = camera.apply((object.model->vertexes)[face.i1]);
                vs[1]= camera.apply((object.model->vertexes)[face.i2]);
                vs[2] = camera.apply((object.model->vertexes)[face.i3]);
                ret.push_back(triangle3d(vs[0], vs[1], vs[2]));
            } return ret;
		}

};


/*

 +----------------------+
 | main function        |
 +----------------------+

 */
