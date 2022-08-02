#include <stdio.h>

/* <<< BytePusher hexkey test >>>
 * By Javamannen (Adam Danielsson)
 * Last revised: 2010-09-22
 *
 * This program generates the file "Keyboard Test.BytePusher",
 * which is a BytePusher app to test the machine's hex keyboard.
 * To run the generated program you need a BytePusher VM,
 * which can be downloaded from the BytePusher page at:
 * http://esolangs.org/wiki/BytePusher.
 * If you've created a BytePusher program or VM, please post it on that page!
 */

unsigned char hex_digits[16][16][16] = {
  {"................","................","................","................","................","................","................","................","................","................","................","................","................","................","................","................"},
  {".....xxxxxx.....",".......xx.......","..xxxxxxxxxxx...",".xxxxxxxxxxxx...","...........xx...",".xxxxxxxxxxxxxx.",".....xxxxxxxx...",".xxxxxxxxxxxxxx.","....xxxxxxxx....","....xxxxxxxx....","....xxxxxxxx....",".xxxxxxxxxxx....","...xxxxxxxxxxxx.",".xxxxxxxxxx.....",".xxxxxxxxxxxxxx.",".xxxxxxxxxxxxxx."},
  {"....xxxxxxxx....","......xxx.......","..xxxxxxxxxxxx..",".xxxxxxxxxxxxx..","..........xxx...",".xxxxxxxxxxxxxx.","....xxxxxxxxx...",".xxxxxxxxxxxxxx.","...xxxxxxxxxx...","...xxxxxxxxxx...","...xxxxxxxxxx...",".xxxxxxxxxxxx...","..xxxxxxxxxxxxx.",".xxxxxxxxxxx....",".xxxxxxxxxxxxxx.",".xxxxxxxxxxxxxx."},
  {"...xx......xx...",".....xxxx.......",".............xx.",".............xx.",".........xxxx...",".xx.............","...xx...........","............xx..","..xx........xx..","..xx........xx..","..xx........xx..",".xx.........xx..",".xx.............",".xx........xx...",".xx.............",".xx............."},
  {"..xx.......xxx..","....xx.xx.......",".............xx.",".............xx.","........xx.xx...",".xx.............","..xx............","...........xx...",".xx..........xx.",".xx..........xx.",".xx..........xx.",".xx..........xx.",".xx.............",".xx.........xx..",".xx.............",".xx............."},
  {".xx.......x..xx.","...xx..xx.......",".............xx.",".............xx.",".......xx..xx...",".xx.............",".xx.............","..........xx....",".xx..........xx.",".xx..........xx.",".xx..........xx.",".xx..........xx.",".xx.............",".xx..........xx.",".xx.............",".xx............."},
  {".xx......x...xx.",".......xx.......",".............xx.",".............xx.","......xx...xx...",".xx.............",".xx.............",".........xx.....","..xx........xx..","..xx.........xx.",".xx..........xx.",".xx.........xx..",".xx.............",".xx..........xx.",".xx.............",".xx............."},
  {".xx.....x....xx.",".......xx.......","....xxxxxxxxxx..","....xxxxxxxxxx..",".....xx....xx...",".xxxxxxxxxxx....",".xxxxxxxxxxx....","........xx......","...xxxxxxxxxx...","...xxxxxxxxxxx..",".xxxxxxxxxxxxxx.",".xxxxxxxxxxxx...",".xx.............",".xx..........xx.",".xxxxxxxxxxxx...",".xxxxxxxxxxxx..."},
  {".xx....x.....xx.",".......xx.......","...xxxxxxxxxx...","....xxxxxxxxx...","....xx.....xx...",".xxxxxxxxxxxx...",".xxxxxxxxxxxx...",".......xx.......","...xxxxxxxxxx...","....xxxxxxxxx...",".xxxxxxxxxxxxxx.",".xxxxxxxxxxxx...",".xx.............",".xx..........xx.",".xxxxxxxxxxxx...",".xxxxxxxxxxxx..."},
  {".xx...x......xx.",".......xx.......","..xx............","............xx..","...xx......xx...","............xx..",".xx.........xx..","......xx........","..xx........xx..","..........xx....",".xx..........xx.",".xx.........xx..",".xx.............",".xx..........xx.",".xx.............",".xx............."},
  {".xx..x.......xx.",".......xx.......",".xx.............",".............xx.","..xxxxxxxxxxxxx.",".............xx.",".xx..........xx.",".....xx.........",".xx..........xx.",".........xx.....",".xx..........xx.",".xx..........xx.",".xx.............",".xx..........xx.",".xx.............",".xx............."},
  {"..xxx.......xx..",".......xx.......",".xx.............",".............xx.",".xxxxxxxxxxxxxx.",".............xx.",".xx..........xx.","....xx..........",".xx..........xx.","........xx......",".xx..........xx.",".xx..........xx.",".xx.............",".xx.........xx..",".xx.............",".xx............."},
  {"...xx......xx...",".......xx.......",".xx.............",".............xx.","...........xx...",".............xx.",".xx..........xx.","...xx...........",".xx..........xx.",".......xx.......",".xx..........xx.",".xx..........xx.",".xx.............",".xx........xx...",".xx.............",".xx............."},
  {"....xxxxxxxx....","...xxxxxxxxxx...",".xxxxxxxxxxxxxx.",".xxxxxxxxxxxxx..","...........xx...",".xxxxxxxxxxxxx..","..xxxxxxxxxxxx..","..xx............","..xxxxxxxxxxxx..","......xx........",".xx..........xx.",".xxxxxxxxxxxxx..","..xxxxxxxxxxxxx.",".xxxxxxxxxxx....",".xxxxxxxxxxxxxx.",".xx............."},
  {".....xxxxxx.....","...xxxxxxxxxx...",".xxxxxxxxxxxxxx.",".xxxxxxxxxxxx...","...........xx...",".xxxxxxxxxxxx...","...xxxxxxxxxx...",".xx.............","...xxxxxxxxxx...",".....xx.........",".xx..........xx.",".xxxxxxxxxxxx...","...xxxxxxxxxxxx.",".xxxxxxxxxx.....",".xxxxxxxxxxxxxx.",".xx............."},
  {"................","................","................","................","................","................","................","................","................","................","................","................","................","................","................","................"}
};

