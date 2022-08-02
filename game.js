var canvas;
var ctx;

var MEM;
var PAL;
var PKEY;
var PPC;
var PGFX;
var PSND;

var PC=0,A=0,B=0,C=0;

var KEY=0x0000;

var cycles=0;
var ps=1;

var cx=0,cy=0;
var ox=0,oy=0;

function exec(PPC) {

  ctx.fillStyle="#c0c0c0";
  ctx.fillRect(0,0,256,64);
  
  ctx.font="32px monospace";
  ctx.fillStyle="#000000";
  ctx.fillText(cycles,32,32);

  var i=65536;
  
  do {

    PC=MEM[PPC+0]<<16+MEM[PPC+1]<<8+MEM[PPC+2];  
    A=MEM[PC+0]<<16+MEM[PC+1]<<8+MEM[PC+2];
    B=MEM[PC+3]<<16+MEM[PC+4]<<8+MEM[PC+5];
    C=MEM[PC+6]<<16+MEM[PC+7]<<8+MEM[PC+8];

    if(A==B && PC==C) break; 

    MEM[B]=MEM[A];
    
    MEM[PPC+0]=MEM[C+0];
    MEM[PPC+1]=MEM[C+1];
    MEM[PPC+2]=MEM[C+2];

  } while(--i);
  
  cycles++;
  
}


function rnd(n) {
	return Math.floor(Math.random()*n);
}

function drawPoint(x,y,c) {
  MEM[ MEM[PGFX]*256*256+parseInt(y,10)*256+parseInt(x,10) ]=parseInt(c)%256;
}

function readPoint(x,y) {
  return MEM[ MEM[PGFX]*256*256+parseInt(y,10)*256+parseInt(x,10) ];
}


function drawScreen(ox,oy,x0,y0,x1,y1) {
  ox=parseInt(ox,10);
  oy=parseInt(oy,10);
  x0=parseInt(x0,10);
  y0=parseInt(y0,10);
  x1=parseInt(x1,10);
  y1=parseInt(y1,10);

  if(x0>x1) { var t=x0; x0=x1; x1=t; }
  if(y0>y1) { var t=y0; y0=y1; y1=t; }

  for(var j=y0;j<=y1;j++)
    for(var i=x0;i<=x1;i++) {
      ctx.fillStyle=PAL[ MEM[ MEM[PGFX]*256*256+j*256+i ] ];
      ctx.fillRect(ox+i*ps,oy+j*ps,ps,ps);
    }
}

function drawRect(x,y,w,h,c) {
  
  for(var j=0;j<=h;j++) {
    drawPoint(x,y+j,c);
    drawPoint(x+w,y+j,c);
  }  

  for(var i=0;i<=w;i++) {
    drawPoint(x+i,y,c);
    drawPoint(x+i,y+h,c);
  }  

}


function fillRect(x,y,w,h,c) {
  for(var j=0;j<=h;j++) {
    for(var i=0;i<=w;i++) {
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

  err=parseInt(err,10);

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


function loadbin(url) {
  var byteArray = [];
  var req = new XMLHttpRequest();
  req.open('GET', url, false);
  req.overrideMimeType('text\/plain; charset=x-user-defined');
  req.send(null);
  if (req.status != 200) return byteArray;
  for (var i = 0; i < req.responseText.length; ++i) {
    byteArray.push(req.responseText.charCodeAt(i) & 0xff)
  }
  return Uint8Array.from(byteArray);
}

function keydown(e) {
  switch(e.which) {
    case 49: KEY|=0x0001; break;
    case 50: KEY|=0x0002; break;
    case 51: KEY|=0x0004; break;
    case 52: KEY|=0x0008; break;
    case 81: KEY|=0x0010; break;
    case 87: KEY|=0x0020; break;
    case 69: KEY|=0x0040; break;
    case 82: KEY|=0x0080; break;
    case 65: KEY|=0x0100; break;
    case 83: KEY|=0x0200; break;
    case 68: KEY|=0x0400; break;
    case 70: KEY|=0x0800; break;
    case 90: KEY|=0x1000; break;
    case 88: KEY|=0x2000; break;
    case 67: KEY|=0x4000; break;
    case 86: KEY|=0x8000; break;
  }
}

function keyup(e) {
  switch(e.which) {
    case 49: KEY&=0xFFFE; break;
    case 50: KEY&=0xFFFD; break;
    case 51: KEY&=0xFFFB; break;
    case 52: KEY&=0xFFF7; break;
    case 81: KEY&=0xFFEF; break;
    case 87: KEY&=0xFFDF; break;
    case 69: KEY&=0xFFBF; break;
    case 82: KEY&=0xFF7F; break;
    case 65: KEY&=0xFEFF; break;
    case 83: KEY&=0xFDFF; break;
    case 68: KEY&=0xFBFF; break;
    case 70: KEY&=0xF7FF; break;
    case 90: KEY&=0xEFFF; break;
    case 88: KEY&=0xDFFF; break;
    case 67: KEY&=0xBFFF; break;
    case 86: KEY&=0x7FFF; break;
  }
}

function updateKeys(KEY) {
  MEM[PKEY+0]=(KEY >> 8) & 0xFF;
  MEM[PKEY+1]=KEY & 0xFF;
}

function update() {

  updateKeys(KEY);

  exec(PPC);
  
/*  

  for(var i=0;i<16;i++) {
    if(KEY&(1<<i)) 
      fillRect(i%4*64,Math.floor(i/4)*64,64,64,215);
    else {       
      fillRect(i%4*64,Math.floor(i/4)*64,64,64,0);
      drawRect(i%4*64,Math.floor(i/4)*64,64,64,215);
    }
  }  
  
*/

  drawScreen(ox,oy,0,0,255,255);
  


  window.requestAnimationFrame(update);
}

function main() {

  MEM=new Uint8Array(1024*1024*16);
  PAL=new Array(256);
  
  PKEY=0;
  PPC=2;
  PGFX=5;
  PSND=6;
  
  KEY=0x0000;
  
  PC=0,A=0,B=0,C=0;
  
  cycles=0;
  
  ps=1;

  canvas=document.getElementById("canvas");
  ctx=canvas.getContext("2d");

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

/*
  MEM[PGFX]=1;
  MEM[PPC+0]=0;
  MEM[PPC+1]=0;
  MEM[PPC+2]=1;
*/

  MEM=loadbin("pt.bp");

	ctx.fillStyle="#FFFFFF";
	ctx.fillRect(0,0,canvas.width,canvas.height);

  drawScreen(ox,oy,0,0,255,255);
  
  window.onkeydown=keydown;
  window.onkeyup=keyup;

  window.requestAnimationFrame(update);
}

window.onload=function() {
  main();
}
