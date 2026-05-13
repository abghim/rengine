#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <strstream>
#include <string>
#include <utility>
#include <vector>
#include <stdlib.h>

#define DEV_SCALE_MESH 1.3
#define BLACK rgb()

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


vec3d operator+(const vec3d &a, const vec3d &b);
vec3d operator-(const vec3d &a, const vec3d &b);
vec3d operator*(const vec3d &a, const double b);
vec3d cross(const vec3d &a, const vec3d &b);
double dot(const vec3d &a, const vec3d &b);
vec3d calculateUnitNormal(const vec3d &p1, const vec3d &p2, const vec3d &p3);
vec3d reflectedDirection(const vec3d &L, const vec3d &N);


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

	triangle3d(vec3d p1, vec3d p2, vec3d p3, rgb color)
	{
        p[0] = p1;
        p[1] = p2;
        p[2] = p3;
		this->color = color;
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

		std::string line;
		while (std::getline(f, line))
		{
			if (line.empty()) continue;

			std::stringstream s(line);
			char junk;

			if (line[0] == 'v' && line.size() > 1 && line[1] != 'n' && line[1] != 't')
			{
				vec3d v;
				s >> junk >> v.x >> v.y >> v.z;
				vertexes.push_back(v);
			}

			if (line[0] == 'f')
			{
				s >> junk;

				vector<int> indexes;
				std::string token;
				while (s >> token) {
					size_t slash = token.find('/');
					std::string vertex_index = token.substr(0, slash);
					if (vertex_index.empty()) continue;

					int index = std::stoi(vertex_index);
					if (index > 0) index--;
					else if (index < 0) index = (int) vertexes.size() + index;

					indexes.push_back(index);
				}

				for (size_t i = 1; i + 1 < indexes.size(); i++) {
	                triangle face;
					face.i1 = indexes[0];
					face.i2 = indexes[i];
					face.i3 = indexes[i + 1];
					faces.push_back(face);
				}
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
		double znear, zfar;

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
		double getznear() { return znear; }
		double getzfar() { return zfar; }
		mat4x4 getproject() { return project; }
		mat4x4 getview() { return view; }

		vec3d apply(vec3d &in)
		{
		    // updateview(pos.x, pos.y, pos.z, rot.x, rot.y, rot.z);
			vec3d p(
				in.x * DEV_SCALE_MESH,
				in.y * DEV_SCALE_MESH,
				in.z * DEV_SCALE_MESH
			);


			vec3d ndc = project*(view*p);
			// vec3d ndc = (project*(view*in));
			//
			
			return vec3d((ndc.x+1)*width/2, (-ndc.y+1)*height/2, -1.0/(view*p).z);
			// return ndc;
		}
};

/*

 +----------------------+
 | shader               |
 +----------------------+

 */

class Shader {

	/* add later: vector<Lightsrc> lights; */

	/* current light: directional light source facing -y axis */

	public:
		rgb apply(vec3d v1, vec3d v2, vec3d v3, vec3d campos)
			/* triangle-level shading -- pixel-level to be added */
		{
			vec3d light_dir = vec3d(0.0, -1.0, -1.0).normalize();
			vec3d normal = calculateUnitNormal(v1, v2, v3);
			vec3d center = (v1 + v2 + v3) * (1.0 / 3.0);
			vec3d view_dir = (campos - center).normalize();

			double diffuse = dot(normal, light_dir * -1.0);
			if (diffuse < 0.0) diffuse = 0.0;

			vec3d reflected = reflectedDirection(light_dir, normal).normalize();
			double specular = dot(view_dir, reflected);
			if (specular < 0.0) specular = 0.0;
			specular = specular * specular * specular * specular;

			double intensity = 0.18 + 0.72 * diffuse + 0.10 * specular;
			if (intensity > 1.0) intensity = 1.0;

			unsigned char shade = (unsigned char) (intensity * 255.0);
			return rgb(shade, shade, shade);
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
        data = new pixel[w*h];
        if (data == NULL) fprintf(stderr, "Screen allocation failed.\n");
    }

    ~screen() {
        delete[] data;
    }

	bool put(int x, int y, rgb color, double invz) {
		return putcolor(x, y, color) && putzbuf(x, y, invz);
	}

    bool putcolor(int x, int y, rgb color)
    {
		if (x<0 || y<0 || x >= width || y >= height) {
	//		fprintf(stderr, "Accessing outside screen %d %d \n", x, y);
			return false;
		}
		unsigned char r = color.r;
		unsigned char b = color.b;
		unsigned char g = color.g;
        if (r>255 || r<0 || g>255 || g<0 || b>255 || b<0) return false;
        data[x+width*y].putcolor(r, g, b);
        return true;
    }

    bool putzbuf(int x, int y, double invz)
    {
        if (x<0 || y<0 || x >= width || y >= height) {
            return false;
        }
        if (invz<0 || invz<this->data[x+width*y].invz) return false;
        data[x+width*y].setdepth(invz);
        return true;
    }

    pixel get(int x, int y)
    {
		if (x<0 || y<0 || x >= width || y >= height) {
	//		fprintf(stderr, "Accessing outside screen %d %d \n", x, y);
			return pixel();
		}
        return data[x+width*y];
    }

};



class Scene {

	public:
		actor object;
		Camera camera;
		Shader shader;
		screen display;

		/* simple obj viewer (no complex scene, no actor transforms) */
		Scene(mesh *meshobj, /* cameara */ double x, double y, double z, double yaw, double pitch, double roll,  double fov, double znear, double zfar, double width, double height /* no shader settings (yet) */) 
			: camera(x, y, z, yaw, pitch, roll, fov, znear, zfar, width, height), object(meshobj), shader(), display(width, height){
            // printf("Scene object initialized at <%p>: actor <%p>, camera <%p>, shader <%p>\n", this, &(this->object), &(this->camera), &(this->shader));
		}

		Scene(mesh *meshobj, /* cameara */ vec3d pos, vec3d rot,  double fov, double znear, double zfar, double width, double height /* no shader settings (yet) */)
			: camera(pos, rot, fov, znear, zfar, width, height), object(meshobj), shader(), display(width, height) {
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

		void _showscene /* dev option: don't have SDL visualizer yet, output to file instead => no longer relevant */ ()
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
			mat4x4 view = camera.getview();
			double znear = camera.getznear();
			for (triangle face : object.model->faces) {
				vec3d v1 = (object.model->vertexes)[face.i1] * DEV_SCALE_MESH;
				vec3d v2 = (object.model->vertexes)[face.i2] * DEV_SCALE_MESH;
				vec3d v3 = (object.model->vertexes)[face.i3] * DEV_SCALE_MESH;

				/* very patchy solution for too-close triangles, revise later */
				if ((view * v1).z >= -znear || (view * v2).z >= -znear || (view * v3).z >= -znear) continue;

			    vec3d vs[3];
	                vs[0] = camera.apply(v1);
	                vs[1]= camera.apply(v2);
	                vs[2] = camera.apply(v3);
	                ret.push_back(triangle3d(vs[0], vs[1], vs[2], shader.apply(v1, v2, v3, camera.getpos())));
            } return ret;
		}

		void update() {
			for (int k=0; k<display.width; k++) {
				for (int l=0; l<display.height; l++) {
					display.putcolor(k, l, BLACK);
					display.data[k+display.width*l].setdepth(0.0);
				}
			}
			
			for (triangle3d tri : tris()) {
				puttri(tri);
			}
	
		}

	private:
		int getx(vec3d a, vec3d b, int y) {
			if (a.y == b.y) {
				return -1;
			}
			return (int) ((y-a.y)*(a.x-b.x)/(a.y-b.y)+a.x);
		}

		double getinvz(vec3d a, vec3d b, int y) {
					if (a.y == b.y) {
				return -1;
			}
			return ((y-a.y)*(a.z-b.z)/(a.y-b.y)+a.z);
		}


		bool oob(vec3d p) {
			return p.x<0 ||p.y<0 || p.x >= display.width || p.y >= display.height;
		}

		void puttri(triangle3d t) {
			if (oob(t.p[0]) && oob(t.p[1]) && oob(t.p[2])) return;

			vec3d top, mid, bottom;
			if (t.p[0].y >= t.p[1].y) {
				if (t.p[1].y >= t.p[2].y) {
					top = t.p[0]; mid = t.p[1]; bottom = t.p[2];
				} else {
					if (t.p[0].y >= t.p[2].y) {
						top = t.p[0]; mid = t.p[2]; bottom = t.p[1];
					} else {
						top = t.p[2]; mid = t.p[0]; bottom = t.p[1];
					}
				}
			} else {
				if (t.p[0].y >= t.p[2].y) {
					top = t.p[1]; mid = t.p[0]; bottom = t.p[2];
				} else {
					if (t.p[1].y >= t.p[2].y) {
						top = t.p[1]; mid = t.p[2]; bottom = t.p[0];
					} else {
						top = t.p[2]; mid = t.p[1]; bottom = t.p[0];
					}
				}
			}

			int lower_start = (int) std::ceil(bottom.y);
			int upper_start = (int) std::ceil(mid.y);
			int upper_end = (int) std::floor(top.y);

			for (int y=lower_start; y<upper_start; y++) {
				int left = getx(top, bottom, y);
				int right = getx(bottom, mid, y);
				double invz_left = getinvz(top, bottom, y);
				double invz_right = getinvz(bottom, mid, y);


				if (left == -1 || right == -1) return;

				int temp;

				if (left > right) {
					std::swap(left, right);
					std::swap(invz_left, invz_right);
				}
				
				for (int x=left; x<=right; x++) {
					double h;

					if (left == right) {
						h = invz_left;
					} else {
						double tx = (double)(x - left) / (double)(right - left);
						h = invz_left + tx * (invz_right - invz_left);
					}

					if (h > display.get(x, y).invz) {
						display.putcolor(x, y, t.color);
						display.putzbuf(x, y, h);
					}
				/*TODO: add z-buffering*/
				}
			}

			for (int y=upper_start; y<=upper_end; y++) {
				int left = getx(top, bottom, y);
				int right = getx(top, mid, y);
				double invz_left = getinvz(top, bottom, y);
				double invz_right = getinvz(top, mid, y);


				if (left == -1 || right == -1) return;
				int temp;

				if (left > right) {
					std::swap(left, right);
					std::swap(invz_left, invz_right);
				}
				
				for (int x=left; x<=right; x++) {
					double h;

					if (left == right) {
						h = invz_left;
					} else {
						double tx = (double)(x - left) / (double)(right - left);
						h = invz_left + tx * (invz_right - invz_left);
					}

					if (h > display.get(x, y).invz) {
						display.putcolor(x, y, t.color);
						display.putzbuf(x, y, h);
					}
				/*TODO: add z-buffering*/
				}
			}
		}

};


/*

 +----------------------+
 | main function        |
 +----------------------+

 */