int color(int r, int g, int b) {return r*36 + g*6 + b;};

enum {PAGE = 256, BANK = PAGE*PAGE, MEMSIZE = PAGE*BANK};
unsigned char mem[MEMSIZE];
int pc;
enum {ADDR = 3, INSTR = 3*ADDR};
#define next (pc+INSTR)
#define prev (pc-INSTR)

enum {
  KEYS  = 0x00,
  RESET = 0x02,
  GFX   = 0x05,
  SND   = 0x06
};
// zeropage
int digit_colors;
// pages
int samples, CONST, rshift, lsb_color, prog;
// banks
int pixels = BANK;

void org (int a) {pc  = a;}
void skip(int x) {pc += x;}
void next_page(void) {pc  = (pc & ~(PAGE-1)) + PAGE;}
void next_bank(void) {pc  = (pc & ~(BANK-1)) + BANK;}

void byte(int a) {mem[pc++] = a;}
void word(int a) {byte(a >>  8); byte(a);}
void addr(int a) {byte(a >> 16); word(a);}

void bbj (int a, int b, int c) {addr(a); addr(b); addr(c);}
void mov (int a, int b) {bbj(a, b, next);}
void jmp (int c       ) {bbj(0, 0, c   );}
void nop (void        ) {bbj(0, 0, next);}
void wait(void        ) {bbj(0, 0, pc  );}

void print_digits(void) {
  int c, c2, x, y, xx, yy, i, v;
  static int keymap[16] = {13,0,1,2,4,5,6,8,9,10,12,14,3,7,11,15};
  for (c = 0; c < 16; c++) {
    c2 = keymap[c]; yy = c2/4*64; xx = c2%4*64; v = digit_colors+c;
    for (y = 0; y < 16; y++) {
      for (x = 0; x < 16; x++) {
        if (hex_digits[y][c][x] == 'x')
          mov(v, pixels+(yy+24+y)*256+(xx+24+x));
      }
    }
    // draw a box around the digit
    for (i = 0; i < 63; i++) mov(v, pixels +  yy    *256+xx+ i);
    for (i = 0; i < 63; i++) mov(v, pixels + (yy+62)*256+xx+ i);
    for (i = 0; i < 63; i++) mov(v, pixels + (yy+ i)*256+xx   );
    for (i = 0; i < 63; i++) mov(v, pixels + (yy+ i)*256+xx+62);
  }
}

void read_keys(void) {
  int i, j;
  for (j = 0; j < 2; j++) {
    mov(KEYS+j, next+2);
    for (i = 0; i < 7; i++) {
      mov(lsb_color, digit_colors+i+8*(1-j));
      mov(prev+2, next+2);
      mov(rshift, next+2);
    }
    mov(lsb_color, digit_colors+i+8*(1-j));
  }
}

void assemble(void) {
  org(prog);
  read_keys();
  print_digits();
  wait();
}

void save_file(void) {
  FILE *f = fopen("Keyboard Test.BytePusher", "wb");
  fwrite(mem, 1, pc, f); fclose(f);
}

void init_zeropage(void) {
  org (RESET); addr(prog);
  byte(pixels  / BANK);
  word(samples / PAGE);
  digit_colors = pc; skip(16);
}

void init_pages(void) {
  int i;
  org(PAGE);
  samples   = pc; next_page();
  CONST     = pc; for (i = 0; i < PAGE; i++) byte(i);
  rshift    = pc; for (i = 0; i < PAGE; i++) byte(i >> 1);
  lsb_color = pc; for (i = 0; i < PAGE; i++) byte((i & 1) ? color(5,5,5) : color(0,0,5));
  prog      = pc;
}

int main(int argc, char *argv[]) {
  init_pages();
  init_zeropage();
  assemble();
  save_file();
  return 0;
}
