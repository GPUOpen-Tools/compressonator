#define USE_CMP_FIDELITY_FX_H
#ifndef A_CPU
#define A_CPU
#endif
#include "common_def.h"

// Call to setup required constant values (works on CPU or GPU).
A_STATIC void FsrEasuCon(CMP_OUT CGU_Vec4ui con0,
                         CMP_OUT CGU_Vec4ui con1,
                         CMP_OUT CGU_Vec4ui con2,
                         CMP_OUT CGU_Vec4ui con3,
                         // This the rendered part of the input image.
                         CGU_FLOAT inputViewportInPixelsX,
                         CGU_FLOAT inputViewportInPixelsY,
                         // This is the size of the input image.
                         CGU_FLOAT inputSizeInPixelsX,
                         CGU_FLOAT inputSizeInPixelsY,
                         CGU_FLOAT outputSizeInPixelsX,
                         CGU_FLOAT outputSizeInPixelsY )
{
     // Output integer position to a pixel position in viewport.

     con0[0]=AU1_AF1(inputViewportInPixelsX*ARcpF1(outputSizeInPixelsX));
     con0[1]=AU1_AF1(inputViewportInPixelsY*ARcpF1(outputSizeInPixelsY));
     con0[2]=AU1_AF1(0.5f*inputViewportInPixelsX*ARcpF1(outputSizeInPixelsX)-0.5f);
     con0[3]=AU1_AF1(0.5f*inputViewportInPixelsY*ARcpF1(outputSizeInPixelsY)-0.5f);

     // Viewport pixel position to normalized image space.
     // This is used to get upper-left of 'F' tap.

     con1[0]=AU1_AF1(ARcpF1(inputSizeInPixelsX));
     con1[1]=AU1_AF1(ARcpF1(inputSizeInPixelsY));

     // Centers of gather4, first offset from upper-left of 'F'.
     //      +---+---+
     //      |   |   |
     //      +--(0)--+
     //      | b | c |
     //  +---F---+---+---+
     //  | e | f | g | h |
     //  +--(1)--+--(2)--+
     //  | i | j | k | l |
     //  +---+---+---+---+
     //      | n | o |
     //      +--(3)--+
     //      |   |   |
     //      +---+---+

     con1[2]=AU1_AF1( 1.0f*ARcpF1(inputSizeInPixelsX));
     con1[3]=AU1_AF1(-1.0f*ARcpF1(inputSizeInPixelsY));

     // These are from (0) instead of 'F'.

     con2[0]=AU1_AF1(-1.0f*ARcpF1(inputSizeInPixelsX));
     con2[1]=AU1_AF1( 2.0f*ARcpF1(inputSizeInPixelsY));
     con2[2]=AU1_AF1( 1.0f*ARcpF1(inputSizeInPixelsX));
     con2[3]=AU1_AF1( 2.0f*ARcpF1(inputSizeInPixelsY));
     con3[0]=AU1_AF1( 0.0f*ARcpF1(inputSizeInPixelsX));
     con3[1]=AU1_AF1( 4.0f*ARcpF1(inputSizeInPixelsY));
     con3[2]=con3[3]=0;
}

//==============================================================================================================================
//                                                   NON-PACKED 32-BIT VERSION
//==============================================================================================================================
// CPU Prototypes.: Get pixel value ar given co-ordinates, Red, Green, Blue 
CGU_Vec4f FsrEasuRF(CGU_Vec2f p) {} ;
CGU_Vec4f FsrEasuGF(CGU_Vec2f p) {} ;
CGU_Vec4f FsrEasuBF(CGU_Vec2f p) {} ;


//------------------------------------------------------------------------------------------------------------------------------
 // Filtering for a given tap for the scalar.
 void FsrEasuTapF(CGU_Vec3f aC,  // Accumulated color, with negative lobe.
                  CGU_FLOAT aW,  // Accumulated weight.
                  CGU_Vec2f off, // Pixel offset from resolve position to tap.
                  CGU_Vec2f dir, // Gradient direction.
                  CGU_Vec2f len, // Length.
                  CGU_FLOAT lob, // Negative lobe strength.
                  CGU_FLOAT clp, // Clipping point.
                  CGU_Vec3f c)
 { 
  // Tap color.
  // Rotate offset by direction.
  CGU_Vec2f v;
  v.x =(off.x*( dir.x))+(off.y*dir.y);
  v.y =(off.x*(-dir.y))+(off.y*dir.x);
  // Anisotropy.
  v = len*v;
  // Compute distance^2.
  CGU_FLOAT d2=v.x*v.x+v.y*v.y;
  // Limit to the window as at corner, 2 taps can easily be outside.
  d2=min(d2,clp);
  // Approximation of lancos2 without sin() or rcp(), or sqrt() to get x.
  //  (25/16 * (2/5 * x^2 - 1)^2 - (25/16 - 1)) * (1/4 * x^2 - 1)^2
  //  |_______________________________________|   |_______________|
  //                   base                             window
  // The general form of the 'base' is,
  //  (a*(b*x^2-1)^2-(a-1))
  // Where 'a=1/(2*b-b^2)' and 'b' moves around the negative lobe.
  CGU_FLOAT wB=(2.0f/5.0f)*d2-1.0f;
  CGU_FLOAT wA=lob*d2-1.0f;
  wB*=wB;
  wA*=wA;
  wB=(25.0f/16.0f)*wB-(25.0f/16.0f-1.0f);
  CGU_FLOAT w=wB*wA;
  // Do weighted average.
  aC+=c*w;
  aW+=w;
 }

 
