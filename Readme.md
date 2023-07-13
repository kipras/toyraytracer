## About

This is a toy ray-tracer that i wrote for fun.

An example rendered image:  
![Example rendered image](/readme_assets/readme_main_render.jpg?raw=true "Example rendered image")

There are a few more images [below in the readme](#bonus-a-few-more-rendered-images-), from when the ray-tracer wasn't working properly yet,
but that resulted in interesting looking images imho :)

## How to run it

To run this you will need to download SDL2-2.24.0 to a subdirectory of this project: https://github.com/libsdl-org/SDL/releases/tag/release-2.24.0

Depending on your OS:
* Windows:
  1. Install MSYS2 (MINGW64) environment. You can compile and run the project from an "MSYS2 MinGW x64" shell.  
     See https://www.msys2.org/  
     Also see [the "Notes" section](#notes) below in the readme for more info about MSYS2 environments.
  2. Download the SDL2 version for mingw: https://github.com/libsdl-org/SDL/releases/download/release-2.24.0/SDL2-devel-2.24.0-mingw.zip  
     Extract it so that it would be placed in an `SDL2-devel-2.24.0-mingw/` subdirectory of this project.
     The commands for doing this (replace `path_to_toyraytracer_repo` with the path to the directory in your file system, where you cloned
     this repo and run them from an MSYS2 shell):
     ```
     cd <path_to_toyraytracer_repo>
     wget https://github.com/libsdl-org/SDL/releases/download/release-2.24.0/SDL2-devel-2.24.0-mingw.tar.gz
     tar -C SDL2-devel-2.24.0-mingw -xzvf SDL2-devel-2.24.0-mingw.tar.gz
     rm -f SDL2-devel-2.24.0-mingw.tar.gz
     ```
  3. Download the SDL2.dll (for 64 bit) and manually extract it to the root of this project: https://github.com/libsdl-org/SDL/releases/download/release-2.24.0/SDL2-2.24.0-win32-x64.zip

* Linux:
  1. You need to have `gcc` and `make` installed, to compile and run the project. If they are not yet installed -
     install them using your Linux distribution's package manager.
  2. Download SDL2 source: https://github.com/libsdl-org/SDL/releases/download/release-2.24.0/SDL2-2.24.0.tar.gz  
     Extract the archive to the root of this project (the `SDL2-2.24.0/` subdirectory should contain many files/subdirectories afterwards)
     and compile it (but you don't need to install it).  
     The commands for doing this (replace `path_to_toyraytracer_repo` with the path to the directory in your file system, where you cloned
     this repo):
     ```
     cd <path_to_toyraytracer_repo>
     wget https://github.com/libsdl-org/SDL/releases/download/release-2.24.0/SDL2-2.24.0.tar.gz
     tar -xzvf SDL2-2.24.0.tar.gz
     rm -f SDL2-2.24.0.tar.gz
     cd SDL2-2.24.0
     mkdir build
     cd build
     ../configure
     make -j8
     ```

## Notes

Some notes about the project:
* Written in pure C.
* Written by mostly following the "Ray Tracing in One Weekend" book:
  http://in1weekend.blogspot.com/2016/01/ray-tracing-in-one-weekend.html  
  https://github.com/RayTracing/raytracing.github.io  
  https://raytracing.github.io/books/RayTracingInOneWeekend.html  
  Though i first started off purely from scratch and got to the point where the ray-tracer would render statically colored spheres, but then
  i scrapped that implementation and followed this book instead.
* Can render only spheres and light sources (which are also spheres :) ).
* Supports matte, metal and dielectric (see-through) materials for the spheres.
* Renders in a single thread (multi-threaded rendering is not implemented).
* Uses SDL2 to do the actual drawing to the screen (for compatibility with both Windows and Linux).  
  Drawing is done using a single SDL "streaming" texture (updated using `SDL_LockTexture()`, `SDL_UnlockTexture()`).  
  This has ~2.1x less overhead than drawing each individual pixel with `SDL_SetRenderDrawColor()`, `SDL_RenderDrawPoint()`, and just
  _slightly_ less overhead than an SDL "static" texture (updated with `SDL_UpdateTexture()`).
* Uses gcc compiler, including on Windows (instead of MSVC), via the MSYS2/MINGW64 environment. You could also try MSYS2/UCRT64 - it works
  fine, except for printing the "Rays per second" statistic to `stdout` - i couldn't get the thousands separator (i.e. `"%'f"`) format for
  `printf()` to work in that environment (but it works in MSYS2/MINGW64).  
  See:  
  https://www.msys2.org/  
  https://www.msys2.org/docs/environments/
* Rendering allocates memory almost only in the stack.  
  Only the main application data, some of the scene data and a small amount of rendering data is created in the heap. But the actual ray
  tracing uses mostly only stack memory. This should perform somewhat better (than allocating memory for temporary data in the heap),
  because stack allocation is much cheaper than heap allocation. However, no performance measurement has been done for this (because to do
  that - we would have to implement a version of this that does use heap allocation, to compare against).
* Performance: renders ~2.6 million rays per second for the scene depicted in the screenshot above, on an Intel Core i7-3770K@3.50GHz
  (released 2012 Apr), PC3-12800 (800 MHz) DDR3 memory.  
  Note: this is single-thread performance (multi-threading is not implemented).  
  Note: graphics card shouldn't matter here, because rendering is done solely in the CPU.
* Tested on:
  * Windows 10 64 bit.
  *  Ubuntu Linux 16.04 Xenial 64 bit.

### Code style notes
* Source code line hard wrap set at 140 chars.
* Function names use `snake_case`.
* Variable and function parameter names use `camelCase`.

## Ideas for future improvements
A list of some improvements that could be made:
* Parallel (threaded) rendering. Currently we're only using one thread, so the rendering is multiple times slower than it could be on
  today's multi-core machines.
* Movable camera: the ability to move and rotate the camera, using keyboard keys. Note that after every camera move - the `allFrames` (the
  sum of all previously rendered frames) would have to be cleared. So after every move you would get a sudden drop in image quality.
* Performance: `ray_distance_to_sphere()` calculates `a = vector3_dot(&ray->direction, &ray->direction)`.
  Which is simply the length^2 of the `ray->direction` vector.
  And since we are using unit vectors for `ray->direction` - there is no need to calculate this (the length is always 1.0) and we could
  optimize this out (remove this).
* Performance: use CPU vector instructions.
* Try (once again) fixing the issue where spheres become "eggs" (toward image sides/corners) on larger FOVs (e.g. 90 degrees).
  Games render with FOV 90 just fine, so i'm guessing this ray-tracer should also be able to render the spheres without skewing them with
  that FOV.  
  I think what could be tried is using vector rotation, when producing the camera rays matrix (the map of image pixels to camera rays
  destination vectors).  
  See:
  https://en.wikipedia.org/wiki/Rotation_matrix
  https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
  https://en.wikipedia.org/wiki/Euler%E2%80%93Rodrigues_formula

## Bonus: a few more rendered images :)

A few more rendered images back from when the ray-tracer was still a work in progress (wasn't working properly yet), but that resulted in
interesting looking images imho :)

### Incorrect refraction in a glass material results in interesting looking outcome

A couple images where light refraction through glass spheres was incorrect and ended up looking kinda funky:
![Funky glass refraction 1](/readme_assets/42__glass_ri_1.5.jpg?raw=true "Funky glass refraction 1")
![Funky glass refraction 2](/readme_assets/43__glass_ri_5.0.jpg?raw=true "Funky glass refraction 2")

### Testing Schlick's approximation for Fresnel reflection

A couple images from when i was testing [Schlick's approximation](https://en.wikipedia.org/wiki/Schlick%27s_approximation) for
[Fresnel reflection](https://en.wikipedia.org/wiki/Fresnel_equations). This reflection is where a glass surface  reflects more light (and
refracts less light), when the ray of light hits it at a sharp angle (an example of this is where a window behaves more like a mirror, when
looking at it at a sharp angle).

Here, the pixels that got an incoming ray from a sphere where light should be _reflected_ are colored white.  
And pixels that got a ray where light should be _refracted_ are colored black.

Schlick's approximation test - after ~50 rays per pixel (the noise on the "ground" is not caused by Fresnel reflection, but is just a normal image with a low number of
rays per pixel):  
![Schlick's approximation for Fresnel reflection - ~50 rays per pixel](/readme_assets/44__schlick_approximation_of_fresnel_equation.jpg?raw=true "Schlick's approximation for Fresnel reflection - ~50 rays per pixel")

Notice that the edges of the two black spheres are gray - this is where a significant part of light would be reflected (because the angle
between the camera and the sphere is sharp). Also, notice that there are some gray pixels elsewhere on those spheres as well - _some_ light
is always reflected by a glass material (at any angle).

Schlick's approximation test - after ~3400 rays per pixel:  
![Schlick's approximation for Fresnel reflection - ~3400 rays per pixel](/readme_assets/53_schlick_3_min.jpg?raw=true "Schlick's approximation for Fresnel reflection - ~3400 rays per pixel")
