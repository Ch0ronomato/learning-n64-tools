import bpy
import os.path
import shutil

scene = bpy.context.scene
# get the meshes
visible = [obj for obj in scene.objects if obj.type not in ("EMPTY", "CAMERA")]

MAIN_PATH = r'\\wsl.localhost\Ubuntu\home\ischweer\dev\n64dev\texturegame' 
CPP_PATH = os.path.join(MAIN_PATH, 'data')

# generate the header file
header = """
// THIS CODE WAS GENERATED IN BLENDER\n
#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#define NUM_GAME_OBJECTS REPLACE_WITH_COUNT
#include "../math/vector3.h"

// We need to register these somewhere. The pattern from Rampage is a big gRampage state object
// LoL does the whole GameObjectManager thing. 
struct game_object
{
    struct Vector3 position;    
    struct Vector3 scale;
    const char* name;
    const char* rom;
    const unsigned int numTriangles;
};

static struct game_object gGameObjects[NUM_GAME_OBJECTS] = {
"""

header = header.replace("REPLACE_WITH_COUNT", str(len(visible)))
objects = []
structs = []
enum_values = []
with open(os.path.join(CPP_PATH, "game_object.h"), "w+") as f:
    f.write(header)
    for i,object in enumerate(visible):
        # Export the in game properties to some c file
        # Eventually this should produce a dynamic_object
        # from the collision system? but for now, we'll
        # just make sure it all works together
        struct = """    {{ {POSITION}, {SCALE}, {NAME}, {ROM}, {TRIS} }}"""
        struct = struct.format(
            i=i,
            POSITION=".position = {" + f"{object.location.x:.6f}f, {object.location.y:.6f}f, {object.location.z:.6f}f" + "}",
            SCALE=".scale = {" + f"{object.scale.x:.6f}f, {object.scale.y:.6f}f, {object.scale.z:.6f}f" + "}",
            NAME=f".name = \"{object.name}\"",
            ROM=f".rom = \"rom:/{object.name.lower()}.t3dm\"",
            TRIS=f".numTriangles = {len(object.data.polygons)}"
        )
        structs.append(struct)
        objects.append(object.name.lower())
        enum_values.append(f"{object.name.upper()} = {i}")
    f.write(",\n".join(structs))
    f.write("\n};\n")
    f.write("enum ObjectType {" + ", ".join(enum_values) + " };\n")
    
    getter_def = """void get_object(enum ObjectType object, struct game_object *outObject)
{{
    switch(object)
    {{
{cases}     
    }}
}}\n"""
    f.write(getter_def.format(cases="\n".join(f"        case {i}: outObject = &gGameObjects[{i}]; break;" for i,x in enumerate(visible))))
    f.write("#endif")
    

# Export the object as a glb
for object in visible:
    object.select_set(False)
    
for object in visible:
    object.select_set(True)
    path = os.path.join(MAIN_PATH, "assets", f"{object.name.lower()}.glb")
    bpy.ops.export_scene.gltf(filepath=path, export_extras=True)
    object.select_set(False)

# (nice to have) copy the textures over
for object in visible:
    if hasattr(object.data.materials[0], "f3d_mat"):
        # this has a material
        mat = object.data.materials[0].f3d_mat
        if hasattr(mat, "tex0") and mat.tex0.tex is not None:
            tex = mat.tex0.tex
            name = os.path.basename(tex.filepath)
            dest = os.path.join(MAIN_PATH, "assets", name)
            shutil.copy(tex.filepath_from_user(), dest)

# lets handle the camera
# i'm a grug brain, only 1 camera please
assert len(list(filter(lambda x: x.type == "CAMERA", scene.objects))) == 1
camera = [x for x in scene.objects if x.type == "CAMERA"][0]
camera_struct = "struct camera_opts { float fov; float near; float far; float pos[3];};\n"
camera_struct_data = """static struct camera_opts gCamera = 
{{
    .fov = {FOV},
    .near = {NEAR},
    .far = {FAR},
    .pos = {POS}
}};
"""
camera_struct_data = camera_struct_data.format(
    FOV=camera.fov,
    NEAR=f"{camera.data.clip_start:.6f}",
    FAR=f"{camera.data.clip_end:.6f}",
    POS="{" + ", ".join(f"{f:.6f}" for f in camera.location) + "}"
)

with open(os.path.join(CPP_PATH, "camera.h"), "w+") as f:
    f.write(camera_struct + camera_struct_data)