//------------------------------------------------------------------------------------------------------------------------------
// Accumulate direction and length.
//------------------------------------------------------------------------------------------------------------------------------
 void FsrEasuSetF( CMP_INOUT CGU_Vec2f dir,
                   CMP_INOUT CGU_FLOAT len,
                   CGU_Vec2f pp,
                   AP1 biS,AP1 biT,AP1 biU,AP1 biV,
                   CGU_FLOAT lA,CGU_FLOAT lB,CGU_FLOAT lC,CGU_FLOAT lD,CGU_FLOAT lE)
 {
  // Compute bilinear weight, branches factor out as predicates are compiler time immediates.
  //  s t
  //  u v
  CGU_FLOAT w;
  if(biS) w= (1.0f-pp.x)*1.0f-pp.y;
  if(biT) w= pp.x *1.0f-pp.y;
  if(biU) w= (1.0f-pp.x)* pp.y ;
  if(biV) w= pp.x * pp.y;

  // Direction is the '+' diff.
  //    a
  //  b c d
  //    e
  // Then takes magnitude from abs average of both sides of 'c'.
  // Length converts gradient reversal to 0, smoothly to non-reversal at 1, shaped, then adding horz and vert terms.

  CGU_FLOAT dc=lD-lC;
  CGU_FLOAT cb=lC-lB;
  CGU_FLOAT lenX=max(abs(dc),abs(cb));

  lenX=APrxLoRcpF1(lenX);

  CGU_FLOAT dirX=lD-lB;
  dir.x+=dirX*w;
  lenX=ASatF1(abs(dirX)*lenX);
  lenX*=lenX;
  len+=lenX*w;
  
  // Repeat for the y axis.
  CGU_FLOAT ec=lE-lC;
  CGU_FLOAT ca=lC-lA;
  CGU_FLOAT lenY=max(abs(ec),abs(ca));
  lenY=APrxLoRcpF1(lenY);
  CGU_FLOAT dirY=lE-lA;
  dir.y+=dirY*w;
  lenY=ASatF1(abs(dirY)*lenY);
  lenY*=lenY;
  len+=lenY*w;
}


