#include "slides.h"
#include "rdpq_paragraph.h"
#include "rdpq_text.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>

namespace {
size_t kCharsPerTick = 3;
size_t currentCharsRendered = 0;
uint8_t currentSlideIndex = 0;
bool newSlide = true;
rdpq_textparms_t kTextParams{
    // .line_spacing = -3,
    .width = 320,         .height = 450,     .align = ALIGN_LEFT,
    .valign = VALIGN_TOP, .wrap = WRAP_WORD,
};

void transition_slide() {
  currentCharsRendered = 0;
  newSlide = true;
}
} // namespace
void slide_render() {
  int length = 0;
  if (newSlide) {
    if (currentCharsRendered < slides[currentSlideIndex].size) {
      // render a slice
      currentCharsRendered += kCharsPerTick;
      length = std::min(currentCharsRendered, slides[currentSlideIndex].size);
    } else {
      // render the slide
      length = slides[currentSlideIndex].size;
      newSlide = false;
    }
  } else {
    length = slides[currentSlideIndex].size;
  }

  auto paragraph = rdpq_paragraph_build(
      &kTextParams, 1, slides[currentSlideIndex].mData, &length);

  rdpq_paragraph_render(paragraph, 50, 50);
  rdpq_paragraph_free(paragraph);
}

void slide_update() {
  joypad_poll();
  auto pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
  if (pressed.a || pressed.b) {
    transition_slide();
    if (pressed.a) {
      currentSlideIndex = std::clamp(currentSlideIndex + 1, 0, numSlides - 1);
      debugf("Current %i total %i\n", currentSlideIndex, numSlides);
    } else {
      currentSlideIndex = std::clamp(currentSlideIndex - 1, 0, numSlides - 1);
      debugf("Current %i total %i\n", currentSlideIndex, numSlides);
    }
  }
}
