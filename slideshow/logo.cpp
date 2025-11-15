#include "slides.h"
#include <algorithm>
#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>

#define SPINSPEED 0.9f

namespace {
T3DVec3 UP{0.f, 1.f, 0.f};
}

// Easing function for quadratic ease-out
//  t = current time
//  b = start value
//  c = change in value
//  d = duration
static float ease_out_quad(float t, float b, float c, float d) {
  t /= d;
  if (t > 1)
    t = 1;
  return -c * t * (t - 2) + b;
}

void n64brew_logo(void) {
  const color_t VAN_DYKE = RGBA32(0x48, 0x3C, 0x3F, 0xFF);

  display_init(RESOLUTION_640x480, DEPTH_16_BPP, 3, GAMMA_NONE,
               FILTERS_DISABLED);
  t3d_init((T3DInitParams){});
  T3DModel *brew = t3d_model_load("rom:/brew_logo.t3dm");

  T3DViewport viewport = t3d_viewport_create();
  T3DVec3 camPos = {{0, 0.0f, 50.0f}};
  T3DVec3 camTarget = {{0, 0.0f, 0}};
  uint8_t colorAmbient[4] = {0xff, 0xff, 0xff, 0xFF};
  float scale = 0.08f;

  T3DMat4FP *mtx = (T3DMat4FP *)malloc_uncached(sizeof(T3DMat4FP));

  float mt0 = get_ticks_ms();
  float angle = T3D_DEG_TO_RAD(-90.0f);
  int anim_part = 0;
  while (1) {
    float tt = std::min(get_ticks_ms() - mt0, 4000.f);
    if (tt < 1500) {
      anim_part = 0;
      angle += SPINSPEED * display_get_delta_time();
    } else if (tt < 3500) {
      anim_part = 1;
      tt -= 1500;
      camTarget.v[0] = ease_out_quad(tt, 0.0f, -125.0f, 2000.0f);
      camTarget.v[1] = ease_out_quad(tt, 0.0f, -5.0f, 2000.0f);
      camPos.v[1] = ease_out_quad(tt, 0.0f, -5.0f, 2000.0f);
      camPos.v[2] = ease_out_quad(tt, 50.0f, 110.0f, 2000.0f);
      angle += SPINSPEED * display_get_delta_time();
    } else /* if (tt < 3800) */ {
      anim_part = 2;
      angle += SPINSPEED * display_get_delta_time();
      slide_update();
    }

    rdpq_attach(display_get(), display_get_zbuf());
    rdpq_clear(VAN_DYKE);
    rdpq_clear_z(ZBUF_MAX);

    t3d_frame_start();
    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(85.0f), 20.0f,
                                160.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &UP);
    t3d_viewport_attach(&viewport);
    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_count(0);

    t3d_mat4fp_from_srt_euler(mtx, (float[3]){scale, scale, scale},
                              (float[3]){0.0f, angle * 0.8f, 0},
                              (float[3]){0, 0, 0});
    t3d_matrix_push(mtx);
    t3d_model_draw(brew);
    t3d_matrix_pop(1);
    rdpq_sync_pipe();
    if (tt >= 3800) {
      slide_render();
    }
    rdpq_detach_show();
  }

  wait_ms(1000);
  rspq_wait();
  t3d_model_free(brew);
  t3d_destroy();
  display_close();
}
