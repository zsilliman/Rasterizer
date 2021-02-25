# 3D Rasterizer

I implemented a 3D rasterizer in C++ that can render a simple 3D scene of meshes. The rasterizer is CPU-bound and does not rely on the GPU or any 3D libraries to render the objects. The purpose of this project is to learn more about rasterization and the graphics pipeline by implementing my own 3D rasterizer from scratch. The project uses SDL, Eigen, and tinyobjloader.


It has the following features:
- obj file loading
- Custom vertex and fragment shaders
- Back-face culling
- Perspective-correct vertex attribute interpolation
- Blinn-Phong lighting
- UV texturing
- Rotation, translation and scale transformations
- Z-buffer to solve the visibility problem
- Multi-threaded rendering (assign objects to a specified number of threads)
