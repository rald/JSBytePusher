var canvas=document.getElementById("canvas");
var ctx=canvas.getContext("2d");

var MEM=new Uint8Array(1024*1024*16);
var PAL=new Array(256);

var PKEY=0;
var PPC=2;
var PVID=5;
var PSND=6;

var ps=1;

var cx=0,cy=0;
var ox=0,oy=0;

var x=new Array(3),y=new Array(3);
var nx=new Array(3),ny=new Array(3);
var f=0;

function rnd(n) {
	return Math.floor(Math.random()*n);
}

function drawPoint(x,y,c) {
  MEM[ MEM[PVID]*256*256+parseInt(y,10)*256+parseInt(x,10) ]=c;
}

function readPoint(x,y) {
  return MEM[ MEM[PVID]*256*256+parseInt(y,10)*256+parseInt(x,10) ];
}


function drawScreen(ox,oy,x0,y0,x1,y1) {
  x0=parseInt(x0,10);
  y0=parseInt(y0,10);
  x1=parseInt(x1,10);
  y1=parseInt(y1,10);
  ox=parseInt(ox,10);
  oy=parseInt(oy,10);

  if(x0>x1) { var t=x0; x0=x1; x1=t; }
  if(y0>y1) { var t=y0; y0=y1; y1=t; }

  for(var j=y0;j<=y1;j++)
    for(var i=x0;i<=x1;i++) {
      ctx.fillStyle=PAL[ MEM[ MEM[PVID]*256*256+j*256+i ] ];
      ctx.fillRect(ox+i*ps,oy+j*ps,ps,ps);
    }
}

function fillRect(x,y,w,h,c) {
  for(var j=0;j<h;j++) {
    for(var i=0;i<w;i++) {
      drawPoint(x+i,y+j,c);
    }
  }
}

function drawLine(x0,y0,x1,y1,c) {

  x0=parseInt(x0,10);
  y0=parseInt(y0,10);
  x1=parseInt(x1,10);
  y1=parseInt(y1,10);
  c=parseInt(c,10);

  var dx = Math.abs(x1-x0), sx = x0<x1 ? 1 : -1;
  var dy = Math.abs(y1-y0), sy = y0<y1 ? 1 : -1;
  var err = (dx>dy ? dx : -dy)/2, e2;

  for(;;){
    drawPoint(x0,y0,c);
    if (x0==x1 && y0==y1) break;
    e2 = err;
    if (e2 >-dx) { err -= dy; x0 += sx; }
    if (e2 < dy) { err += dx; y0 += sy; }
  }
}

function dim(ox,oy,x0,y0,x1,y1) {
  x0=parseInt(x0,10);
  y0=parseInt(y0,10);
  x1=parseInt(x1,10);
  y1=parseInt(y1,10);
  ox=parseInt(ox,10);
  oy=parseInt(oy,10);

  if(x0>x1) { var t=x0; x0=x1; x1=t; }
  if(y0>y1) { var t=y0; y0=y1; y1=t; }

  for(var j=y0;j<=y1;j++) {
    for(var i=x0;i<=x1;i++) {
      drawPoint(i,j,readPoint(i,j)/1.5);
    }
  }
  
}

function update() {

//  dim(ox,oy,0,0,255,255);

  nx[0]=128+Math.cos(f/7)*32;
  ny[0]=128+Math.sin(f/11)*64;
  nx[1]=128+Math.cos(f/11)*64;
  ny[1]=128+Math.sin(f/7)*32;
  nx[2]=128+Math.cos(f/13)*128;
  ny[2]=128+Math.sin(f/17)*128;


  for(var i=0;i<3;i++) {
    if(f==0) {
      x[i]=nx[i];
      y[i]=ny[i];
    }
    drawLine(x[i],y[i],nx[i],ny[i],215);
    x[i]=nx[i]; y[i]=ny[i];
  }

  drawScreen(ox,oy,0,0,255,255);

  f++;
  
  window.requestAnimationFrame(update);
}

function main() {

	canvas.width = window.innerWidth;
	canvas.height = window.innerHeight;

  cx=canvas.width/2;
  cy=canvas.height/2;

  ox=(canvas.width-256)/2;
  oy=(canvas.height-256)/2;

  for(var i=0;i<MEM.length;i++) {
    MEM[i]=0;
  }

  for(var k=0;k<6;k++) {
    for(var j=0;j<6;j++) {
      for(var i=0;i<6;i++) {
        var l=k*36+j*6+i;
        PAL[l]="#"+
          (k*0x33<16?"0":"")+(k*0x33).toString(16)+
          (j*0x33<16?"0":"")+(j*0x33).toString(16)+
          (i*0x33<16?"0":"")+(i*0x33).toString(16);
      }
    }
  }
  
  for(var i=216;i<256;i++) {
    PAL[i]="#000000";
  }
  
  MEM[PVID]=1;

	ctx.fillStyle="#515151";
	ctx.fillRect(0,0,canvas.width,canvas.height);

  drawScreen(ox,oy,0,0,256,256);

  window.requestAnimationFrame(update);
}

main();
