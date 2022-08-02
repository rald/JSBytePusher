#include <stdio.h>
#include <math.h>

/* <<< Sprites example for the BytePusher VM       >>>
 * <<< By Javamannen (Adam Danielsson)  2010-10-06 >>>
 *
 * This program generates "Sprites.BytePusher",
 * which moves some sprites in various patterns
 * over a background image.
 *
 * To run the generated program you need a BytePusher VM,
 * which can be downloaded from the BytePusher page at:
 * http://esolangs.org/wiki/BytePusher.
 * If you've created a BytePusher program or VM, please post it on that page!
 */

enum {NSPRITES = 10, SPRITEW = 25, SPRITEH = SPRITEW};
unsigned char sprd[NSPRITES*SPRITEW*SPRITEH] =
"xxxxxxxxxxxxxxxxxxxxx    xxxxxx             xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx    xxxxx               xxxxx    xxxxxxxxxxxxxxxxxxxxxxxxxx               xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx    "
"x....................x   x.....x           x.....xx.......................xx.......................xx....................x   x...x               x...x   x....................xx...x               x...xx.......................xx....................x   "
"x.....................x  x......x         x......xx.......................xx.......................xx.....................x  x...x               x...x  x.....................xx...x               x...xx.......................xx.....................x  "
"x......................x x.......x       x.......xx.......................xx.......................xx......................x x...x               x...x x......................xx...x               x...xx.......................xx......................x "
"x...xxxxxxxxxxxx........xx........x     x........xx.......................xx........xxxxxxxxxxxxxxxxx...xxxxxxxxxxxx........xx...x               x...xx.........xxxxxxxxxxxxxxxx...x               x...xx........xxxxxxxxxxxxxxxxx...xxxxxxxxxxxxxxxxx...x"
"x...x          x........xx.........xxxxx.........xx.......................xx........x               x...x          x........xx...x               x...xx........x               x...x               x...xx........x               x...x               x...x"
"x...x          x.......x  x.....................x x.......................xx........x               x...x          x........xx...x               x...xx.......x                x...x               x...xx........x               x...x               x...x"
"x...x          x......x    x...................x  x.......................xx........x               x...x          x........xx...x               x...xx......x                 x...x               x...xx........x               x...x               x...x"
"x...x          x.....x      x.................x   x.......................xx........x               x...x          x........xx...x               x...xx.....x                  x...x               x...xx........x               x...x               x...x"
"x...x          x....x        x...............x    xxxxxxxxxxx...xxxxxxxxxxxx........x               x...x          x........xx...x               x...xx....x                   x...x               x...xx........x               x...x               x...x"
"x...xxxxxxxxxxxx....x         x.............x               x...x          x........xxxxxxxxxxx     x...xxxxxxxxxxxx........xx...x               x...xx...xxxxxxxxxxxxxxxxx    x...xxxxxxxxxxxxxxxxx...xx........xxxxxxxxxxx     x...xxxxxxxxxxxxxxxxx...x"
"x....................x         x...........x                x...x          x..................x     x......................x x...x               x...x x...................x   x.......................xx..................x     x......................x "
"x.....................x         x.........x                 x...x          x..................x     x.....................x  x...x               x...x  x...................x  x.......................xx..................x     x.....................x  "
"x......................x         x.......x                  x...x          x..................x     x....................x   x...x               x...x   x...................x x.......................xx..................x     x....................x   "
"x...xxxxxxxxxxxx........x         x.....x                   x...x          x........xxxxxxxxxxx     x...xxxxxxxxxxxxxxxxx    x...x               x...x    xxxxxxxxxxxxxxxxx...xx.......................xx........xxxxxxxxxxx     x...xxxxxx..........x    "
"x...x          x........x          x...x                    x...x          x........x               x...x                    x....x             x....x                   x....xx.......................xx........x               x...x     x.........x    "
"x...x          x........x          x...x                    x...x          x........x               x...x                    x.....x           x.....x                  x.....xx.......................xx........x               x...x      x.........x   "
"x...x          x........x          x...x                    x...x          x........x               x...x                    x......x         x......x                 x......xx.......................xx........x               x...x       x.........x  "
"x...x          x........x          x...x                    x...x          x........x               x...x                    x.......x       x.......x                x.......xx.......................xx........x               x...x        x.........x "
"x...x          x........x          x...x                    x...x          x........x               x...x                    x........x     x........x               x........xx...xxxxxxxxxxxxxxxxx...xx........x               x...x         x.........x"
"x...xxxxxxxxxxxx........x          x...x                    x...x          x........xxxxxxxxxxxxxxxxx...x                    x.........xxxxx.........xxxxxxxxxxxxxxxx.........xx...x               x...xx........xxxxxxxxxxxxxxxxx...x          x........x"
"x......................x           x...x                    x...x          x.......................xx...x                     x.....................x x......................x x...x               x...xx.......................xx...x           x.......x"
"x.....................x            x...x                    x...x          x.......................xx...x                      x...................x  x.....................x  x...x               x...xx.......................xx...x            x......x"
"x....................x             x...x                    x...x          x.......................xx...x                       x.................x   x....................x   x...x               x...xx.......................xx...x             x.....x"
"xxxxxxxxxxxxxxxxxxxxx              xxxxx                    xxxxx          xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                        xxxxxxxxxxxxxxxxx    xxxxxxxxxxxxxxxxxxxxx    xxxxx               xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx              xxxxxx"
;

