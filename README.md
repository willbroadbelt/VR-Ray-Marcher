# VR-Ray-Marcher - Real Time Ray-tracing in Virtual Reality
This is a  3rd year Computer science project aimed at creating a proof-of-concept graphics application.

It uses a GLSL fragment shader to hold a signed distance funciton ray marcher to create ray-traced images. The program uses the OpenGL pipeline to create two image textures which are the provided to OpenVR that connects them to connected HTC Vive headset.

The ray-marcher can be found in ```resources/sdf_ray.frag```
and the main loop is run from ```canvas.cpp```

Written in MS VisualStudios
