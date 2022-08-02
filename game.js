var canvas=document.getElementById("canvas");
var ctx=canvas.getContext("2d");

var MEM=new Uint8Array(1024*1024*16);
var PAL=new Array(256);

var PKEY=0;
var PPC=2;
var PGFX=5;
var PSND=6;

var ps=1;

var cx=0,cy=0;
var ox=0,oy=0;

var x=new Array(3),y=new Array(3);
var nx=new Array(3),ny=new Array(3);
var f=0;
var key=0x0000;

function rnd(n) {
	return Math.floor(Math.random()*n);
}

function drawPoint(x,y,c) {
  MEM[ MEM[PGFX]*256*256+parseInt(y,10)*256+parseInt(x,10) ]=c;
}

function readPoint(x,y) {
  return MEM[ MEM[PGFX]*256*256+parseInt(y,10)*256+parseInt(x,10) ];
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

function exec(PPC) {
  i=65536;
  while(i>0) {

    var PC=MEM[PPC+2]*256*256+MEM[PPC+1]*256+MEM[PPC+0];

    var A=MEM[PC+0+2]*256*256+MEM[PC+0+1]*256+MEM[PC+0+0];
    var B=MEM[PC+1+2]*256*256+MEM[PC+1+1]*256+MEM[PC+1+0];
    var C=MEM[PC+2+2]*256*256+MEM[PC+2+1]*256+MEM[PC+2+0];
    
    MEM[B]=MEM[A];

    MEM[PPC+0]=MEM[C+0];
    MEM[PPC+1]=MEM[C+1];
    MEM[PPC+2]=MEM[C+2];

    i--;
  }
}

function load(url) {
  var byteArray = [];
  var req = new XMLHttpRequest();
  req.open('GET', url, false);
  req.overrideMimeType('text\/plain; charset=x-user-defined');
  req.send(null);
  if (req.status != 200) return byteArray;
  for (var i = 0; i < req.responseText.length; ++i) {
    byteArray.push(req.responseText.charCodeAt(i) & 0xff)
  }
  return byteArray;
}

function keydown(e) {
  switch(e.which) {
    case 49: key|=0x0001; break;
    case 50: key|=0x0002; break;
    case 51: key|=0x0004; break;
    case 52: key|=0x0008; break;
    case 81: key|=0x0010; break;
    case 87: key|=0x0020; break;
    case 69: key|=0x0040; break;
    case 82: key|=0x0080; break;
    case 65: key|=0x0100; break;
    case 83: key|=0x0200; break;
    case 68: key|=0x0400; break;
    case 70: key|=0x0800; break;
    case 90: key|=0x1000; break;
    case 88: key|=0x2000; break;
    case 67: key|=0x4000; break;
    case 86: key|=0x8000; break;
  }
}

function keyup(e) {
  switch(e.which) {
    case 49: key&=0xFFFE; break;
    case 50: key&=0xFFFD; break;
    case 51: key&=0xFFFB; break;
    case 52: key&=0xFFF7; break;
    case 81: key&=0xFFEF; break;
    case 87: key&=0xFFDF; break;
    case 69: key&=0xFFBF; break;
    case 82: key&=0xFF7F; break;
    case 65: key&=0xFEFF; break;
    case 83: key&=0xFDFF; break;
    case 68: key&=0xFBFF; break;
    case 70: key&=0xF7FF; break;
    case 90: key&=0xEFFF; break;
    case 88: key&=0xDFFF; break;
    case 67: key&=0xBFFF; break;
    case 86: key&=0x7FFF; break;
  }
}

function updateKeys(key) {
  MEM[PKEY+0]=(key >> 8) & 0xFF;
  MEM[PKEY+1]=key & 0xFF;
}

function update() {

    
  exec(PPC);
  
/*  
  for(var i=0;i<16;i++) {
    if(key&(1<<i)) 
      fillRect(i%4*64,Math.floor(i/4)*64,64,64,215);
    else {       
      fillRect(i%4*64,Math.floor(i/4)*64,64,64,0);
      drawRect(i%4*64,Math.floor(i/4)*64,64,64,215);
    }
  }  
  
  */

  drawScreen(ox,oy,0,0,255,255);
  
  updateKeys(key);


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

/*
  MEM[PGFX]=1;
  
  MEM[PPC+0]=0;
  MEM[PPC+1]=0;
  MEM[PPC+2]=1;
*/

  MEM=load("pt.bp");

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