int n_spritepix;
void count_pixels(void) {int i; for (i=0;i<sizeof sprd;i++) if (sprd[i]!=' ') n_spritepix++;}

enum {PAGE = 256, BANK = PAGE*PAGE, MEMSIZE = PAGE*BANK};
unsigned char mem[MEMSIZE];
int pc;
enum {ADDR = 3, INSTR = 3*ADDR};
#define next (pc+INSTR)
#define prev (pc-INSTR)

// zeropage
enum {
  KEYS  = 0x00,
  RESET = 0x02,
  GFX   = 0x05,
  SND   = 0x06
};
// pages
int samples, CONST, inc, incnyb, add16, iszero, select, prog;
// banks
int pixels;

void org (int a) {pc  = a;}
void skip(int x) {pc += x;}
void next_page(void) {pc = (pc & ~(PAGE-1)) + PAGE;}
void next_bank(void) {pc = (pc & ~(BANK-1)) + BANK;}
void byte(int a) {mem[pc++] = a;}
void word(int a) {byte(a >> 8) ; byte(a);}
void addr(int a) {byte(a >> 16); word(a);}

// Mnemonics
void bbj (int a, int b, int c) {addr(a); addr(b); addr(c);}
void mov (int a, int b) {bbj(a, b, next);}
void jmp (int c       ) {bbj(0, 0, c   );}
void nop (void        ) {bbj(0, 0, next);}
void wait(void        ) {bbj(0, 0, pc  );}

int color(int r, int g, int b) {return r*36 + g*6 + b;};

int tmp;
int xycount, xtable, ytable;
int spritex, spritey;
int progstart, end, savepix;
int draw_spr_start  , draw_spr_return;
int restore_bg_start, restore_bg_return;
int save_bg_start   , save_bg_return;
int write_xy_start  , write_xy_return;

void call(int start, int ret) {
  mem[ret+0] = next>>16;
  mem[ret+1] = next>> 8;
  mem[ret+2] = next>> 0;
  jmp(start);
}

