/*
** mrb_gpiotft.c - GpioTft class
**
** Copyright (c) Hiroki Mori 2019
**
** See Copyright Notice in LICENSE
*/

#include "mruby.h"
#include "mruby/data.h"
#include "mruby/array.h"
#include "mrb_gpiotft.h"

#include <fcntl.h>
#include <libgpio.h>
#include <unistd.h>
#include <sys/endian.h>

#define DONE mrb_gc_arena_restore(mrb, 0);

#define PH0	125
#define PI0	153

/* ILI9325 PI0 */
#define RS	(1 << 3)
#define CD	(1 << 19)
#define RD	(1 << 18)

/* ILI9325 PH0 */
#define WR	(1 << 6)

/* ILI9325 data = PH7 - PH14 */

typedef struct {
  gpio_handle_t handle;
  int width;
  int hight;
} mrb_bsdgpio_data;

static const struct mrb_data_type mrb_bsdgpio_data_type = {
  "mrb_bsdgpio_data", mrb_free,
};

static mrb_value mrb_gpiotft_init(mrb_state *mrb, mrb_value self)
{
  mrb_bsdgpio_data *data;
  mrb_int num;

  data = (mrb_bsdgpio_data *)DATA_PTR(self);
  if (data) {
    mrb_free(mrb, data);
  }
  DATA_TYPE(self) = &mrb_bsdgpio_data_type;
  DATA_PTR(self) = NULL;

  mrb_get_args(mrb, "i", &num);
  data = (mrb_bsdgpio_data *)mrb_malloc(mrb, sizeof(mrb_bsdgpio_data));
  data->handle = gpio_open(num);

  DATA_PTR(self) = data;

  return self;
}

void WriteDone(int fd)
{
  struct gpio_access_32 gacc32;

  gacc32.first_pin = PH0;
  gacc32.clear_pins = WR;
  gacc32.change_pins = WR;
  ioctl(fd, GPIOACCESS32, &gacc32);
}

void lcdWrite8(int fd, int byte)
{
  struct gpio_access_32 gacc32;

  gacc32.first_pin = PH0;
  gacc32.clear_pins = 0xff << 7 | WR;
  gacc32.change_pins = (byte & 0xff)  << 7;
  ioctl(fd, GPIOACCESS32, &gacc32); // set PH
}

void lcdWriteRegisterWord(int fd, int addr, int val)
{
  struct gpio_access_32 gacc32;
  /* addr */
  gacc32.first_pin = PI0;
  gacc32.clear_pins = RS | RD;
  gacc32.change_pins = RD;
  ioctl(fd, GPIOACCESS32, &gacc32); // set PI

  lcdWrite8(fd, addr >> 8);

  WriteDone(fd);

  lcdWrite8(fd, addr & 0xff);

  WriteDone(fd);

  gacc32.first_pin = PI0;
  gacc32.clear_pins = RS | RD;
  gacc32.change_pins = RS | RD;
  ioctl(fd, GPIOACCESS32, &gacc32); // set PI

  lcdWrite8(fd, val >> 8);

  WriteDone(fd);

  lcdWrite8(fd, val & 0xff);

  WriteDone(fd);

}

void
lcdWriteRegisterPixel(int fd, int val)
{

  lcdWrite8(fd, val >> 8);

  WriteDone(fd);

  lcdWrite8(fd, val & 0xff);

  WriteDone(fd);
}

static mrb_value mrb_gpiotft_setsize(mrb_state *mrb, mrb_value self)
{
  mrb_bsdgpio_data *data = DATA_PTR(self);
  mrb_int w, h;

  mrb_get_args(mrb, "ii", &w, &h);
  data->width = w;
  data->hight = h;

  return mrb_fixnum_value(0);
}

static mrb_value mrb_gpiotft_width(mrb_state *mrb, mrb_value self)
{
  mrb_bsdgpio_data *data = DATA_PTR(self);

  return mrb_fixnum_value(data->width);
}

static mrb_value mrb_gpiotft_hight(mrb_state *mrb, mrb_value self)
{
  mrb_bsdgpio_data *data = DATA_PTR(self);

  return mrb_fixnum_value(data->hight);
}

static mrb_value mrb_gpiotft_writereg(mrb_state *mrb, mrb_value self)
{
  mrb_bsdgpio_data *data = DATA_PTR(self);
  mrb_int addr, val;
  struct gpio_access_32 gacc32;

  mrb_get_args(mrb, "ii", &addr, &val);

  gacc32.first_pin = PI0;
  gacc32.clear_pins = CD;
  gacc32.change_pins = 0;
  ioctl(data->handle, GPIOACCESS32, &gacc32); // set PI

  lcdWriteRegisterWord(data->handle, addr, val);

  gacc32.first_pin = PI0;
  gacc32.clear_pins = CD;
  gacc32.change_pins = CD;
  ioctl(data->handle, GPIOACCESS32, &gacc32); // set PI

  return mrb_fixnum_value(0);
}

