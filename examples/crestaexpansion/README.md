
This is the same example as the small, "crestamodule" example, but shows how full projects can be expanded on top of CRESTA and linked dynamically.
The data folder is also included, showing that seperate databases can also be setup in new custom applications, by specifying them at the start
(see l236 in app/expansion_g4sim.cc)

# Build Instructions
#########################################

mkdir build
g4make ../


# Modification for a CRESTA Extension
#########################################

To modify this example for your own project, just copy the entire directory hierarchy.
Copy your own files to src, and then update the PROJECT option at the very start of the CMakeLists to something else.
e.g. 
project(WATERMON)

Additional full applications can also be loaded. The one in app is a carbon copy of cosmicraysim, but provides the rough idea.