// Dance my little sprites, dance!
void init_xytables(void) {
  int i,j,xt,yt;
  const double max    = PAGE-SPRITEW;
  const double center = max / 2.0;
  const double pi = 3.1415926535897932384626433832795;
  double a,b;
  for (i=b=0;i<NSPRITES;i++,b+=1.0/(NSPRITES-1)) {
    xt=xtable+i*4096;
    yt=ytable+i*4096;
    for (j=a=0;j<1*64;j++,a+=1.0/64) {
      mem[xt++] = center-a*max*(b-0.5);
      mem[yt++] = center;}
    for (a=0.0;j<2*64;j++,a+=1.0/64) {
      mem[xt++] = center-(1.0-a)*max*(b-0.5);
      mem[yt++] = center;}
    for (a=0.0;j<3*64;j++,a+=1.0/64) {
      mem[xt++] = center+a*max*(b-0.5);
      mem[yt++] = center;}
    for (a=0.0;j<4*64;j++,a+=1.0/64) {
      mem[xt++] = max*b;
      mem[yt++] = center+a*max*(b-0.5);}
    for (a=0.0;j<5*64;j++,a+=1.0/64) {
      mem[xt++] = center-(a-0.5)*2.0*max*(b-0.5);
      mem[yt++] = max*b;}
    for (a=0.0;j<6*64;j++,a+=1.0/64) {
      mem[xt++] = max*(1.0-b);
      mem[yt++] = center-(a-0.5)*2.0*max*(b-0.5);}
    for (a=0.0;j<7*64;j++,a+=1.0/64) {
      mem[xt++] = center+2.0*(a-0.5)*max*(b-0.5);
      mem[yt++] = max*(1.0-b);}
    for (a=0.0;j<8*64;j++,a+=1.0/64) {
      mem[xt++] = max*b;
      mem[yt++] = center-(1.0-a)*max*(b-0.5);}
    for (a=0.0;j<9*64;j++,a+=1.0/64) {
      mem[xt++] = max*b;
      mem[yt++] = center-a*center*sin((b+a)*2*pi);}
    for (a=0.0;j<10*64;j++,a+=1.0/64) {
      mem[xt++] = max*b;
      mem[yt++] = center-(1.0-a)*center*sin((b+a)*2*pi);}
    for (a=0.0;j<11*64;j++,a+=1.0/64) {
      mem[xt++] = center+(1.0-a)*max*(b-0.5);
      mem[yt++] = center;}
    for (a=0.0;j<12*64;j++,a+=1.0/64) {
      mem[xt++] = center+a*center*cos(b*2*pi*((NSPRITES-1)/(double)NSPRITES));
      mem[yt++] = center+a*center*sin(b*2*pi*((NSPRITES-1)/(double)NSPRITES));}
    for (a=0.0;j<13*64;j++,a+=1.0/64) {
      mem[xt++] = center+(1.0-a)*center*cos(b*2*pi*((NSPRITES-1)/(double)NSPRITES));
      mem[yt++] = center+(1.0-a)*center*sin(b*2*pi*((NSPRITES-1)/(double)NSPRITES));}
    for (a=0.0;j<14*64;j++,a+=1.0/64) {
      mem[xt++] = center+a*center*cos((1.0-b)*pi);
      mem[yt++] = center-a*center*sin((1.0-b)*pi);}
    for (a=0.0;j<18*64;j++,a+=1.0/64) {
      mem[xt++] = center+center*cos((a*0.5+1.0-b)*pi);
      mem[yt++] = center-center*sin((a*0.5+1.0-b)*pi);}
    for (a=0.0;j<19*64;j++,a+=1.0/64) {
      mem[xt++] = center+center*(1.0-a)*cos((1.0-b)*pi);
      mem[yt++] = center-center*(1.0-a)*sin((1.0-b)*pi);}
    double fade = 1.0;
    for (a=0.0;j<32*64;j++,a+=1.0/256) {
      mem[xt++] = center+fade*sin(a*a  )*center*cos((4.3*a-3.2*b)*pi);
      mem[yt++] = center-fade*sin(a*b*b)*center*sin((2.6*b-1.0*a)*pi);
      if (j >= 30*64) fade-=1.0/128;}
    // Let's get freaky!
    for (j=0;j<2048;j++) {
      mem[xt+j] = center+(mem[xt-j-1]-center)*2;
      mem[yt+j] = center+(mem[yt-j-1]-center)*3;}
  }
}

void restore_background(void) {
  int n,x,y,i,ch;
  restore_bg_start = pc;
  for (n=i=0;n<NSPRITES;n++) {
    for (y=0;y<SPRITEH;y++) {
      for (x=0;x<SPRITEW;x++) {
        ch = sprd[(y*NSPRITES+n)*SPRITEW+x];
        if (ch!=' ') mov(savepix+i++,pixels);
      }
    }
  }
  restore_bg_return = pc+6; jmp(0);
}

void save_background(void) {
  int n,x,y,i,ch;
  save_bg_start = pc;
  for (n=i=0;n<NSPRITES;n++) {
    for (y=0;y<SPRITEH;y++) {
      for (x=0;x<SPRITEW;x++) {
        ch = sprd[(y*NSPRITES+n)*SPRITEW+x];
        if (ch!=' ') mov(pixels,savepix+i++);
      }
    }
  }
  save_bg_return = pc+6; jmp(0);
}

void write_xy(void) {
  int i;
  write_xy_start = pc;
  for (i = 0; i < NSPRITES; i++) mov(xtable+i*4096,spritex+i*SPRITEW);
  for (i = 0; i < NSPRITES; i++) mov(ytable+i*4096,spritey+i*SPRITEH);
  write_xy_return = pc+6; jmp(0);
}

void update_spritepos(void) {
  int i,j;

  // xycount_lo++
  mov(xycount+1,next+2);
  mov(inc      ,xycount+1);

  // If (xycount_lo == 0) xycount_hi = xycount_hi++ & 15
  mov(CONST+(CONST  >>8), select+0);
  mov(CONST+(incnyb >>8), select+1);
  mov(xycount+1,next+2);
  mov(iszero   ,next+2);
  mov(select   ,next+INSTR+1);
  mov(xycount+0,next+2);
  mov(0        ,xycount+0);

  // Copy xycount_lo to table indices
  for (i=0;i<2*NSPRITES;i++) mov(xycount+1, write_xy_start+i*INSTR+2);

  // Copy xycount_hi to table indices
  mov(xycount,next+2); j=write_xy_start+1;
  for (i=0;i<2*NSPRITES-1;i++,j+=INSTR) {
    mov(CONST ,     j);
    mov(prev+2,next+2);
    mov(add16 ,next+2);
  }
  mov(CONST,j);

  // Read from tables, store into spritex, spritey
  call(write_xy_start, write_xy_return);
}

