#define LEDMATRIX_HEIGHT 8u
#define LEDMATRIX_WIDTH 32u
#define GFX_TEXTTOSHOW_BUFFER_SIZE 256u

uint8_t ledmatrix_framebuffer[LEDMATRIX_WIDTH][LEDMATRIX_HEIGHT];
uint8_t ledmatrix_framebuffer_onscreen[LEDMATRIX_WIDTH][LEDMATRIX_HEIGHT];

int32_t txtscroll_fall_off_character_position = LEDMATRIX_WIDTH + 1;
int32_t txtscroll_last_character_end_position = LEDMATRIX_WIDTH + 1;
uint32_t txtscroll_string_iter = 0u;
const uint8_t* text_to_show_data[GFX_TEXTTOSHOW_BUFFER_SIZE];

void clear_framebuffer()
{
  memset(ledmatrix_framebuffer, false, LEDMATRIX_WIDTH * LEDMATRIX_HEIGHT);
}

void render_framebuffer_to_screen()
{
  for (uint8_t x = 0; x < LEDMATRIX_WIDTH; x++) {
    for (uint8_t y = 0; y < LEDMATRIX_HEIGHT; y++) {
      max7219_set_pixel(x, y, ledmatrix_framebuffer[x][y]);
    }
  }
}

void inline render_pixel_to_frambuffer(uint8_t x, uint8_t y, bool val)
{
  if (x >= LEDMATRIX_WIDTH || y >= LEDMATRIX_HEIGHT) {
    return;
  }
  ledmatrix_framebuffer[x][y] = val;
}

void shift_framebuffer_left()
{
  /* Shift every column to the left */
  for (uint8_t x = 0; x < LEDMATRIX_WIDTH - 1; x++) {
    for (uint8_t y = 0; y < LEDMATRIX_HEIGHT; y++) {
      ledmatrix_framebuffer[x][y] = ledmatrix_framebuffer[x + 1][y];
    }
  }

  /* Fill the last column with false values */
  for (uint8_t y = 0; y < LEDMATRIX_HEIGHT; y++) {
    ledmatrix_framebuffer[LEDMATRIX_WIDTH - 1][y] = false;
  }
}

void setxy_framebuffer(uint8_t x, uint8_t y, bool val)
{
  if (x >= LEDMATRIX_WIDTH || y >= LEDMATRIX_HEIGHT) {
    return;
  }
  ledmatrix_framebuffer[x][y] = val;
}

void setxy_framebuffer(uint8_t x, uint8_t y, uint8_t val)
{
  if (x >= LEDMATRIX_WIDTH || y >= LEDMATRIX_HEIGHT) {
    return;
  }
  ledmatrix_framebuffer[x][y] = val;
}

bool getxy_framebuffer(uint8_t x, uint8_t y) {
  if (x >= LEDMATRIX_WIDTH || y >= LEDMATRIX_HEIGHT) {
    return false;
  }
  return ledmatrix_framebuffer[x][y];
}

void draw_num(uint8_t x_offset, uint8_t y_offset, uint8_t number)
{
  if (number >= ledmatrix_number_font_len) {
    return;
  }
  for (uint8_t cols = 0u; cols < LEDMATRIX_CHAR_Y_LEN; cols++) {
    for (uint8_t rows = 0u; rows < LEDMATRIX_CHAR_X_LEN; rows++) {
      /* If the bit at the current position is set*/
      if (ledmatrix_number_font[number][cols] & (1 << (LEDMATRIX_CHAR_X_LEN - 1 - rows))) {
        setxy_framebuffer(x_offset + rows, y_offset + cols, true);
      }
    }
  }
}

void draw_small_num(uint8_t x_offset, uint8_t y_offset, uint8_t number)
{
  if (number >= ledmatrix_small_number_font_len) {
    return;
  }
  for (uint8_t cols = 0u; cols < LEDMATRIX_SMALL_CHAR_Y_LEN; cols++) {
    for (uint8_t rows = 0u; rows < LEDMATRIX_SMALL_CHAR_X_LEN; rows++) {
      /* If the bit at the current position is set*/
      if (ledmatrix_small_number_font[number][cols] & (1 << (LEDMATRIX_SMALL_CHAR_X_LEN - 1 - rows))) {
        setxy_framebuffer(x_offset + rows, y_offset + cols, true);
      }
    }
  }
}

