# rengine

3D rendering engine written in C++, using SDL2 for primitive graphics. The rendering logic is built from scratch, without 3D libraries such as OpenGL.

### Current Progress (Jul 19th)
No shaders / depth buffering, only perspective projection applied to the utah teapot. It is possible to move the camera with WASD, EQ, and the arrow keys, all of which work as expected.
<img width="912" height="740" alt="Screenshot 2025-07-17 at 2 21 15 PM" src="https://github.com/user-attachments/assets/ac0ab0ce-2b3f-497e-9805-f55a35a7c811" />


## To-do
- add advanced shading -- current model is rudimentary prototype
	- specifically add light sources and apply it to illumination
	- Blinn-Phong shading => pixel level shading
	- add Lightsrc class
- add model transform of actors
- add functioning color system
- allow for multiple actors in a scene
