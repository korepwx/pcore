#!/usr/bin/env python
# -*- encoding: utf-8 -*-

def dump_palette(pal):
  for p in pal:
    print '    {%s, %s, %s, 255},' % (p[0], p[1], p[2])

def make_code(name, comment, pal, size):
  print ('// ---- %s palette ----' % comment)
  print ('static RgbaColor default%sColors[] = {' % name)
  dump_palette(pal)
  print ('};')
  print ('ColorPalette kDefault%sPalette = {' % name)
  print ('  .size = %s,' % size)
  print ('  .fillsize = %s,' % len(pal))
  print ('  .data = default%sColors' % name)
  print ('};')
  print ('')
  
## Generate file headers ##
print ('#include <color.h>')
print ('')

## gray 16 palette ##
pal = [0] * 16
for r in range(0, 16):
  pal[r] = [int(r*(255/15.0))] * 3
make_code('16Gray', '16 gray', pal, 16)

## console 16 palette ##
# Browse MSDN for COLORREF to learn more about these RGB values
pal = [
    0x00000000, 0x00800000, 0x00008000, 0x00808000,
    0x00000080, 0x00800080, 0x00008080, 0x00c0c0c0,
    0x00808080, 0x00ff0000, 0x0000ff00, 0x00ffff00,
    0x000000ff, 0x00ff00ff, 0x0000ffff, 0x00ffffff
]
G = lambda x: (x & 0xff, (x >> 8) & 0xff, (x >> 16) & 0xff)
Q = lambda x: [255 - c for c in x]
pal = [G(c) for c in pal]
make_code('16Cons', '16 console', pal, 16)

## grayscale 256 palette ##
pal = [0] * 256
for r in range(0, 256):
  pal[r] = (r, r, r)
make_code('256Gray', '256 grayscale', pal, 256)

## 6-8-5 levels RGB palette, (40×R)+(5×G)+B. Remain 16 fill with 7 grays. ##
pal = [0] * 256
for r in range(0, 6):
  for g in range(0, 8):
    for b in range(0, 5):
      pal[r * 40 + g * 5 + b] = (int(r * (255.0 / 5.0)), int(g * (255.0 / 7.0)), int(b * (255.0 / 4.0)))
for v in range(0, 7):
  pal[240 + v] = tuple([int((v+1) * (255.0 / 7.0))] * 3)
for i in range(0, 9):
  pal[247+i] = (0, 0, 0)

make_code('256Rgb', '256 rgb', pal, 256)

