![alt text](https://github.com/alexnardini/SOPGI/blob/master/img/SOPGI_cover.jpg)
`Very high poly count geometry processed with SOPGI at high settings. Viewport is set to Hidden Wireframe`

# SOPGI
A SideFX Houdini `VEX raytracer` with photon mapping global illumination and full recursive reflections and refractions.

Tested on SideFX Houdini 16.x.x

# SOPGI work on the geometry points, so to viz the result properly set your viewport to HIDDEN LINE INVISIBLE mode ( WIREFRAME ) 
![alt text](https://github.com/alexnardini/SOPGI/blob/master/img/SOPGI_intro.jpg)

# DISCLAIMER
 SOPGI is a personal side project to teach me
 how to do mostly global illumination with photons
 and many other things I learned on the way.
 It try to be correct enough to make things look good.

 It also doesn't want to be a renderer.
 It is mainly a solution to store lighting, shading and GI
 data into the geometry to use later for things
 like any other attribute.
 
 It's been architected to give you the best result as quick as
 I could possibly make it so it is to be considered
 a very Novel approach in many areas.
 
 Everything in here is more for educational purpose
 so do not expect a SolidAngle Arnold render quality 
 and performance out of this.
 
 Some of the functionality have been implemented in a
 simple fashion.
 Please update this repository if you add or improve anything.
______________________________________________________

# CONTENTS
SOPGI is comprised of a set of nodes to generate and store
a Raytrace, GI, ShadowMaps and Recursions
on a per point level.

Every aspect of the render data
will be stored on a per points attributes
for later use as you would with any other attributes.

SOPGI Node's list:
# SOPGI ARay
# SOPGI BRDF
# SOPGI CornellBox
# SOPGI Light
# SOPGI RAYviz
______________________________________________________

# SOPGI Aray
This node is the core renderer.


It will generate all the render data and output the final attributes value.
It will also comp out the final result and store it on the Cd channel for you ro review.
The second optional output, will output only the photonmap points.
![alt text](https://github.com/alexnardini/SOPGI/blob/master/img/SOPGI_aov_attrib.jpg)
`Final SOPGI result followed from Left to Right by: BRDF, AO, GI and Shadows samples.`


![alt text](https://github.com/alexnardini/SOPGI/blob/master/img/SOPGI_aray_node_menus.jpg)
It is subdivided in 3 main Tabs:

#### Aray Core:
Will give you the ability to change the number of samples to use for the Raycasting calculation.
For example you can change the number of photons or AmbientOcclusion rays to use.
There are also optimization parameters you can tweak to speed up the calculation and
every parameter responsible for speeding up the process
are marked with an Astersisk `*` in front of their name.

#### ARay Sample Work:
Since everthing get stored on photons, SOPGI ARay create a point cloud
with all the necessary attributes and everything get transferred to the Geometry
using point clouds mechanism. This tab let you control how those values get
transferred like the point cloud Radius, precision and Gaussian Blur filter.

#### ARay Compositing:
This tab let you tweak the way every single render data get composited together.
Since this happen after the raycasting, depending on your geometry tessellation,
it can be as fast as realtime.

______________________________________________________

![alt text](https://github.com/alexnardini/SOPGI/blob/master/img/SOPGI_useBRDF.jpg)
The " `USE BRDF` " option on the main ARay option tab will disable all the heavy calculation
and only output Diffuse and Specular to work with the Recursive Reflections and Refractions.
This way you can preview Recursion very quickly before moving into tweaking GI etc.

More info about this node soon.
______________________________________________________


# SOPGI BRDF
This node will prepare a geometry to be plugged and processed by the SOPGI ARay node.
It will attach all the shading attributes so in some way this node is your Geometry Shader.
![alt text](https://github.com/alexnardini/SOPGI/blob/master/img/SOPGI_brdf.jpg)

______________________________________________________

# SOPGI CornellBox
![alt text](https://github.com/alexnardini/SOPGI/blob/master/img/SOPGI_cornellBox.jpg)

This node is a simple utility node that will let you quickly generate
a CornellBox geometry and tweak its colors.
It will also let you create some simple geometry inside the box 
like a sphere and some standard Houdini geo and they will also be all animated 
so the sphere and other geo will rotate inside the CornellBox
for you to test out SOPGI render feautures.

______________________________________________________

# SOPGI Light
This node will let you convert any Geo into a SOPGI Light to be plugged into the SOPGI ARay node.
In this implementation LIghts are very simple, they have a color, and intensity, a distance radius and samples count.

______________________________________________________

# SOPGI RAYviz
This node is an utility node to let you visualize Light Rays along their traced path.
It include light bounces so you can specify how many light bounces you want to viz.
Since Recursions has been implemented later, this node does not take into account
transparent/IOR geometry.
![alt text](https://github.com/alexnardini/SOPGI/blob/master/img/SOPGI_RAYviz.jpg)


