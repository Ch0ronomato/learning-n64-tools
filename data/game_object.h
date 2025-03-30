
// THIS CODE WAS GENERATED IN BLENDER

#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#define NUM_GAME_OBJECTS 2
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
    { .position = {0.643463f, -0.003669f, 0.480978f}, .scale = {0.050000f, 0.050000f, 0.050000f}, .name = "Sphere", .rom = "rom:/sphere.t3dm", .numTriangles = 512 },
    { .position = {0.000000f, 0.000000f, 0.000000f}, .scale = {3.070956f, 3.070956f, 3.070956f}, .name = "Glass", .rom = "rom:/glass.t3dm", .numTriangles = 986 }
};
enum ObjectType {SPHERE = 0, GLASS = 1 };
void get_object(enum ObjectType object, struct game_object *outObject)
{
    switch(object)
    {
        case 0: outObject = &gGameObjects[0]; break;
        case 1: outObject = &gGameObjects[1]; break;     
    }
}
#endif