static mrb_value mrb_gpiotft_setline(mrb_state *mrb, mrb_value self)
{
  mrb_bsdgpio_data *data = DATA_PTR(self);
  mrb_int x, y, len;
  mrb_value arr;
  int rgb565;
  int i, r, g, b;
  struct gpio_access_32 gacc32;

  gacc32.first_pin = PI0;
  gacc32.clear_pins = CD;
  gacc32.change_pins = 0;
  ioctl(data->handle, GPIOACCESS32, &gacc32); // set PI

  mrb_get_args(mrb, "iiiA", &x, &y, &len, &arr);

  if (RARRAY_LEN( arr ) != len * 4)
    return mrb_fixnum_value(0);

  lcdWriteRegisterWord(data->handle, 0x0020, x);
  lcdWriteRegisterWord(data->handle, 0x0021, y);
  for (i = 0; i < len; ++i) {
    r = mrb_fixnum( mrb_ary_ref( mrb, arr, i * 4 + 2));
    g = mrb_fixnum( mrb_ary_ref( mrb, arr, i * 4 + 1));
    b = mrb_fixnum( mrb_ary_ref( mrb, arr, i * 4 + 0));
    rgb565 = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
    if (i == 0)
      lcdWriteRegisterWord(data->handle, 0x0022, rgb565);
    else
      lcdWriteRegisterPixel(data->handle, rgb565);
  }

  gacc32.first_pin = PI0;
  gacc32.clear_pins = CD;
  gacc32.change_pins = CD;
  ioctl(data->handle, GPIOACCESS32, &gacc32); // set PI

  return mrb_fixnum_value(0);
}

static mrb_value mrb_gpiotft_transfer2(mrb_state *mrb, mrb_value self)
{
  mrb_bsdgpio_data *data = DATA_PTR(self);
  mrb_int ptr;
  int color;
  int x, y;
  struct gpio_access_32 gacc32;
  unsigned char *framedata;

  mrb_get_args(mrb, "i", &ptr);

  framedata = (unsigned char *)ptr;

  gacc32.first_pin = PI0;
  gacc32.clear_pins = CD;
  gacc32.change_pins = 0;
  ioctl(data->handle, GPIOACCESS32, &gacc32); // set PI

  for (y = data->hight - 1; y >= 0; --y) {

    lcdWriteRegisterWord(data->handle, 0x0020, 0);
    lcdWriteRegisterWord(data->handle, 0x0021, y);
    for (x = 0; x < data->width; ++x) {
#if BYTE_ORDER == BIG_ENDIAN
      color = (*(framedata + 1) >> 3) << 11;
      color |= (*(framedata + 2) >> 2) << 5;
      color |= *(framedata + 3) >> 3;
#else
      color = (*(framedata + 2) >> 3) << 11;
      color |= (*(framedata + 1) >> 2) << 5;
      color |= *(framedata + 0) >> 3;
#endif
      framedata += 4;
      if (x == 0)
        lcdWriteRegisterWord(data->handle, 0x0022, color);
      else
        lcdWriteRegisterPixel(data->handle, color);
    }
  }

  gacc32.first_pin = PI0;
  gacc32.clear_pins = CD;
  gacc32.change_pins = CD;
  ioctl(data->handle, GPIOACCESS32, &gacc32); // set PI

  return mrb_fixnum_value(0);
}

void mrb_mruby_gpiotft_gem_init(mrb_state *mrb)
{
  struct RClass* bsdgpio = mrb_class_get(mrb, "BsdGpio");
  struct RClass *gpiotft;
  gpiotft = mrb_define_class(mrb, "GpioTft", bsdgpio);
  mrb_define_method(mrb, gpiotft, "initialize", mrb_gpiotft_init, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, gpiotft, "setsize", mrb_gpiotft_setsize, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, gpiotft, "width", mrb_gpiotft_width, MRB_ARGS_NONE());
  mrb_define_method(mrb, gpiotft, "hight", mrb_gpiotft_hight, MRB_ARGS_NONE());
  mrb_define_method(mrb, gpiotft, "writereg", mrb_gpiotft_writereg, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, gpiotft, "setline", mrb_gpiotft_setline, MRB_ARGS_REQ(4));
  mrb_define_method(mrb, gpiotft, "transfer2", mrb_gpiotft_transfer2, MRB_ARGS_REQ(1));
  DONE;
}

void mrb_mruby_gpiotft_gem_final(mrb_state *mrb)
{
}

