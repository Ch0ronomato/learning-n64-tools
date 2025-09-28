#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>
#include "./data/game_object.h"
// @todo: Can we handle the camera?
#include "./data/camera.h"

/**
 * This shows how you can draw multiple objects (actors) in a scene.
 * Each actor has a scale, rotation, translation (knows as SRT) and a model matrix.
 */
#define RAD_360 6.28318530718f

static float objTimeLast = 0.0f;
static float objTime = 0.0f;
static float baseSpeed = 1.0f;

// Holds our actor data, relevant for t3d is 'modelMat'.
typedef struct {
  uint32_t id;
  float pos[3];
  float rot[3];
  float scale[3];

  rspq_block_t *dpl;
  T3DMat4FP *modelMat;
} Actor;

Actor actor_create(uint32_t id, rspq_block_t *dpl)
{
  // our model scales come from blender in game_object.h
  Actor actor = (Actor){
    .id = id,
    .pos = {gGameObjects[id].position.x, gGameObjects[id].position.y, gGameObjects[id].position.z},
    .rot = {0, 0, 0},
    .scale = {gGameObjects[id].scale.x, gGameObjects[id].scale.y, gGameObjects[id].scale.z},
    .dpl = dpl,
    .modelMat = malloc_uncached(sizeof(T3DMat4FP)) // needed for t3d
  };
  t3d_mat4fp_identity(actor.modelMat);
  return actor;
}

void actor_update(Actor *actor) {
  // set some random position and rotation
  float randRot = (float)fm_fmodf(actor->id * 123.1f, 5.0f);

  // keep x and z the same
  actor->rot[2] = fm_fmodf(randRot + objTime * 1.03f, RAD_360);
  // actor->pos[1] = randDist * fm_sinf(objTime * 1.5f + randRot);

  // t3d lets you directly construct a fixed-point matrix from SRT
  t3d_mat4fp_from_srt_euler(actor->modelMat, actor->scale, actor->rot, actor->pos);
}

void actor_draw(int index, Actor *actor) {
  t3d_matrix_set(actor->modelMat, true);
  rspq_block_run(actor->dpl);
}

void actor_delete(Actor *actor) {
  free_uncached(actor->modelMat);
}

float get_time_s()  { return (float)((double)get_ticks_ms() / 1000.0); }
float get_time_ms() { return (float)((double)get_ticks_us() / 1000.0); }

int main()
{
	debug_init_isviewer();
	debug_init_usblog();
	asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);

  rdpq_init();
  joypad_init();

  t3d_init((T3DInitParams){});
  T3DViewport viewport = t3d_viewport_create();
  rdpq_text_register_font(FONT_BUILTIN_DEBUG_MONO, rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO));

  // Load a some models
  rspq_block_t *dpls[2];
  T3DModel *models[NUM_GAME_OBJECTS];
  int triangleCount[NUM_GAME_OBJECTS];
  for (size_t i = 0; i < NUM_GAME_OBJECTS; i++)
  {
    models[i] = t3d_model_load(gGameObjects[i].rom);
    triangleCount[i] = gGameObjects[i].numTriangles;
  }

  for(int i=0; i<NUM_GAME_OBJECTS; ++i) {
    rspq_block_begin();
    t3d_model_draw(models[i]);
    dpls[i] = rspq_block_end();
  }

  Actor actors[NUM_GAME_OBJECTS];
  for(int i=0; i<NUM_GAME_OBJECTS; ++i) {
    actors[i] = actor_create(i, dpls[i]);
  }

  const T3DVec3 camPos = {{gCamera.pos[0], gCamera.pos[1], gCamera.pos[2]}};
  const T3DVec3 camTarget = {{0,0,0}}; // look at the origin I guess

  uint8_t colorAmbient[4] = {80, 50, 50, 0xFF};
  T3DVec3 lightDirVec = {{1.0f, 1.0f, 1.0f}};
  uint8_t lightDirColor[4] = {120, 120, 120, 0xFF};
  t3d_vec3_norm(&lightDirVec);

  for(;;)
  {
    // ======== Update ======== //
    joypad_poll();
    // joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);

    float newTime = get_time_s();
    float deltaTime = (newTime - objTimeLast) * baseSpeed;
    objTimeLast = newTime;
    objTime += deltaTime;

    float timeUpdate = get_time_ms();
    for(int i=0; i<NUM_GAME_OBJECTS; ++i) {
      actor_update(&actors[i]);
    }
    timeUpdate = get_time_ms() - timeUpdate;

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(65.0f), 10.f, 500.f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,0,1}});

    // ======== Draw (3D) ======== //
    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();
    t3d_viewport_attach(&viewport);

    rdpq_set_prim_color(RGBA32(0, 0, 0, 0xFF));

    t3d_screen_clear_color(RGBA32(100, 120, 160, 0xFF));
    t3d_screen_clear_depth();

    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_directional(0, lightDirColor, &lightDirVec);
    t3d_light_set_count(1);

    t3d_matrix_push_pos(1);
    for(int i=0; i<NUM_GAME_OBJECTS; ++i) {
      actor_draw(i, &actors[i]);
    }
    t3d_matrix_pop(1);

    // ======== Draw (2D) ======== //
    rdpq_sync_pipe();

    int totalTris = 0;
    for(int i=0; i<NUM_GAME_OBJECTS; ++i) {
      totalTris += triangleCount[i];
    }

    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 210, "    [C] Actors: %d", NUM_GAME_OBJECTS);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 220, "[STICK] Speed : %.2f", baseSpeed);

    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 200, 200, "Tris  : %d", totalTris);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 200, 210, "Update: %.2fms", timeUpdate);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 200, 220, "FPS   : %.2f", display_get_fps());

    rdpq_detach_show();
  }

  t3d_destroy();
  return 0;
}