//------------------------------------------------------------------------------------------------------------------------------
 void FsrEasuF( CMP_OUT CGU_Vec3f pix,
                CGU_Vec2ui ip,     // Integer pixel position in output.
                CGU_Vec4ui con0,   // Constants generated by FsrEasuCon().
                CGU_Vec4ui con1,
                CGU_Vec4ui con2,
                CGU_Vec4ui con3)
 {
    //------------------------------------------------------------------------------------------------------------------------------
    // Get position of 'f'.
    CGU_Vec2f pp = CGU_Vec2f(ip.x,ip.y)*CGU_Vec2f(con0.x,con0.y)+CGU_Vec2f(con0.z,con0.w);
    CGU_Vec2f fp = CGU_Vec2f(cmp_floor(pp.x),cmp_floor(pp.x));
    pp-=fp;

    //------------------------------------------------------------------------------------------------------------------------------
    // 12-tap kernel.
    //    b c
    //  e f g h
    //  i j k l
    //    n o
    // Gather 4 ordering.
    //  a b
    //  r g
    // For packed FP16, need either {rg} or {ab} so using the following setup for gather in all versions,
    //    a b    <- unused (z)
    //    r g
    //  a b a b
    //  r g r g
    //    a b
    //    r g    <- unused (z)

    // Allowing dead-code removal to remove the 'z's.
    CGU_Vec2f p0=fp*CGU_Vec2f(con1.x,con1.y)+CGU_Vec2f(con1.z,con1.w);

    // These are from p0 to avoid pulling two constants on pre-Navi hardware.
    CGU_Vec2f p1=p0+CGU_Vec2f(con2.x,con2.y);
    CGU_Vec2f p2=p0+CGU_Vec2f(con2.z,con2.w);
    CGU_Vec2f p3=p0+CGU_Vec2f(con3.x,con3.y);
    CGU_Vec4f bczzR=FsrEasuRF(p0);
    CGU_Vec4f bczzG=FsrEasuGF(p0);
    CGU_Vec4f bczzB=FsrEasuBF(p0);
    CGU_Vec4f ijfeR=FsrEasuRF(p1);
    CGU_Vec4f ijfeG=FsrEasuGF(p1);
    CGU_Vec4f ijfeB=FsrEasuBF(p1);
    CGU_Vec4f klhgR=FsrEasuRF(p2);
    CGU_Vec4f klhgG=FsrEasuGF(p2);
    CGU_Vec4f klhgB=FsrEasuBF(p2);
    CGU_Vec4f zzonR=FsrEasuRF(p3);
    CGU_Vec4f zzonG=FsrEasuGF(p3);
    CGU_Vec4f zzonB=FsrEasuBF(p3);
//  ------------------------------------------------------------------------------------------------------------------------------
    // Simplest multi-channel approximate luma possible (luma times 2, in 2 FMA/MAD).
    CGU_Vec4f bczzL=bczzB*CGU_Vec4f(0.5)+(bczzR*CGU_Vec4f(0.5)+bczzG);
    CGU_Vec4f ijfeL=ijfeB*CGU_Vec4f(0.5)+(ijfeR*CGU_Vec4f(0.5)+ijfeG);
    CGU_Vec4f klhgL=klhgB*CGU_Vec4f(0.5)+(klhgR*CGU_Vec4f(0.5)+klhgG);
    CGU_Vec4f zzonL=zzonB*CGU_Vec4f(0.5)+(zzonR*CGU_Vec4f(0.5)+zzonG);
    // Rename.
    CGU_FLOAT bL=bczzL.x;
    CGU_FLOAT cL=bczzL.y;
    CGU_FLOAT iL=ijfeL.x;
    CGU_FLOAT jL=ijfeL.y;
    CGU_FLOAT fL=ijfeL.z;
    CGU_FLOAT eL=ijfeL.w;
    CGU_FLOAT kL=klhgL.x;
    CGU_FLOAT lL=klhgL.y;
    CGU_FLOAT hL=klhgL.z;
    CGU_FLOAT gL=klhgL.w;
    CGU_FLOAT oL=zzonL.z;
    CGU_FLOAT nL=zzonL.w;

    // Accumulate for bilinear interpolation.
    CGU_Vec2f dir= {0.0f,0.0f};
    CGU_FLOAT len= 0.0f;

    FsrEasuSetF(dir,len,pp,true, false,false,false,bL,eL,fL,gL,jL);
    FsrEasuSetF(dir,len,pp,false,true ,false,false,cL,fL,gL,hL,kL);
    FsrEasuSetF(dir,len,pp,false,false,true ,false,fL,iL,jL,kL,nL);
    FsrEasuSetF(dir,len,pp,false,false,false,true ,gL,jL,kL,lL,oL);

    //---------------------------------------------------------
    // Normalize with approximation, and cleanup close to zero.
    //---------------------------------------------------------
    CGU_Vec2f dir2=dir*dir;
    CGU_FLOAT dirR=dir2.x+dir2.y;
    AP1 zro=dirR<(1.0f/32768.0f);
    dirR=APrxLoRsqF1(dirR);
    dirR=zro?1.0f:dirR;
    dir.x=zro?1.0f:dir.x;
    dir = CGU_Vec2f(dirR)*dir;

    //---------------------------------------------------------
    // Transform from {0 to 2} to {0 to 1} range, and shape with square.
    //---------------------------------------------------------
    len=len*0.5f;
    len*=len;

    //---------------------------------------------------------
    // Stretch kernel {1.0 vert|horz, to sqrt(2.0) on diagonal}.
    //---------------------------------------------------------

    CGU_FLOAT stretch=(dir.x*dir.x+dir.y*dir.y)*APrxLoRcpF1(CMP_MAX(abs(dir.x),abs(dir.y)));

    //---------------------------------------------------------
    // Anisotropic length after rotation,
    //  x := 1.0 lerp to 'stretch' on edges
    //  y := 1.0 lerp to 2x on edges
    //---------------------------------------------------------

    CGU_Vec2f len2=CGU_Vec2f(1.0f+(stretch-1.0f)*len,1.0f-0.5*len);

    //---------------------------------------------------------
    // Based on the amount of 'edge',
    // the window shifts from +/-{sqrt(2.0) to slightly beyond 2.0}.
    //---------------------------------------------------------

    CGU_FLOAT lob=0.5f+((1.0f/4.0f-0.04f)-0.5f)*len;

    //---------------------------------------------------------
    // Set distance^2 clipping point to the end of the adjustable window.
    //---------------------------------------------------------

    CGU_FLOAT clp=APrxLoRcpF1(lob);
    //----------------------------------------------------------
    // Accumulation mixed with min/max of 4 nearest.
    //    b c
    //  e f g h
    //  i j k l
    //    n o
    //---------------------------------------------------------
    CGU_Vec3f min4=CMP_MIN(CMP_MIN(CGU_Vec3f(ijfeR.z,ijfeG.z,ijfeB.z),CGU_Vec3f(klhgR.w,klhgG.w,klhgB.w),CGU_Vec3f(ijfeR.y,ijfeG.y,ijfeB.y)),
                       CGU_Vec3f(klhgR.x,klhgG.x,klhgB.x));
    CGU_Vec3f max4=CMP_MAX(CMP_MAX(CGU_Vec3f(ijfeR.z,ijfeG.z,ijfeB.z),CGU_Vec3f(klhgR.w,klhgG.w,klhgB.w),CGU_Vec3f(ijfeR.y,ijfeG.y,ijfeB.y)),
                       CGU_Vec3f(klhgR.x,klhgG.x,klhgB.x));

    //---------------------------------------------------------
    // Accumulation.
    //---------------------------------------------------------
    CGU_Vec3f aC = {0.0f,0.0f,0.0f};
    CGU_FLOAT aW = 0.0f;

    FsrEasuTapF(aC,aW,CGU_Vec2f( 0.0,-1.0)-pp,dir,len2,lob,clp,CGU_Vec3f(bczzR.x,bczzG.x,bczzB.x)); // b
    FsrEasuTapF(aC,aW,CGU_Vec2f( 1.0,-1.0)-pp,dir,len2,lob,clp,CGU_Vec3f(bczzR.y,bczzG.y,bczzB.y)); // c
    FsrEasuTapF(aC,aW,CGU_Vec2f(-1.0, 1.0)-pp,dir,len2,lob,clp,CGU_Vec3f(ijfeR.x,ijfeG.x,ijfeB.x)); // i
    FsrEasuTapF(aC,aW,CGU_Vec2f( 0.0, 1.0)-pp,dir,len2,lob,clp,CGU_Vec3f(ijfeR.y,ijfeG.y,ijfeB.y)); // j
    FsrEasuTapF(aC,aW,CGU_Vec2f( 0.0, 0.0)-pp,dir,len2,lob,clp,CGU_Vec3f(ijfeR.z,ijfeG.z,ijfeB.z)); // f
    FsrEasuTapF(aC,aW,CGU_Vec2f(-1.0, 0.0)-pp,dir,len2,lob,clp,CGU_Vec3f(ijfeR.w,ijfeG.w,ijfeB.w)); // e
    FsrEasuTapF(aC,aW,CGU_Vec2f( 1.0, 1.0)-pp,dir,len2,lob,clp,CGU_Vec3f(klhgR.x,klhgG.x,klhgB.x)); // k
    FsrEasuTapF(aC,aW,CGU_Vec2f( 2.0, 1.0)-pp,dir,len2,lob,clp,CGU_Vec3f(klhgR.y,klhgG.y,klhgB.y)); // l
    FsrEasuTapF(aC,aW,CGU_Vec2f( 2.0, 0.0)-pp,dir,len2,lob,clp,CGU_Vec3f(klhgR.z,klhgG.z,klhgB.z)); // h
    FsrEasuTapF(aC,aW,CGU_Vec2f( 1.0, 0.0)-pp,dir,len2,lob,clp,CGU_Vec3f(klhgR.w,klhgG.w,klhgB.w)); // g
    FsrEasuTapF(aC,aW,CGU_Vec2f( 1.0, 2.0)-pp,dir,len2,lob,clp,CGU_Vec3f(zzonR.z,zzonG.z,zzonB.z)); // o
    FsrEasuTapF(aC,aW,CGU_Vec2f( 0.0, 2.0)-pp,dir,len2,lob,clp,CGU_Vec3f(zzonR.w,zzonG.w,zzonB.w)); // n
    //------------------------------------------------------------------------------------------------------------------------------
    // Normalize and dering.
    pix=min(max4,max(min4,aC*CGU_Vec3f(ARcpF1(aW))));
}
