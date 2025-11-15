#include "logo.h"
#include "rdpq_font.h"
#include <libdragon.h>

/**
 * Simple example with a 3d-model file created in blender.
 * This uses the builtin model format for loading and drawing a model.
 */
int main() {
  debug_init_isviewer();
  debug_init_usblog();
  asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);
  rdpq_init();
  joypad_init();
  auto style = (rdpq_fontstyle_t){
      .color = RGBA32(0xCC, 0xAE, 0xDD, 0xFF),
  };
  rdpq_font_t *fnt1 = rdpq_font_load("rom:/ga-grotesk-2.font64");
  rdpq_font_style(fnt1, 0, &style);
  rdpq_text_register_font(1, fnt1);
  n64brew_logo();
  return 0;
}