void copy_XY(int p) {
  int i,y,x;
  for (i=0;i<NSPRITES;i++) {
    for (y=0;y<SPRITEH;y++) {
      for(x=0;x<SPRITEW;x++) {
        int ch = sprd[(y*NSPRITES+i)*SPRITEW+x];
        if (ch != ' ') {
          mov(spritex+i*SPRITEW+x, p+1);
          mov(spritey+i*SPRITEH+y, p+0);
          p+=INSTR;
        }
      }
    }
  }
}

void copy_spritepos(void) {
  int i,x,y,p;
  for (i=0;i<NSPRITES;i++) {
    p = spritex+i*SPRITEW;
    for (x=0;x<SPRITEW-1;x++) {
      mov(p+x,next+2);
      mov(inc,p+x+1);}
    p = spritey+i*SPRITEH;
    for (y=0;y<SPRITEH-1;y++) {
      mov(p+y,next+2);
      mov(inc,p+y+1);}
  }
  copy_XY(draw_spr_start  +4);
  copy_XY(restore_bg_start+4);
  copy_XY(save_bg_start   +1);
}

void draw_sprites(void) {
  int n,x,y,ch;
  int color1=color(5,5,5);
  int color2=color(0,0,1);
  draw_spr_start = pc;
  for (n=0;n<NSPRITES;n++) {
    for (y=0;y<SPRITEH;y++) {
      for (x=0;x<SPRITEW;x++) {
        ch = sprd[(y*NSPRITES+n)*SPRITEW+x];
        if      (ch=='.') {mov(CONST+((5-(y+x)/8))*color2,pixels);}
        else if (ch=='x') {mov(CONST+              color1,pixels);}
      }
    }
  }
  draw_spr_return = pc+6; jmp(0);
}

void assemble(void) {
  org(prog);
  draw_sprites(); restore_background(); save_background(); write_xy();
  progstart = pc;
  call(restore_bg_start, restore_bg_return);
  update_spritepos();
  copy_spritepos();
  call(save_bg_start, save_bg_return);
  call(draw_spr_start, draw_spr_return);
  wait();
}

void save_file(void) {
  FILE *f = fopen("Sprites.BytePusher", "wb");
  fwrite(mem, 1, end, f); fclose(f);
}

void init_zeropage(void) {
  org (RESET); addr(progstart);
  byte(pixels  / BANK);
  word(samples / PAGE);
}

void init_memmap(void) {
  int i;
  org(PAGE);
  CONST   = pc; for (i = 0; i < PAGE; i++) byte(i);
  inc     = pc; for (i = 0; i < PAGE; i++) byte(i+1);
  incnyb  = pc; for (i = 0; i < PAGE; i++) byte((i&0xf0)|((i+1)&15));
  add16   = pc; for (i = 0; i < PAGE; i++) byte(i+16);
  iszero  = pc; for (i = 0; i < PAGE; i++) byte(i == 0);
  select  = pc; next_page();
  samples = pc; next_page();
  xycount = pc; skip(2);
  savepix = pc; skip(n_spritepix);
  spritex = pc; skip(NSPRITES*SPRITEW);
  spritey = pc; skip(NSPRITES*SPRITEH);
  if (pc & 0xfff) pc = (pc & 0xfff000) + 0x1000; // align to 4096-byte boundary
  xtable = pc; skip(NSPRITES*4096); // 4096 x-coords per sprite
  ytable = pc; skip(NSPRITES*4096); // 4096 y-coords per sprite
  mem[xycount] = xtable >> 8;
  prog = pc;
  org(7*BANK); // Ugly, but it works.
  pixels = pc; next_bank(); end = pc;
}

// Make a strange checkerboard pattern for the background
void make_background(void) {
  int x,y,c;
  for (y = 0; y < PAGE; y++) {
    for (x = 0; x < PAGE; x++) {
      c = (((y>>4)&1)^((x>>4)&1))*((((y&15)>>1)+((x&15)>>1))+y+x); // you figure it out
      mem[pixels+y*PAGE+x] = c;
    }
  }
}

int main(int argc, char *argv[]) {
  count_pixels();
  init_memmap();
  make_background();
  init_xytables();
  assemble();
  init_zeropage();
  save_file();
  return 0;
}