uint8_t draw_letter(uint8_t x_offset, uint8_t y_offset, const uint8_t* character_data)
{
  /* If there's nothing to draw, just return with the received offset */
  if (character_data == NULL) {
    return x_offset;
  }

  uint8_t charachter_width;
  memcpy_P(&charachter_width, &character_data[0], 1u);
  charachter_width++;

  uint8_t cols;
  uint8_t rows;
  uint8_t character_row;

  for (rows = 0u; rows < LEDMATRIX_CHAR_Y_LEN; rows++) {
    for (cols = 0u; cols < charachter_width; cols++) {
      /* If the it at the current position is set*/
      /* Offset cols by one, because length is stored at 0*/
      memcpy_P(&character_row, &character_data[rows + 1], 1u);
      if (character_row & (1 << (7 - cols))) {
        render_pixel_to_frambuffer(x_offset + cols, y_offset + rows, true);
      }
    }
  }

  /* Return the x position where the next character can safely be drawn */
  return x_offset + charachter_width;
}

void render_text_to_framebuffer(String text, uint8_t x_offset, uint8_t y_offset)
{
  uint8_t character_offset = 0u;
  for (uint8_t i = 0; text[i] != '\0'; i++) {
    /* Workaround - fix to have UTF8 here as well */
    const uint8_t* char_to_draw = ledmatrix_ascii_table[(uint8_t)text[i]];

    character_offset = draw_letter(x_offset + character_offset, y_offset, char_to_draw);
    x_offset = 0;
    if (character_offset >= LEDMATRIX_WIDTH) {
      break;
    }
  }
}

void setup_text_scroll(String text)
{
  /* Initialize the fall off position off the screen, so the scrolling starts from off the screen on the right */
  /* Also add a ten pixel divide to the screen width to have some spacer from anything thats on screen */
  txtscroll_fall_off_character_position = LEDMATRIX_WIDTH + 10;
  txtscroll_last_character_end_position = txtscroll_fall_off_character_position;
  txtscroll_string_iter = 0u;

  convert_utf8_string_to_character_data(text_to_show_data, text);
}

bool scroll_text()
{
  /* If we do not clear the framebuffer on start, it makes for a cool transition */
  /*
     if(string_iter == 0)  {
     ledmatrix_clear_framebuffer();
     }
   */

  /* Render what's visible on screen */
  /* Render the next character where it has fallen off screen */
  int32_t x_render_offset = txtscroll_fall_off_character_position;
  int32_t character_offset;

  for (; text_to_show_data[txtscroll_string_iter] != NULL; txtscroll_string_iter++) {
    /* Draw the current character */
    character_offset = draw_letter(x_render_offset, 0, text_to_show_data[txtscroll_string_iter]);
    txtscroll_last_character_end_position = character_offset;

    /*If the rendered character falls down from the screen on the right end */
    if (character_offset >= (int32_t)LEDMATRIX_WIDTH) {
      /* Remember the starting position of the character that fell off */
      txtscroll_fall_off_character_position = x_render_offset;
      /* Do no render anything else */
      break;
    } else {
      /* The character fitted on the screen, set the render offset to its end */
      x_render_offset = character_offset;
    }
  }

  /* Check if we have finished displaying the text */
  /* If we have ran out of characters to render and we shifted all the text to the left end */
  if (text_to_show_data[txtscroll_string_iter] == NULL && txtscroll_last_character_end_position < 1) {
    txtscroll_fall_off_character_position = LEDMATRIX_WIDTH + 1;
    txtscroll_string_iter = 0u;
    return false;
  }

  render_framebuffer_to_screen();

  /* Move the display to the left */
  shift_framebuffer_left();
  /* Move the falling off character to the left by one */
  /* The next iteration will render it one position to the left */
  if (txtscroll_fall_off_character_position > 0) {
    txtscroll_fall_off_character_position--;
  }
  if (txtscroll_last_character_end_position > 0) {
    txtscroll_last_character_end_position--;
  }

  return true;
}
