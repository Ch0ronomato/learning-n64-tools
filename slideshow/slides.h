#pragma once
#include <libdragon.h>
typedef struct slide_s {
  size_t size;
  size_t id;
  char const *mData;
} slide_t;

#define MAKE_SLIDE(S)                                                          \
  (slide_t) { (sizeof(S) / sizeof(S[0])), __COUNTER__, S }

static slide_t slides[]{
    MAKE_SLIDE("N64 HOMEBREW\n\n\nA whirlwind tour\n"),
    MAKE_SLIDE(
        "Kinds of projects\n\n\n1. New games\n2. Game mods\n3. Hardware"),
    MAKE_SLIDE("NEW GAMES\n\n\n1. My crappy pourover game\n2. Portal 64 "
               "demake\n3. 4K demo"),
    MAKE_SLIDE("GAME MODS\n\n\n1. mods\n2. Decompilations\n3. Ports"),
    MAKE_SLIDE("HARDWARE\n\n\n1. New Console\n2. Flash Carts\n"),
    MAKE_SLIDE("OPEN SOURCE\n\n\n1. Libdragon\n2. Tiny3D\n3. Blender"),
    MAKE_SLIDE(
        "DIY\n\n\nMost of this is all DIY, build it yourself. No engine!"),
};
const int numSlides = __COUNTER__;
void slide_update();
void slide_render();
