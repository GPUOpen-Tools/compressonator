#ifndef __gl_ATI_h_
#define __gl_ATI_h_
/*
** Copyright (C) 1998-2008 ATI Technologies Inc.
*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __linux__

#ifndef APIENTRY
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif

#endif  /* !__linux__ */

/*
** Notes:
**
**  Listed support is for current drivers and should really only be used
**  as a guideline.  ISV should still use glGetString() and
**  wglGetExtensionsString() to determine the exact set of supported
**  GL and WGL extensions.
**
*/

/*
** IMPORTANT:
**
**  All non-final extensions should use token enumerations in the range
**  of 0x6000-0x7fff, as required by the OpenGL spec.
**
*/

/*
**  OpenGL 1.2 functionality
*/
#ifndef GL_VERSION_1_2
#define GL_VERSION_1_2                     1

#define GL_SMOOTH_POINT_SIZE_RANGE         0x0B12
#define GL_SMOOTH_POINT_SIZE_GRANULARITY   0x0B13
#define GL_SMOOTH_LINE_WIDTH_RANGE         0x0B22
#define GL_SMOOTH_LINE_WIDTH_GRANULARITY   0x0B23
#define GL_ALIASED_POINT_SIZE_RANGE        0x846D
#define GL_ALIASED_LINE_WIDTH_RANGE        0x846E

/*
**  D.1 Three-Dimensional Texturing
*/
#define GL_TEXTURE_BINDING_3D              0x806A
#define GL_PACK_SKIP_IMAGES                0x806B
#define GL_PACK_IMAGE_HEIGHT               0x806C
#define GL_UNPACK_SKIP_IMAGES              0x806D
#define GL_UNPACK_IMAGE_HEIGHT             0x806E
#define GL_TEXTURE_3D                      0x806F
#define GL_PROXY_TEXTURE_3D                0x8070
#define GL_TEXTURE_DEPTH                   0x8071
#define GL_TEXTURE_WRAP_R                  0x8072
#define GL_MAX_3D_TEXTURE_SIZE             0x8073

typedef GLvoid (APIENTRY * PFNGLCOPYTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset,
                                                        GLint yoffset, GLint zoffset, GLint x,
                                                        GLint y, GLsizei width, GLsizei height);
typedef GLvoid (APIENTRY * PFNGLTEXIMAGE3DPROC) (GLenum target, GLint level, GLint internalformat,
                                                 GLsizei width, GLsizei height, GLsizei depth,
                                                 GLint border, GLenum format, GLenum type,
                                                 const GLvoid *data);
typedef GLvoid (APIENTRY * PFNGLTEXSUBIMAGE3DPROC) (GLenum target, GLint level,
                                                    GLint xoffset, GLint yoffset, GLint zoffset,
                                                    GLsizei width, GLsizei height, GLsizei depth,
                                                    GLenum format, GLenum type, const GLvoid *data);

/*
**  D.2 BGRA Pixel Formats
*/
#define GL_BGR                             0x80E0
#define GL_BGRA                            0x80E1

/*
**  D.3 Packed Pixel Formats
*/
#define GL_UNSIGNED_BYTE_3_3_2             0x8032
#define GL_UNSIGNED_SHORT_4_4_4_4          0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1          0x8034
#define GL_UNSIGNED_INT_8_8_8_8            0x8035
#define GL_UNSIGNED_INT_10_10_10_2         0x8036
#define GL_UNSIGNED_BYTE_2_3_3_REV         0x8362
#define GL_UNSIGNED_SHORT_5_6_5            0x8363
#define GL_UNSIGNED_SHORT_5_6_5_REV        0x8364
#define GL_UNSIGNED_SHORT_4_4_4_4_REV      0x8365
#define GL_UNSIGNED_SHORT_1_5_5_5_REV      0x8366
#define GL_UNSIGNED_INT_8_8_8_8_REV        0x8367
#define GL_UNSIGNED_INT_2_10_10_10_REV     0x8368

/*
**  D.4 Normal Rescaling
*/
#define GL_RESCALE_NORMAL                  0x803A

/*
**  D.5 Separate Specular Color
*/
#define GL_LIGHT_MODEL_COLOR_CONTROL       0x81F8
#define GL_SINGLE_COLOR                    0x81F9
#define GL_SEPARATE_SPECULAR_COLOR         0x81FA

/*
**  D.6 Texture Coordinate Edge Clamping
*/
#define GL_CLAMP_TO_EDGE                   0x812F

/*
**  D.7 Texture Level of Detail Control
*/
#define GL_TEXTURE_MIN_LOD                 0x813A
#define GL_TEXTURE_MAX_LOD                 0x813B
#define GL_TEXTURE_BASE_LEVEL              0x813C
#define GL_TEXTURE_MAX_LEVEL               0x813D

/*
**  D.8 Vertex Array Draw Element Range
*/
#define GL_MAX_ELEMENTS_VERTICES           0x80E8
#define GL_MAX_ELEMENTS_INDICES            0x80E9

typedef GLvoid (APIENTRY * PFNGLDRAWRANGEELEMENTSPROC) (GLenum mode, GLuint start,
                                                        GLuint end, GLsizei count,
                                                        GLenum type, const GLvoid *indices);
#endif /* GL_VERSION_1_2 */


/*
**  OpenGL 1.3 functionality
*/
#ifndef GL_VERSION_1_3
#define GL_VERSION_1_3                     1

/*
**  F.1 Compressed Textures
*/
#define GL_COMPRESSED_ALPHA                0x84E9
#define GL_COMPRESSED_LUMINANCE            0x84EA
#define GL_COMPRESSED_LUMINANCE_ALPHA      0x84EB
#define GL_COMPRESSED_INTENSITY            0x84EC
#define GL_COMPRESSED_RGB                  0x84ED
#define GL_COMPRESSED_RGBA                 0x84EE
#define GL_TEXTURE_COMPRESSION_HINT        0x84EF
#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE   0x86A0
#define GL_TEXTURE_COMPRESSED              0x86A1
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS  0x86A2
#define GL_COMPRESSED_TEXTURE_FORMATS      0x86A3

typedef GLvoid (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE1DPROC) (GLenum target, GLint level,
                                                           GLenum internalFormat, GLsizei width,
                                                           GLint border, GLsizei imageSize,
                                                           const GLvoid *data);
typedef GLvoid (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE2DPROC) (GLenum target, GLint level,
                                                           GLenum internalFormat, GLsizei width,
                                                           GLsizei height, GLint border,
                                                           GLsizei imageSize, const GLvoid *data);
typedef GLvoid (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE3DPROC) (GLenum target, GLint level,
                                                           GLenum internalFormat, GLsizei width,
                                                           GLsizei height, GLsizei depth,
                                                           GLint border, GLsizei imageSize,
                                                           const GLvoid *data);
typedef GLvoid (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC) (GLenum target, GLint level,
                                                              GLint xoffset, GLsizei width,
                                                              GLenum format, GLsizei imageSize,
                                                              const GLvoid *data);
typedef GLvoid (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) (GLenum target, GLint level,
                                                              GLint xoffset, GLint yoffset,
                                                              GLsizei width, GLsizei height,
                                                              GLenum format, GLsizei imageSize,
                                                              const GLvoid *data);
typedef GLvoid (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) (GLenum target, GLint level,
                                                              GLint xoffset, GLint yoffset,
                                                              GLint zoffset, GLsizei width,
                                                              GLsizei height, GLsizei depth,
                                                              GLenum format, GLsizei imageSize,
                                                              const GLvoid *data);
typedef GLvoid (APIENTRY * PFNGLGETCOMPRESSEDTEXIMAGEPROC) (GLenum target, GLint lod,
                                                            GLvoid *img);

/*
**  F.2 Cube Map Textures
*/
#define GL_NORMAL_MAP                      0x8511
#define GL_REFLECTION_MAP                  0x8512
#define GL_TEXTURE_CUBE_MAP                0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP        0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X     0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X     0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y     0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y     0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z     0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z     0x851A
#define GL_PROXY_TEXTURE_CUBE_MAP          0x851B
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE       0x851C

/*
**  F.3 Multisample
*/
#define GL_MULTISAMPLE                     0x809D
#define GL_SAMPLE_ALPHA_TO_COVERAGE        0x809E
#define GL_SAMPLE_ALPHA_TO_ONE             0x809F
#define GL_SAMPLE_COVERAGE                 0x80A0
#define GL_SAMPLE_BUFFERS                  0x80A8
#define GL_SAMPLES                         0x80A9
#define GL_SAMPLE_COVERAGE_VALUE           0x80AA
#define GL_SAMPLE_COVERAGE_INVERT          0x80AB
#define GL_MULTISAMPLE_BIT                 0x20000000

typedef GLvoid (APIENTRY * PFNGLSAMPLECOVERAGEPROC) (GLclampf value, GLboolean invert);

/*
**  F.4 Multitexture
*/
#define GL_TEXTURE0                        0x84C0
#define GL_TEXTURE1                        0x84C1
#define GL_TEXTURE2                        0x84C2
#define GL_TEXTURE3                        0x84C3
#define GL_TEXTURE4                        0x84C4
#define GL_TEXTURE5                        0x84C5
#define GL_TEXTURE6                        0x84C6
#define GL_TEXTURE7                        0x84C7
#define GL_TEXTURE8                        0x84C8
#define GL_TEXTURE9                        0x84C9
#define GL_TEXTURE10                       0x84CA
#define GL_TEXTURE11                       0x84CB
#define GL_TEXTURE12                       0x84CC
#define GL_TEXTURE13                       0x84CD
#define GL_TEXTURE14                       0x84CE
#define GL_TEXTURE15                       0x84CF
#define GL_TEXTURE16                       0x84D0
#define GL_TEXTURE17                       0x84D1
#define GL_TEXTURE18                       0x84D2
#define GL_TEXTURE19                       0x84D3
#define GL_TEXTURE20                       0x84D4
#define GL_TEXTURE21                       0x84D5
#define GL_TEXTURE22                       0x84D6
#define GL_TEXTURE23                       0x84D7
#define GL_TEXTURE24                       0x84D8
#define GL_TEXTURE25                       0x84D9
#define GL_TEXTURE26                       0x84DA
#define GL_TEXTURE27                       0x84DB
#define GL_TEXTURE28                       0x84DC
#define GL_TEXTURE29                       0x84DD
#define GL_TEXTURE30                       0x84DE
#define GL_TEXTURE31                       0x84DF
#define GL_ACTIVE_TEXTURE                  0x84E0
#define GL_CLIENT_ACTIVE_TEXTURE           0x84E1
#define GL_MAX_TEXTURE_UNITS               0x84E2

typedef GLvoid (APIENTRY * PFNGLACTIVETEXTUREPROC) (GLenum texture);
typedef GLvoid (APIENTRY * PFNGLCLIENTACTIVETEXTUREPROC) (GLenum texture);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD1DPROC) (GLenum texture, GLdouble s);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD1DVPROC) (GLenum texture, const GLdouble *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD1FPROC) (GLenum texture, GLfloat s);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD1FVPROC) (GLenum texture, const GLfloat *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD1IPROC) (GLenum texture, GLint s);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD1IVPROC) (GLenum texture, const GLint *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD1SPROC) (GLenum texture, GLshort s);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD1SVPROC) (GLenum texture, const GLshort *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD2DPROC) (GLenum texture, GLdouble s, GLdouble t);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD2DVPROC) (GLenum texture, const GLdouble *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD2FPROC) (GLenum texture, GLfloat s, GLfloat t);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD2FVPROC) (GLenum texture, const GLfloat *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD2IPROC) (GLenum texture, GLint s, GLint t);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD2IVPROC) (GLenum texture, const GLint *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD2SPROC) (GLenum texture, GLshort s, GLshort t);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD2SVPROC) (GLenum texture, const GLshort *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD3DPROC) (GLenum texture, GLdouble s, GLdouble t, GLdouble r);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD3DVPROC) (GLenum texture, const GLdouble *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD3FPROC) (GLenum texture, GLfloat s, GLfloat t, GLfloat r);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD3FVPROC) (GLenum texture, const GLfloat *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD3IPROC) (GLenum texture, GLint s, GLint t, GLint r);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD3IVPROC) (GLenum texture, const GLint *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD3SPROC) (GLenum texture, GLshort s, GLshort t, GLshort r);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD3SVPROC) (GLenum texture, const GLshort *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD4DPROC) (GLenum texture, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD4DVPROC) (GLenum texture, const GLdouble *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD4FPROC) (GLenum texture, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD4FVPROC) (GLenum texture, const GLfloat *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD4IPROC) (GLenum texture, GLint s, GLint t, GLint r, GLint q);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD4IVPROC) (GLenum texture, const GLint *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD4SPROC) (GLenum texture, GLshort s, GLshort t, GLshort r, GLshort q);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD4SVPROC) (GLenum texture, const GLshort *v);

/*
**  F.6 Texture Combine Environment Mode
*/
#define GL_SUBTRACT                        0x84E7
#define GL_COMBINE                         0x8570
#define GL_COMBINE_RGB                     0x8571
#define GL_COMBINE_ALPHA                   0x8572
#define GL_RGB_SCALE                       0x8573
#define GL_ADD_SIGNED                      0x8574
#define GL_INTERPOLATE                     0x8575
#define GL_CONSTANT                        0x8576
#define GL_PRIMARY_COLOR                   0x8577
#define GL_PREVIOUS                        0x8578
#define GL_SOURCE0_RGB                     0x8580 /* obsolete */
#define GL_SOURCE1_RGB                     0x8581 /* obsolete */
#define GL_SOURCE2_RGB                     0x8582 /* obsolete */
#define GL_SOURCE0_ALPHA                   0x8588 /* obsolete */
#define GL_SOURCE1_ALPHA                   0x8589 /* obsolete */
#define GL_SOURCE2_ALPHA                   0x858A /* obsolete */
#define GL_OPERAND0_RGB                    0x8590
#define GL_OPERAND1_RGB                    0x8591
#define GL_OPERAND2_RGB                    0x8592
#define GL_OPERAND0_ALPHA                  0x8598
#define GL_OPERAND1_ALPHA                  0x8599
#define GL_OPERAND2_ALPHA                  0x859A

/*
**  F.7 Texture Dot3 Environment Mode
*/
#define GL_DOT3_RGB                        0x86AE
#define GL_DOT3_RGBA                       0x86AF

/*
**  F.8 Texture Border Clamp
*/
#define GL_CLAMP_TO_BORDER                 0x812D

/*
**  F.9 Transpose Matrix
*/
#define GL_TRANSPOSE_MODELVIEW_MATRIX      0x84E3
#define GL_TRANSPOSE_PROJECTION_MATRIX     0x84E4
#define GL_TRANSPOSE_TEXTURE_MATRIX        0x84E5
#define GL_TRANSPOSE_COLOR_MATRIX          0x84E6

typedef GLvoid (APIENTRY * PFNGLLOADTRANSPOSEMATRIXDPROC) (const GLdouble m[16]);
typedef GLvoid (APIENTRY * PFNGLLOADTRANSPOSEMATRIXFPROC) (const GLfloat m[16]);
typedef GLvoid (APIENTRY * PFNGLMULTTRANSPOSEMATRIXDPROC) (const GLdouble m[16]);
typedef GLvoid (APIENTRY * PFNGLMULTTRANSPOSEMATRIXFPROC) (const GLfloat m[16]);

#endif /* GL_VERSION_1_3 */


/*
**  OpenGL 1.4 functionality
*/
#ifndef GL_VERSION_1_4
#define GL_VERSION_1_4                     1

/*
**  G.1 Automatic Mipmap Generation
*/
#define GL_GENERATE_MIPMAP                 0x8191
#define GL_GENERATE_MIPMAP_HINT            0x8192

/*
**  G.3 Changes to the Imaging Subset
*/
#define GL_CONSTANT_COLOR                  0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR        0x8002
#define GL_CONSTANT_ALPHA                  0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA        0x8004
#define GL_BLEND_COLOR                     0x8005

typedef GLvoid (APIENTRY * PFNGLBLENDCOLORPROC) (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
typedef GLvoid (APIENTRY * PFNGLBLENDEQUATIONPROC) (GLenum mode);

/*
**  G.4 Depth Textures and Shadows
*/
#define GL_DEPTH_COMPONENT16               0x81A5
#define GL_DEPTH_COMPONENT24               0x81A6
#define GL_DEPTH_COMPONENT32               0x81A7
#define GL_TEXTURE_DEPTH_SIZE              0x884A
#define GL_DEPTH_TEXTURE_MODE              0x884B
#define GL_TEXTURE_COMPARE_MODE            0x884C
#define GL_TEXTURE_COMPARE_FUNC            0X884D
#define GL_COMPARE_R_TO_TEXTURE            0x884E

/*
**  G.5 Fog Coordinate
*/
#define GL_FOG_COORDINATE_SOURCE           0x8450 /* obsolete */
#define GL_FOG_COORDINATE                  0x8451 /* obsolete */
#define GL_FRAGMENT_DEPTH                  0x8452
#define GL_CURRENT_FOG_COORDINATE          0x8453 /* obsolete */
#define GL_FOG_COORDINATE_ARRAY_TYPE       0x8454 /* obsolete */
#define GL_FOG_COORDINATE_ARRAY_STRIDE     0x8455 /* obsolete */
#define GL_FOG_COORDINATE_ARRAY_POINTER    0x8456 /* obsolete */
#define GL_FOG_COORDINATE_ARRAY            0x8457 /* obsolete */

typedef GLvoid (APIENTRY * PFNGLFOGCOORDDPROC) (GLdouble f);
typedef GLvoid (APIENTRY * PFNGLFOGCOORDDVPROC) (const GLdouble *v);
typedef GLvoid (APIENTRY * PFNGLFOGCOORDFPROC) (GLfloat f);
typedef GLvoid (APIENTRY * PFNGLFOGCOORDFVPROC) (const GLfloat *v);
typedef GLvoid (APIENTRY * PFNGLFOGCOORDPOINTERPROC) (GLenum type, GLsizei stride, const GLvoid *pointer);

/*
**  G.6 Multiple Draw Arrays
*/
typedef GLvoid (APIENTRY * PFNGLMULTIDRAWARRAYSPROC) (GLenum mode, const GLint *first, const GLsizei *count, GLsizei primcount);
typedef GLvoid (APIENTRY * PFNGLMULTIDRAWELEMENTSPROC) (GLenum mode, const GLsizei *count, GLenum type, const GLvoid **indices, GLsizei primcount);

/*
**  G.7 Point Parameters
*/
#define GL_POINT_SIZE_MIN                  0x8126
#define GL_POINT_SIZE_MAX                  0x8127
#define GL_POINT_FADE_THRESHOLD_SIZE       0x8128
#define GL_POINT_DISTANCE_ATTENUATION      0x8129

typedef GLvoid (APIENTRY * PFNGLPOINTPARAMETERFPROC) (GLenum pname, GLfloat param);
typedef GLvoid (APIENTRY * PFNGLPOINTPARAMETERFVPROC) (GLenum pname, const GLfloat *params);
typedef GLvoid (APIENTRY * PFNGLPOINTPARAMETERIPROC) (GLenum pname, GLint param);
typedef GLvoid (APIENTRY * PFNGLPOINTPARAMETERIVPROC) (GLenum pname, const GLint *params);

/*
**  G.8 Secondary Color
*/
#define GL_COLOR_SUM                       0x8458
#define GL_CURRENT_SECONDARY_COLOR         0x8459
#define GL_SECONDARY_COLOR_ARRAY_SIZE      0x845A
#define GL_SECONDARY_COLOR_ARRAY_TYPE      0x845B
#define GL_SECONDARY_COLOR_ARRAY_STRIDE    0x845C
#define GL_SECONDARY_COLOR_ARRAY_POINTER   0x845D
#define GL_SECONDARY_COLOR_ARRAY           0x845E

typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3BPROC) (GLbyte r, GLbyte g, GLbyte b);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3BVPROC) (const GLbyte *v);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3DPROC) (GLdouble r, GLdouble g, GLdouble b);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3DVPROC) (const GLdouble *v);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3FPROC) (GLfloat r, GLfloat g, GLfloat b);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3FVPROC) (const GLfloat *v);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3IPROC) (GLint r, GLint g, GLint b);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3IVPROC) (const GLint *v);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3SPROC) (GLshort r, GLshort g, GLshort b);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3SVPROC) (const GLshort *v);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3UBPROC) (GLubyte r, GLubyte g, GLubyte b);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3UBVPROC) (const GLubyte *v);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3UIPROC) (GLuint r, GLuint g, GLuint b);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3UIVPROC) (const GLuint *v);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3USPROC) (GLushort r, GLushort g, GLushort b);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3USVPROC) (const GLushort *v);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLORPOINTERPROC)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);

/*
**  G.9 Separate Blend Functions
*/
#define GL_BLEND_DST_RGB                   0x80C8
#define GL_BLEND_SRC_RGB                   0x80C9
#define GL_BLEND_DST_ALPHA                 0x80CA
#define GL_BLEND_SRC_ALPHA                 0x80CB

typedef GLvoid (APIENTRY * PFNGLBLENDFUNCSEPARATEPROC) (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);

/*
**  G.10 Stencil Wrap
*/
#define GL_INCR_WRAP                       0x8507
#define GL_DECR_WRAP                       0x8508

/*
**  G.12 Texture LOD Bias
*/
#define GL_MAX_TEXTURE_LOD_BIAS            0x84FD
#define GL_TEXTURE_FILTER_CONTROL          0x8500
#define GL_TEXTURE_LOD_BIAS                0x8501

/*
**  G.13 Texture Mirrored Repeat
*/
#define GL_MIRRORED_REPEAT                 0x8370

/*
**  G.14 Window Raster Position
*/
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS2DPROC) (GLdouble x, GLdouble y);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS2DVPROC) (const GLdouble *p);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS2FPROC) (GLfloat x, GLfloat y);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS2FVPROC) (const GLfloat *p);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS2IPROC) (GLint x, GLint y);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS2IVPROC) (const GLint *p);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS2SPROC) (GLshort x, GLshort y);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS2SVPROC) (const GLshort *p);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS3DPROC) (GLdouble x, GLdouble y, GLdouble z);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS3DVPROC) (const GLdouble *p);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS3FPROC) (GLfloat x, GLfloat y, GLfloat z);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS3FVPROC) (const GLfloat *p);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS3IPROC) (GLint x, GLint y, GLint z);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS3IVPROC) (const GLint *p);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS3SPROC) (GLshort x, GLshort y, GLshort z);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS3SVPROC) (const GLshort *p);

#endif /* GL_VERSION_1_4 */

/*
**  OpenGL 1.5 functionality
*/
#ifndef GL_VERSION_1_5
#define GL_VERSION_1_5                     1


/*
**  GL2 Shading Language
*/
#define GL_SHADER_TYPE                       0x8B4F
#define GL_DELETE_STATUS                     0x8B80
#define GL_COMPILE_STATUS                    0x8B81
#define GL_LINK_STATUS                       0x8B82
#define GL_VALIDATE_STATUS                   0x8B83
#define GL_INFO_LOG_LENGTH                   0x8B84
#define GL_ATTACHED_SHADERS                  0x8B85
#define GL_ACTIVE_UNIFORMS                   0x8B86
#define GL_ACTIVE_UNIFORM_MAX_LENGTH         0x8B87
#define GL_SHADER_SOURCE_LENGTH              0x8B88

#define GL_FLOAT_VEC2                        0x8B50
#define GL_FLOAT_VEC3                        0x8B51
#define GL_FLOAT_VEC4                        0x8B52
#define GL_INT_VEC2                          0x8B53
#define GL_INT_VEC3                          0x8B54
#define GL_INT_VEC4                          0x8B55
#define GL_BOOL                              0x8B56
#define GL_BOOL_VEC2                         0x8B57
#define GL_BOOL_VEC3                         0x8B58
#define GL_BOOL_VEC4                         0x8B59
#define GL_FLOAT_MAT2                        0x8B5A
#define GL_FLOAT_MAT3                        0x8B5B
#define GL_FLOAT_MAT4                        0x8B5C
#define GL_SAMPLER_1D                        0x8B5D
#define GL_SAMPLER_2D                        0x8B5E
#define GL_SAMPLER_3D                        0x8B5F
#define GL_SAMPLER_CUBE                      0x8B60
#define GL_SAMPLER_1D_SHADOW                 0x8B61
#define GL_SAMPLER_2D_SHADOW                 0x8B62
#define GL_SAMPLER_2D_RECT                   0x8B63
#define GL_SAMPLER_2D_RECT_SHADOW            0x8B64

#define GL_VERTEX_SHADER                     0x8B31

#define GL_MAX_VERTEX_UNIFORM_COMPONENTS     0x8B4A
#define GL_MAX_VARYING_FLOATS                0x8B4B
#define GL_MAX_VERTEX_ATTRIBS                0x8869
#define GL_MAX_TEXTURE_IMAGE_UNITS           0x8872
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS    0x8B4C
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS  0x8B4D
#define GL_MAX_TEXTURE_COORDS                0x8871

#define GL_VERTEX_PROGRAM_POINT_SIZE         0x8642
#define GL_VERTEX_PROGRAM_TWO_SIDE           0x8643

#define GL_ACTIVE_ATTRIBUTES                 0x8B89
#define GL_ACTIVE_ATTRIBUTE_MAX_LENGTH       0x8B8A

#define GL_VERTEX_ATTRIB_ARRAY_ENABLED       0x8622
#define GL_VERTEX_ATTRIB_ARRAY_SIZE          0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE        0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE          0x8625
#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED    0x886A
#define GL_CURRENT_VERTEX_ATTRIB             0x8626

#define GL_VERTEX_ATTRIB_ARRAY_POINTER       0x8645

#define GL_FRAGMENT_SHADER                   0x8B30

#define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS   0x8B49

#define GL_FRAGMENT_SHADER_DERIVATIVE_HINT   0x8B8B

#define GL_SHADING_LANGUAGE_VERSION          0x8B8C
#define GL_CURRENT_PROGRAM                   0x8B8D


/*
**  H.1 Buffer Objects
*/

#ifndef GL_POINTERS_DEFINED
typedef INT_PTR GLintptr;
typedef INT_PTR GLsizeiptr;
#define GL_POINTERS_DEFINED
#endif

#define GL_BUFFER_SIZE                              0x8764
#define GL_BUFFER_USAGE                             0x8765
#define GL_ARRAY_BUFFER                             0x8892
#define GL_ELEMENT_ARRAY_BUFFER                     0x8893
#define GL_ARRAY_BUFFER_BINDING                     0x8894
#define GL_ELEMENT_ARRAY_BUFFER_BINDING             0x8895
#define GL_VERTEX_ARRAY_BUFFER_BINDING              0x8896
#define GL_NORMAL_ARRAY_BUFFER_BINDING              0x8897
#define GL_COLOR_ARRAY_BUFFER_BINDING               0x8898
#define GL_INDEX_ARRAY_BUFFER_BINDING               0x8899
#define GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING       0x889A
#define GL_EDGE_FLAG_ARRAY_BUFFER_BINDING           0x889B
#define GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING     0x889C
#define GL_FOG_COORD_ARRAY_BUFFER_BINDING           0x889D
#define GL_READ_ONLY                                0x88B8
#define GL_WRITE_ONLY                               0x88B9
#define GL_READ_WRITE                               0x88BA
#define GL_BUFFER_ACCESS                            0x88BB
#define GL_BUFFER_MAPPED                            0x88BC
#define GL_BUFFER_MAP_POINTER                       0x88BD
#define GL_STREAM_DRAW                              0x88E0
#define GL_STREAM_READ                              0x88E1
#define GL_STREAM_COPY                              0x88E2
#define GL_STATIC_DRAW                              0x88E4
#define GL_STATIC_READ                              0x88E5
#define GL_STATIC_COPY                              0x88E6
#define GL_DYNAMIC_DRAW                             0x88E8
#define GL_DYNAMIC_READ                             0x88E9
#define GL_DYNAMIC_COPY                             0x88EA

typedef GLvoid    (APIENTRY * PFNGLBINDBUFFERPROC) (GLenum target, GLuint buffer);
typedef GLvoid    (APIENTRY * PFNGLBUFFERDATAPROC) (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
typedef GLvoid    (APIENTRY * PFNGLBUFFERSUBDATAPROC) (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
typedef GLvoid    (APIENTRY * PFNGLDELETEBUFFERSPROC) (GLsizei n, const GLuint *buffers);
typedef GLvoid    (APIENTRY * PFNGLGENBUFFERSPROC) (GLsizei n, GLuint *buffers);
typedef GLvoid    (APIENTRY * PFNGLGETBUFFERPARAMETERIVPROC) (GLenum target, GLenum value, GLint *data);
typedef GLvoid    (APIENTRY * PFNGLGETBUFFERPOINTERVPROC) (GLenum target, GLenum pname, GLvoid **params);
typedef GLvoid    (APIENTRY * PFNGLGETBUFFERSUBDATAPROC) (GLenum target, GLintptr offset, GLsizeiptr size, GLvoid *data);
typedef GLboolean (APIENTRY * PFNGLISBUFFERPROC) (GLuint buffer);
typedef GLvoid *  (APIENTRY * PFNGLMAPBUFFERPROC) (GLenum target, GLenum access);
typedef GLboolean (APIENTRY * PFNGLUNMAPBUFFERPROC) (GLenum target);

/*
**  H.2 Occlusion Queries
*/
#define GL_QUERY_COUNTER_BITS             0x8864
#define GL_CURRENT_QUERY                  0x8865
#define GL_QUERY_RESULT                   0x8866
#define GL_QUERY_RESULT_AVAILABLE         0x8867
#define GL_SAMPLES_PASSED                 0x8914

typedef GLvoid    (APIENTRY * PFNGLBEGINQUERYPROC)(GLenum target, GLuint id);
typedef GLvoid    (APIENTRY * PFNGLDELETEQUERIESPROC)(GLsizei n, const GLuint *ids);
typedef GLvoid    (APIENTRY * PFNGLENDQUERYPROC)(GLenum target);
typedef GLvoid    (APIENTRY * PFNGLGENQUERIESPROC)(GLsizei n, GLuint *ids);
typedef GLvoid    (APIENTRY * PFNGLGETQUERYIVPROC)(GLenum target, GLenum pname, GLint *params);
typedef GLvoid    (APIENTRY * PFNGLGETQUERYOBJECTIVPROC)(GLuint id, GLenum pname, GLint *params);
typedef GLvoid    (APIENTRY * PFNGLGETQUERYOBJECTUIVPROC)(GLuint id, GLenum pname, GLuint *params);
typedef GLboolean (APIENTRY * PFNGLISQUERYPROC)(GLuint id);

//Conditional Rendering
typedef GLvoid    (APIENTRY * PFNGLBEGINCONDITIONALRENDERNV)(GLuint id, GLenum mode);
typedef GLvoid    (APIENTRY * PFNGLENDCONDITIONALRENDERNV)(void);

/*
**  H.4 Changed Tokens
*/
#define GL_FOG_COORD_SRC                   0x8450
#define GL_FOG_COORD                       0x8451
#define GL_CURRENT_FOG_COORD               0x8453
#define GL_FOG_COORD_ARRAY_TYPE            0x8454
#define GL_FOG_COORD_ARRAY_STRIDE          0x8455
#define GL_FOG_COORD_ARRAY_POINTER         0x8456
#define GL_FOG_COORD_ARRAY                 0x8457
#define GL_SRC0_RGB                        0x8580
#define GL_SRC1_RGB                        0x8581
#define GL_SRC2_RGB                        0x8582
#define GL_SRC0_ALPHA                      0x8588
#define GL_SRC1_ALPHA                      0x8589
#define GL_SRC2_ALPHA                      0x858A

#endif /* GL_VERSION_1_5 */

/*
**  OpenGL 2.0 functionality
*/
#ifndef GL_VERSION_2_0
#define GL_VERSION_2_0                     1

/*
**  I.1 Programmable Shading
*/

#define GL_SHADER_TYPE                        0x8B4F
#define GL_DELETE_STATUS                      0x8B80
#define GL_COMPILE_STATUS                     0x8B81
#define GL_LINK_STATUS                        0x8B82
#define GL_VALIDATE_STATUS                    0x8B83
#define GL_INFO_LOG_LENGTH                    0x8B84
#define GL_ATTACHED_SHADERS                   0x8B85
#define GL_ACTIVE_UNIFORMS                    0x8B86
#define GL_ACTIVE_UNIFORM_MAX_LENGTH          0x8B87
#define GL_SHADER_SOURCE_LENGTH               0x8B88

#define GL_FLOAT_VEC2                         0x8B50
#define GL_FLOAT_VEC3                         0x8B51
#define GL_FLOAT_VEC4                         0x8B52
#define GL_INT_VEC2                           0x8B53
#define GL_INT_VEC3                           0x8B54
#define GL_INT_VEC4                           0x8B55
#define GL_BOOL                               0x8B56
#define GL_BOOL_VEC2                          0x8B57
#define GL_BOOL_VEC3                          0x8B58
#define GL_BOOL_VEC4                          0x8B59
#define GL_FLOAT_MAT2                         0x8B5A
#define GL_FLOAT_MAT3                         0x8B5B
#define GL_FLOAT_MAT4                         0x8B5C
#define GL_SAMPLER_1D                         0x8B5D
#define GL_SAMPLER_2D                         0x8B5E
#define GL_SAMPLER_3D                         0x8B5F
#define GL_SAMPLER_CUBE                       0x8B60
#define GL_SAMPLER_1D_SHADOW                  0x8B61
#define GL_SAMPLER_2D_SHADOW                  0x8B62
#define GL_SAMPLER_2D_RECT                    0x8B63
#define GL_SAMPLER_2D_RECT_SHADOW             0x8B64

#define GL_VERTEX_SHADER                      0x8B31

#define GL_MAX_VERTEX_UNIFORM_COMPONENTS      0x8B4A
#define GL_MAX_VARYING_FLOATS                 0x8B4B
#define GL_MAX_VERTEX_ATTRIBS                 0x8869
#define GL_MAX_TEXTURE_IMAGE_UNITS            0x8872
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS     0x8B4C
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS   0x8B4D
#define GL_MAX_TEXTURE_COORDS                 0x8871

#define GL_VERTEX_PROGRAM_POINT_SIZE          0x8642
#define GL_VERTEX_PROGRAM_TWO_SIDE            0x8643

#define GL_ACTIVE_ATTRIBUTES                  0x8B89
#define GL_ACTIVE_ATTRIBUTE_MAX_LENGTH        0x8B8A

#define GL_VERTEX_ATTRIB_ARRAY_ENABLED        0x8622
#define GL_VERTEX_ATTRIB_ARRAY_SIZE           0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE         0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE           0x8625
#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED     0x886A
#define GL_CURRENT_VERTEX_ATTRIB              0x8626

#define GL_VERTEX_ATTRIB_ARRAY_POINTER        0x8645

#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING 0x889F

#define GL_FRAGMENT_SHADER                    0x8B30

#define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS    0x8B49

#define GL_FRAGMENT_SHADER_DERIVATIVE_HINT    0x8B8B

#define GL_SHADING_LANGUAGE_VERSION           0x8B8C
#define GL_CURRENT_PROGRAM                    0x8B8D

#ifndef GL_CHAR_DEFINED
typedef char         GLchar;
#define GL_CHAR_DEFINED
#endif

typedef GLvoid    (APIENTRY *PFNGLDETACHSHADERPROC)(GLuint program, GLuint shader);
typedef GLuint    (APIENTRY *PFNGLCREATESHADERPROC)(GLenum type);
typedef GLvoid    (APIENTRY *PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count,
                                                    const GLchar **string, const GLint *length);
typedef GLvoid    (APIENTRY *PFNGLCOMPILESHADERPROC)(GLuint shader);
typedef GLuint    (APIENTRY *PFNGLCREATEPROGRAMPROC)(void);
typedef GLvoid    (APIENTRY *PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
typedef GLvoid    (APIENTRY *PFNGLLINKPROGRAMPROC)(GLuint program);
typedef GLvoid    (APIENTRY *PFNGLUSEPROGRAMPROC)(GLuint program);
typedef GLvoid    (APIENTRY *PFNGLVALIDATEPROGRAMPROC)(GLuint program);

typedef GLvoid    (APIENTRY *PFNGLUNIFORM1FPROC)(GLint location, GLfloat v0);
typedef GLvoid    (APIENTRY *PFNGLUNIFORM2FPROC)(GLint location, GLfloat v0, GLfloat v1);
typedef GLvoid    (APIENTRY *PFNGLUNIFORM3FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef GLvoid    (APIENTRY *PFNGLUNIFORM4FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef GLvoid    (APIENTRY *PFNGLUNIFORM1IPROC)(GLint location, GLint v0);
typedef GLvoid    (APIENTRY *PFNGLUNIFORM2IPROC)(GLint location, GLint v0, GLint v1);
typedef GLvoid    (APIENTRY *PFNGLUNIFORM3IPROC)(GLint location, GLint v0, GLint v1, GLint v2);
typedef GLvoid    (APIENTRY *PFNGLUNIFORM4IPROC)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
typedef GLvoid    (APIENTRY *PFNGLUNIFORM1FVPROC)(GLint location, GLsizei count, const GLfloat *value);
typedef GLvoid    (APIENTRY *PFNGLUNIFORM2FVPROC)(GLint location, GLsizei count, const GLfloat *value);
typedef GLvoid    (APIENTRY *PFNGLUNIFORM3FVPROC)(GLint location, GLsizei count, const GLfloat *value);
typedef GLvoid    (APIENTRY *PFNGLUNIFORM4FVPROC)(GLint location, GLsizei count, const GLfloat *value);
typedef GLvoid    (APIENTRY *PFNGLUNIFORM1IVPROC)(GLint location, GLsizei count, const GLint *value);
typedef GLvoid    (APIENTRY *PFNGLUNIFORM2IVPROC)(GLint location, GLsizei count, const GLint *value);
typedef GLvoid    (APIENTRY *PFNGLUNIFORM3IVPROC)(GLint location, GLsizei count, const GLint *value);
typedef GLvoid    (APIENTRY *PFNGLUNIFORM4IVPROC)(GLint location, GLsizei count, const GLint *value);
typedef GLvoid    (APIENTRY *PFNGLUNIFORMMATRIX2FVPROC)(GLint location, GLsizei count,
                                                        GLboolean transpose, const GLfloat *value);
typedef GLvoid    (APIENTRY *PFNGLUNIFORMMATRIX3FVPROC)(GLint location, GLsizei count,
                                                        GLboolean transpose, const GLfloat *value);
typedef GLvoid    (APIENTRY *PFNGLUNIFORMMATRIX4FVPROC)(GLint location, GLsizei count,
                                                        GLboolean transpose, const GLfloat *value);

typedef GLvoid    (APIENTRY *PFNGLGETATTACHEDSHADERSPROC)(GLuint program, GLsizei maxCount,
                                                          GLsizei *count, GLuint *shaders);
typedef GLint     (APIENTRY *PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const GLchar *name);
typedef GLvoid    (APIENTRY *PFNGLGETACTIVEUNIFORMPROC)(GLuint program, GLuint index,
                                                        GLsizei bufSize, GLsizei *length,
                                                        GLint *size, GLenum *type, GLchar *name);
typedef GLvoid    (APIENTRY *PFNGLGETUNIFORMFVPROC)(GLuint program, GLint location, GLfloat *params);
typedef GLvoid    (APIENTRY *PFNGLGETUNIFORMIVPROC)(GLuint program, GLint location, GLint *params);
typedef GLvoid    (APIENTRY *PFNGLGETSHADERSOURCEPROC)(GLuint shader, GLsizei bufSize,
                                                       GLsizei *length, GLchar *source);

typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB1SPROC)(GLuint index, GLshort x);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB1FPROC)(GLuint index, GLfloat x);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB1DPROC)(GLuint index, GLdouble x);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB2SPROC)(GLuint index, GLshort x, GLshort y);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB2FPROC)(GLuint index, GLfloat x, GLfloat y);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB2DPROC)(GLuint index, GLdouble x, GLdouble y);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB3SPROC)(GLuint index, GLshort x, GLshort y, GLshort z);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB3FPROC)(GLuint index, GLfloat x, GLfloat y, GLfloat z);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB3DPROC)(GLuint index, GLdouble x, GLdouble y, GLdouble z);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB4SPROC)(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB4FPROC)(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB4DPROC)(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB4NUBPROC)(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB1SVPROC)(GLuint index, const GLshort *v);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB1FVPROC)(GLuint index, const GLfloat *v);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB1DVPROC)(GLuint index, const GLdouble *v);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB2SVPROC)(GLuint index, const GLshort *v);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB2FVPROC)(GLuint index, const GLfloat *v);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB2DVPROC)(GLuint index, const GLdouble *v);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB3SVPROC)(GLuint index, const GLshort *v);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB3FVPROC)(GLuint index, const GLfloat *v);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB3DVPROC)(GLuint index, const GLdouble *v);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB4BVPROC)(GLuint index, const GLbyte *v);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB4SVPROC)(GLuint index, const GLshort *v);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB4IVPROC)(GLuint index, const GLint *v);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB4UBVPROC)(GLuint index, const GLubyte *v);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB4USVPROC)(GLuint index, const GLushort *v);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB4UIVPROC)(GLuint index, const GLuint *v);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB4FVPROC)(GLuint index, const GLfloat *v);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB4DVPROC)(GLuint index, const GLdouble *v);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB4NBVPROC)(GLuint index, const GLbyte *v);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB4NSVPROC)(GLuint index, const GLshort *v);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB4NIVPROC)(GLuint index, const GLint *v);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB4NUBVPROC)(GLuint index, const GLubyte *v);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB4NUSVPROC)(GLuint index, const GLushort *v);
typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIB4NUIVPROC)(GLuint index, const GLuint *v);

typedef GLvoid    (APIENTRY *PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type,
                                                           GLboolean normalized, GLsizei stride,
                                                           const GLvoid *pointer);

typedef GLvoid    (APIENTRY *PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef GLvoid    (APIENTRY *PFNGLDISABLEVERTEXATTRIBARRAYPROC)(GLuint index);

typedef GLvoid    (APIENTRY *PFNGLBINDATTRIBLOCATIONPROC)(GLuint program, GLuint index, const GLchar *name);

typedef GLvoid    (APIENTRY *PFNGLGETACTIVEATTRIBPROC)(GLuint program, GLuint index, GLsizei bufSize,
                                                       GLsizei *length, GLint *size, GLenum *type, GLchar *name);

typedef GLint     (APIENTRY *PFNGLGETATTRIBLOCATIONPROC)(GLuint program, const GLchar *name);

typedef GLvoid    (APIENTRY *PFNGLGETVERTEXATTRIBDVPROC)(GLuint index, GLenum pname, GLdouble *params);
typedef GLvoid    (APIENTRY *PFNGLGETVERTEXATTRIBFVPROC)(GLuint index, GLenum pname, GLfloat *params);
typedef GLvoid    (APIENTRY *PFNGLGETVERTEXATTRIBIVPROC)(GLuint index, GLenum pname, GLint *params);
typedef GLvoid    (APIENTRY *PFNGLGETVERTEXATTRIBPOINTERVPROC)(GLuint index, GLenum pname, GLvoid **pointer);

typedef GLvoid    (APIENTRY *PFNGLDELETESHADERPROC)(GLuint shader);
typedef GLboolean (APIENTRY *PFNGLISSHADERPROC)(GLuint shader);
typedef GLvoid    (APIENTRY *PFNGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint *params);
typedef GLvoid    (APIENTRY *PFNGETSHADERINFOLOGPROC)(GLuint shader, GLsizei bufSize,
                                                      GLsizei *length, GLchar *infoLog);

typedef GLvoid    (APIENTRY *PFNGLDELETEPROGRAMPROC)(GLuint program);
typedef GLboolean (APIENTRY *PFNGLISPROGRAMPROC)(GLuint program);
typedef GLvoid    (APIENTRY *PFNGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint *params);
typedef GLvoid    (APIENTRY *PFNGETPROGRAMINFOLOGPROC)(GLuint program, GLsizei bufSize,
                                                       GLsizei *length, GLchar *infoLog);

/*
**  I.2 Multiple Render Targets
*/
#define GL_MAX_DRAW_BUFFERS                0x8824
#define GL_DRAW_BUFFER0                    0x8825
#define GL_DRAW_BUFFER1                    0x8826
#define GL_DRAW_BUFFER2                    0x8827
#define GL_DRAW_BUFFER3                    0x8828
#define GL_DRAW_BUFFER4                    0x8829
#define GL_DRAW_BUFFER5                    0x882A
#define GL_DRAW_BUFFER6                    0x882B
#define GL_DRAW_BUFFER7                    0x882C
#define GL_DRAW_BUFFER8                    0x882D
#define GL_DRAW_BUFFER9                    0x882E
#define GL_DRAW_BUFFER10                   0x882F
#define GL_DRAW_BUFFER11                   0x8830
#define GL_DRAW_BUFFER12                   0x8831
#define GL_DRAW_BUFFER13                   0x8832
#define GL_DRAW_BUFFER14                   0x8833
#define GL_DRAW_BUFFER15                   0x8834

typedef GLvoid    (APIENTRY *PFNGLDRAWBUFFERSPROC)(GLsizei n, const GLenum *bufs);

/*
**  I.4 Point Sprites
*/
#define GL_POINT_SPRITE                    0x8861
#define GL_COORD_REPLACE                   0x8862
#define GL_POINT_SPRITE_COORD_ORIGIN       0x8CA0
#define GL_LOWER_LEFT                      0x8CA1
#define GL_UPPER_LEFT                      0x8CA2


/*
**  I.5 Separate Stencil
*/

#define GL_STENCIL_BACK_FUNC               0x8800
#define GL_STENCIL_BACK_FAIL               0x8801
#define GL_STENCIL_BACK_PASS_DEPTH_FAIL    0x8802
#define GL_STENCIL_BACK_PASS_DEPTH_PASS    0x8803
#define GL_STENCIL_BACK_REF                0x8CA3
#define GL_STENCIL_BACK_VALUE_MASK         0x8CA4
#define GL_STENCIL_BACK_WRITEMASK          0x8CA5

typedef void      (APIENTRY *PFNGLSTENCILOPSEPARATE)(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
typedef void      (APIENTRY *PFNGLSTENCILFUNCSEPARATE)(GLenum face, GLenum func, GLint ref, GLuint mask);
typedef void      (APIENTRY *PFNGLSTENCILMASKSEPARATE)(GLenum face, GLuint mask);

/*
**  BlendEquationSeparate
*/
#define GL_BLEND_EQUATION_RGB              GL_BLEND_EQUATION
#define GL_BLEND_EQUATION_ALPHA            0x883D

typedef void      (APIENTRY *PFNBLENDEQUATIONSEPARATEPROC)(GLenum modeRGB, GLenum modeAlpha);

#endif /* GL_VERSION_2_0 */


/*
**  OpenGL 3.0 functionality
*/
#ifndef GL_VERSION_3_0
#define GL_VERSION_3_0  1

/*
** Aliases to existing items
*/
/* Clipping */
#define GL_MAX_CLIP_DISTANCES                               0x0D32  // Alias of GL_MAX_CLIP_PLANES
#define GL_CLIP_DISTANCE0                                   0x3000  // Alias of GL_CLIP_PLANE0
#define GL_CLIP_DISTANCE1                                   0x3001  // Alias of GL_CLIP_PLANE1
#define GL_CLIP_DISTANCE2                                   0x3002  // Alias of GL_CLIP_PLANE2
#define GL_CLIP_DISTANCE3                                   0x3003  // Alias of GL_CLIP_PLANE3
#define GL_CLIP_DISTANCE4                                   0x3004  // Alias of GL_CLIP_PLANE4
#define GL_CLIP_DISTANCE5                                   0x3005  // Alias of GL_CLIP_PLANE5
#define GL_CLIP_DISTANCE6                                   0x3006
#define GL_CLIP_DISTANCE7                                   0x3007
/* Texture Compare */
#define GL_COMPARE_REF_TO_TEXTURE                           0x884E  // Alias of GL_COMPARE_R_TO_TEXTURE_ARB
/* Varyings */
#define GL_MAX_VARYING_COMPONENTS                           0x8B4B  // Alias of GL_MAX_VARYING_FLOATS

/*
** New Extension query
*/
#define GL_NUM_EXTENSIONS                                   0x821D

typedef GLubyte* (APIENTRY * PFNGETSTRINGIPROC) (GLenum name, GLuint index);

/*
** New Context items (for new create context)
*/
#define GL_MAJOR_VERSION                                    0x821B
#define GL_MINOR_VERSION                                    0x821C
#define GL_CONTEXT_FLAGS                                    0x821E
#define GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT              0x0001

/*
** Color buffer float
*/
#define GL_RGBA_FLOAT_MODE                                  0x8820
#define GL_CLAMP_VERTEX_COLOR                               0x891A
#define GL_CLAMP_FRAGMENT_COLOR                             0x891B
#define GL_CLAMP_READ_COLOR                                 0x891C
#define GL_FIXED_ONLY                                       0x891D

typedef GLvoid (APIENTRY * PFNGLCLAMPCOLORPROC) (GLenum target, GLenum clamp);

/*
** Conditional rendering
*/
#define GL_QUERY_WAIT                                       0x8E13
#define GL_QUERY_NO_WAIT                                    0x8E14
#define GL_QUERY_BY_REGION_WAIT                             0x8E15
#define GL_QUERY_BY_REGION_NO_WAIT                          0x8E16

typedef GLvoid    (APIENTRY * PFNGLBEGINCONDITIONALRENDERPROC) (GLuint id, GLenum mode);
typedef GLvoid    (APIENTRY * PFNGLENDCONDITIONALRENDERPROC) (void);

/*
** Depth buffer float
*/ 
#define GL_DEPTH_COMPONENT32F                               0x8CAC
#define GL_DEPTH32F_STENCIL8                                0x8CAD
#define GL_FLOAT_32_UNSIGNED_INT_24_8_REV                   0x8DAD

/*
** Draw buffers 2
*/
typedef GLvoid       (APIENTRY * PFNGLGETINTEGERI_VPROC) (GLenum target, GLuint index, GLint* data);
typedef GLvoid       (APIENTRY * PFNGLGETBOOLEANI_VPROC) (GLenum target, GLuint index, GLboolean* data);
typedef GLvoid       (APIENTRY * PFNGLCOLORMASKIPROC) (GLuint buf, GLboolean r, GLboolean g, GLboolean b, GLboolean a);
typedef GLvoid       (APIENTRY * PFNGLDISABLEIPROC) (GLenum target, GLuint index);
typedef GLvoid       (APIENTRY * PFNGLENABLEIPROC) (GLenum target, GLuint index);
typedef GLboolean    (APIENTRY * PFNGLISENABLEDIPROC) (GLenum target, GLuint index);
typedef GLvoid       (APIENTRY * PFNGLCLEARBUFFERFVPROC) (GLenum buffer, GLint drawbuffer, const GLfloat* value);
typedef GLvoid       (APIENTRY * PFNGLCLEARBUFFERIVPROC) (GLenum buffer, GLint drawbuffer, const GLint* value);
typedef GLvoid       (APIENTRY * PFNGLCLEARBUFFERUIVPROC) (GLenum buffer, GLint drawbuffer, const GLuint* value);
typedef GLvoid       (APIENTRY * PFNGLCLEARBUFFERFIPROC) (GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);

/*
** Framebuffer object
*/
#define GL_FRAMEBUFFER                                      0x8D40
#define GL_READ_FRAMEBUFFER                                 0x8CA8
#define GL_DRAW_FRAMEBUFFER                                 0x8CA9
#define GL_RENDERBUFFER                                     0x8D41
#define GL_STENCIL_INDEX1                                   0x8D46
#define GL_STENCIL_INDEX4                                   0x8D47
#define GL_STENCIL_INDEX8                                   0x8D48
#define GL_STENCIL_INDEX16                                  0x8D49
#define GL_RENDERBUFFER_WIDTH                               0x8D42
#define GL_RENDERBUFFER_HEIGHT                              0x8D43
#define GL_RENDERBUFFER_INTERNAL_FORMAT                     0x8D44
#define GL_RENDERBUFFER_RED_SIZE                            0x8D50
#define GL_RENDERBUFFER_GREEN_SIZE                          0x8D51
#define GL_RENDERBUFFER_BLUE_SIZE                           0x8D52
#define GL_RENDERBUFFER_ALPHA_SIZE                          0x8D53
#define GL_RENDERBUFFER_DEPTH_SIZE                          0x8D54
#define GL_RENDERBUFFER_STENCIL_SIZE                        0x8D55
#define GL_RENDERBUFFER_SAMPLES                             0x8CAB
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE               0x8CD0
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME               0x8CD1
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL             0x8CD2
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE     0x8CD3
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER             0x8CD4
#define GL_COLOR_ATTACHMENT0                                0x8CE0
#define GL_COLOR_ATTACHMENT1                                0x8CE1
#define GL_COLOR_ATTACHMENT2                                0x8CE2
#define GL_COLOR_ATTACHMENT3                                0x8CE3
#define GL_COLOR_ATTACHMENT4                                0x8CE4
#define GL_COLOR_ATTACHMENT5                                0x8CE5
#define GL_COLOR_ATTACHMENT6                                0x8CE6
#define GL_COLOR_ATTACHMENT7                                0x8CE7
#define GL_COLOR_ATTACHMENT8                                0x8CE8
#define GL_COLOR_ATTACHMENT9                                0x8CE9
#define GL_COLOR_ATTACHMENT10                               0x8CEA
#define GL_COLOR_ATTACHMENT11                               0x8CEB
#define GL_COLOR_ATTACHMENT12                               0x8CEC
#define GL_COLOR_ATTACHMENT13                               0x8CED
#define GL_COLOR_ATTACHMENT14                               0x8CEE
#define GL_COLOR_ATTACHMENT15                               0x8CEF
#define GL_DEPTH_ATTACHMENT                                 0x8D00
#define GL_STENCIL_ATTACHMENT                               0x8D20
#define GL_DEPTH_STENCIL_ATTACHMENT                         0x821A
#define GL_MAX_SAMPLES                                      0x8D57
#define GL_FRAMEBUFFER_COMPLETE                             0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT                0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT        0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER               0x8CDB
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER               0x8CDC
#define GL_FRAMEBUFFER_UNSUPPORTED                          0x8CDD
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE               0x8D56
#define GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING            0x8210
#define GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE            0x8211
#define GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE                  0x8212
#define GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE                0x8213          
#define GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE                 0x8214
#define GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE                0x8215
#define GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE                0x8216
#define GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE              0x8217
#define GL_FRAMEBUFFER_DEFAULT                              0x8218
#define GL_FRAMEBUFFER_UNDEFINED                            0x8219
#define GL_INDEX                                            0x8222
#define GL_DEPTH_BUFFER                                     0x8223
#define GL_STENCIL_BUFFER                                   0x8224
#define GL_INVALID_FRAMEBUFFER_OPERATION                    0x0506
#define GL_FRAMEBUFFER_BINDING                              0x8CA6
#define GL_DRAW_FRAMEBUFFER_BINDING                         0x8CA6 // alias FRAMEBUFFER_BINDING
#define GL_READ_FRAMEBUFFER_BINDING                         0x8CAA
#define GL_RENDERBUFFER_BINDING                             0x8CA7
#define GL_MAX_COLOR_ATTACHMENTS                            0x8CDF
#define GL_MAX_RENDERBUFFER_SIZE                            0x84E8

typedef GLboolean    (APIENTRY * PFNGLISRENDERBUFFERPROC) (GLuint renderbuffer);
typedef GLvoid       (APIENTRY * PFNGLBINDRENDERBUFFERPROC) (GLenum target, GLuint renderbuffer);
typedef GLvoid       (APIENTRY * PFNGLDELETERENDERBUFFERSPROC) (GLsizei n, const GLuint *renderbuffers);
typedef GLvoid       (APIENTRY * PFNGLGENRENDERBUFFERSPROC) (GLsizei n, GLuint *renderbuffers);
typedef GLvoid       (APIENTRY * PFNGLRENDERBUFFERSTORAGEPROC) (GLenum target, GLenum internalformat,
                                                                  GLsizei width, GLsizei height);
typedef GLvoid       (APIENTRY * PFNGLGETRENDERBUFFERPARAMETERIVPROC) (GLenum target, GLenum pname, GLint* params);
typedef GLboolean    (APIENTRY * PFNGLISFRAMEBUFFERPROC) (GLuint framebuffer);
typedef GLvoid       (APIENTRY * PFNGLBINDFRAMEBUFFERPROC) (GLenum target, GLuint framebuffer);
typedef GLvoid       (APIENTRY * PFNGLDELETEFRAMEBUFFERSPROC) (GLsizei n, const GLuint *framebuffers);
typedef GLvoid       (APIENTRY * PFNGLGENFRAMEBUFFERSPROC) (GLsizei n, GLuint *framebuffers);
typedef GLenum       (APIENTRY * PFNGLCHECKFRAMEBUFFERSTATUSPROC) (GLenum target);
typedef GLvoid       (APIENTRY * PFNGLFRAMEBUFFERTEXTURE1DPROC) (GLenum target, GLenum attachment,
                                                                   GLenum textarget, GLuint texture, GLint level);
typedef GLvoid       (APIENTRY * PFNGLFRAMEBUFFERTEXTURE2DPROC) (GLenum target, GLenum attachment,
                                                                   GLenum textarget, GLuint texture, GLint level);
typedef GLvoid       (APIENTRY * PFNGLFRAMEBUFFERTEXTURE3DPROC) (GLenum target, GLenum attachment,
                                                                   GLenum textarget, GLuint texture,
                                                                   GLint level, GLint zoffset);
typedef GLvoid       (APIENTRY * PFNGLFRAMEBUFFERRENDERBUFFERPROC) (GLenum target, GLenum attachment,
                                                                      GLenum renderbuffertarget, GLuint renderbuffer);
typedef GLvoid       (APIENTRY * PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) (GLenum target, GLenum attachment,
                                                                                  GLenum pname, GLint *params);
typedef GLvoid       (APIENTRY * PFNGLGENERATEMIPMAPPROC) (GLenum target);

/*
** framebuffer sRGB
*/
#define GL_FRAMEBUFFER_SRGB                                 0x8DB9

/*
**  GPU shader4
*/
#define GL_VERTEX_ATTRIB_ARRAY_INTEGER                      0x88FD
#define GL_SAMPLER_1D_ARRAY                                 0x8DC0
#define GL_SAMPLER_2D_ARRAY                                 0x8DC1
#define GL_SAMPLER_BUFFER                                   0x8DC2
#define GL_SAMPLER_1D_ARRAY_SHADOW                          0x8DC3
#define GL_SAMPLER_2D_ARRAY_SHADOW                          0x8DC4
#define GL_SAMPLER_CUBE_SHADOW                              0x8DC5
#define GL_UNSIGNED_INT_VEC2                                0x8DC6
#define GL_UNSIGNED_INT_VEC3                                0x8DC7
#define GL_UNSIGNED_INT_VEC4                                0x8DC8
#define GL_INT_SAMPLER_1D                                   0x8DC9
#define GL_INT_SAMPLER_2D                                   0x8DCA
#define GL_INT_SAMPLER_3D                                   0x8DCB
#define GL_INT_SAMPLER_CUBE                                 0x8DCC
#define GL_INT_SAMPLER_2D_RECT                              0x8DCD
#define GL_INT_SAMPLER_1D_ARRAY                             0x8DCE
#define GL_INT_SAMPLER_2D_ARRAY                             0x8DCF
#define GL_INT_SAMPLER_BUFFER                               0x8DD0
#define GL_UNSIGNED_INT_SAMPLER_1D                          0x8DD1
#define GL_UNSIGNED_INT_SAMPLER_2D                          0x8DD2
#define GL_UNSIGNED_INT_SAMPLER_3D                          0x8DD3
#define GL_UNSIGNED_INT_SAMPLER_CUBE                        0x8DD4
#define GL_UNSIGNED_INT_SAMPLER_2D_RECT                     0x8DD5
#define GL_UNSIGNED_INT_SAMPLER_1D_ARRAY                    0x8DD6
#define GL_UNSIGNED_INT_SAMPLER_2D_ARRAY                    0x8DD7
#define GL_UNSIGNED_INT_SAMPLER_BUFFER                      0x8DD8
#define GL_MIN_PROGRAM_TEXEL_OFFSET                         0x8904
#define GL_MAX_PROGRAM_TEXEL_OFFSET                         0x8905

typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI1IPROC) (GLuint index, GLint x);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI2IPROC) (GLuint index, GLint x, GLint y);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI3IPROC) (GLuint index, GLint x, GLint y, GLint z);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI4IPROC) (GLuint index, GLint x, GLint y, GLint z, GLint w);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI1UIPROC) (GLuint index, GLuint x);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI2UIPROC) (GLuint index, GLuint x, GLuint y);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI3UIPROC) (GLuint index, GLuint x, GLuint y, GLuint z);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI4UIPROC) (GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI1IVPROC) (GLuint index, const GLint* v);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI2IVPROC) (GLuint index, const GLint* v);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI3IVPROC) (GLuint index, const GLint* v);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI4IVPROC) (GLuint index, const GLint* v);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI1UIVPROC) (GLuint index, const GLuint* v);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI2UIVPROC) (GLuint index, const GLuint* v);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI3UIVPROC) (GLuint index, const GLuint* v);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI4UIVPROC) (GLuint index, const GLuint* v);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI4BVPROC) (GLuint index, const GLbyte* v);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI4SVPROC) (GLuint index, const GLshort* v);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI4UBVPROC) (GLuint index, const GLubyte* v);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI4USVPROC) (GLuint index, const GLushort* v);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBIPOINTERPROC) (GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
typedef GLvoid (APIENTRY * PFNGLGETVERTEXATTRIBIIVPROC) (GLuint index, GLenum pname, GLint* params);
typedef GLvoid (APIENTRY * PFNGLGETVERTEXATTRIBIUIVPROC) (GLuint index, GLenum pname, GLuint* params);
typedef GLvoid (APIENTRY * PFNGLUNIFORM1UIPROC) (GLint location, GLuint v0);
typedef GLvoid (APIENTRY * PFNGLUNIFORM2UIPROC) (GLint location, GLuint v0, GLuint v1);
typedef GLvoid (APIENTRY * PFNGLUNIFORM3UIPROC) (GLint location, GLuint v0, GLuint v1, GLuint v2);
typedef GLvoid (APIENTRY * PFNGLUNIFORM4UIPROC) (GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
typedef GLvoid (APIENTRY * PFNGLUNIFORM1UIVPROC) (GLint location, GLsizei count, const GLuint* value);
typedef GLvoid (APIENTRY * PFNGLUNIFORM2UIVPROC) (GLint location, GLsizei count, const GLuint* value);
typedef GLvoid (APIENTRY * PFNGLUNIFORM3UIVPROC) (GLint location, GLsizei count, const GLuint* value);
typedef GLvoid (APIENTRY * PFNGLUNIFORM4UIVPROC) (GLint location, GLsizei count, const GLuint* value);
typedef GLvoid (APIENTRY * PFNGLGETUNIFORMUIVPROC) (GLuint program, GLint location, GLuint* params);
typedef GLvoid (APIENTRY * PFNGLBINDFRAGDATALOCATIONPROC) (GLuint program, GLuint colorNumber, const GLchar* name);
typedef GLvoid (APIENTRY * PFNGLGETFRAGDATALOCATIONPROC) (GLuint program, const GLchar* name);

/*
** Geometry Shader Layers (only layers)
*/
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS             0x8DA8
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT               0x8DA9

#define GL_FRAMEBUFFER_ATTACHMENT_LAYERED                   0x8DA7
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER             0x8CD4

/*
** Half float
*/
#define GL_HALF_FLOAT                                       0x140B

/*
** Map buffer range
*/
#define GL_MAP_READ_BIT                                     0x0001
#define GL_MAP_WRITE_BIT                                    0x0002
#define GL_MAP_INVALIDATE_RANGE_BIT                         0x0004
#define GL_MAP_INVALIDATE_BUFFER_BIT                        0x0008
#define GL_MAP_FLUSH_EXPLICIT_BIT                           0x0010
#define GL_MAP_UNSYNCHRONIZED_BIT                           0x0020
#define GL_TEXTURE_BUFFER                                   0x8C2A
#define GL_UNIFORM_BUFFER                                   0x8DEE

typedef GLvoid*      (APIENTRY * PFNGLMAPBUFFERRANGEPROC) (GLenum target, GLintptr offset, GLsizeiptr size, GLbitfield access);
typedef GLvoid       (APIENTRY * PFNGLFLUSHMAPPEDBUFFERRANGEPROC) (GLenum target, GLintptr offset, GLsizeiptr size);

/*
** packed depth and stencil format 
*/
#define GL_DEPTH_STENCIL                                    0x84F9
#define GL_UNSIGNED_INT_24_8                                0x84FA
#define GL_DEPTH24_STENCIL8                                 0x88F0
#define GL_TEXTURE_STENCIL_SIZE                             0x88F1

/*
** packed float
*/
#define GL_R11F_G11F_B10F                                   0x8C3A
#define GL_UNSIGNED_INT_10F_11F_11F_REV                     0x8C3B

/*
** texture arrays
*/
#define GL_MAX_ARRAY_TEXTURE_LAYERS                         0x88FF
#define GL_TEXTURE_1D_ARRAY                                 0x8C18
#define GL_PROXY_TEXTURE_1D_ARRAY                           0x8C19
#define GL_TEXTURE_2D_ARRAY                                 0x8C1A
#define GL_PROXY_TEXTURE_2D_ARRAY                           0x8C1B
#define GL_TEXTURE_BINDING_1D_ARRAY                         0x8C1C
#define GL_TEXTURE_BINDING_2D_ARRAY                         0x8C1D

/*
** texture compression
*/
#define GL_COMPRESSED_LUMINANCE_LATC1                       0x8C70
#define GL_COMPRESSED_SIGNED_LUMINANCE_LATC1                0x8C71
#define GL_COMPRESSED_LUMINANCE_ALPHA_LATC2                 0x8C72
#define GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2          0x8C73
#define GL_COMPRESSED_RED_RGTC1                             0x8DBB
#define GL_COMPRESSED_SIGNED_RED_RGTC1                      0x8DBC
#define GL_COMPRESSED_RED_GREEN_RGTC2                       0x8DBD
#define GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2                0x8DBE

/*
** texture integer
*/
#define GL_RGBA32UI                                         0x8D70
#define GL_RGB32UI                                          0x8D71
#define GL_RGBA16UI                                         0x8D76
#define GL_RGB16UI                                          0x8D77
#define GL_RGBA8UI                                          0x8D7C
#define GL_RGB8UI                                           0x8D7D
#define GL_RGBA32I                                          0x8D82
#define GL_RGB32I                                           0x8D83
#define GL_RGBA16I                                          0x8D88
#define GL_RGB16I                                           0x8D89
#define GL_RGBA8I                                           0x8D8E
#define GL_RGB8I                                            0x8D8F
#define GL_RED_INTEGER                                      0x8D94
#define GL_GREEN_INTEGER                                    0x8D95
#define GL_BLUE_INTEGER                                     0x8D96
#define GL_ALPHA_INTEGER                                    0x8D97
#define GL_RGB_INTEGER                                      0x8D98
#define GL_RGBA_INTEGER                                     0x8D99
#define GL_BGR_INTEGER                                      0x8D9A
#define GL_BGRA_INTEGER                                     0x8D9B
#define GL_RGB10_A2UI                                       0x906F

typedef GLvoid       (APIENTRY * PFNGLTEXPARAMETERIIVPROC) (GLenum target, GLenum pname, const GLint* params);
typedef GLvoid       (APIENTRY * PFNGLTEXPARAMETERIUIVPROC) (GLenum target, GLenum pname, const GLuint* params);
typedef GLvoid       (APIENTRY * PFNGLGETTEXPARAMETERIIVPROC) (GLenum target, GLenum pname, GLint* params);
typedef GLvoid       (APIENTRY * PFNGLGETTEXPARAMETERIUIVPROC) (GLenum target, GLenum pname, GLuint* params);

/*
** texture float
*/
#define GL_TEXTURE_RED_TYPE                                 0x8C10
#define GL_TEXTURE_GREEN_TYPE                               0x8C11
#define GL_TEXTURE_BLUE_TYPE                                0x8C12
#define GL_TEXTURE_ALPHA_TYPE                               0x8C13
#define GL_TEXTURE_LUMINANCE_TYPE                           0x8C14
#define GL_TEXTURE_INTENSITY_TYPE                           0x8C15
#define GL_TEXTURE_DEPTH_TYPE                               0x8C16
#define GL_UNSIGNED_NORMALIZED                              0x8C17
#define GL_RGBA32F                                          0x8814
#define GL_RGB32F                                           0x8815
#define GL_ALPHA32F                                         0x8816
#define GL_INTENSITY32F                                     0x8817
#define GL_LUMINANCE32F                                     0x8818
#define GL_LUMINANCE_ALPHA32F                               0x8819
#define GL_RGBA16F                                          0x881A
#define GL_RGB16F                                           0x881B
#define GL_ALPHA16F                                         0x881C
#define GL_INTENSITY16F                                     0x881D
#define GL_LUMINANCE16F                                     0x881E
#define GL_LUMINANCE_ALPHA16F                               0x881F

/*
** Texture R/RG
*/
#define GL_COMPRESSED_RED                                   0x8225
#define GL_COMPRESSED_RG                                    0x8226
#define GL_RG                                               0x8227
#define GL_RG_INTEGER                                       0x8228
#define GL_R8                                               0x8229
#define GL_R16                                              0x822A
#define GL_RG8                                              0x822B
#define GL_RG16                                             0x822C
#define GL_R16F                                             0x822D
#define GL_R32F                                             0x822E
#define GL_RG16F                                            0x822F
#define GL_RG32F                                            0x8230
#define GL_R8I                                              0x8231
#define GL_R8UI                                             0x8232
#define GL_R16I                                             0x8233
#define GL_R16UI                                            0x8234
#define GL_R32I                                             0x8235
#define GL_R32UI                                            0x8236
#define GL_RG8I                                             0x8237
#define GL_RG8UI                                            0x8238
#define GL_RG16I                                            0x8239
#define GL_RG16UI                                           0x823A
#define GL_RG32I                                            0x823B
#define GL_RG32UI                                           0x823C

/*
** Texture Shared Exponent
*/
#define GL_RGB9_E5                                          0x8C3D
#define GL_UNSIGNED_INT_5_9_9_9_REV                         0x8C3E
#define GL_TEXTURE_SHARED_SIZE                              0x8C3F

/*
** Transform feedback
*/
#define GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH            0x8C76
#define GL_TRANSFORM_FEEDBACK_BUFFER_MODE                   0x8C7F
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS       0x8C80
#define GL_TRANSFORM_FEEDBACK_VARYINGS                      0x8C83
#define GL_TRANSFORM_FEEDBACK_BUFFER_START                  0x8C84
#define GL_TRANSFORM_FEEDBACK_BUFFER_SIZE                   0x8C85
#define GL_PRIMITIVES_GENERATED                             0x8C87
#define GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN            0x8C88
#define GL_RASTERIZER_DISCARD                               0x8C89
#define GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS    0x8C8A
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS          0x8C8B
#define GL_INTERLEAVED_ATTRIBS                              0x8C8C
#define GL_SEPARATE_ATTRIBS                                 0x8C8D
#define GL_TRANSFORM_FEEDBACK_BUFFER                        0x8C8E
#define GL_TRANSFORM_FEEDBACK_BUFFER_BINDING                0x8C8F

typedef GLvoid       (APIENTRY * PFNGLBINDBUFFERRANGEPROC) (GLenum target, GLuint index, GLuint buffer, GLint* offset, GLsizei* size);
typedef GLvoid       (APIENTRY * PFNGLBINDBUFFERBASEPROC) (GLenum target, GLuint index, GLuint buffer);
typedef GLvoid       (APIENTRY * PFNGLBEGINTRANSFORMFEEDBACKPROC) (GLenum primitiveMode);
typedef GLvoid       (APIENTRY * PFNGLENDTRANSFORMFEEDBACKPROC) (void);
typedef GLvoid       (APIENTRY * PFNGLTRANSFORMFEEDBACKVARYINGSPROC) (GLuint program, GLsizei count, const char** varyings, GLenum bufferMode);
typedef GLvoid       (APIENTRY * PFNGLGETTRANSFORMFEEDBACKVARYINGPROC) (GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLsizei* size, GLenum* type, GLchar* name);

/*
** Vertex array object
*/
#define GL_VERTEX_ARRAY_BINDING                 0x85B5

typedef GLvoid       (APIENTRY * PFNGLGENVERTEXARRAYSPROC) (GLsizei n, GLuint* arrays);
typedef GLvoid       (APIENTRY * PFNGLDELETEVERTEXARRAYSPROC) (GLsizei n, const GLuint* arrays);
typedef GLvoid       (APIENTRY * PFNGLBINDVERTEXARRAYPROC) (GLuint array);
typedef GLboolean    (APIENTRY * PFNGLISVERTEXARRAYPROC) (GLuint array);

#endif /* GL_VERSION_3_0 */


/*
**  OpenGL 3.1 functionality
*/
#ifndef GL_VERSION_3_1
#define GL_VERSION_3_1  1

#define GL_COPY_READ_BUFFER                                0x8F36
#define GL_COPY_WRITE_BUFFER                               0x8F37

// GL_EXT_texture_snorm
#define GL_ALPHA_SNORM                                      0x9010
#define GL_LUMINANCE_SNORM                                  0x9011
#define GL_LUMINANCE_ALPHA_SNORM                            0x9012
#define GL_INTENSITY_SNORM                                  0x9013
#define GL_RED_SNORM                                        0x8F90
#define GL_RG_SNORM                                         0x8F91
#define GL_RGB_SNORM                                        0x8F92 
#define GL_RGBA_SNORM                                       0x8F93 
#define GL_ALPHA8_SNORM                                     0x9014
#define GL_LUMINANCE8_SNORM                                 0x9015
#define GL_LUMINANCE8_ALPHA8_SNORM                          0x9016
#define GL_INTENSITY8_SNORM                                 0x9017
#define GL_R8_SNORM                                         0x8F94
#define GL_RG8_SNORM                                        0x8F95
#define GL_RGB8_SNORM                                       0x8F96
#define GL_RGBA8_SNORM                                      0x8F97
#define GL_ALPHA16_SNORM                                    0x9018
#define GL_LUMINANCE16_SNORM                                0x9019
#define GL_LUMINANCE16_ALPHA16_SNORM                        0x901A
#define GL_INTENSITY16_SNORM                                0x901B
#define GL_R16_SNORM                                        0x8F98
#define GL_RG16_SNORM                                       0x8F99
#define GL_RGB16_SNORM                                      0x8F9A
#define GL_RGBA16_SNORM                                     0x8F9B
#define GL_SIGNED_NORMALIZED                                0x8F9C

typedef GLvoid (APIENTRY * PFNGLCOPYBUFFERSUBDATAPROC)(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);

// primitive restart
#define GL_PRIMITIVE_RESTART                                0x8F9D  // temporary value
#define GL_PRIMITIVE_RESTART_INDEX                          0x8F9E  // temporary value

typedef GLvoid (APIENTRY * PFNGLPRIMITIVERESTARTINDEXPROC)(GLuint index);

// texture buffer objects
#define GL_TEXTURE_BUFFER                                   0x8C2A

#define GL_MAX_TEXTURE_BUFFER_SIZE                          0x8C2B
#define GL_TEXTURE_BINDING_BUFFER                           0x8C2C
#define GL_TEXTURE_BUFFER_DATA_STORE_BINDING                0x8C2D

typedef GLvoid (APIENTRY * PFNGLTEXBUFFERPROC)(GLenum target, GLenum internalformat, GLuint buffer);

#endif /* GL_VERSION_3_1 */

/*
**  OpenGL 3.2 functionality
*/
#ifndef GL_VERSION_3_2
#define GL_VERSION_3_2  1

// WGL/GLX_create_context get values
#define GL_CONTEXT_CORE_PROFILE_BIT          0x00000001
#define GL_CONTEXT_COMPATIBILITY_PROFILE_BIT 0x00000002
#define GL_CONTEXT_PROFILE_MASK              0x9126

#ifndef GLint64
#define GLint64 GLint64
typedef long long GLint64;
#endif

typedef GLvoid (APIENTRY * PFNGLGETINTEGER64VPROC)(GLenum pname, GLint64 *params);
typedef GLvoid (APIENTRY * PFNGLGETINTEGER64I_VPROC)(GLenum target, GLuint index, GLint64 *data);
typedef GLvoid (APIENTRY * PFNGLGETBUFFERPARAMETERI64VPROC) (GLenum target, GLenum value, GLint *data);

// sync objects
#ifndef GLsync
#define GLsync GLsync
struct __GLsync;
typedef struct __GLsync* GLsync;
#endif

#define GL_MAX_SERVER_WAIT_TIMEOUT                          0x9111
#define GL_OBJECT_TYPE                                      0x9112
#define GL_SYNC_CONDITION                                   0x9113
#define GL_SYNC_STATUS                                      0x9114
#define GL_SYNC_FLAGS                                       0x9115
#define GL_SYNC_FENCE                                       0x9116
#define GL_SYNC_GPU_COMMANDS_COMPLETE                       0x9117
#define GL_UNSIGNALED                                       0x9118
#define GL_SIGNALED                                         0x9119
#define GL_ALREADY_SIGNALED                                 0x911A
#define GL_TIMEOUT_EXPIRED                                  0x911B
#define GL_CONDITION_SATISFIED                              0x911C
#define GL_WAIT_FAILED                                      0x911D

#define GL_SYNC_FLUSH_COMMANDS_BIT                          0x00000001

#define GL_TIMEOUT_IGNORED                                  0xFFFFFFFFFFFFFFFFull

#ifndef GLuint64
#define GLuint64 GLuint64
typedef unsigned long long GLuint64;
#endif

typedef GLsync (APIENTRY * PFNGLFENCESYNCPROC)(GLenum condition, GLbitfield flags);
typedef GLboolean (APIENTRY * PFNGLISSYNCPROC)(GLsync sync);
typedef GLvoid (APIENTRY * PFNGLDELETESYNCPROC)(GLsync sync);

typedef GLenum (APIENTRY * PFNGLCLIENTWAITSYNCPROC)(GLsync sync, GLbitfield flags, GLuint64 timeout);
typedef GLvoid (APIENTRY * PFNGLWAITSYNCPROC)(GLsync sync, GLbitfield flags, GLuint64 timeout);
typedef GLvoid (APIENTRY * PFNGLGETSYNCIVPROC)(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values);

#endif /* OpenGL 3.2 functionality */


/*
**  OpenGL 3.3 functionality
*/
#ifndef GL_VERSION_3_3
#define GL_VERSION_3_3  1

// GL_ARB_texture_swizzle
#define GL_TEXTURE_SWIZZLE_R            0x8E42 
#define GL_TEXTURE_SWIZZLE_G            0x8E43 
#define GL_TEXTURE_SWIZZLE_B            0x8E44 
#define GL_TEXTURE_SWIZZLE_A            0x8E45 
#define GL_TEXTURE_SWIZZLE_RGBA         0x8E46 

// GL_ARB_timer_query
#define GL_TIME_ELAPSED                 0x88BF
#define GL_TIMESTAMP                    0x8E28

#endif /* OpenGL 3.3 functionality */

/*
**  ARB EXTENSIONS
*/

// ARB_sampler_object
#ifndef GL_ARB_sampler_object
#define GL_ARB_sampler_object  1
#define GL_SAMPLER_BINDING                0x8919
#endif /* ARB_sampler_object */

/*
** GL_ARB_color_buffer_float
**
**  Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_color_buffer_float
#define GL_ARB_color_buffer_float   1

#define GL_RGBA_FLOAT_MODE_ARB          0x8820

#define GL_CLAMP_VERTEX_COLOR_ARB       0x891A
#define GL_CLAMP_FRAGMENT_COLOR_ARB     0x891B
#define GL_CLAMP_READ_COLOR_ARB         0x891C
#define GL_FIXED_ONLY_ARB               0x891D

typedef GLvoid (APIENTRY * PFNGLCLAMPCOLORARBPROC) (GLenum target, GLenum clamp);

#endif /* GL_ARB_color_buffer_float */

/*
** GL_ARB_depth_texture
**
** Support:
**   Rage   128    based  : Not Supported
**   Radeon 7000+  based  : Not Supported
**   Radeon 8500+  based  : Not Supported
**   Radeon 9500+  based  : Not Supported
**   Radeon X800+  based  : Not Supported
**   Radeon X1000+ based  : Not Supported
**   Radeon X2000+ based  : Not Supported
**   Radeon HD2400+ based : Supported
*/

#ifndef GL_ARB_depth_texture
#define GL_ARB_depth_texture            1

#define GL_DEPTH_COMPONENT16_ARB        0x81A5
#define GL_DEPTH_COMPONENT24_ARB        0x81A6
#define GL_DEPTH_COMPONENT32_ARB        0x81A7
#define GL_TEXTURE_DEPTH_SIZE_ARB       0x884A
#define GL_DEPTH_TEXTURE_MODE_ARB       0x884B

#endif /* GL_ARB_depth_texture */

/*
** GL_ARB_depth_buffer_float
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/

#ifndef GL_ARB_depth_buffer_float
#define GL_ARB_depth_buffer_float               1

#define GL_DEPTH_COMPONENT32F_ARB               0x8CAC
#define GL_DEPTH32F_STENCIL8_ARB                0x8CAD
#define GL_FLOAT_32_UNSIGNED_INT_24_8_REV_ARB   0x8DAD
#define GL_DEPTH_BUFFER_FLOAT_MODE_ARB          0x8CAE

#endif /* GL_ARB_depth_buffer_float */

/*
** GL_ARB_draw_buffers
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_draw_buffers
#define GL_ARB_draw_buffers     1
//
// requires GL_ARB_fragment_program
//

#define GL_MAX_DRAW_BUFFERS_ARB 0x8824
#define GL_DRAW_BUFFER0_ARB     0x8825
#define GL_DRAW_BUFFER1_ARB     0x8826
#define GL_DRAW_BUFFER2_ARB     0x8827
#define GL_DRAW_BUFFER3_ARB     0x8828
#define GL_DRAW_BUFFER4_ARB     0x8829
#define GL_DRAW_BUFFER5_ARB     0x882A
#define GL_DRAW_BUFFER6_ARB     0x882B
#define GL_DRAW_BUFFER7_ARB     0x882C
#define GL_DRAW_BUFFER8_ARB     0x882D
#define GL_DRAW_BUFFER9_ARB     0x882E
#define GL_DRAW_BUFFER10_ARB    0x882F
#define GL_DRAW_BUFFER11_ARB    0x8830
#define GL_DRAW_BUFFER12_ARB    0x8831
#define GL_DRAW_BUFFER13_ARB    0x8832
#define GL_DRAW_BUFFER14_ARB    0x8833
#define GL_DRAW_BUFFER15_ARB    0x8834

typedef GLvoid (APIENTRY * PFNGLDRAWBUFFERSARBPROC) (GLsizei n, const GLenum *bufs);
#endif /* GL_ARB_draw_buffers */

/*
** GL_ARB_fragment_program
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_fragment_program
#define GL_ARB_fragment_program                    1

#define GL_FRAGMENT_PROGRAM_ARB                    0x8804
#define GL_PROGRAM_ALU_INSTRUCTIONS_ARB            0x8805
#define GL_PROGRAM_TEX_INSTRUCTIONS_ARB            0x8806
#define GL_PROGRAM_TEX_INDIRECTIONS_ARB            0x8807
#define GL_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB     0x8808
#define GL_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB     0x8809
#define GL_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB     0x880A
#define GL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB        0x880B
#define GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB        0x880C
#define GL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB        0x880D
#define GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB 0x880E
#define GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB 0x880F
#define GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB 0x8810
#define GL_MAX_TEXTURE_COORDS_ARB                  0x8871
#define GL_MAX_TEXTURE_IMAGE_UNITS_ARB             0x8872

#endif /* GL_ARB_fragment_program */

/*
** GL_ARB_fragment_program_shadow
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_fragment_program_shadow
#define GL_ARB_fragment_program_shadow             1
#endif /* GL_ARB_fragment_program_shadow */

/*
** GL_ARB_fragment_shader
**
** Support
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_fragment_shader
#define GL_ARB_fragment_shader                      1

#define GL_FRAGMENT_SHADER_ARB                      0x8B30

#define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB      0x8B49

#define GL_FRAGMENT_SHADER_DERIVATIVE_HINT_ARB      0x8B8B

#endif /* GL_ARB_fragment_shader */

/*
** GL_ARB_imaging
**
** Support:
**   Rage 128 based      : Not Supported
**   Radeon 7000+ based  : Not Supported
**   Radeon 8500+ based  : Not Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_imaging
#define GL_ARB_imaging
#define GL_CONSTANT_COLOR                 0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR       0x8002
#define GL_CONSTANT_ALPHA                 0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA       0x8004
#define GL_BLEND_COLOR                    0x8005
#define GL_FUNC_ADD                       0x8006
#define GL_MIN                            0x8007
#define GL_MAX                            0x8008
#define GL_BLEND_EQUATION                 0x8009
#define GL_FUNC_SUBTRACT                  0x800A
#define GL_FUNC_REVERSE_SUBTRACT          0x800B
#define GL_CONVOLUTION_1D                 0x8010
#define GL_CONVOLUTION_2D                 0x8011
#define GL_SEPARABLE_2D                   0x8012
#define GL_CONVOLUTION_BORDER_MODE        0x8013
#define GL_CONVOLUTION_FILTER_SCALE       0x8014
#define GL_CONVOLUTION_FILTER_BIAS        0x8015
#define GL_REDUCE                         0x8016
#define GL_CONVOLUTION_FORMAT             0x8017
#define GL_CONVOLUTION_WIDTH              0x8018
#define GL_CONVOLUTION_HEIGHT             0x8019
#define GL_MAX_CONVOLUTION_WIDTH          0x801A
#define GL_MAX_CONVOLUTION_HEIGHT         0x801B
#define GL_POST_CONVOLUTION_RED_SCALE     0x801C
#define GL_POST_CONVOLUTION_GREEN_SCALE   0x801D
#define GL_POST_CONVOLUTION_BLUE_SCALE    0x801E
#define GL_POST_CONVOLUTION_ALPHA_SCALE   0x801F
#define GL_POST_CONVOLUTION_RED_BIAS      0x8020
#define GL_POST_CONVOLUTION_GREEN_BIAS    0x8021
#define GL_POST_CONVOLUTION_BLUE_BIAS     0x8022
#define GL_POST_CONVOLUTION_ALPHA_BIAS    0x8023
#define GL_HISTOGRAM                      0x8024
#define GL_PROXY_HISTOGRAM                0x8025
#define GL_HISTOGRAM_WIDTH                0x8026
#define GL_HISTOGRAM_FORMAT               0x8027
#define GL_HISTOGRAM_RED_SIZE             0x8028
#define GL_HISTOGRAM_GREEN_SIZE           0x8029
#define GL_HISTOGRAM_BLUE_SIZE            0x802A
#define GL_HISTOGRAM_ALPHA_SIZE           0x802B
#define GL_HISTOGRAM_LUMINANCE_SIZE       0x802C
#define GL_HISTOGRAM_SINK                 0x802D
#define GL_MINMAX                         0x802E
#define GL_MINMAX_FORMAT                  0x802F
#define GL_MINMAX_SINK                    0x8030
#define GL_TABLE_TOO_LARGE                0x8031
#define GL_COLOR_MATRIX                   0x80B1
#define GL_COLOR_MATRIX_STACK_DEPTH       0x80B2
#define GL_MAX_COLOR_MATRIX_STACK_DEPTH   0x80B3
#define GL_POST_COLOR_MATRIX_RED_SCALE    0x80B4
#define GL_POST_COLOR_MATRIX_GREEN_SCALE  0x80B5
#define GL_POST_COLOR_MATRIX_BLUE_SCALE   0x80B6
#define GL_POST_COLOR_MATRIX_ALPHA_SCALE  0x80B7
#define GL_POST_COLOR_MATRIX_RED_BIAS     0x80B8
#define GL_POST_COLOR_MATRIX_GREEN_BIAS   0x80B9
#define GL_POST_COLOR_MATRIX_BLUE_BIAS    0x80BA
#define GL_POST_COLOR_MATRIX_ALPHA_BIAS   0x80BB
#define GL_COLOR_TABLE                    0x80D0
#define GL_POST_CONVOLUTION_COLOR_TABLE   0x80D1
#define GL_POST_COLOR_MATRIX_COLOR_TABLE  0x80D2
#define GL_PROXY_COLOR_TABLE              0x80D3
#define GL_PROXY_POST_CONVOLUTION_COLOR_TABLE 0x80D4
#define GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE 0x80D5
#define GL_COLOR_TABLE_SCALE              0x80D6
#define GL_COLOR_TABLE_BIAS               0x80D7
#define GL_COLOR_TABLE_FORMAT             0x80D8
#define GL_COLOR_TABLE_WIDTH              0x80D9
#define GL_COLOR_TABLE_RED_SIZE           0x80DA
#define GL_COLOR_TABLE_GREEN_SIZE         0x80DB
#define GL_COLOR_TABLE_BLUE_SIZE          0x80DC
#define GL_COLOR_TABLE_ALPHA_SIZE         0x80DD
#define GL_COLOR_TABLE_LUMINANCE_SIZE     0x80DE
#define GL_COLOR_TABLE_INTENSITY_SIZE     0x80DF
#define GL_CONSTANT_BORDER                0x8151
#define GL_REPLICATE_BORDER               0x8153
#define GL_CONVOLUTION_BORDER_COLOR       0x8154

typedef GLvoid (APIENTRY * PFNGLCOLORTABLE)( GLenum,GLenum, GLsizei, GLenum, GLenum, const GLvoid *);
typedef GLvoid (APIENTRY * PFNGLCOLORTABLEPARAMETERFV)( GLenum,GLenum, const GLfloat *);
typedef GLvoid (APIENTRY * PFNGLCOLORTABLEPARAMETERIV)( GLenum,GLenum, const GLint *);
typedef GLvoid (APIENTRY * PFNGLCOPYCOLORTABLE)( GLenum,GLenum, GLint, GLint, GLsizei);
typedef GLvoid (APIENTRY * PFNGLGETCOLORTABLE)( GLenum,GLenum, GLenum, GLvoid *);
typedef GLvoid (APIENTRY * PFNGLGETCOLORTABLEPARAMETERFV)( GLenum,GLenum, GLfloat *);
typedef GLvoid (APIENTRY * PFNGLGETCOLORTABLEPARAMETERIV)( GLenum,GLenum, GLint *);
typedef GLvoid (APIENTRY * PFNGLCOLORSUBTABLE)( GLenum,GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);
typedef GLvoid (APIENTRY * PFNGLCOPYCOLORSUBTABLE)( GLenum,GLsizei, GLint, GLint, GLsizei);
typedef GLvoid (APIENTRY * PFNGLCONVOLUTIONFILTER1D)( GLenum,GLenum, GLsizei, GLenum, GLenum, const GLvoid *);
typedef GLvoid (APIENTRY * PFNGLCONVOLUTIONFILTER2D)( GLenum,GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);
typedef GLvoid (APIENTRY * PFNGLCONVOLUTIONPARAMETERF)( GLenum,GLenum, GLfloat);
typedef GLvoid (APIENTRY * PFNGLCONVOLUTIONPARAMETERFV)( GLenum,GLenum, const GLfloat *);
typedef GLvoid (APIENTRY * PFNGLCONVOLUTIONPARAMETERI)( GLenum,GLenum, GLint);
typedef GLvoid (APIENTRY * PFNGLCONVOLUTIONPARAMETERIV)( GLenum,GLenum, const GLint *);
typedef GLvoid (APIENTRY * PFNGLCOPYCONVOLUTIONFILTER1D)( GLenum,GLenum, GLint, GLint, GLsizei);
typedef GLvoid (APIENTRY * PFNGLCOPYCONVOLUTIONFILTER2D)( GLenum,GLenum, GLint, GLint, GLsizei, GLsizei);
typedef GLvoid (APIENTRY * PFNGLGETCONVOLUTIONFILTER)( GLenum,GLenum, GLenum, GLvoid *);
typedef GLvoid (APIENTRY * PFNGLGETCONVOLUTIONPARAMETERFV)( GLenum,GLenum, GLfloat *);
typedef GLvoid (APIENTRY * PFNGLGETCONVOLUTIONPARAMETERIV)( GLenum,GLenum, GLint *);
typedef GLvoid (APIENTRY * PFNGLGETSEPARABLEFILTER)( GLenum,GLenum, GLenum, GLvoid *, GLvoid *, GLvoid *);
typedef GLvoid (APIENTRY * PFNGLSEPARABLEFILTER2D)( GLenum,GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *, const GLvoid *);
typedef GLvoid (APIENTRY * PFNGLGETHISTOGRAM)( GLenum,GLboolean, GLenum, GLenum, GLvoid *);
typedef GLvoid (APIENTRY * PFNGLGETHISTOGRAMPARAMETERFV)( GLenum,GLenum, GLfloat *);
typedef GLvoid (APIENTRY * PFNGLGETHISTOGRAMPARAMETERIV)( GLenum,GLenum, GLint *);
typedef GLvoid (APIENTRY * PFNGLGETMINMAX)( GLenum,GLboolean, GLenum, GLenum, GLvoid *);
typedef GLvoid (APIENTRY * PFNGLGETMINMAXPARAMETERFV)( GLenum,GLenum, GLfloat *);
typedef GLvoid (APIENTRY * PFNGLGETMINMAXPARAMETERIV)( GLenum,GLenum, GLint *);
typedef GLvoid (APIENTRY * PFNGLHISTOGRAM)( GLenum,GLsizei, GLenum, GLboolean);
typedef GLvoid (APIENTRY * PFNGLMINMAX)( GLenum,GLenum, GLboolean);
typedef GLvoid (APIENTRY * PFNGLRESETHISTOGRAM)( GLenum);
typedef GLvoid (APIENTRY * PFNGLRESETMINMAX)( GLenum);

#endif /* GL_ARB_imaging */

/*
** GL_ARB_matrix_palette
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Not Supported
**   Radeon 9500+  based : Not Supported
**   Radeon X800+  based : Not Supported
**   Radeon X1000+ based : Not Supported
**   Radeon X2000+ based : Not Supported
*/
#ifndef GL_ARB_matrix_palette
#define GL_ARB_matrix_palette                   1

#define GL_MATRIX_PALETTE_ARB                   0x8840
#define GL_MAX_MATRIX_PALETTE_STACK_DEPTH_ARB   0x8841
#define GL_MAX_PALETTE_MATRICES_ARB             0x8842
#define GL_CURRENT_PALETTE_MATRIX_ARB           0x8843
#define GL_MATRIX_INDEX_ARRAY_ARB               0x8844
#define GL_CURRENT_MATRIX_INDEX_ARB             0x8845
#define GL_MATRIX_INDEX_ARRAY_SIZE_ARB          0x8846
#define GL_MATRIX_INDEX_ARRAY_TYPE_ARB          0x8847
#define GL_MATRIX_INDEX_ARRAY_STRIDE_ARB        0x8848
#define GL_MATRIX_INDEX_ARRAY_POINTER_ARB       0x8849

typedef GLvoid (APIENTRY * PFNGLCURRENTPALETTEMATRIXARBPROC) (GLint index);
typedef GLvoid (APIENTRY * PFNGLMATRIXINDEXUBVARBPROC) (GLint size, GLubyte *indices);
typedef GLvoid (APIENTRY * PFNGLMATRIXINDEXUSVARBPROC) (GLint size, GLushort *indices);
typedef GLvoid (APIENTRY * PFNGLMATRIXINDEXUIVARBPROC) (GLint size, GLuint *indices);
typedef GLvoid (APIENTRY * PFNGLMATRIXINDEXPOINTERARBPROC) (GLint size, GLenum type, GLsizei stride, GLvoid *pointer);

#endif /* GL_ARB_matrix_palette */

// fixme es11 mod add single precision extension
#ifndef GL_ES11_single_precision
#define GL_ES11_single_precision                     1

typedef GLvoid (APIENTRY * PFNGLCLEARDEPTHFOES) (GLclampf depth);
typedef GLvoid (APIENTRY * PFNGLCLIPPLANEFOES) (GLenum plane, const GLfloat *equation);
typedef GLvoid (APIENTRY * PFNGLDEPTHRANGEFOES) (GLclampf zNear, GLclampf zFar);
typedef GLvoid (APIENTRY * PFNGLFRUSTUMFOES) (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
typedef GLvoid (APIENTRY * PFNGLGETCLIPPLANEFOES) (GLenum plane, GLfloat *equation);
typedef GLvoid (APIENTRY * PFNGLORTHOFOES) (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);

#endif

// fixme es11 mod add fixed point
#ifndef GL_ES11_fixed_point
#define GL_ES11_fixed_point                     1

#ifndef GL_FIXEDPNT_DEFINED
typedef int GLfixed;
typedef int GLclampx;
#define GL_FIXEDPNT_DEFINED
#endif

typedef GLvoid (APIENTRY * PFNGLALPHAFUNCXOES)  (GLenum func, GLclampx ref);
typedef GLvoid (APIENTRY * PFNGLCLEARCOLORXOES) (GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha);
typedef GLvoid (APIENTRY * PFNGLCLEARDEPTHXOES) (GLclampx depth);
typedef GLvoid (APIENTRY * PFNGLCLIPPLANEXOES) (GLenum plane, const GLfixed *equation);
typedef GLvoid (APIENTRY * PFNGLCOLOR4XOES) (GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);
typedef GLvoid (APIENTRY * PFNGLDEPTHRANGEXOES) (GLclampx zNear, GLclampx zFar);
typedef GLvoid (APIENTRY * PFNGLFOGXOES) (GLenum pname, GLfixed param);
typedef GLvoid (APIENTRY * PFNGLFOGXVOES) (GLenum pname, const GLfixed * params);
typedef GLvoid (APIENTRY * PFNGLFRUSTUMXOES) (GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
typedef GLvoid (APIENTRY * PFNGLGETCLIPPLANEXOES) (GLenum plane, GLfixed *equation);
typedef GLvoid (APIENTRY * PFNGLGETFIXEDVOES) (GLenum pname, GLfixed * params);
typedef GLvoid (APIENTRY * PFNGLGETLIGHTXVOES) (GLenum light, GLenum pname, GLfixed * params);
typedef GLvoid (APIENTRY * PFNGLGETMATERIALXVOES) (GLenum face, GLenum pname, GLfixed * params);
typedef GLvoid (APIENTRY * PFNGLGETTEXENVXVOES) (GLenum target, GLenum pname, GLfixed* params);
typedef GLvoid (APIENTRY * PFNGLGETTEXPARAMETERXVOES) (GLenum target, GLenum pname, GLfixed * params);
typedef GLvoid (APIENTRY * PFNGLLIGHTMODELXOES) (GLenum pname, GLfixed param);
typedef GLvoid (APIENTRY * PFNGLLIGHTMODELXVOES) (GLenum pname, const GLfixed * params);
typedef GLvoid (APIENTRY * PFNGLLIGHTXOES) (GLenum light, GLenum pname, GLfixed param);
typedef GLvoid (APIENTRY * PFNGLLIGHTXVOES) (GLenum light, GLenum pname, const GLfixed * params);
typedef GLvoid (APIENTRY * PFNGLLINEWIDTHXOES) (GLfixed width);
typedef GLvoid (APIENTRY * PFNGLLOADMATRIXXOES) (const GLfixed * m);
typedef GLvoid (APIENTRY * PFNGLMATERIALXOES) (GLenum face, GLenum pname, GLfixed param);
typedef GLvoid (APIENTRY * PFNGLMATERIALXVOES) (GLenum face, GLenum pname, const GLfixed * params);
typedef GLvoid (APIENTRY * PFNGLMULTMATRIXXOES) (const GLfixed * m);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD4XOES) (GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q);
typedef GLvoid (APIENTRY * PFNGLNORMAL3XOES) (GLfixed nx, GLfixed ny, GLfixed nz);
typedef GLvoid (APIENTRY * PFNGLORTHOXOES) (GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed near, GLfixed far);
typedef GLvoid (APIENTRY * PFNGLPOINTPARAMETERXOES) (GLenum pname, GLfixed param);
typedef GLvoid (APIENTRY * PFNGLPOINTPARAMETERXVOES) (GLenum pname, const GLfixed * params);
typedef GLvoid (APIENTRY * PFNGLPOINTSIZEXOES) (GLfixed size);
typedef GLvoid (APIENTRY * PFNGLPOLYGONOFFSETXOES) (GLfixed factor, GLfixed units);
typedef GLvoid (APIENTRY * PFNGLROTATEXOES) (GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
typedef GLvoid (APIENTRY * PFNGLSAMPLECOVERAGEXOES) (GLclampx value, GLboolean invert);
typedef GLvoid (APIENTRY * PFNGLSCALEXOES) (GLfixed x, GLfixed y, GLfixed z);
typedef GLvoid (APIENTRY * PFNGLTEXENVXOES) (GLenum target, GLenum pname, GLfixed param);
typedef GLvoid (APIENTRY * PFNGLTEXENVXVOES) (GLenum target, GLenum pname, const GLfixed * params);
typedef GLvoid (APIENTRY * PFNGLTEXPARAMETERXOES) (GLenum target, GLenum pname, GLfixed param);
typedef GLvoid (APIENTRY * PFNGLTEXPARAMETERXVOES) (GLenum target, GLenum pname, const GLfixed* params);
typedef GLvoid (APIENTRY * PFNGLTRANSLATEXOES) (GLfixed x, GLfixed y, GLfixed z);

typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD4BOES) (GLenum texture, GLbyte s, GLbyte t, GLbyte r, GLbyte q);

#endif

/*
** GL_ARB_multisample
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_multisample
#define GL_ARB_multisample              1

#define GL_MULTISAMPLE_ARB              0x809D
#define GL_SAMPLE_ALPHA_TO_COVERAGE_ARB 0x809E
#define GL_SAMPLE_ALPHA_TO_ONE_ARB      0x809F
#define GL_SAMPLE_COVERAGE_ARB          0x80A0
#define GL_SAMPLE_BUFFERS_ARB           0x80A8
#define GL_SAMPLES_ARB                  0x80A9
#define GL_SAMPLE_COVERAGE_VALUE_ARB    0x80AA
#define GL_SAMPLE_COVERAGE_INVERT_ARB   0x80AB
#define GL_MULTISAMPLE_BIT_ARB          0x20000000

typedef GLvoid (APIENTRY * PFNGLSAMPLECOVERAGEARBPROC)(GLclampf value, GLboolean invert);

#endif /* GL_ARB_multisample */

/*
** GL_ARB_multitexture
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_multitexture
#define GL_ARB_multitexture                 1

#define GL_ACTIVE_TEXTURE_ARB               0x84E0
#define GL_CLIENT_ACTIVE_TEXTURE_ARB        0x84E1
#define GL_MAX_TEXTURE_UNITS_ARB            0x84E2
#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1
#define GL_TEXTURE2_ARB                     0x84C2
#define GL_TEXTURE3_ARB                     0x84C3
#define GL_TEXTURE4_ARB                     0x84C4
#define GL_TEXTURE5_ARB                     0x84C5
#define GL_TEXTURE6_ARB                     0x84C6
#define GL_TEXTURE7_ARB                     0x84C7
#define GL_TEXTURE8_ARB                     0x84C8
#define GL_TEXTURE9_ARB                     0x84C9
#define GL_TEXTURE10_ARB                    0x84CA
#define GL_TEXTURE11_ARB                    0x84CB
#define GL_TEXTURE12_ARB                    0x84CC
#define GL_TEXTURE13_ARB                    0x84CD
#define GL_TEXTURE14_ARB                    0x84CE
#define GL_TEXTURE15_ARB                    0x84CF
#define GL_TEXTURE16_ARB                    0x84D0
#define GL_TEXTURE17_ARB                    0x84D1
#define GL_TEXTURE18_ARB                    0x84D2
#define GL_TEXTURE19_ARB                    0x84D3
#define GL_TEXTURE20_ARB                    0x84D4
#define GL_TEXTURE21_ARB                    0x84D5
#define GL_TEXTURE22_ARB                    0x84D6
#define GL_TEXTURE23_ARB                    0x84D7
#define GL_TEXTURE24_ARB                    0x84D8
#define GL_TEXTURE25_ARB                    0x84D9
#define GL_TEXTURE26_ARB                    0x84DA
#define GL_TEXTURE27_ARB                    0x84DB
#define GL_TEXTURE28_ARB                    0x84DC
#define GL_TEXTURE29_ARB                    0x84DD
#define GL_TEXTURE30_ARB                    0x84DE
#define GL_TEXTURE31_ARB                    0x84DF

typedef GLvoid (APIENTRY * PFNGLACTIVETEXTUREARBPROC) (GLenum texture);
typedef GLvoid (APIENTRY * PFNGLCLIENTACTIVETEXTUREARBPROC) (GLenum texture);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD1DARBPROC) (GLenum texture, GLdouble s);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD1DVARBPROC) (GLenum texture, const GLdouble *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD1FARBPROC) (GLenum texture, GLfloat s);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD1FVARBPROC) (GLenum texture, const GLfloat *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD1IARBPROC) (GLenum texture, GLint s);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD1IVARBPROC) (GLenum texture, const GLint *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD1SARBPROC) (GLenum texture, GLshort s);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD1SVARBPROC) (GLenum texture, const GLshort *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD2DARBPROC) (GLenum texture, GLdouble s, GLdouble t);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD2DVARBPROC) (GLenum texture, const GLdouble *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD2FARBPROC) (GLenum texture, GLfloat s, GLfloat t);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD2FVARBPROC) (GLenum texture, const GLfloat *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD2IARBPROC) (GLenum texture, GLint s, GLint t);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD2IVARBPROC) (GLenum texture, const GLint *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD2SARBPROC) (GLenum texture, GLshort s, GLshort t);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD2SVARBPROC) (GLenum texture, const GLshort *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD3DARBPROC) (GLenum texture, GLdouble s, GLdouble t, GLdouble r);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD3DVARBPROC) (GLenum texture, const GLdouble *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD3FARBPROC) (GLenum texture, GLfloat s, GLfloat t, GLfloat r);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD3FVARBPROC) (GLenum texture, const GLfloat *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD3IARBPROC) (GLenum texture, GLint s, GLint t, GLint r);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD3IVARBPROC) (GLenum texture, const GLint *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD3SARBPROC) (GLenum texture, GLshort s, GLshort t, GLshort r);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD3SVARBPROC) (GLenum texture, const GLshort *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD4DARBPROC) (GLenum texture, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD4DVARBPROC) (GLenum texture, const GLdouble *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD4FARBPROC) (GLenum texture, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD4FVARBPROC) (GLenum texture, const GLfloat *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD4IARBPROC) (GLenum texture, GLint s, GLint t, GLint r, GLint q);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD4IVARBPROC) (GLenum texture, const GLint *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD4SARBPROC) (GLenum texture, GLshort s, GLshort t, GLshort r, GLshort q);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD4SVARBPROC) (GLenum texture, const GLshort *v);

#endif /* GL_ARB_multitexture */

/*
** GL_ARB_occlusion_query
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_occlusion_query
#define GL_ARB_occlusion_query                1

#define GL_SAMPLES_PASSED_ARB                 0x8914
#define GL_QUERY_COUNTER_BITS_ARB             0x8864
#define GL_CURRENT_QUERY_ARB                  0x8865
#define GL_QUERY_RESULT_ARB                   0x8866
#define GL_QUERY_RESULT_AVAILABLE_ARB         0x8867

typedef GLvoid (APIENTRY * PFNGLGENQUERIESARBPROC)(GLsizei n, GLuint *ids);
typedef GLvoid (APIENTRY * PFNGLDELETEQUERIESARBPROC)(GLsizei n, const GLuint *ids);
typedef GLboolean (APIENTRY * PFNGLISQUERYARBPROC)(GLuint id);
typedef GLvoid (APIENTRY * PFNGLBEGINQUERYARBPROC)(GLenum target, GLuint id);
typedef GLvoid (APIENTRY * PFNGLENDQUERYARBPROC)(GLenum target);
typedef GLvoid (APIENTRY * PFNGLGETQUERYIVARBPROC)(GLenum target, GLenum pname, GLint *params);
typedef GLvoid (APIENTRY * PFNGLGETQUERYOBJECTIVARBPROC)(GLuint id, GLenum pname, GLint *params);
typedef GLvoid (APIENTRY * PFNGLGETQUERYOBJECTUIVARBPROC)(GLuint id, GLenum pname, GLuint *params);

#endif /* GL_ARB_occlusion_query */

/*
** GL_ARB_occlusion_query2
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_occlusion_query2
#define GL_ARB_occlusion_query2                1

#define GL_ANY_SAMPLES_PASSED                 0x8C2F

#endif /* GL_ARB_occlusion_query2 */

/*
** GL_ARB_pixel_buffer_object
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_pixel_buffer_object
#define GL_ARB_pixel_buffer_object          1

#define PIXEL_PACK_BUFFER_ARB               0x88EB
#define PIXEL_UNPACK_BUFFER_ARB             0x88EC
#define PIXEL_PACK_BUFFER_BINDING_ARB       0x88ED
#define PIXEL_UNPACK_BUFFER_BINDING_ARB     0x88EF

#endif /* GL_ARB_pixel_buffer_object */


/*
** GL_ARB_point_parameters
**
**  Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_point_parameters
#define GL_ARB_point_parameters              1

#define GL_POINT_SIZE_MIN_ARB                0x8126
#define GL_POINT_SIZE_MAX_ARB                0x8127
#define GL_POINT_FADE_THRESHOLD_SIZE_ARB     0x8128
#define GL_POINT_DISTANCE_ATTENUATION_ARB    0x8129

typedef GLvoid (APIENTRY * PFNGLPOINTPARAMETERFARBPROC)  (GLenum pname, GLfloat param);
typedef GLvoid (APIENTRY * PFNGLPOINTPARAMETERFVARBPROC)  (GLenum pname, GLfloat *params);

#endif /* GL_ARB_point_parameters */

/*
** GL_ARB_point_sprite
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_point_sprite
#define GL_ARB_point_sprite                   1

#define GL_POINT_SPRITE_ARB                   0x8861
#define GL_COORD_REPLACE_ARB                  0x8862

#endif /* GL_ARB_point_sprite */

/*
** GL_ARB_shader_objects
**
** Support
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_shader_objects
#define GL_ARB_shader_objects                       1

#define GL_PROGRAM_OBJECT_ARB                       0x8B40

#define GL_OBJECT_TYPE_ARB                          0x8B4E
#define GL_OBJECT_SUBTYPE_ARB                       0x8B4F
#define GL_OBJECT_DELETE_STATUS_ARB                 0x8B80
#define GL_OBJECT_COMPILE_STATUS_ARB                0x8B81
#define GL_OBJECT_LINK_STATUS_ARB                   0x8B82
#define GL_OBJECT_VALIDATE_STATUS_ARB               0x8B83
#define GL_OBJECT_INFO_LOG_LENGTH_ARB               0x8B84
#define GL_OBJECT_ATTACHED_OBJECTS_ARB              0x8B85
#define GL_OBJECT_ACTIVE_UNIFORMS_ARB               0x8B86
#define GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB     0x8B87
#define GL_OBJECT_SHADER_SOURCE_LENGTH_ARB          0x8B88

#define GL_SHADER_OBJECT_ARB                        0x8B48

#define GL_FLOAT_VEC2_ARB                           0x8B50
#define GL_FLOAT_VEC3_ARB                           0x8B51
#define GL_FLOAT_VEC4_ARB                           0x8B52
#define GL_INT_VEC2_ARB                             0x8B53
#define GL_INT_VEC3_ARB                             0x8B54
#define GL_INT_VEC4_ARB                             0x8B55
#define GL_BOOL_ARB                                 0x8B56
#define GL_BOOL_VEC2_ARB                            0x8B57
#define GL_BOOL_VEC3_ARB                            0x8B58
#define GL_BOOL_VEC4_ARB                            0x8B59
#define GL_FLOAT_MAT2_ARB                           0x8B5A
#define GL_FLOAT_MAT3_ARB                           0x8B5B
#define GL_FLOAT_MAT4_ARB                           0x8B5C
#define GL_SAMPLER_1D_ARB                           0x8B5D
#define GL_SAMPLER_2D_ARB                           0x8B5E
#define GL_SAMPLER_3D_ARB                           0x8B5F
#define GL_SAMPLER_CUBE_ARB                         0x8B60
#define GL_SAMPLER_1D_SHADOW_ARB                    0x8B61
#define GL_SAMPLER_2D_SHADOW_ARB                    0x8B62
#define GL_SAMPLER_2D_RECT_ARB                      0x8B63
#define GL_SAMPLER_2D_RECT_SHADOW_ARB               0x8B64

#ifndef GL_CHAR_ARB_DEFINED
typedef char         GLcharARB;
#define GL_CHAR_ARB_DEFINED
#endif

#ifndef GL_HANDLE_ARB_DEFINED
typedef unsigned int GLhandleARB;
#define GL_HANDLE_ARB_DEFINED
#endif

typedef GLvoid              (APIENTRY *PFNGLDELETEOBJECTARBPROC)(GLhandleARB obj);
typedef GLhandleARB         (APIENTRY *PFNGLGETHANDLEARBPROC)(GLenum pname);
typedef GLvoid              (APIENTRY *PFNGLDETACHOBJECTARBPROC)(GLhandleARB containerObj,
                                                                 GLhandleARB attachedObj);
typedef GLhandleARB         (APIENTRY *PFNGLCREATESHADEROBJECTARBPROC)(GLenum shaderType);
typedef GLvoid              (APIENTRY *PFNGLSHADERSOURCEARBPROC)(GLhandleARB shaderObj,
                                                                 GLsizei count,
                                                                 const GLcharARB **string,
                                                                 const GLint *length);
typedef GLvoid              (APIENTRY *PFNGLCOMPILESHADERARBPROC)(GLhandleARB shaderObj);
typedef GLhandleARB         (APIENTRY *PFNGLCREATEPROGRAMOBJECTARBPROC)(void);
typedef GLvoid              (APIENTRY *PFNGLATTACHOBJECTARBPROC)(GLhandleARB containerObj,
                                                                 GLhandleARB obj);
typedef GLvoid              (APIENTRY *PFNGLLINKPROGRAMARBPROC)(GLhandleARB programObj);
typedef GLvoid              (APIENTRY *PFNGLUSEPROGRAMOBJECTARBPROC)(GLhandleARB programObj);
typedef GLvoid              (APIENTRY *PFNGLVALIDATEPROGRAMARBPROC)(GLhandleARB programObj);
typedef GLvoid              (APIENTRY *PFNGLUNIFORM1FARBPROC)(GLint location,
                                                              GLfloat v0);
typedef GLvoid              (APIENTRY *PFNGLUNIFORM2FARBPROC)(GLint location,
                                                              GLfloat v0,
                                                              GLfloat v1);
typedef GLvoid              (APIENTRY *PFNGLUNIFORM3FARBPROC)(GLint location,
                                                              GLfloat v0,
                                                              GLfloat v1,
                                                              GLfloat v2);
typedef GLvoid              (APIENTRY *PFNGLUNIFORM4FARBPROC)(GLint location,
                                                              GLfloat v0,
                                                              GLfloat v1,
                                                              GLfloat v2,
                                                              GLfloat v3);
typedef GLvoid              (APIENTRY *PFNGLUNIFORM1IARBPROC)(GLint location,
                                                              GLint v0);
typedef GLvoid              (APIENTRY *PFNGLUNIFORM2IARBPROC)(GLint location,
                                                              GLint v0,
                                                              GLint v1);
typedef GLvoid              (APIENTRY *PFNGLUNIFORM3IARBPROC)(GLint location,
                                                              GLint v0,
                                                              GLint v1,
                                                              GLint v2);
typedef GLvoid              (APIENTRY *PFNGLUNIFORM4IARBPROC)(GLint location,
                                                              GLint v0,
                                                              GLint v1,
                                                              GLint v2,
                                                              GLint v3);
typedef GLvoid              (APIENTRY *PFNGLUNIFORM1FVARBPROC)(GLint location,
                                                               GLsizei count,
                                                               const GLfloat *value);
typedef GLvoid              (APIENTRY *PFNGLUNIFORM2FVARBPROC)(GLint location,
                                                               GLsizei count,
                                                               const GLfloat *value);
typedef GLvoid              (APIENTRY *PFNGLUNIFORM3FVARBPROC)(GLint location,
                                                               GLsizei count,
                                                               const GLfloat *value);
typedef GLvoid              (APIENTRY *PFNGLUNIFORM4FVARBPROC)(GLint location,
                                                               GLsizei count,
                                                               const GLfloat *value);
typedef GLvoid              (APIENTRY *PFNGLUNIFORM1IVARBPROC)(GLint location,
                                                               GLsizei count,
                                                               const GLint *value);
typedef GLvoid              (APIENTRY *PFNGLUNIFORM2IVARBPROC)(GLint location,
                                                               GLsizei count,
                                                               const GLint *value);
typedef GLvoid              (APIENTRY *PFNGLUNIFORM3IVARBPROC)(GLint location,
                                                               GLsizei count,
                                                               const GLint *value);
typedef GLvoid              (APIENTRY *PFNGLUNIFORM4IVARBPROC)(GLint location,
                                                               GLsizei count,
                                                               const GLint *value);
typedef GLvoid              (APIENTRY *PFNGLUNIFORMMATRIX2FVARBPROC)(GLint location,
                                                                     GLsizei count,
                                                                     GLboolean transpose,
                                                                     GLfloat *value);
typedef GLvoid              (APIENTRY *PFNGLUNIFORMMATRIX3FVARBPROC)(GLint location,
                                                                     GLsizei count,
                                                                     GLboolean transpose,
                                                                     GLfloat *value);
typedef GLvoid              (APIENTRY *PFNGLUNIFORMMATRIX4FVARBPROC)(GLint location,
                                                                     GLsizei count,
                                                                     GLboolean transpose,
                                                                     GLfloat *value);
typedef GLvoid              (APIENTRY *PFNGLGETOBJECTPARAMETERFVARBPROC)(GLhandleARB obj,
                                                                         GLenum pname,
                                                                         GLfloat *params);
typedef GLvoid              (APIENTRY *PFNGLGETOBJECTPARAMETERIVARBPROC)(GLhandleARB obj,
                                                                         GLenum pname,
                                                                         GLint *params);
typedef GLvoid              (APIENTRY *PFNGLGETINFOLOGARBPROC)(GLhandleARB obj,
                                                               GLsizei maxLength,
                                                               GLsizei *length,
                                                               GLcharARB *infoLog);
typedef GLvoid              (APIENTRY *PFNGLGETATTACHEDOBJECTSARBPROC)(GLhandleARB containerObj,
                                                                       GLsizei maxCount,
                                                                       GLsizei *count,
                                                                       GLhandleARB *obj);
typedef GLint               (APIENTRY *PFNGLGETUNIFORMLOCATIONARBPROC)(GLhandleARB programObj,
                                                                       const GLcharARB *name);
typedef GLvoid              (APIENTRY *PFNGLGETACTIVEUNIFORMARBPROC)(GLhandleARB programObj,
                                                                     GLuint index,
                                                                     GLsizei maxLength,
                                                                     GLsizei *length,
                                                                     GLint *size,
                                                                     GLenum *type,
                                                                     GLcharARB *name);
typedef GLvoid              (APIENTRY *PFNGLGETUNIFORMFVARBPROC)(GLhandleARB programObj,
                                                                 GLint location,
                                                                 GLfloat *params);
typedef GLvoid              (APIENTRY *PFNGLGETUNIFORMIVARBPROC)(GLhandleARB programObj,
                                                                 GLint location,
                                                                 GLint *params);
typedef GLvoid              (APIENTRY *PFNGLGETSHADERSOURCEARBPROC)(GLhandleARB obj,
                                                                    GLsizei maxLength,
                                                                    GLsizei *length,
                                                                    GLcharARB *source);

#endif /* GL_ARB_shader_objects */

/*
** GL_ARB_shading_language_100
**
** Support
**   Rage 128     based  : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_shading_language_100
#define GL_ARB_shading_language_100                 1

#define GL_SHADING_LANGUAGE_VERSION_ARB             0x8B8C

#endif /* GL_ARB_shading_language_100 */

/*
** GL_ARB_shadow
**
** Support:
**   Rage 128      based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/

#ifndef GL_ARB_shadow
#define GL_ARB_shadow                   1

/*
** GL_ARB_shadow
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/

#define GL_TEXTURE_COMPARE_MODE_ARB     0x884C
#define GL_TEXTURE_COMPARE_FUNC_ARB     0X884D
#define GL_COMPARE_R_TO_TEXTURE_ARB     0x884E

#endif  /* GL_ARB_shadow */

/*
** GL_ARB_shadow_ambient
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_shadow_ambient
#define GL_ARB_shadow_ambient             1

#define GL_TEXTURE_COMPARE_FAIL_VALUE_ARB 0x80BF

#endif /* GL_ARB_shadow_ambient */

/*
** GL_ARB_texture_border_clamp
**
** Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_texture_border_clamp
#define GL_ARB_texture_border_clamp 1

#define GL_CLAMP_TO_BORDER_ARB  0x812D

#endif /* GL_ARB_texture_border_clamp */

/*
** GL_ARB_texture_compression
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_texture_compression
#define GL_ARB_texture_compression 1

#define GL_COMPRESSED_ALPHA_ARB               0x84E9
#define GL_COMPRESSED_LUMINANCE_ARB           0x84EA
#define GL_COMPRESSED_LUMINANCE_ALPHA_ARB     0x84EB
#define GL_COMPRESSED_INTENSITY_ARB           0x84EC
#define GL_COMPRESSED_RGB_ARB                 0x84ED
#define GL_COMPRESSED_RGBA_ARB                0x84EE
#define GL_TEXTURE_COMPRESSION_HINT_ARB       0x84EF
#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB  0x86A0
#define GL_TEXTURE_COMPRESSED_ARB             0x86A1
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB 0x86A2
#define GL_COMPRESSED_TEXTURE_FORMATS_ARB     0x86A3

typedef GLvoid (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE3DARBPROC)(GLenum target, GLint level,
                                                             GLenum internalFormat, GLsizei width,
                                                             GLsizei height, GLsizei depth,
                                                             GLint border, GLsizei imageSize,
                                                             const GLvoid *data);
typedef GLvoid (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)(GLenum target, GLint level,
                                                             GLenum internalFormat, GLsizei width,
                                                             GLsizei height, GLint border,
                                                             GLsizei imageSize, const GLvoid *data);
typedef GLvoid (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE1DARBPROC)(GLenum target, GLint level,
                                                             GLenum internalFormat, GLsizei width,
                                                             GLint border, GLsizei imageSize,
                                                             const GLvoid *data);
typedef GLvoid (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC)(GLenum target, GLint level,
                                                                GLint xoffset, GLint yoffset,
                                                                GLint zoffset, GLsizei width,
                                                                GLsizei height, GLsizei depth,
                                                                GLenum format, GLsizei imageSize,
                                                                const GLvoid *data);
typedef GLvoid (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC)(GLenum target, GLint level,
                                                                GLint xoffset, GLint yoffset,
                                                                GLsizei width, GLsizei height,
                                                                GLenum format, GLsizei imageSize,
                                                                const GLvoid *data);
typedef GLvoid (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC)(GLenum target, GLint level,
                                                                GLint xoffset, GLsizei width,
                                                                GLenum format, GLsizei imageSize,
                                                                const GLvoid *data);
typedef GLvoid (APIENTRY * PFNGLGETCOMPRESSEDTEXIMAGEARBPROC)(GLenum target, GLint lod,
                                                              GLvoid *img);


#endif /* GL_ARB_texture_compression */

/*
** GL_ARB_texture_cube_map
**
**  Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_texture_cube_map
#define GL_ARB_texture_cube_map 1

#define GL_NORMAL_MAP_ARB                  0x8511
#define GL_REFLECTION_MAP_ARB              0x8512
#define GL_TEXTURE_CUBE_MAP_ARB            0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP_ARB    0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB 0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB 0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB 0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB 0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB 0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB 0x851A
#define GL_PROXY_TEXTURE_CUBE_MAP_ARB      0x851B
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB   0x851C

#endif /* GL_ARB_texture_cube_map */

/*
** GL_ARB_texture_env_add
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_texture_env_add
#define GL_ARB_texture_env_add  1

#endif /* GL_ARB_texture_env_add */

/*
** GL_ARB_texture_env_combine
**
**  Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/

#ifndef GL_ARB_texture_env_combine
#define GL_ARB_texture_env_combine  1

#define GL_COMBINE_ARB        0x8570
#define GL_COMBINE_RGB_ARB    0x8571
#define GL_COMBINE_ALPHA_ARB  0x8572
#define GL_SOURCE0_RGB_ARB    0x8580
#define GL_SOURCE1_RGB_ARB    0x8581
#define GL_SOURCE2_RGB_ARB    0x8582
#define GL_SOURCE0_ALPHA_ARB  0x8588
#define GL_SOURCE1_ALPHA_ARB  0x8589
#define GL_SOURCE2_ALPHA_ARB  0x858A
#define GL_OPERAND0_RGB_ARB   0x8590
#define GL_OPERAND1_RGB_ARB   0x8591
#define GL_OPERAND2_RGB_ARB   0x8592
#define GL_OPERAND0_ALPHA_ARB 0x8598
#define GL_OPERAND1_ALPHA_ARB 0x8599
#define GL_OPERAND2_ALPHA_ARB 0x859A
#define GL_RGB_SCALE_ARB      0x8573
#define GL_ADD_SIGNED_ARB     0x8574
#define GL_INTERPOLATE_ARB    0x8575
#define GL_CONSTANT_ARB       0x8576
#define GL_PRIMARY_COLOR_ARB  0x8577
#define GL_PREVIOUS_ARB       0x8578
#define GL_SUBTRACT_ARB       0x84E7

#endif /* GL_ARB_texture_env_combine */

/*
** GL_ARB_texture_env_crossbar
**
**  Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_texture_env_crossbar
#define GL_ARB_texture_env_crossbar 1

#endif /* GL_ARB_texture_env_crossbar */

/*
** GL_ARB_texture_env_dot3
**
**  Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_texture_env_dot3
#define GL_ARB_texture_env_dot3 1

#define GL_DOT3_RGB_ARB  0x86AE
#define GL_DOT3_RGBA_ARB 0x86AF

#endif /* GL_ARB_texture_env_dot3 */

/*
** GL_ARB_texture_float
**
**  Support:
**   Rage 128      based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_texture_float
#define GL_ARB_texture_float 1

#define GL_TEXTURE_RED_TYPE_ARB             0x8C10
#define GL_TEXTURE_GREEN_TYPE_ARB           0x8C11
#define GL_TEXTURE_BLUE_TYPE_ARB            0x8C12
#define GL_TEXTURE_ALPHA_TYPE_ARB           0x8C13
#define GL_TEXTURE_LUMINANCE_TYPE_ARB       0x8C14
#define GL_TEXTURE_INTENSITY_TYPE_ARB       0x8C15
#define GL_TEXTURE_DEPTH_TYPE_ARB           0x8C16

#define GL_UNSIGNED_NORMALIZED_ARB          0x8C17

#define GL_RGBA32F_ARB                      0x8814
#define GL_RGB32F_ARB                       0x8815
#define GL_ALPHA32F_ARB                     0x8816
#define GL_INTENSITY32F_ARB                 0x8817
#define GL_LUMINANCE32F_ARB                 0x8818
#define GL_LUMINANCE_ALPHA32F_ARB           0x8819
#define GL_RGBA16F_ARB                      0x881A
#define GL_RGB16F_ARB                       0x881B
#define GL_ALPHA16F_ARB                     0x881C
#define GL_INTENSITY16F_ARB                 0x881D
#define GL_LUMINANCE16F_ARB                 0x881E
#define GL_LUMINANCE_ALPHA16F_ARB           0x881F

#endif /* GL_ARB_texture_float */

/*
** GL_ARB_texture_mirrored_repeat
**
**  Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_texture_mirrored_repeat
#define GL_ARB_texture_mirrored_repeat 1

#define GL_MIRRORED_REPEAT_ARB               0x8370

#endif /* GL_ARB_texture_mirrored_repeat */

/*
** GL_ARB_texture_non_power_of_two
**
**  Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_texture_non_power_of_two
#define GL_ARB_texture_non_power_of_two              1
#endif /* GL_ARB_texture_non_power_of_two */

/*
** GL_ARB_texture_rectangle
**
**  Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_texture_rectangle
#define GL_ARB_texture_rectangle            1

#define GL_TEXTURE_RECTANGLE_ARB            0x84F5
#define GL_TEXTURE_BINDING_RECTANGLE_ARB    0x84F6
#define GL_PROXY_TEXTURE_RECTANGLE_ARB      0x84F7
#define GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB   0x84F8

#endif /* GL_ARB_texture_rectangle */


/*
** GL_ARB_transpose_matrix
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_transpose_matrix
#define GL_ARB_transpose_matrix 1

#define GL_TRANSPOSE_MODELVIEW_MATRIX_ARB  0x84E3
#define GL_TRANSPOSE_PROJECTION_MATRIX_ARB 0x84E4
#define GL_TRANSPOSE_TEXTURE_MATRIX_ARB    0x84E5
#define GL_TRANSPOSE_COLOR_MATRIX_ARB      0x84E6

typedef GLvoid (APIENTRY * PFNGLLOADTRANSPOSEMATRIXFARBPROC)(const GLfloat m[16]);
typedef GLvoid (APIENTRY * PFNGLLOADTRANSPOSEMATRIXDARBPROC)(const GLdouble m[16]);
typedef GLvoid (APIENTRY * PFNGLMULTTRANSPOSEMATRIXFARBPROC)(const GLfloat m[16]);
typedef GLvoid (APIENTRY * PFNGLMULTTRANSPOSEMATRIXDARBPROC)(const GLdouble m[16]);

#endif /* GL_ARB_transpose_matrix */

/*
** GL_ARB_vertex_blend
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_vertex_blend
#define GL_ARB_vertex_blend         1

#define GL_MAX_VERTEX_UNITS_ARB     0x86A4
#define GL_ACTIVE_VERTEX_UNITS_ARB  0x86A5
#define GL_WEIGHT_SUM_UNITY_ARB     0x86A6
#define GL_VERTEX_BLEND_ARB         0x86A7
#define GL_CURRENT_WEIGHT_ARB       0x86A8
#define GL_WEIGHT_ARRAY_TYPE_ARB    0x86A9
#define GL_WEIGHT_ARRAY_STRIDE_ARB  0x86AA
#define GL_WEIGHT_ARRAY_SIZE_ARB    0x86AB
#define GL_WEIGHT_ARRAY_POINTER_ARB 0x86AC
#define GL_WEIGHT_ARRAY_ARB         0x86AD
#define GL_MODELVIEW0_ARB           0x1700
#define GL_MODELVIEW1_ARB           0x850a
#define GL_MODELVIEW2_ARB           0x8722
#define GL_MODELVIEW3_ARB           0x8723
#define GL_MODELVIEW4_ARB           0x8724
#define GL_MODELVIEW5_ARB           0x8725
#define GL_MODELVIEW6_ARB           0x8726
#define GL_MODELVIEW7_ARB           0x8727
#define GL_MODELVIEW8_ARB           0x8728
#define GL_MODELVIEW9_ARB           0x8729
#define GL_MODELVIEW10_ARB          0x872A
#define GL_MODELVIEW11_ARB          0x872B
#define GL_MODELVIEW12_ARB          0x872C
#define GL_MODELVIEW13_ARB          0x872D
#define GL_MODELVIEW14_ARB          0x872E
#define GL_MODELVIEW15_ARB          0x872F
#define GL_MODELVIEW16_ARB          0x8730
#define GL_MODELVIEW17_ARB          0x8731
#define GL_MODELVIEW18_ARB          0x8732
#define GL_MODELVIEW19_ARB          0x8733
#define GL_MODELVIEW20_ARB          0x8734
#define GL_MODELVIEW21_ARB          0x8735
#define GL_MODELVIEW22_ARB          0x8736
#define GL_MODELVIEW23_ARB          0x8737
#define GL_MODELVIEW24_ARB          0x8738
#define GL_MODELVIEW25_ARB          0x8739
#define GL_MODELVIEW26_ARB          0x873A
#define GL_MODELVIEW27_ARB          0x873B
#define GL_MODELVIEW28_ARB          0x873C
#define GL_MODELVIEW29_ARB          0x873D
#define GL_MODELVIEW30_ARB          0x873E
#define GL_MODELVIEW31_ARB          0x873F

typedef GLvoid (APIENTRY *PFNGLWEIGHTBVARBPROC)(GLint size, GLbyte *weights);
typedef GLvoid (APIENTRY *PFNGLWEIGHTSVARBPROC)(GLint size, GLshort *weights);
typedef GLvoid (APIENTRY *PFNGLWEIGHTIVARBPROC)(GLint size, GLint *weights);
typedef GLvoid (APIENTRY *PFNGLWEIGHTFVARBPROC)(GLint size, GLfloat *weights);
typedef GLvoid (APIENTRY *PFNGLWEIGHTDVARBPROC)(GLint size, GLdouble *weights);
typedef GLvoid (APIENTRY *PFNGLWEIGHTUBVARBPROC)(GLint size, GLubyte *weights);
typedef GLvoid (APIENTRY *PFNGLWEIGHTUSVARBPROC)(GLint size, GLushort *weights);
typedef GLvoid (APIENTRY *PFNGLWEIGHTUIVARBPROC)(GLint size, GLuint *weights);
typedef GLvoid (APIENTRY *PFNGLWEIGHTPOINTERARBPROC)(GLint size, GLenum type, GLsizei stride, GLvoid *pointer);
typedef GLvoid (APIENTRY *PFNGLVERTEXBLENDARBPROC)(GLint count);

#endif /* GL_ARB_vertex_blend */

/*
** GL_ARB_vertex_buffer_object
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_vertex_buffer_object
#define GL_ARB_vertex_buffer_object                     1

typedef INT_PTR GLintptrARB;
typedef INT_PTR GLsizeiptrARB;

#define GL_ARRAY_BUFFER_ARB                             0x8892
#define GL_ELEMENT_ARRAY_BUFFER_ARB                     0x8893
#define GL_ARRAY_BUFFER_BINDING_ARB                     0x8894
#define GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB             0x8895
#define GL_VERTEX_ARRAY_BUFFER_BINDING_ARB              0x8896
#define GL_NORMAL_ARRAY_BUFFER_BINDING_ARB              0x8897
#define GL_COLOR_ARRAY_BUFFER_BINDING_ARB               0x8898
#define GL_INDEX_ARRAY_BUFFER_BINDING_ARB               0x8899
#define GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING_ARB       0x889A
#define GL_EDGE_FLAG_ARRAY_BUFFER_BINDING_ARB           0x889B
#define GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING_ARB     0x889C
#define GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING_ARB      0x889D
#define GL_WEIGHT_ARRAY_BUFFER_BINDING_ARB              0x889E
#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB       0x889F
#define GL_STREAM_DRAW_ARB                              0x88E0
#define GL_STREAM_READ_ARB                              0x88E1
#define GL_STREAM_COPY_ARB                              0x88E2
#define GL_STATIC_DRAW_ARB                              0x88E4
#define GL_STATIC_READ_ARB                              0x88E5
#define GL_STATIC_COPY_ARB                              0x88E6
#define GL_DYNAMIC_DRAW_ARB                             0x88E8
#define GL_DYNAMIC_READ_ARB                             0x88E9
#define GL_DYNAMIC_COPY_ARB                             0x88EA
#define GL_READ_ONLY_ARB                                0x88B8
#define GL_WRITE_ONLY_ARB                               0x88B9
#define GL_READ_WRITE_ARB                               0x88BA
#define GL_BUFFER_SIZE_ARB                              0x8764
#define GL_BUFFER_USAGE_ARB                             0x8765
#define GL_BUFFER_ACCESS_ARB                            0x88BB
#define GL_BUFFER_ACCESS_FLAGS_ARB                      0x911F
#define GL_BUFFER_MAPPED_ARB                            0x88BC
#define GL_BUFFER_MAP_LENGTH_ARB                        0x9120
#define GL_BUFFER_MAP_OFFSET_ARB                        0x9121
#define GL_BUFFER_MAP_POINTER_ARB                       0x88BD

typedef GLvoid      (APIENTRY * PFNGLBINDBUFFERARBPROC) (GLenum target, GLuint buffer);
typedef GLvoid      (APIENTRY * PFNGLDELETEBUFFERSARBPROC) (GLsizei n, const GLuint *buffers);
typedef GLvoid      (APIENTRY * PFNGLGENBUFFERSARBPROC) (GLsizei n, GLuint *buffers);
typedef GLboolean   (APIENTRY * PFNGLISBUFFERARBPROC) (GLuint buffer);
typedef GLvoid      (APIENTRY * PFNGLBUFFERDATAARBPROC) (GLenum target, GLsizeiptrARB size, const GLvoid *data, GLenum usage);
typedef GLvoid      (APIENTRY * PFNGLBUFFERSUBDATAARBPROC) (GLenum target, GLintptrARB offset, GLsizeiptrARB size, const GLvoid *data);
typedef GLvoid      (APIENTRY * PFNGLGETBUFFERSUBDATAARBPROC) (GLenum target, GLintptrARB offset, GLsizeiptrARB size, GLvoid *data);
typedef GLvoid *    (APIENTRY * PFNGLMAPBUFFERARBPROC) (GLenum target, GLenum access);
typedef GLboolean   (APIENTRY * PFNGLUNMAPBUFFERARBPROC) (GLenum target);
typedef GLvoid      (APIENTRY * PFNGLGETBUFFERPARAMETERIVARBPROC) (GLenum target, GLenum pname, GLint *params);
typedef GLvoid      (APIENTRY * PFNGLGETBUFFERPOINTERVARBPROC) (GLenum target, GLenum pname, GLvoid **params);

#endif /* GL_ARB_vertex_buffer_object*/

/*
** GL_ARB_vertex_program
**
**  Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_vertex_program
#define GL_ARB_vertex_program 1

#define GL_VERTEX_PROGRAM_ARB                               0x8620
#define GL_VERTEX_PROGRAM_POINT_SIZE_ARB                    0x8642
#define GL_VERTEX_PROGRAM_TWO_SIDE_ARB                      0x8643
#define GL_COLOR_SUM_ARB                                    0x8458
#define GL_PROGRAM_FORMAT_ASCII_ARB                         0x8875
#define GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB                  0x8622
#define GL_VERTEX_ATTRIB_ARRAY_SIZE_ARB                     0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE_ARB                   0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE_ARB                     0x8625
#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED_ARB               0x886A
#define GL_CURRENT_VERTEX_ATTRIB_ARB                        0x8626
#define GL_VERTEX_ATTRIB_ARRAY_POINTER_ARB                  0x8645
#define GL_PROGRAM_LENGTH_ARB                               0x8627
#define GL_PROGRAM_FORMAT_ARB                               0x8876
#define GL_PROGRAM_BINDING_ARB                              0x8677
#define GL_PROGRAM_INSTRUCTIONS_ARB                         0x88A0
#define GL_MAX_PROGRAM_INSTRUCTIONS_ARB                     0x88A1
#define GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB                  0x88A2
#define GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB              0x88A3
#define GL_PROGRAM_TEMPORARIES_ARB                          0x88A4
#define GL_MAX_PROGRAM_TEMPORARIES_ARB                      0x88A5
#define GL_PROGRAM_NATIVE_TEMPORARIES_ARB                   0x88A6
#define GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB               0x88A7
#define GL_PROGRAM_PARAMETERS_ARB                           0x88A8
#define GL_MAX_PROGRAM_PARAMETERS_ARB                       0x88A9
#define GL_PROGRAM_NATIVE_PARAMETERS_ARB                    0x88AA
#define GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB                0x88AB
#define GL_PROGRAM_ATTRIBS_ARB                              0x88AC
#define GL_MAX_PROGRAM_ATTRIBS_ARB                          0x88AD
#define GL_PROGRAM_NATIVE_ATTRIBS_ARB                       0x88AE
#define GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB                   0x88AF
#define GL_PROGRAM_ADDRESS_REGISTERS_ARB                    0x88B0
#define GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB                0x88B1
#define GL_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB             0x88B2
#define GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB         0x88B3
#define GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB                 0x88B4
#define GL_MAX_PROGRAM_ENV_PARAMETERS_ARB                   0x88B5
#define GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB                  0x88B6
#define GL_PROGRAM_STRING_ARB                               0x8628
#define GL_PROGRAM_ERROR_POSITION_ARB                       0x864B
#define GL_CURRENT_MATRIX_ARB                               0x8641
#define GL_TRANSPOSE_CURRENT_MATRIX_ARB                     0x88B7
#define GL_CURRENT_MATRIX_STACK_DEPTH_ARB                   0x8640
#define GL_MAX_VERTEX_ATTRIBS_ARB                           0x8869
#define GL_MAX_PROGRAM_MATRICES_ARB                         0x862F
#define GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB               0x862E
#define GL_PROGRAM_ERROR_STRING_ARB                         0x8874
#define GL_MATRIX0_ARB                                      0x88C0
#define GL_MATRIX1_ARB                                      0x88C1
#define GL_MATRIX2_ARB                                      0x88C2
#define GL_MATRIX3_ARB                                      0x88C3
#define GL_MATRIX4_ARB                                      0x88C4
#define GL_MATRIX5_ARB                                      0x88C5
#define GL_MATRIX6_ARB                                      0x88C6
#define GL_MATRIX7_ARB                                      0x88C7
#define GL_MATRIX8_ARB                                      0x88C8
#define GL_MATRIX9_ARB                                      0x88C9
#define GL_MATRIX10_ARB                                     0x88CA
#define GL_MATRIX11_ARB                                     0x88CB
#define GL_MATRIX12_ARB                                     0x88CC
#define GL_MATRIX13_ARB                                     0x88CD
#define GL_MATRIX14_ARB                                     0x88CE
#define GL_MATRIX15_ARB                                     0x88CF
#define GL_MATRIX16_ARB                                     0x88D0
#define GL_MATRIX17_ARB                                     0x88D1
#define GL_MATRIX18_ARB                                     0x88D2
#define GL_MATRIX19_ARB                                     0x88D3
#define GL_MATRIX20_ARB                                     0x88D4
#define GL_MATRIX21_ARB                                     0x88D5
#define GL_MATRIX22_ARB                                     0x88D6
#define GL_MATRIX23_ARB                                     0x88D7
#define GL_MATRIX24_ARB                                     0x88D8
#define GL_MATRIX25_ARB                                     0x88D9
#define GL_MATRIX26_ARB                                     0x88DA
#define GL_MATRIX27_ARB                                     0x88DB
#define GL_MATRIX28_ARB                                     0x88DC
#define GL_MATRIX29_ARB                                     0x88DD
#define GL_MATRIX30_ARB                                     0x88DE
#define GL_MATRIX31_ARB                                     0x88DF

typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB1SARBPROC)(GLuint index, GLshort x);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB1FARBPROC)(GLuint index, GLfloat x);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB1DARBPROC)(GLuint index, GLdouble x);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB2SARBPROC)(GLuint index, GLshort x, GLshort y);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB2FARBPROC)(GLuint index, GLfloat x, GLfloat y);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB2DARBPROC)(GLuint index, GLdouble x, GLdouble y);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB3SARBPROC)(GLuint index, GLshort x, GLshort y, GLshort z);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB3FARBPROC)(GLuint index, GLfloat x, GLfloat y, GLfloat z);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB3DARBPROC)(GLuint index, GLdouble x, GLdouble y, GLdouble z);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB4SARBPROC)(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB4FARBPROC)(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB4DARBPROC)(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB4NUBARBPROC)(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB1SVARBPROC)(GLuint index, const GLshort *v);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB1FVARBPROC)(GLuint index, const GLfloat *v);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB1DVARBPROC)(GLuint index, const GLdouble *v);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB2SVARBPROC)(GLuint index, const GLshort *v);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB2FVARBPROC)(GLuint index, const GLfloat *v);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB2DVARBPROC)(GLuint index, const GLdouble *v);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB3SVARBPROC)(GLuint index, const GLshort *v);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB3FVARBPROC)(GLuint index, const GLfloat *v);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB3DVARBPROC)(GLuint index, const GLdouble *v);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB4BVARBPROC)(GLuint index, const GLbyte *v);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB4SVARBPROC)(GLuint index, const GLshort *v);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB4IVARBPROC)(GLuint index, const GLint *v);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB4UBVARBPROC)(GLuint index, const GLubyte *v);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB4USVARBPROC)(GLuint index, const GLushort *v);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB4UIVARBPROC)(GLuint index, const GLuint *v);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB4FVARBPROC)(GLuint index, const GLfloat *v);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB4DVARBPROC)(GLuint index, const GLdouble *v);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB4NBVARBPROC)(GLuint index, const GLbyte *v);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB4NSVARBPROC)(GLuint index, const GLshort *v);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB4NIVARBPROC)(GLuint index, const GLint *v);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB4NUBVARBPROC)(GLuint index, const GLubyte *v);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB4NUSVARBPROC)(GLuint index, const GLushort *v);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIB4NUIVARBPROC)(GLuint index, const GLuint *v);
typedef GLvoid (APIENTRY *PFNGLVERTEXATTRIBPOINTERARBPROC)(GLuint index, GLint size, GLenum type,
                                                           GLboolean normalized, GLsizei stride,
                                                           const GLvoid *pointer);
typedef GLvoid (APIENTRY *PFNGLENABLEVERTEXATTRIBARRAYARBPROC)(GLuint index);
typedef GLvoid (APIENTRY *PFNGLDISABLEVERTEXATTRIBARRAYARBPROC)(GLuint index);
typedef GLvoid (APIENTRY *PFNGLPROGRAMSTRINGARBPROC)(GLenum target, GLenum format, GLsizei len, const GLvoid *string);
typedef GLvoid (APIENTRY *PFNGLBINDPROGRAMARBPROC)(GLenum target, GLuint program);
typedef GLvoid (APIENTRY *PFNGLDELETEPROGRAMSARBPROC)(GLsizei n, const GLuint *programs);
typedef GLvoid (APIENTRY *PFNGLGENPROGRAMSARBPROC)(GLsizei n, GLuint *programs);
typedef GLvoid (APIENTRY *PFNGLPROGRAMENVPARAMETER4FARBPROC)(GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef GLvoid (APIENTRY *PFNGLPROGRAMENVPARAMETER4DARBPROC)(GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef GLvoid (APIENTRY *PFNGLPROGRAMENVPARAMETER4FVARBPROC)(GLenum target, GLuint index, const GLfloat *params);
typedef GLvoid (APIENTRY *PFNGLPROGRAMENVPARAMETER4DVARBPROC)(GLenum target, GLuint index, const GLdouble *params);
typedef GLvoid (APIENTRY *PFNGLPROGRAMLOCALPARAMETER4FARBPROC)(GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef GLvoid (APIENTRY *PFNGLPROGRAMLOCALPARAMETER4DARBPROC)(GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef GLvoid (APIENTRY *PFNGLPROGRAMLOCALPARAMETER4FVARBPROC)(GLenum target, GLuint index, const GLfloat *params);
typedef GLvoid (APIENTRY *PFNGLPROGRAMLOCALPARAMETER4DVARBPROC)(GLenum target, GLuint index, const GLdouble *params);
typedef GLvoid (APIENTRY *PFNGLGETPROGRAMENVPARAMETERFVARBPROC)(GLenum target, GLuint index, GLfloat *params);
typedef GLvoid (APIENTRY *PFNGLGETPROGRAMENVPARAMETERDVARBPROC)(GLenum target, GLuint index, GLdouble *params);
typedef GLvoid (APIENTRY *PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC)(GLenum target, GLuint index, GLfloat *params);
typedef GLvoid (APIENTRY *PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC)(GLenum target, GLuint index, GLdouble *params);
typedef GLvoid (APIENTRY *PFNGLGETPROGRAMIVARBPROC)(GLenum target, GLenum pname, GLint *params);
typedef GLvoid (APIENTRY *PFNGLGETPROGRAMSTRINGARBPROC)(GLenum target, GLenum pname, GLvoid *string);
typedef GLvoid (APIENTRY *PFNGLGETVERTEXATTRIBDVARBPROC)(GLuint index, GLenum pname, GLdouble *params);
typedef GLvoid (APIENTRY *PFNGLGETVERTEXATTRIBFVARBPROC)(GLuint index, GLenum pname, GLfloat *params);
typedef GLvoid (APIENTRY *PFNGLGETVERTEXATTRIBIVARBPROC)(GLuint index, GLenum pname, GLint *params);
typedef GLvoid (APIENTRY *PFNGLGETVERTEXATTRIBPOINTERVARBPROC)(GLuint index, GLenum pname, GLvoid **pointer);
typedef GLboolean (APIENTRY *PFNGLISPROGRAMARBPROC)(GLuint program);

#endif /* GL_ARB_vertex_program */

/*
** GL_ARB_vertex_shader
**
** Support
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_vertex_shader
#define GL_ARB_vertex_shader                        1

#define GL_VERTEX_SHADER_ARB                        0x8B31

#define GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB        0x8B4A
#define GL_MAX_VARYING_FLOATS_ARB                   0x8B4B
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB       0x8B4C
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB     0x8B4D

#define GL_OBJECT_ACTIVE_ATTRIBUTES_ARB             0x8B89
#define GL_OBJECT_ACTIVE_ATTRIBUTE_MAX_LENGTH_ARB   0x8B8A



typedef GLvoid      (APIENTRY *PFNGLBINDATTRIBLOCATIONARBPROC)(GLhandleARB programObj,
                                                               GLuint index,
                                                               const GLcharARB *name);
typedef GLvoid      (APIENTRY *PFNGLGETACTIVEATTRIBARBPROC)(GLhandleARB programObj,
                                                            GLuint index,
                                                            GLsizei maxLength,
                                                            GLsizei *length,
                                                            GLint *size,
                                                            GLenum *type,
                                                            GLcharARB *name);
typedef GLint       (APIENTRY *PFNGLGETATTRIBLOCATIONARBPROC)(GLhandleARB programObj,
                                                              const GLcharARB *name);

#endif /* GL_ARB_vertex_shader */

/*
** GL_ARB_window_pos
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ARB_window_pos
#define GL_ARB_window_pos   1

typedef GLvoid (APIENTRY * PFNGLWINDOWPOS2DARBPROC) (double x, double y);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS2FARBPROC) (float x, float y);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS2IARBPROC) (int x, int y);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS2SARBPROC) (short x, short y);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS2IVARBPROC) (const int *p);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS2SVARBPROC) (const short *p);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS2FVARBPROC) (const float *p);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS2DVARBPROC) (const double *p);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS3IARBPROC) (int x, int y, int z);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS3SARBPROC) (short x, short y, short z);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS3FARBPROC) (float x, float y, float z);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS3DARBPROC) (double x, double y, double z);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS3IVARBPROC) (const int *p);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS3SVARBPROC) (const short *p);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS3FVARBPROC) (const float *p);
typedef GLvoid (APIENTRY * PFNGLWINDOWPOS3DVARBPROC) (const double *p);

#endif /* GL_ARB_window_pos */

/*
** GL_ARB_draw_instanced
**
**  Support:
**   Rage   128     based : Not Supported
**   Radeon 7000+   based : Not Supported
**   Radeon 8500+   based : Not Supported
**   Radeon 9500+   based : Not Supported
**   Radeon X800+   based : Not Supported
**   Radeon X1000+  based : Not Supported
**   Radeon HD2000+ based : Supported
**   Radeon HD3000+ based : Supported
**   Radeon HD4000+ based : Supported
*/

#ifndef GL_ARB_draw_instanced
#define GL_ARB_draw_instanced 1

typedef GLvoid (APIENTRY * PFNGLDRAWARRAYSINSTANCEDARBPROC) (GLenum mode, GLint first, GLsizei count, GLsizei primcount);
typedef GLvoid (APIENTRY * PFNGLDRAWELEMENTSINSTANCEDARBPROC) (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei primcount);

#endif

/*
** GL_ARB_instanced_arrays
**
**  Support:
**   Rage   128     based : Not Supported
**   Radeon 7000+   based : Not Supported
**   Radeon 8500+   based : Not Supported
**   Radeon 9500+   based : Not Supported
**   Radeon X800+   based : Not Supported
**   Radeon X1000+  based : Not Supported
**   Radeon HD2000+ based : Supported
**   Radeon HD3000+ based : Supported
**   Radeon HD4000+ based : Supported
*/

#ifndef GL_ARB_instanced_arrays
#define GL_ARB_instanced_arrays 1

#define GL_VERTEX_ATTRIB_ARRAY_DIVISOR_ARB          0x88FE

typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBDIVISORARBPROC)(GLuint index, GLuint divisor);

#endif

/*
** GL_ARB_texture_buffer_object
**
**  Support:
**   Rage   128     based : Not Supported
**   Radeon 7000+   based : Not Supported
**   Radeon 8500+   based : Not Supported
**   Radeon 9500+   based : Not Supported
**   Radeon X800+   based : Not Supported
**   Radeon X1000+  based : Not Supported
**   Radeon HD2000+ based : Supported
**   Radeon HD3000+ based : Supported
**   Radeon HD4000+ based : Supported
*/

#ifndef GL_ARB_texture_buffer_object
#define GL_ARB_texture_buffer_object 1

#define GL_TEXTURE_BUFFER_ARB                              0x8C2A

#define GL_MAX_TEXTURE_BUFFER_SIZE_ARB                     0x8C2B
#define GL_TEXTURE_BINDING_BUFFER_ARB                      0x8C2C
#define GL_TEXTURE_BUFFER_DATA_STORE_BINDING_ARB           0x8C2D
#define GL_TEXTURE_BUFFER_FORMAT_ARB                       0x8C2E

typedef GLvoid (APIENTRY * PFNGLTEXBUFFERARBPROC)(GLenum target, GLenum internalformat, GLuint buffer);

#endif


/*
** GL_ARB_uniform_buffer_object
**
**  Support:
**   Rage   128     based : Not Supported
**   Radeon 7000+   based : Not Supported
**   Radeon 8500+   based : Not Supported
**   Radeon 9500+   based : Not Supported
**   Radeon X800+   based : Not Supported
**   Radeon X1000+  based : Not Supported
**   Radeon HD2000+ based : Supported
**   Radeon HD3000+ based : Supported
**   Radeon HD4000+ based : Supported
*/

#ifndef GL_ARB_uniform_buffer_object
#define GL_ARB_uniform_buffer_object 1

#define GL_UNIFORM_BUFFER_ARB                               0x8A11
#define GL_UNIFORM_BUFFER_BINDING_ARB                       0x8A28
#define GL_UNIFORM_BUFFER_START_ARB                         0x8A29
#define GL_UNIFORM_BUFFER_SIZE_ARB                          0x8A2A
#define GL_MAX_VERTEX_UNIFORM_BLOCKS_ARB                    0x8A2B
#define GL_MAX_GEOMETRY_UNIFORM_BLOCKS_ARB                  0x8A2C
#define GL_MAX_FRAGMENT_UNIFORM_BLOCKS_ARB                  0x8A2D
#define GL_MAX_COMBINED_UNIFORM_BLOCKS_ARB                  0x8A2E
#define GL_MAX_UNIFORM_BUFFER_BINDINGS_ARB                  0x8A2F
#define GL_MAX_UNIFORM_BLOCK_SIZE_ARB                       0x8A30
#define GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS_ARB       0x8A31
#define GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS_ARB     0x8A32
#define GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS_ARB     0x8A33
#define GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT_ARB              0x8A34
#define GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH_ARB         0x8A35
#define GL_ACTIVE_UNIFORM_BLOCKS_ARB                        0x8A36
#define GL_UNIFORM_TYPE_ARB                                 0x8A37
#define GL_UNIFORM_SIZE_ARB                                 0x8A38
#define GL_UNIFORM_NAME_LENGTH_ARB                          0x8A39
#define GL_UNIFORM_BLOCK_INDEX_ARB                          0x8A3A
#define GL_UNIFORM_OFFSET_ARB                               0x8A3B
#define GL_UNIFORM_ARRAY_STRIDE_ARB                         0x8A3C
#define GL_UNIFORM_MATRIX_STRIDE_ARB                        0x8A3D
#define GL_UNIFORM_IS_ROW_MAJOR_ARB                         0x8A3E
#define GL_UNIFORM_BLOCK_BINDING_ARB                        0x8A3F
#define GL_UNIFORM_BLOCK_DATA_SIZE_ARB                      0x8A40
#define GL_UNIFORM_BLOCK_NAME_LENGTH_ARB                    0x8A41
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS_ARB                0x8A42
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES_ARB         0x8A43
#define GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER_ARB    0x8A44
#define GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER_ARB  0x8A45
#define GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER_ARB  0x8A46
#define GL_INVALID_INDEX_ARB                                0xFFFFFFFFu

typedef GLvoid (APIENTRY * PFNGETUNIFORMINDICESARBPROC)(GLuint program, GLsizei uniformCount, const GLchar** uniformNames, GLuint* uniformIndices);
typedef GLvoid (APIENTRY * PFNGETACTIVEUNIFORMSIVARBPROC)(GLuint program, GLsizei uniformCount, const GLuint* uniformIndices, GLenum pname, GLint* params);
typedef GLvoid (APIENTRY * PFNGETACTIVEUNIFORMNAMEARBPROC)(GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformName);
typedef GLuint (APIENTRY * PFNGETUNIFORMBLOCKINDEXARBPROC)(GLuint program, const GLchar* uniformBlockName);
typedef GLvoid (APIENTRY * PFNGETACTIVEUNIFORMBLOCKIVARBPROC)(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params);
typedef GLvoid (APIENTRY * PFNGETACTIVEUNIFORMBLOCKNAMEARBPROC)(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformBlockName);
typedef GLvoid (APIENTRY * PFNUNIFORMBLOCKBINDINGARBPROC)(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);

#endif

/*
**  ATI EXTENSIONS
*/

/*
** GL_ATI_draw_buffers
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ATI_draw_buffers
#define GL_ATI_draw_buffers     1

#define GL_MAX_DRAW_BUFFERS_ATI 0x8824
#define GL_DRAW_BUFFER0_ATI     0x8825
#define GL_DRAW_BUFFER1_ATI     0x8826
#define GL_DRAW_BUFFER2_ATI     0x8827
#define GL_DRAW_BUFFER3_ATI     0x8828
#define GL_DRAW_BUFFER4_ATI     0x8829
#define GL_DRAW_BUFFER5_ATI     0x882A
#define GL_DRAW_BUFFER6_ATI     0x882B
#define GL_DRAW_BUFFER7_ATI     0x882C
#define GL_DRAW_BUFFER8_ATI     0x882D
#define GL_DRAW_BUFFER9_ATI     0x882E
#define GL_DRAW_BUFFER10_ATI    0x882F
#define GL_DRAW_BUFFER11_ATI    0x8830
#define GL_DRAW_BUFFER12_ATI    0x8831
#define GL_DRAW_BUFFER13_ATI    0x8832
#define GL_DRAW_BUFFER14_ATI    0x8833
#define GL_DRAW_BUFFER15_ATI    0x8834

typedef GLvoid (APIENTRY * PFNGLDRAWBUFFERSATIPROC) (GLsizei n, const GLenum *bufs);
#endif

/*
** GL_ATI_element_array
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ATI_element_array
#define GL_ATI_element_array            1

#define GL_ELEMENT_ARRAY_ATI            0x8768

#define GL_ELEMENT_ARRAY_TYPE_ATI       0x8769
#define GL_ELEMENT_ARRAY_POINTER_ATI    0x876A

typedef GLvoid (APIENTRY * PFNGLELEMENTPOINTERATIPROC)(GLenum type, const GLvoid *pointer);
typedef GLvoid (APIENTRY * PFNGLDRAWELEMENTARRAYATIPROC)(GLenum mode, GLsizei count);
typedef GLvoid (APIENTRY * PFNGLDRAWRANGEELEMENTARRAYATIPROC)(GLenum mode, GLuint start, GLuint end, GLsizei count);

#endif /* GL_ATI_element_array */

/*
** GL_ATI_envmap_bumpmap
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ATI_envmap_bumpmap
#define GL_ATI_envmap_bumpmap       1

#define GL_BUMP_ROT_MATRIX_ATI      0x8775
#define GL_BUMP_ROT_MATRIX_SIZE_ATI 0x8776
#define GL_BUMP_NUM_TEX_UNITS_ATI   0x8777
#define GL_BUMP_TEX_UNITS_ATI       0x8778
#define GL_DUDV_ATI                 0x8779
#define GL_DU8DV8_ATI               0x877A
#define GL_BUMP_ENVMAP_ATI          0x877B
#define GL_BUMP_TARGET_ATI          0x877C

typedef GLvoid (APIENTRY * PFNGLTEXBUMPPARAMETERIVATIPROC) (GLenum pname, GLint *param);
typedef GLvoid (APIENTRY * PFNGLTEXBUMPPARAMETERFVATIPROC) (GLenum pname, GLfloat *param);
typedef GLvoid (APIENTRY * PFNGLGETTEXBUMPPARAMETERIVATIPROC) (GLenum pname, GLint *param);
typedef GLvoid (APIENTRY * PFNGLGETTEXBUMPPARAMETERFVATIPROC) (GLenum pname, GLfloat *param);

#endif /* GL_ATI_envmap_bumpmap */

/*
** GL_ATI_fragment_shader
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ATI_fragment_shader
#define GL_ATI_fragment_shader                   1

#define GL_FRAGMENT_SHADER_ATI                   0x8920
#define GL_REG_0_ATI                             0x8921
#define GL_REG_1_ATI                             0x8922
#define GL_REG_2_ATI                             0x8923
#define GL_REG_3_ATI                             0x8924
#define GL_REG_4_ATI                             0x8925
#define GL_REG_5_ATI                             0x8926
#define GL_REG_6_ATI                             0x8927
#define GL_REG_7_ATI                             0x8928
#define GL_REG_8_ATI                             0x8929
#define GL_REG_9_ATI                             0x892A
#define GL_REG_10_ATI                            0x892B
#define GL_REG_11_ATI                            0x892C
#define GL_REG_12_ATI                            0x892D
#define GL_REG_13_ATI                            0x892E
#define GL_REG_14_ATI                            0x892F
#define GL_REG_15_ATI                            0x8930
#define GL_REG_16_ATI                            0x8931
#define GL_REG_17_ATI                            0x8932
#define GL_REG_18_ATI                            0x8933
#define GL_REG_19_ATI                            0x8934
#define GL_REG_20_ATI                            0x8935
#define GL_REG_21_ATI                            0x8936
#define GL_REG_22_ATI                            0x8937
#define GL_REG_23_ATI                            0x8938
#define GL_REG_24_ATI                            0x8939
#define GL_REG_25_ATI                            0x893A
#define GL_REG_26_ATI                            0x893B
#define GL_REG_27_ATI                            0x893C
#define GL_REG_28_ATI                            0x893D
#define GL_REG_29_ATI                            0x893E
#define GL_REG_30_ATI                            0x893F
#define GL_REG_31_ATI                            0x8940
#define GL_CON_0_ATI                             0x8941
#define GL_CON_1_ATI                             0x8942
#define GL_CON_2_ATI                             0x8943
#define GL_CON_3_ATI                             0x8944
#define GL_CON_4_ATI                             0x8945
#define GL_CON_5_ATI                             0x8946
#define GL_CON_6_ATI                             0x8947
#define GL_CON_7_ATI                             0x8948
#define GL_CON_8_ATI                             0x8949
#define GL_CON_9_ATI                             0x894A
#define GL_CON_10_ATI                            0x894B
#define GL_CON_11_ATI                            0x894C
#define GL_CON_12_ATI                            0x894D
#define GL_CON_13_ATI                            0x894E
#define GL_CON_14_ATI                            0x894F
#define GL_CON_15_ATI                            0x8950
#define GL_CON_16_ATI                            0x8951
#define GL_CON_17_ATI                            0x8952
#define GL_CON_18_ATI                            0x8953
#define GL_CON_19_ATI                            0x8954
#define GL_CON_20_ATI                            0x8955
#define GL_CON_21_ATI                            0x8956
#define GL_CON_22_ATI                            0x8957
#define GL_CON_23_ATI                            0x8958
#define GL_CON_24_ATI                            0x8959
#define GL_CON_25_ATI                            0x895A
#define GL_CON_26_ATI                            0x895B
#define GL_CON_27_ATI                            0x895C
#define GL_CON_28_ATI                            0x895D
#define GL_CON_29_ATI                            0x895E
#define GL_CON_30_ATI                            0x895F
#define GL_CON_31_ATI                            0x8960
#define GL_MOV_ATI                               0x8961
#define GL_ADD_ATI                               0x8963
#define GL_MUL_ATI                               0x8964
#define GL_SUB_ATI                               0x8965
#define GL_DOT3_ATI                              0x8966
#define GL_DOT4_ATI                              0x8967
#define GL_MAD_ATI                               0x8968
#define GL_LERP_ATI                              0x8969
#define GL_CND_ATI                               0x896A
#define GL_CND0_ATI                              0x896B
#define GL_DOT2_ADD_ATI                          0x896C
#define GL_SECONDARY_INTERPOLATOR_ATI            0x896D
#define GL_NUM_FRAGMENT_REGISTERS_ATI            0x896E
#define GL_NUM_FRAGMENT_CONSTANTS_ATI            0x896F
#define GL_NUM_PASSES_ATI                        0x8970
#define GL_NUM_INSTRUCTIONS_PER_PASS_ATI         0x8971
#define GL_NUM_INSTRUCTIONS_TOTAL_ATI            0x8972
#define GL_NUM_INPUT_INTERPOLATOR_COMPONENTS_ATI 0x8973
#define GL_NUM_LOOPBACK_COMPONENTS_ATI           0x8974
#define GL_COLOR_ALPHA_PAIRING_ATI               0x8975
#define GL_SWIZZLE_STR_ATI                       0x8976
#define GL_SWIZZLE_STQ_ATI                       0x8977
#define GL_SWIZZLE_STR_DR_ATI                    0x8978
#define GL_SWIZZLE_STQ_DQ_ATI                    0x8979
#define GL_SWIZZLE_STRQ_ATI                      0x897A
#define GL_SWIZZLE_STRQ_DQ_ATI                   0x897B
#define GL_RED_BIT_ATI                           0x00000001
#define GL_GREEN_BIT_ATI                         0x00000002
#define GL_BLUE_BIT_ATI                          0x00000004
#define GL_2X_BIT_ATI                            0x00000001
#define GL_4X_BIT_ATI                            0x00000002
#define GL_8X_BIT_ATI                            0x00000004
#define GL_HALF_BIT_ATI                          0x00000008
#define GL_QUARTER_BIT_ATI                       0x00000010
#define GL_EIGHTH_BIT_ATI                        0x00000020
#define GL_SATURATE_BIT_ATI                      0x00000040
#define GL_COMP_BIT_ATI                          0x00000002
#define GL_NEGATE_BIT_ATI                        0x00000004
#define GL_BIAS_BIT_ATI                          0x00000008

typedef GLuint (APIENTRY *PFNGLGENFRAGMENTSHADERSATIPROC)(GLuint range);
typedef GLvoid (APIENTRY *PFNGLBINDFRAGMENTSHADERATIPROC)(GLuint id);
typedef GLvoid (APIENTRY *PFNGLDELETEFRAGMENTSHADERATIPROC)(GLuint id);
typedef GLvoid (APIENTRY *PFNGLBEGINFRAGMENTSHADERATIPROC)(void);
typedef GLvoid (APIENTRY *PFNGLENDFRAGMENTSHADERATIPROC)(void);
typedef GLvoid (APIENTRY *PFNGLPASSTEXCOORDATIPROC)(GLuint dst, GLuint coord, GLenum swizzle);
typedef GLvoid (APIENTRY *PFNGLSAMPLEMAPATIPROC)(GLuint dst, GLuint interp, GLenum swizzle);
typedef GLvoid (APIENTRY *PFNGLCOLORFRAGMENTOP1ATIPROC)(GLenum op, GLuint dst, GLuint dstMask,
                                                        GLuint dstMod, GLuint arg1, GLuint arg1Rep,
                                                        GLuint arg1Mod);
typedef GLvoid (APIENTRY *PFNGLCOLORFRAGMENTOP2ATIPROC)(GLenum op, GLuint dst, GLuint dstMask,
                                                        GLuint dstMod, GLuint arg1, GLuint arg1Rep,
                                                        GLuint arg1Mod, GLuint arg2, GLuint arg2Rep,
                                                        GLuint arg2Mod);
typedef GLvoid (APIENTRY *PFNGLCOLORFRAGMENTOP3ATIPROC)(GLenum op, GLuint dst, GLuint dstMask,
                                                        GLuint dstMod, GLuint arg1, GLuint arg1Rep,
                                                        GLuint arg1Mod, GLuint arg2, GLuint arg2Rep,
                                                        GLuint arg2Mod, GLuint arg3, GLuint arg3Rep,
                                                        GLuint arg3Mod);
typedef GLvoid (APIENTRY *PFNGLALPHAFRAGMENTOP1ATIPROC)(GLenum op, GLuint dst, GLuint dstMod,
                                                        GLuint arg1, GLuint arg1Rep, GLuint arg1Mod);
typedef GLvoid (APIENTRY *PFNGLALPHAFRAGMENTOP2ATIPROC)(GLenum op, GLuint dst, GLuint dstMod,
                                                        GLuint arg1, GLuint arg1Rep, GLuint arg1Mod,
                                                        GLuint arg2, GLuint arg2Rep, GLuint arg2Mod);
typedef GLvoid (APIENTRY *PFNGLALPHAFRAGMENTOP3ATIPROC)(GLenum op, GLuint dst, GLuint dstMod,
                                                        GLuint arg1, GLuint arg1Rep, GLuint arg1Mod,
                                                        GLuint arg2, GLuint arg2Rep, GLuint arg2Mod,
                                                        GLuint arg3, GLuint arg3Rep, GLuint arg3Mod);
typedef GLvoid (APIENTRY *PFNGLSETFRAGMENTSHADERCONSTANTATIPROC)(GLuint dst, const GLfloat *value);

#endif /* GL_ATI_fragment_shader */

/*
** GL_ATI_map_object_buffer
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ATI_map_object_buffer
#define GL_ATI_map_object_buffer                1

typedef GLvoid *(APIENTRY * PFNGLMAPOBJECTBUFFERATIPROC)(GLuint buffer);
typedef GLvoid (APIENTRY * PFNGLUNMAPOBJECTBUFFERATIPROC)(GLuint buffer);

#endif /* GL_ATI_map_object_buffer */

/*
** GL_ATI_pn_triangles
**
** Support
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Not Supported
**   Radeon X800+  based : Not Supported
**   Radeon X1000+ based : Not Supported
**   Radeon X2000+ based : Not Supported
*/
#ifndef GL_ATI_pn_triangles
#define GL_ATI_pn_triangles                       1

#define GL_PN_TRIANGLES_ATI                       0x87F0
#define GL_MAX_PN_TRIANGLES_TESSELATION_LEVEL_ATI 0x87F1
#define GL_PN_TRIANGLES_POINT_MODE_ATI            0x87F2
#define GL_PN_TRIANGLES_NORMAL_MODE_ATI           0x87F3
#define GL_PN_TRIANGLES_TESSELATION_LEVEL_ATI     0x87F4
#define GL_PN_TRIANGLES_POINT_MODE_LINEAR_ATI     0x87F5
#define GL_PN_TRIANGLES_POINT_MODE_CUBIC_ATI      0x87F6
#define GL_PN_TRIANGLES_NORMAL_MODE_LINEAR_ATI    0x87F7
#define GL_PN_TRIANGLES_NORMAL_MODE_QUADRATIC_ATI 0x87F8

typedef GLvoid (APIENTRY *PFNGLPNTRIANGLESIATIPROC)(GLenum pname, GLint param);
typedef GLvoid (APIENTRY *PFNGLPNTRIANGLESFATIPROC)(GLenum pname, GLfloat param);

#endif /* GL_ATI_pn_triangles */

/*
** GL_ATI_separate_stencil
**
** Support
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ATI_separate_stencil
#define GL_ATI_separate_stencil                     1

#define GL_STENCIL_BACK_FUNC_ATI                    0x8800
#define GL_STENCIL_BACK_FAIL_ATI                    0x8801
#define GL_STENCIL_BACK_PASS_DEPTH_FAIL_ATI         0x8802
#define GL_STENCIL_BACK_PASS_DEPTH_PASS_ATI         0x8803

typedef GLvoid (APIENTRY *PFNGLSTENCILOPSEPARATEATIPROC)(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
typedef GLvoid (APIENTRY *PFNGLSTENCILFUNCSEPARATEATIPROC)(GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask);

#endif /* GL_ATI_separate_stencil */

/*
** GL_ATI_shader_texture_lod
**
** Support
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Not Supported
**   Radeon X800+  based : Not Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/

#ifndef GL_ATI_shader_texture_lod
#define GL_ATI_shader_texture_lod                 1
#endif /* GL_ATI_shader_texture_lod */

/*
** GL_ATI_texture_compression_3dc
**
**  Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Not Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ATI_texture_compression_3dc
#define GL_ATI_texture_compression_3dc  1

#define GL_COMPRESSED_LUMINANCE_ALPHA_3DC_ATI     0x8837

#endif /* GL_ATI_texture_compression_3dc */

/*
** GL_ATI_texture_float
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ATI_texture_float
#define GL_ATI_texture_float                1

#define GL_RGBA_FLOAT32_ATI                 0x8814
#define GL_RGB_FLOAT32_ATI                  0x8815
#define GL_ALPHA_FLOAT32_ATI                0x8816
#define GL_INTENSITY_FLOAT32_ATI            0x8817
#define GL_LUMINANCE_FLOAT32_ATI            0x8818
#define GL_LUMINANCE_ALPHA_FLOAT32_ATI      0x8819
#define GL_RGBA_FLOAT16_ATI                 0x881A
#define GL_RGB_FLOAT16_ATI                  0x881B
#define GL_ALPHA_FLOAT16_ATI                0x881C
#define GL_INTENSITY_FLOAT16_ATI            0x881D
#define GL_LUMINANCE_FLOAT16_ATI            0x881E
#define GL_LUMINANCE_ALPHA_FLOAT16_ATI      0x881F
#endif  /* GL_ATI_texture_float */

/*
** GL_ATI_texture_mirror_once
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ATI_texture_mirror_once
#define GL_ATI_texture_mirror_once  1

#define GL_MIRROR_CLAMP_ATI         0x8742
#define GL_MIRROR_CLAMP_TO_EDGE_ATI 0x8743

#endif /* GL_ATI_texture_mirror_once */

/*
** GL_ATI_vertex_array_object
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ATI_vertex_array_object
#define GL_ATI_vertex_array_object              1

#define GL_STATIC_ATI                           0x8760
#define GL_DYNAMIC_ATI                          0x8761
#define GL_PRESERVE_ATI                         0x8762
#define GL_DISCARD_ATI                          0x8763

#define GL_OBJECT_BUFFER_SIZE_ATI               0x8764
#define GL_OBJECT_BUFFER_USAGE_ATI              0x8765
#define GL_ARRAY_OBJECT_BUFFER_ATI              0x8766
#define GL_ARRAY_OBJECT_OFFSET_ATI              0x8767

typedef GLuint (APIENTRY * PFNGLNEWOBJECTBUFFERATIPROC)(GLsizei size, const GLvoid *pointer, GLenum usage);
typedef GLboolean (APIENTRY * PFNGLISOBJECTBUFFERATIPROC)(GLuint buffer);
typedef GLvoid (APIENTRY * PFNGLUPDATEOBJECTBUFFERATIPROC)(GLuint buffer, GLuint offset, GLsizei size,
                                                           const GLvoid *pointer, GLenum preserve);
typedef GLvoid (APIENTRY * PFNGLGETOBJECTBUFFERFVATIPROC)(GLuint buffer, GLenum pname, GLfloat *params);
typedef GLvoid (APIENTRY * PFNGLGETOBJECTBUFFERIVATIPROC)(GLuint buffer, GLenum pname, GLint *params);
typedef GLvoid (APIENTRY * PFNGLFREEOBJECTBUFFERATIPROC)(GLuint buffer);
typedef GLvoid (APIENTRY * PFNGLARRAYOBJECTATIPROC)(GLenum array, GLint size, GLenum type,
                                                    GLsizei stride, GLuint buffer, GLuint offset);
typedef GLvoid (APIENTRY * PFNGLGETARRAYOBJECTFVATIPROC)(GLenum array, GLenum pname, GLfloat *params);
typedef GLvoid (APIENTRY * PFNGLGETARRAYOBJECTIVATIPROC)(GLenum array, GLenum pname, GLint *params);
typedef GLvoid (APIENTRY * PFNGLVARIANTARRAYOBJECTATIPROC)(GLuint id, GLenum type,
                                                           GLsizei stride, GLuint buffer, GLuint offset);
typedef GLvoid (APIENTRY * PFNGLGETVARIANTARRAYOBJECTFVATIPROC)(GLuint id, GLenum pname, GLfloat *params);
typedef GLvoid (APIENTRY * PFNGLGETVARIANTARRAYOBJECTIVATIPROC)(GLuint id, GLenum pname, GLint *params);
#endif  /* GL_ATI_vertex_array_object */

/*
** GL_ATI_vertex_attrib_array_object
**
**  Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ATI_vertex_attrib_array_object
#define GL_ATI_vertex_attrib_array_object 1

typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBARRAYOBJECTATIPROC)(GLuint index, GLint size, GLenum type,
                                                                GLboolean normalized, GLsizei stride,
                                                                GLuint buffer, GLuint offset);
typedef GLvoid (APIENTRY * PFNGLGETVERTEXATTRIBARRAYOBJECTFVATIPROC)(GLuint index, GLenum pname,
                                                                   GLfloat *params);
typedef GLvoid (APIENTRY * PFNGLGETVERTEXATTRIBARRAYOBJECTIVATIPROC)(GLuint index, GLenum pname,
                                                                     GLint *params);

#endif /* GL_ATI_vertex_attrib_array_object */

/*
** GL_ATI_vertex_streams
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ATI_vertex_streams
#define GL_ATI_vertex_streams                                   1

#define GL_MAX_VERTEX_STREAMS_ATI                               0x876B
#define GL_VERTEX_SOURCE_ATI                                    0x876C
#define GL_VERTEX_STREAM0_ATI                                   0x876D
#define GL_VERTEX_STREAM1_ATI                                   0x876E
#define GL_VERTEX_STREAM2_ATI                                   0x876F
#define GL_VERTEX_STREAM3_ATI                                   0x8770
#define GL_VERTEX_STREAM4_ATI                                   0x8771
#define GL_VERTEX_STREAM5_ATI                                   0x8772
#define GL_VERTEX_STREAM6_ATI                                   0x8773
#define GL_VERTEX_STREAM7_ATI                                   0x8774

typedef GLvoid (APIENTRY * PFNGLCLIENTACTIVEVERTEXSTREAMATIPROC) (GLenum stream);
typedef GLvoid (APIENTRY * PFNGLVERTEXBLENDENVIATIPROC) (GLenum pname, GLint param);
typedef GLvoid (APIENTRY * PFNGLVERTEXBLENDENVFATIPROC) (GLenum pname, GLfloat param);
typedef GLvoid (APIENTRY * PFNGLVERTEXSTREAM2SATIPROC) (GLenum stream, GLshort x, GLshort y);
typedef GLvoid (APIENTRY * PFNGLVERTEXSTREAM2SVATIPROC) (GLenum stream, const GLshort *v);
typedef GLvoid (APIENTRY * PFNGLVERTEXSTREAM2IATIPROC) (GLenum stream, GLint x, GLint y);
typedef GLvoid (APIENTRY * PFNGLVERTEXSTREAM2IVATIPROC) (GLenum stream, const GLint *v);
typedef GLvoid (APIENTRY * PFNGLVERTEXSTREAM2FATIPROC) (GLenum stream, GLfloat x, GLfloat y);
typedef GLvoid (APIENTRY * PFNGLVERTEXSTREAM2FVATIPROC) (GLenum stream, const GLfloat *v);
typedef GLvoid (APIENTRY * PFNGLVERTEXSTREAM2DATIPROC) (GLenum stream, GLdouble x, GLdouble y);
typedef GLvoid (APIENTRY * PFNGLVERTEXSTREAM2DVATIPROC) (GLenum stream, const GLdouble *v);
typedef GLvoid (APIENTRY * PFNGLVERTEXSTREAM3SATIPROC) (GLenum stream, GLshort x, GLshort y, GLshort z);
typedef GLvoid (APIENTRY * PFNGLVERTEXSTREAM3SVATIPROC) (GLenum stream, const GLshort *v);
typedef GLvoid (APIENTRY * PFNGLVERTEXSTREAM3IATIPROC) (GLenum stream, GLint x, GLint y, GLint z);
typedef GLvoid (APIENTRY * PFNGLVERTEXSTREAM3IVATIPROC) (GLenum stream, const GLint *v);
typedef GLvoid (APIENTRY * PFNGLVERTEXSTREAM3FATIPROC) (GLenum stream, GLfloat x, GLfloat y, GLfloat z);
typedef GLvoid (APIENTRY * PFNGLVERTEXSTREAM3FVATIPROC) (GLenum stream, const GLfloat *v);
typedef GLvoid (APIENTRY * PFNGLVERTEXSTREAM3DATIPROC) (GLenum stream, GLdouble x, GLdouble y, GLdouble z);
typedef GLvoid (APIENTRY * PFNGLVERTEXSTREAM3DVATIPROC) (GLenum stream, const GLdouble *v);
typedef GLvoid (APIENTRY * PFNGLVERTEXSTREAM4SATIPROC) (GLenum stream, GLshort x, GLshort y, GLshort z, GLshort w);
typedef GLvoid (APIENTRY * PFNGLVERTEXSTREAM4SVATIPROC) (GLenum stream, const GLshort *v);
typedef GLvoid (APIENTRY * PFNGLVERTEXSTREAM4IATIPROC) (GLenum stream, GLint x, GLint y, GLint z, GLint w);
typedef GLvoid (APIENTRY * PFNGLVERTEXSTREAM4IVATIPROC) (GLenum stream, const GLint *v);
typedef GLvoid (APIENTRY * PFNGLVERTEXSTREAM4FATIPROC) (GLenum stream, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef GLvoid (APIENTRY * PFNGLVERTEXSTREAM4FVATIPROC) (GLenum stream, const GLfloat *v);
typedef GLvoid (APIENTRY * PFNGLVERTEXSTREAM4DATIPROC) (GLenum stream, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef GLvoid (APIENTRY * PFNGLVERTEXSTREAM4DVATIPROC) (GLenum stream, const GLdouble *v);
typedef GLvoid (APIENTRY * PFNGLNORMALSTREAM3BATIPROC) (GLenum stream, GLbyte x, GLbyte y, GLbyte z);
typedef GLvoid (APIENTRY * PFNGLNORMALSTREAM3BVATIPROC) (GLenum stream, const GLbyte *v);
typedef GLvoid (APIENTRY * PFNGLNORMALSTREAM3SATIPROC) (GLenum stream, GLshort x, GLshort y, GLshort z);
typedef GLvoid (APIENTRY * PFNGLNORMALSTREAM3SVATIPROC) (GLenum stream, const GLshort *v);
typedef GLvoid (APIENTRY * PFNGLNORMALSTREAM3IATIPROC) (GLenum stream, GLint x, GLint y, GLint z);
typedef GLvoid (APIENTRY * PFNGLNORMALSTREAM3IVATIPROC) (GLenum stream, const GLint *v);
typedef GLvoid (APIENTRY * PFNGLNORMALSTREAM3FATIPROC) (GLenum stream, GLfloat x, GLfloat y, GLfloat z);
typedef GLvoid (APIENTRY * PFNGLNORMALSTREAM3FVATIPROC) (GLenum stream, const GLfloat *v);
typedef GLvoid (APIENTRY * PFNGLNORMALSTREAM3DATIPROC) (GLenum stream, GLdouble x, GLdouble y, GLdouble z);
typedef GLvoid (APIENTRY * PFNGLNORMALSTREAM3DVATIPROC) (GLenum stream, const GLdouble *v);

#endif /* GL_ATI_vertex_streams */

/*
** GL_ATI_visibility_query
**
**  Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ATI_visibility_query
#define GL_ATI_visibility_query 1

#define GL_MAX_VISIBILITY_QUERY_IDS_ATI        0x8836

typedef GLvoid (APIENTRY * PFNGLGENVISIBILITYQUERIESATIPROC)(GLsizei n, GLuint *ids);
typedef GLvoid (APIENTRY * PFNGLDELETEVISIBILITYQUERIESATIPROC)(GLsizei n, const GLuint *ids);
typedef GLvoid (APIENTRY * PFNGLBEGINDEFINEVISIBILITYQUERYATIPROC)(GLuint id);
typedef GLvoid (APIENTRY * PFNGLENDDEFINEVISIBILITYQUERYATIPROC)(void);
typedef GLvoid (APIENTRY * PFNGLBEGINUSEVISIBILITYQUERYATIPROC)(GLuint id);
typedef GLvoid (APIENTRY * PFNGLENDUSEVISIBILITYQUERYATIPROC)(void);

#endif /* GL_ATI_visibility_query */

/*
**  ATIX EXTENSIONS
*/

/*
** GL_ATI_texture_env_combine3
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ATIX_texture_env_combine3
#define GL_ATIX_texture_env_combine3

#define GL_MODULATE_ADD_ATIX                            0x8744
#define GL_MODULATE_SIGNED_ADD_ATIX                     0x8745
#define GL_MODULATE_SUBTRACT_ATIX                       0x8746

#endif /* GL_ATIX_texture_env_combine3 */

#ifndef GL_ATI_texture_env_combine3
#define GL_ATI_texture_env_combine3

#define GL_MODULATE_ADD_ATI                             0x8744
#define GL_MODULATE_SIGNED_ADD_ATI                      0x8745
#define GL_MODULATE_SUBTRACT_ATI                        0x8746

#endif /* GL_ATI_texture_env_combine3 */

/*
** GL_ATIX_texture_env_route
**
** Support
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ATIX_texture_env_route
#define GL_ATIX_texture_env_route 1

#define GL_SECONDARY_COLOR_ATIX                 0x8747
#define GL_TEXTURE_OUTPUT_RGB_ATIX              0x8748
#define GL_TEXTURE_OUTPUT_ALPHA_ATIX            0x8749

#endif /* GL_ATIX_texture_env_route */

/*
** GL_ATIX_vertex_shader_output_point_size
**
**  Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_ATIX_vertex_shader_output_point_size
#define GL_ATIX_vertex_shader_output_point_size 1

#define GL_OUTPUT_POINT_SIZE_ATIX       0x610E

#endif /* GL_ATIX_vertex_shader_output_point_size */

/*
**  EXT EXTENSIONS
*/
/*
** GL_EXT_abgr
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_abgr
#define GL_EXT_abgr                         1

#define GL_ABGR_EXT                         0x8000

#endif /* GL_EXT_abgr */


/*
** GL_EXT_bgra
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_bgra
#define GL_EXT_bgra                         1

#define GL_BGR_EXT                          0x80E0
#define GL_BGRA_EXT                         0x80E1

#endif /* GL_EXT_bgra */

/*
** GL_EXT_blend_color
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_blend_color
#define GL_EXT_blend_color              1

#define GL_CONSTANT_COLOR_EXT           0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR_EXT 0x8002
#define GL_CONSTANT_ALPHA_EXT           0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA_EXT 0x8004
#define GL_BLEND_COLOR_EXT              0x8005

typedef GLvoid (APIENTRY * PFNGLBLENDCOLOREXTPROC) (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);

#endif  /* GL_EXT_blend_color */

/*
** GL_EXT_blend_func_separate
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/

#ifndef GL_EXT_blend_func_separate
#define GL_EXT_blend_func_separate  1

#define GL_BLEND_DST_RGB_EXT    0x80C8
#define GL_BLEND_SRC_RGB_EXT    0x80C9
#define GL_BLEND_DST_ALPHA_EXT  0x80CA
#define GL_BLEND_SRC_ALPHA_EXT  0x80CB

typedef GLvoid (APIENTRY * PFNGLBLENDFUNCSEPARATEEXTPROC) (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
#endif /* GL_EXT_blend_func_separate */

/*
** GL_EXT_blend_minmax
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_blend_minmax
#define GL_EXT_blend_minmax     1

#define GL_FUNC_ADD_EXT         0x8006
#define GL_MIN_EXT              0x8007
#define GL_MAX_EXT              0x8008
#define GL_BLEND_EQUATION_EXT   0x8009

typedef GLvoid (APIENTRY * PFNGLBLENDEQUATIONEXTPROC) (GLenum mode);

#endif /* GL_EXT_blend_minmax */

/*
** GL_EXT_blend_subtract
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_blend_subtract
#define GL_EXT_blend_subtract                1

#define GL_FUNC_SUBTRACT_EXT                 0x800A
#define GL_FUNC_REVERSE_SUBTRACT_EXT         0x800B

#endif /* GL_EXT_blend_subtract */

/*
** GL_EXT_clip_volume_hint
**
**  Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_clip_volume_hint
#define GL_EXT_clip_volume_hint             1

#define GL_CLIP_VOLUME_CLIPPING_HINT_EXT    0x80F0

#endif /* GL_EXT_clip_volume_hint */

/*
** GL_EXT_compiled_vertex_array
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_compiled_vertex_array
#define GL_EXT_compiled_vertex_array        1

#define GL_ARRAY_ELEMENT_LOCK_FIRST_EXT     0x81A8
#define GL_ARRAY_ELEMENT_LOCK_COUNT_EXT     0x81A9

typedef GLvoid (APIENTRY * PFNGLLOCKARRAYSEXTPROC) (GLint first, GLsizei count);
typedef GLvoid (APIENTRY * PFNGLUNLOCKARRAYSEXTPROC) (void);

#endif /* GL_EXT_compiled_vertex_array */

/*
** GL_EXT_direct_state_access
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_direct_state_access
#define GL_EXT_direct_state_access  1

#define GL_PROGRAM_MATRIX_EXT                               0x8E2D
#define GL_TRANSPOSE_PROGRAM_MATRIX_EXT                     0x8E2E
#define GL_PROGRAM_MATRIX_STACK_DEPTH_EXT                   0x8E2F

#endif /* GL_EXT_direct_state_access */

/*
** GL_EXT_draw_range_elements
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_draw_range_elements
#define GL_EXT_draw_range_elements           1

#define GL_MAX_ELEMENTS_VERTICES_EXT         0x80E8
#define GL_MAX_ELEMENTS_INDICES_EXT          0x80E9

typedef GLvoid (APIENTRY * PFNGLDRAWRANGEELEMENTSEXTPROC) (GLenum  mode, GLuint start,
                                                           GLuint end, GLsizei count,
                                                           GLenum type, const GLvoid *indices);

#endif /* GL_EXT_draw_range_elements */

/*
** GL_EXT_fog_coord
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_fog_coord
#define GL_EXT_fog_coord                     1

#define GL_FOG_COORDINATE_SOURCE_EXT         0x8450
#define GL_FOG_COORDINATE_EXT                0x8451
#define GL_FRAGMENT_DEPTH_EXT                0x8452
#define GL_CURRENT_FOG_COORDINATE_EXT        0x8453
#define GL_FOG_COORDINATE_ARRAY_TYPE_EXT     0x8454
#define GL_FOG_COORDINATE_ARRAY_STRIDE_EXT   0x8455
#define GL_FOG_COORDINATE_ARRAY_POINTER_EXT  0x8456
#define GL_FOG_COORDINATE_ARRAY_EXT          0x8457

typedef GLvoid (APIENTRY * PFNGLFOGCOORDFEXTPROC) (GLfloat f);
typedef GLvoid (APIENTRY * PFNGLFOGCOORDDEXTPROC) (GLdouble f);
typedef GLvoid (APIENTRY * PFNGLFOGCOORDFVEXTPROC) (const GLfloat *v);
typedef GLvoid (APIENTRY * PFNGLFOGCOORDDVEXTPROC) (const GLdouble *v);
typedef GLvoid (APIENTRY * PFNGLFOGCOORDPOINTEREXTPROC) (GLenum type, GLsizei stride, GLvoid *pointer);

#endif /* GL_EXT_fog_coord */

/*
** GL_EXT_framebuffer_object
**
**  Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_framebuffer_object
#define GL_EXT_framebuffer_object                         1

#define GL_FRAMEBUFFER_EXT                                   0x8D40
#define GL_RENDERBUFFER_EXT                                  0x8D41
#define GL_STENCIL_INDEX1_EXT                                0x8D46
#define GL_STENCIL_INDEX4_EXT                                0x8D47
#define GL_STENCIL_INDEX8_EXT                                0x8D48
#define GL_STENCIL_INDEX16_EXT                               0x8D49
#define GL_RENDERBUFFER_WIDTH_EXT                            0x8D42
#define GL_RENDERBUFFER_HEIGHT_EXT                           0x8D43
#define GL_RENDERBUFFER_INTERNAL_FORMAT_EXT                  0x8D44
#define GL_RENDERBUFFER_RED_SIZE_EXT                         0x8D50
#define GL_RENDERBUFFER_GREEN_SIZE_EXT                       0x8D51
#define GL_RENDERBUFFER_BLUE_SIZE_EXT                        0x8D52
#define GL_RENDERBUFFER_ALPHA_SIZE_EXT                       0x8D53
#define GL_RENDERBUFFER_DEPTH_SIZE_EXT                       0x8D54
#define GL_RENDERBUFFER_STENCIL_SIZE_EXT                     0x8D55
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT            0x8CD0
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT            0x8CD1
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT          0x8CD2
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT  0x8CD3
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT     0x8CD4
#define GL_COLOR_ATTACHMENT0_EXT                             0x8CE0
#define GL_COLOR_ATTACHMENT1_EXT                             0x8CE1
#define GL_COLOR_ATTACHMENT2_EXT                             0x8CE2
#define GL_COLOR_ATTACHMENT3_EXT                             0x8CE3
#define GL_COLOR_ATTACHMENT4_EXT                             0x8CE4
#define GL_COLOR_ATTACHMENT5_EXT                             0x8CE5
#define GL_COLOR_ATTACHMENT6_EXT                             0x8CE6
#define GL_COLOR_ATTACHMENT7_EXT                             0x8CE7
#define GL_COLOR_ATTACHMENT8_EXT                             0x8CE8
#define GL_COLOR_ATTACHMENT9_EXT                             0x8CE9
#define GL_COLOR_ATTACHMENT10_EXT                            0x8CEA
#define GL_COLOR_ATTACHMENT11_EXT                            0x8CEB
#define GL_COLOR_ATTACHMENT12_EXT                            0x8CEC
#define GL_COLOR_ATTACHMENT13_EXT                            0x8CED
#define GL_COLOR_ATTACHMENT14_EXT                            0x8CEE
#define GL_COLOR_ATTACHMENT15_EXT                            0x8CEF
#define GL_DEPTH_ATTACHMENT_EXT                              0x8D00
#define GL_STENCIL_ATTACHMENT_EXT                            0x8D20
#define GL_FRAMEBUFFER_COMPLETE_EXT                          0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT             0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT     0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT   0x8CD8
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT             0x8CD9
#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT                0x8CDA
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT            0x8CDB
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT            0x8CDC
#define GL_FRAMEBUFFER_UNSUPPORTED_EXT                       0x8CDD
#define GL_FRAMEBUFFER_STATUS_ERROR_EXT                      0x8CDE
#define GL_FRAMEBUFFER_BINDING_EXT                           0x8CA6
#define GL_RENDERBUFFER_BINDING_EXT                          0x8CA7
#define GL_MAX_COLOR_ATTACHMENTS_EXT                         0x8CDF
#define GL_MAX_RENDERBUFFER_SIZE_EXT                         0x84E8
#define GL_INVALID_FRAMEBUFFER_OPERATION_EXT                 0x0506

typedef GLboolean    (APIENTRY * PFNGLISRENDERBUFFEREXTPROC)(GLuint renderbuffer);
typedef GLvoid       (APIENTRY * PFNGLBINDRENDERBUFFEREXTPROC)(GLenum target, GLuint renderbuffer);
typedef GLvoid       (APIENTRY * PFNGLDELETERENDERBUFFERSEXTPROC)(GLsizei n, const GLuint *renderbuffers);
typedef GLvoid       (APIENTRY * PFNGLGENRENDERBUFFERSEXTPROC)(GLsizei n, GLuint *renderbuffers);
typedef GLvoid       (APIENTRY * PFNGLRENDERBUFFERSTORAGEEXTPROC)(GLenum target, GLenum internalformat,
                                                                  GLsizei width, GLsizei height);
typedef GLvoid       (APIENTRY * PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC)(GLenum target, GLenum pname, GLint* params);
typedef GLboolean    (APIENTRY * PFNGLISFRAMEBUFFEREXTPROC)(GLuint framebuffer);
typedef GLvoid       (APIENTRY * PFNGLBINDFRAMEBUFFEREXTPROC)(GLenum target, GLuint framebuffer);
typedef GLvoid       (APIENTRY * PFNGLDELETEFRAMEBUFFERSEXTPROC)(GLsizei n, const GLuint *framebuffers);
typedef GLvoid       (APIENTRY * PFNGLGENFRAMEBUFFERSEXTPROC)(GLsizei n, GLuint *framebuffers);
typedef GLenum       (APIENTRY * PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)(GLenum target);
typedef GLvoid       (APIENTRY * PFNGLFRAMEBUFFERTEXTURE1DEXTPROC)(GLenum target, GLenum attachment,
                                                                   GLenum textarget, GLuint texture, GLint level);
typedef GLvoid       (APIENTRY * PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)(GLenum target, GLenum attachment,
                                                                   GLenum textarget, GLuint texture, GLint level);
typedef GLvoid       (APIENTRY * PFNGLFRAMEBUFFERTEXTURE3DEXTPROC)(GLenum target, GLenum attachment,
                                                                   GLenum textarget, GLuint texture,
                                                                   GLint level, GLint zoffset);
typedef GLvoid       (APIENTRY * PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)(GLenum target, GLenum attachment,
                                                                      GLenum renderbuffertarget, GLuint renderbuffer);
typedef GLvoid       (APIENTRY * PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)(GLenum target, GLenum attachment,
                                                                                  GLenum pname, GLint *params);
typedef GLvoid       (APIENTRY * PFNGLGENERATEMIPMAPEXTPROC)(GLenum target);

#endif /* GL_EXT_framebuffer_object */

/*
** GL_EXT_framebuffer_sRGB
**
** Support:
**  Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Not Supported
**   Radeon X800+  based : Not Supported
**   Radeon X1000+ based : Not Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_framebuffer_sRGB
#define GL_EXT_framebuffer_sRGB             1

#define GL_FRAMEBUFFER_SRGB_EXT             0X8DB9
#define GL_FRAMEBUFFER_SRGB_CAPABLE_EXT     0X8DBA

#endif /* GL_EXT_framebuffer_sRGB */


/*
** GL_EXT_gpu_shader4
**
**  Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Not Supported
**   Radeon X800+  based : Not Supported
**   Radeon X1000+ based : Not Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_gpu_shader4
#define GL_EXT_gpu_shader4                      1

#define GL_VERTEX_ATTRIB_ARRAY_INTEGER_EXT      0x88FD
#define GL_SAMPLER_1D_ARRAY_EXT                 0x8DC0
#define GL_SAMPLER_2D_ARRAY_EXT                 0x8DC1
#define GL_SAMPLER_BUFFER_EXT                   0x8DC2
#define GL_SAMPLER_1D_ARRAY_SHADOW_EXT          0x8DC3
#define GL_SAMPLER_2D_ARRAY_SHADOW_EXT          0x8DC4
#define GL_SAMPLER_CUBE_SHADOW_EXT              0x8DC5
#define GL_UNSIGNED_INT_VEC2_EXT                0x8DC6
#define GL_UNSIGNED_INT_VEC3_EXT                0x8DC7
#define GL_UNSIGNED_INT_VEC4_EXT                0x8DC8
#define GL_INT_SAMPLER_1D_EXT                   0x8DC9
#define GL_INT_SAMPLER_2D_EXT                   0x8DCA
#define GL_INT_SAMPLER_3D_EXT                   0x8DCB
#define GL_INT_SAMPLER_CUBE_EXT                 0x8DCC
#define GL_INT_SAMPLER_2D_RECT_EXT              0x8DCD
#define GL_INT_SAMPLER_1D_ARRAY_EXT             0x8DCE
#define GL_INT_SAMPLER_2D_ARRAY_EXT             0x8DCF
#define GL_INT_SAMPLER_BUFFER_EXT               0x8DD0
#define GL_UNSIGNED_INT_SAMPLER_1D_EXT          0x8DD1
#define GL_UNSIGNED_INT_SAMPLER_2D_EXT          0x8DD2
#define GL_UNSIGNED_INT_SAMPLER_3D_EXT          0x8DD3
#define GL_UNSIGNED_INT_SAMPLER_CUBE_EXT        0x8DD4
#define GL_UNSIGNED_INT_SAMPLER_2D_RECT_EXT     0x8DD5
#define GL_UNSIGNED_INT_SAMPLER_1D_ARRAY_EXT    0x8DD6
#define GL_UNSIGNED_INT_SAMPLER_2D_ARRAY_EXT    0x8DD7
#define GL_UNSIGNED_INT_SAMPLER_BUFFER_EXT      0x8DD8
#define GL_MIN_PROGRAM_TEXEL_OFFSET_EXT         0x8904
#define GL_MAX_PROGRAM_TEXEL_OFFSET_EXT         0x8905

typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI1IEXT) (GLuint index, GLint x);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI2IEXT) (GLuint index, GLint x, GLint y);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI3IEXT) (GLuint index, GLint x, GLint y, GLint z);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI4IEXT) (GLuint index, GLint x, GLint y, GLint z, GLint w);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI1UIEXT) (GLuint index, GLuint x);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI2UIEXT) (GLuint index, GLuint x, GLuint y);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI3UIEXT) (GLuint index, GLuint x, GLuint y, GLuint z);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI4UIEXT) (GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI1IVEXT) (GLuint index, const GLint* v);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI2IVEXT) (GLuint index, const GLint* v);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI3IVEXT) (GLuint index, const GLint* v);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI4IVEXT) (GLuint index, const GLint* v);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI1UIVEXT) (GLuint index, const GLuint* v);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI2UIVEXT) (GLuint index, const GLuint* v);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI3UIVEXT) (GLuint index, const GLuint* v);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI4UIVEXT) (GLuint index, const GLuint* v);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI4BVEXT) (GLuint index, const GLbyte* v);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI4SVEXT) (GLuint index, const GLshort* v);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI4UBVEXT) (GLuint index, const GLubyte* v);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBI4USVEXT) (GLuint index, const GLushort* v);
typedef GLvoid (APIENTRY * PFNGLVERTEXATTRIBIPOINTEREXT) (GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
typedef GLvoid (APIENTRY * PFNGLGETVERTEXATTRIBIIVEXT) (GLuint index, GLenum pname, GLint* params);
typedef GLvoid (APIENTRY * PFNGLGETVERTEXATTRIBIUIVEXT) (GLuint index, GLenum pname, GLuint* params);
typedef GLvoid (APIENTRY * PFNGLUNIFORM1UIEXT) (GLint location, GLuint v0);
typedef GLvoid (APIENTRY * PFNGLUNIFORM2UIEXT) (GLint location, GLuint v0, GLuint v1);
typedef GLvoid (APIENTRY * PFNGLUNIFORM3UIEXT) (GLint location, GLuint v0, GLuint v1, GLuint v2);
typedef GLvoid (APIENTRY * PFNGLUNIFORM4UIEXT) (GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
typedef GLvoid (APIENTRY * PFNGLUNIFORM1UIVEXT) (GLint location, GLsizei count, const GLuint* value);
typedef GLvoid (APIENTRY * PFNGLUNIFORM2UIVEXT) (GLint location, GLsizei count, const GLuint* value);
typedef GLvoid (APIENTRY * PFNGLUNIFORM3UIVEXT) (GLint location, GLsizei count, const GLuint* value);
typedef GLvoid (APIENTRY * PFNGLUNIFORM4UIVEXT) (GLint location, GLsizei count, const GLuint* value);
typedef GLvoid (APIENTRY * PFNGLGETUNIFORMUIVEXT) (GLuint program, GLint location, GLuint* params);
typedef GLvoid (APIENTRY * PFNGLBINDFRAGDATALOCATIONEXT) (GLuint program, GLuint colorNumber, const GLchar* name);
typedef GLvoid (APIENTRY * PFNGLGETFRAGDATALOCATIONEXT) (GLuint program, const GLchar* name);
#endif /* GL_EXT_gpu_shader4 */


/*
** GL_EXT_multi_draw_arrays
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_multi_draw_arrays
#define GL_EXT_multi_draw_arrays                1

typedef GLvoid (APIENTRY * PFNGLMULTIDRAWARRAYSEXTPROC) (GLenum mode, GLint *first, GLsizei *count, GLsizei primcount);
typedef GLvoid (APIENTRY * PFNGLMULTIDRAWELEMENTSEXTPROC) (GLenum mode, GLsizei *count, GLenum type, const GLvoid **indices, GLsizei primcount);

#endif /* GL_EXT_multi_draw_arrays */

/*
** GL_EXT_packed_pixels
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_packed_pixels
#define GL_EXT_packed_pixels                 1

#define GL_UNSIGNED_BYTE_3_3_2_EXT           0x8032
#define GL_UNSIGNED_SHORT_4_4_4_4_EXT        0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1_EXT        0x8034
#define GL_UNSIGNED_INT_8_8_8_8_EXT          0x8035
#define GL_UNSIGNED_INT_10_10_10_2_EXT       0x8036

#endif /* GL_EXT_packed_pixels */

/*
** GL_EXT_point_parameters
**
**  Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_point_parameters
#define GL_EXT_point_parameters              1

#define GL_POINT_SIZE_MIN_EXT                0x8126
#define GL_POINT_SIZE_MAX_EXT                0x8127
#define GL_POINT_FADE_THRESHOLD_SIZE_EXT     0x8128
#define GL_DISTANCE_ATTENUATION_EXT          0x8129

typedef GLvoid (APIENTRY * PFNGLPOINTPARAMETERFEXTPROC)  (GLenum pname, GLfloat param);
typedef GLvoid (APIENTRY * PFNGLPOINTPARAMETERFVEXTPROC)  (GLenum pname, GLfloat *params);

#endif /* GL_EXT_point_parameters */

/*
** GL_EXT_polygon_offset
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_polygon_offset
#define GL_EXT_polygon_offset                1

#define GL_POLYGON_OFFSET_EXT                GL_POLYGON_OFFSET_FILL
#define GL_POLYGON_OFFSET_FACTOR_EXT         GL_POLYGON_OFFSET_FACTOR
#define GL_POLYGON_OFFSET_BIAS_EXT           0x8039

#endif /* GL_EXT_polygon_offset */

/*
** GL_EXT_rescale_normal
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_rescale_normal
#define GL_EXT_rescale_normal                1

#define GL_RESCALE_NORMAL_EXT                0x803A

#endif /* GL_EXT_rescale_normal */

/*
** GL_EXT_secondary_color
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_secondary_color
#define GL_EXT_secondary_color               1

#define GL_COLOR_SUM_EXT                     0x8458
#define GL_CURRENT_SECONDARY_COLOR_EXT       0x8459
#define GL_SECONDARY_COLOR_ARRAY_SIZE_EXT    0x845A
#define GL_SECONDARY_COLOR_ARRAY_TYPE_EXT    0x845B
#define GL_SECONDARY_COLOR_ARRAY_STRIDE_EXT  0x845C
#define GL_SECONDARY_COLOR_ARRAY_POINTER_EXT 0x845D
#define GL_SECONDARY_COLOR_ARRAY_EXT         0x845E

typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3FEXTPROC) (GLfloat r, GLfloat g, GLfloat b);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3DEXTPROC) (GLdouble r, GLdouble g, GLdouble b);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3BEXTPROC) (GLbyte r, GLbyte g, GLbyte b);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3SEXTPROC) (GLshort r, GLshort g, GLshort b);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3IEXTPROC) (GLint r, GLint g, GLint b);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3UBEXTPROC)(GLubyte r, GLubyte g, GLubyte b);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3USEXTPROC)(GLushort r, GLushort g, GLushort b);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3UIEXTPROC)(GLuint r, GLuint g, GLuint b);

typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3FVEXTPROC) (const GLfloat *v);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3DVEXTPROC) (const GLdouble *v);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3BVEXTPROC) (const GLbyte *v);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3SVEXTPROC) (const GLshort *v);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3IVEXTPROC) (const GLint *v);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3UBVEXTPROC)(const GLubyte *v);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3USVEXTPROC)(const GLushort *v);
typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3UIVEXTPROC)(const GLuint *v);

typedef GLvoid (APIENTRY * PFNGLSECONDARYCOLORPOINTEREXTPROC)(GLint size, GLenum type, GLsizei stride, GLvoid *pointer);

#endif /* GL_EXT_secondary_color */

/*
** GL_EXT_separate_specular_color
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_separate_specular_color
#define GL_EXT_separate_specular_color       1

#define GL_LIGHT_MODEL_COLOR_CONTROL_EXT     0x81F8
#define GL_SINGLE_COLOR_EXT                  0x81F9
#define GL_SEPARATE_SPECULAR_COLOR_EXT       0x81FA

#endif /* GL_EXT_separate_specular_color */

/*
** GL_EXT_shadow_funcs
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_shadow_funcs
#define GL_EXT_shadow_funcs                                     1
#endif  /* GL_EXT_shadow_funcs */

/*
** GL_EXT_stencil_wrap
**
** Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_stencil_wrap
#define GL_EXT_stencil_wrap                                     1

#define GL_INCR_WRAP_EXT                                        0x8507
#define GL_DECR_WRAP_EXT                                        0x8508

#endif  /* GL_EXT_stencil_wrap */

/*
** GL_EXT_texgen_reflection
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_texgen_reflection
#define GL_EXT_texgen_reflection              1

#define GL_NORMAL_MAP_EXT                     0x8511
#define GL_REFLECTION_MAP_EXT                 0x8512

#endif /* GL_EXT_texgen_reflection */

/*
** GL_EXT_texture3D
**
** Support:
**   Rage   128    based : Supported (software)
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_texture3D
#define GL_EXT_texture3D                     1

#define GL_PACK_SKIP_IMAGES_EXT              0x806B
#define GL_PACK_IMAGE_HEIGHT_EXT             0x806C
#define GL_UNPACK_SKIP_IMAGES_EXT            0x806D
#define GL_UNPACK_IMAGE_HEIGHT_EXT           0x806E
#define GL_TEXTURE_3D_EXT                    0x806F
#define GL_PROXY_TEXTURE_3D_EXT              0x8070
#define GL_TEXTURE_DEPTH_EXT                 0x8071
#define GL_TEXTURE_WRAP_R_EXT                0x8072
#define GL_MAX_3D_TEXTURE_SIZE_EXT           0x8073


typedef GLvoid (APIENTRY * PFNGLTEXIMAGE3DEXTPROC)(GLenum target, GLint level, GLenum internalformat,
                                                   GLsizei width, GLsizei height, GLsizei depth,
                                                   GLint border, GLenum format, GLenum type,
                                                   const GLvoid* pixels);

#endif /* GL_EXT_texture3D */

/*
** GL_EXT_texture_compression_s3tc
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_texture_compression_s3tc
#define GL_EXT_texture_compression_s3tc      1

#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT      0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT     0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT     0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT     0x83F3

#endif /* GL_EXT_texture_compression_s3tc */

/*
** GL_EXT_texture_cube_map
**
**  Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_texture_cube_map
#define GL_EXT_texture_cube_map              1

#define GL_TEXTURE_CUBE_MAP_EXT              0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP_EXT      0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT   0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT   0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT   0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT   0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT   0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT   0x851A
#define GL_PROXY_TEXTURE_CUBE_MAP_EXT        0x851B
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE_EXT     0x851C

#endif /* GL_EXT_texture_cube_map */

/*
** GL_EXT_texture_edge_clamp
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_texture_edge_clamp
#define GL_EXT_texture_edge_clamp            1

#define GL_CLAMP_TO_EDGE_EXT                 0x812F

#endif /* GL_EXT_texture_edge_clamp */

/*
** GL_EXT_texture_env_add
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_texture_env_add
#define GL_EXT_texture_env_add               1

#endif /* GL_EXT_texture_env_add */

/*
** GL_EXT_texture_env_combine
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_texture_env_combine
#define GL_EXT_texture_env_combine           1

#define GL_COMBINE_EXT                       0x8570
#define GL_COMBINE_RGB_EXT                   0x8571
#define GL_COMBINE_ALPHA_EXT                 0x8572
#define GL_SOURCE0_RGB_EXT                   0x8580
#define GL_SOURCE1_RGB_EXT                   0x8581
#define GL_SOURCE2_RGB_EXT                   0x8582
#define GL_SOURCE0_ALPHA_EXT                 0x8588
#define GL_SOURCE1_ALPHA_EXT                 0x8589
#define GL_SOURCE2_ALPHA_EXT                 0x858A
#define GL_OPERAND0_RGB_EXT                  0x8590
#define GL_OPERAND1_RGB_EXT                  0x8591
#define GL_OPERAND2_RGB_EXT                  0x8592
#define GL_OPERAND0_ALPHA_EXT                0x8598
#define GL_OPERAND1_ALPHA_EXT                0x8599
#define GL_OPERAND2_ALPHA_EXT                0x859A
#define GL_RGB_SCALE_EXT                     0x8573
#define GL_ADD_SIGNED_EXT                    0x8574
#define GL_INTERPOLATE_EXT                   0x8575
#define GL_CONSTANT_EXT                      0x8576
#define GL_PRIMARY_COLOR_EXT                 0x8577
#define GL_PREVIOUS_EXT                      0x8578

#endif /* GL_EXT_texture_env_combine */

/*
** GL_EXT_texture_env_dot3
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_texture_env_dot3
#define GL_EXT_texture_env_dot3 1

#define GL_DOT3_RGB_EXT  0x8740
#define GL_DOT3_RGBA_EXT 0x8741

#endif /* GL_EXT_texture_env_dot3 */

/*
** GL_EXT_texture_filter_anisotropic
**
** Support:
**
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_texture_filter_anisotropic
#define GL_EXT_texture_filter_anisotropic    1

#define GL_TEXTURE_MAX_ANISOTROPY_EXT        0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT    0x84FF

#endif /* GL_EXT_texture_filter_anisotropic */

/*
** GL_EXT_texture_lod_bias
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_texture_lod_bias
#define GL_EXT_texture_lod_bias              1

#define GL_TEXTURE_FILTER_CONTROL_EXT 0x8500
#define GL_TEXTURE_LOD_BIAS_EXT       0x8501
#define GL_MAX_TEXTURE_LOD_BIAS_EXT   0x84FD

#endif /* GL_EXT_texture_lod_bias */

/*
** GL_EXT_texture_mirror_clamp
**
**  Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_texture_mirror_clamp
#define GL_EXT_texture_mirror_clamp  1

#define GL_MIRROR_CLAMP_EXT           0x8742
#define GL_MIRROR_CLAMP_TO_EDGE_EXT   0x8743
#define GL_MIRROR_CLAMP_TO_BORDER_EXT 0x8912

#endif /* GL_EXT_texture_mirror_clamp */

/*
** GL_EXT_texture_object
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_texture_object
#define GL_EXT_texture_object              1

#define GL_TEXTURE_PRIORITY_EXT            0x8066
#define GL_TEXTURE_RESIDENT_EXT            0x8067
#define GL_TEXTURE_1D_BINDING_EXT          0x8068
#define GL_TEXTURE_2D_BINDING_EXT          0x8069
#define GL_TEXTURE_3D_BINDING_EXT          0x806A

typedef GLvoid (APIENTRY * PFNGLGENTEXTURESEXTPROC) (GLsizei n, GLuint *textures);
typedef GLvoid (APIENTRY * PFNGLDELETETEXTURESEXTPROC) (GLsizei n, const GLuint *textures);
typedef GLvoid (APIENTRY * PFNGLBINDTEXTUREEXTPROC) (GLenum target, GLuint texture);
typedef GLvoid (APIENTRY * PFNGLPRIORITIZETEXTURESEXTPROC) (GLsizei n, const GLuint *textures, const GLclampf *priorities);
typedef GLboolean (APIENTRY * PFNGLARETEXTURESRESIDENTEXTPROC) (GLsizei n, const GLuint *textures, GLboolean *residences);
typedef GLboolean (APIENTRY * PFNGLISTEXTUREEXTPROC) (GLuint texture);

#endif /* GL_EXT_texture_object */

/*
** GL_EXT_texture_rectangle
**
**  Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_texture_rectangle
#define GL_EXT_texture_rectangle            1

#define GL_TEXTURE_RECTANGLE_EXT            0x84F5
#define GL_TEXTURE_BINDING_RECTANGLE_EXT    0x84F6
#define GL_PROXY_TEXTURE_RECTANGLE_EXT      0x84F7
#define GL_MAX_RECTANGLE_TEXTURE_SIZE_EXT   0x84F8

#endif /* GL_EXT_texture_rectangle */

/*
** GL_EXT_vertex_array
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_vertex_array
#define GL_EXT_vertex_array                 1

#ifndef ENUMS_GL_EXT_vertex_array
#define ENUMS_GL_EXT_vertex_array
#define GL_VERTEX_ARRAY_EXT                 0x8074
#define GL_NORMAL_ARRAY_EXT                 0x8075
#define GL_COLOR_ARRAY_EXT                  0x8076
#define GL_INDEX_ARRAY_EXT                  0x8077
#define GL_TEXTURE_COORD_ARRAY_EXT          0x8078
#define GL_EDGE_FLAG_ARRAY_EXT              0x8079
#define GL_VERTEX_ARRAY_SIZE_EXT            0x807A
#define GL_VERTEX_ARRAY_TYPE_EXT            0x807B
#define GL_VERTEX_ARRAY_STRIDE_EXT          0x807C
#define GL_VERTEX_ARRAY_COUNT_EXT           0x807D
#define GL_NORMAL_ARRAY_TYPE_EXT            0x807E
#define GL_NORMAL_ARRAY_STRIDE_EXT          0x807F
#define GL_NORMAL_ARRAY_COUNT_EXT           0x8080
#define GL_COLOR_ARRAY_SIZE_EXT             0x8081
#define GL_COLOR_ARRAY_TYPE_EXT             0x8082
#define GL_COLOR_ARRAY_STRIDE_EXT           0x8083
#define GL_COLOR_ARRAY_COUNT_EXT            0x8084
#define GL_INDEX_ARRAY_TYPE_EXT             0x8085
#define GL_INDEX_ARRAY_STRIDE_EXT           0x8086
#define GL_INDEX_ARRAY_COUNT_EXT            0x8087
#define GL_TEXTURE_COORD_ARRAY_SIZE_EXT     0x8088
#define GL_TEXTURE_COORD_ARRAY_TYPE_EXT     0x8089
#define GL_TEXTURE_COORD_ARRAY_STRIDE_EXT   0x808A
#define GL_TEXTURE_COORD_ARRAY_COUNT_EXT    0x808B
#define GL_EDGE_FLAG_ARRAY_STRIDE_EXT       0x808C
#define GL_EDGE_FLAG_ARRAY_COUNT_EXT        0x808D
#define GL_VERTEX_ARRAY_POINTER_EXT         0x808E
#define GL_NORMAL_ARRAY_POINTER_EXT         0x808F
#define GL_COLOR_ARRAY_POINTER_EXT          0x8090
#define GL_INDEX_ARRAY_POINTER_EXT          0x8091
#define GL_TEXTURE_COORD_ARRAY_POINTER_EXT  0x8092
#define GL_EDGE_FLAG_ARRAY_POINTER_EXT      0x8093
#endif // ENUMS_GL_EXT_vertex_array

typedef GLvoid (APIENTRY * PFNGLARRAYELEMENTEXTPROC) (GLint i);
typedef GLvoid (APIENTRY * PFNGLCOLORPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef GLvoid (APIENTRY * PFNGLDRAWARRAYSEXTPROC) (GLenum mode, GLint first, GLsizei count);
typedef GLvoid (APIENTRY * PFNGLEDGEFLAGPOINTEREXTPROC) (GLsizei stride, GLsizei count, const GLboolean *pointer);
typedef GLvoid (APIENTRY * PFNGLGETPOINTERVEXTPROC) (GLenum pname, GLvoid* *params);
typedef GLvoid (APIENTRY * PFNGLINDEXPOINTEREXTPROC) (GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef GLvoid (APIENTRY * PFNGLNORMALPOINTEREXTPROC) (GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef GLvoid (APIENTRY * PFNGLTEXCOORDPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef GLvoid (APIENTRY * PFNGLVERTEXPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);

#endif /* GL_EXT_vertex_array */

/*
** GL_EXT_vertex_shader
**
**  Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_vertex_shader
#define GL_EXT_vertex_shader                               1

#define GL_VERTEX_SHADER_EXT                               0x8780
#define GL_VERTEX_SHADER_BINDING_EXT                       0x8781
#define GL_OP_INDEX_EXT                                    0x8782
#define GL_OP_NEGATE_EXT                                   0x8783
#define GL_OP_DOT3_EXT                                     0x8784
#define GL_OP_DOT4_EXT                                     0x8785
#define GL_OP_MUL_EXT                                      0x8786
#define GL_OP_ADD_EXT                                      0x8787
#define GL_OP_MADD_EXT                                     0x8788
#define GL_OP_FRAC_EXT                                     0x8789
#define GL_OP_MAX_EXT                                      0x878A
#define GL_OP_MIN_EXT                                      0x878B
#define GL_OP_SET_GE_EXT                                   0x878C
#define GL_OP_SET_LT_EXT                                   0x878D
#define GL_OP_CLAMP_EXT                                    0x878E
#define GL_OP_FLOOR_EXT                                    0x878F
#define GL_OP_ROUND_EXT                                    0x8790
#define GL_OP_EXP_BASE_2_EXT                               0x8791
#define GL_OP_LOG_BASE_2_EXT                               0x8792
#define GL_OP_POWER_EXT                                    0x8793
#define GL_OP_RECIP_EXT                                    0x8794
#define GL_OP_RECIP_SQRT_EXT                               0x8795
#define GL_OP_SUB_EXT                                      0x8796
#define GL_OP_CROSS_PRODUCT_EXT                            0x8797
#define GL_OP_MULTIPLY_MATRIX_EXT                          0x8798
#define GL_OP_MOV_EXT                                      0x8799
#define GL_OUTPUT_VERTEX_EXT                               0x879A
#define GL_OUTPUT_COLOR0_EXT                               0x879B
#define GL_OUTPUT_COLOR1_EXT                               0x879C
#define GL_OUTPUT_TEXTURE_COORD0_EXT                       0x879D
#define GL_OUTPUT_TEXTURE_COORD1_EXT                       0x879E
#define GL_OUTPUT_TEXTURE_COORD2_EXT                       0x879F
#define GL_OUTPUT_TEXTURE_COORD3_EXT                       0x87A0
#define GL_OUTPUT_TEXTURE_COORD4_EXT                       0x87A1
#define GL_OUTPUT_TEXTURE_COORD5_EXT                       0x87A2
#define GL_OUTPUT_TEXTURE_COORD6_EXT                       0x87A3
#define GL_OUTPUT_TEXTURE_COORD7_EXT                       0x87A4
#define GL_OUTPUT_TEXTURE_COORD8_EXT                       0x87A5
#define GL_OUTPUT_TEXTURE_COORD9_EXT                       0x87A6
#define GL_OUTPUT_TEXTURE_COORD10_EXT                      0x87A7
#define GL_OUTPUT_TEXTURE_COORD11_EXT                      0x87A8
#define GL_OUTPUT_TEXTURE_COORD12_EXT                      0x87A9
#define GL_OUTPUT_TEXTURE_COORD13_EXT                      0x87AA
#define GL_OUTPUT_TEXTURE_COORD14_EXT                      0x87AB
#define GL_OUTPUT_TEXTURE_COORD15_EXT                      0x87AC
#define GL_OUTPUT_TEXTURE_COORD16_EXT                      0x87AD
#define GL_OUTPUT_TEXTURE_COORD17_EXT                      0x87AE
#define GL_OUTPUT_TEXTURE_COORD18_EXT                      0x87AF
#define GL_OUTPUT_TEXTURE_COORD19_EXT                      0x87B0
#define GL_OUTPUT_TEXTURE_COORD20_EXT                      0x87B1
#define GL_OUTPUT_TEXTURE_COORD21_EXT                      0x87B2
#define GL_OUTPUT_TEXTURE_COORD22_EXT                      0x87B3
#define GL_OUTPUT_TEXTURE_COORD23_EXT                      0x87B4
#define GL_OUTPUT_TEXTURE_COORD24_EXT                      0x87B5
#define GL_OUTPUT_TEXTURE_COORD25_EXT                      0x87B6
#define GL_OUTPUT_TEXTURE_COORD26_EXT                      0x87B7
#define GL_OUTPUT_TEXTURE_COORD27_EXT                      0x87B8
#define GL_OUTPUT_TEXTURE_COORD28_EXT                      0x87B9
#define GL_OUTPUT_TEXTURE_COORD29_EXT                      0x87BA
#define GL_OUTPUT_TEXTURE_COORD30_EXT                      0x87BB
#define GL_OUTPUT_TEXTURE_COORD31_EXT                      0x87BC
#define GL_OUTPUT_FOG_EXT                                  0x87BD
#define GL_SCALAR_EXT                                      0x87BE
#define GL_VECTOR_EXT                                      0x87BF
#define GL_MATRIX_EXT                                      0x87C0
#define GL_VARIANT_EXT                                     0x87C1
#define GL_INVARIANT_EXT                                   0x87C2
#define GL_LOCAL_CONSTANT_EXT                              0x87C3
#define GL_LOCAL_EXT                                       0x87C4
#define GL_MAX_VERTEX_SHADER_INSTRUCTIONS_EXT              0x87C5
#define GL_MAX_VERTEX_SHADER_VARIANTS_EXT                  0x87C6
#define GL_MAX_VERTEX_SHADER_INVARIANTS_EXT                0x87C7
#define GL_MAX_VERTEX_SHADER_LOCAL_CONSTANTS_EXT           0x87C8
#define GL_MAX_VERTEX_SHADER_LOCALS_EXT                    0x87C9
#define GL_MAX_OPTIMIZED_VERTEX_SHADER_INSTRUCTIONS_EXT    0x87CA
#define GL_MAX_OPTIMIZED_VERTEX_SHADER_VARIANTS_EXT        0x87CB
#define GL_MAX_OPTIMIZED_VERTEX_SHADER_LOCAL_CONSTANTS_EXT 0x87CC
#define GL_MAX_OPTIMIZED_VERTEX_SHADER_INVARIANTS_EXT      0x87CD
#define GL_MAX_OPTIMIZED_VERTEX_SHADER_LOCALS_EXT          0x87CE
#define GL_VERTEX_SHADER_INSTRUCTIONS_EXT                  0x87CF
#define GL_VERTEX_SHADER_VARIANTS_EXT                      0x87D0
#define GL_VERTEX_SHADER_INVARIANTS_EXT                    0x87D1
#define GL_VERTEX_SHADER_LOCAL_CONSTANTS_EXT               0x87D2
#define GL_VERTEX_SHADER_LOCALS_EXT                        0x87D3
#define GL_VERTEX_SHADER_OPTIMIZED_EXT                     0x87D4
#define GL_X_EXT                                           0x87D5
#define GL_Y_EXT                                           0x87D6
#define GL_Z_EXT                                           0x87D7
#define GL_W_EXT                                           0x87D8
#define GL_NEGATIVE_X_EXT                                  0x87D9
#define GL_NEGATIVE_Y_EXT                                  0x87DA
#define GL_NEGATIVE_Z_EXT                                  0x87DB
#define GL_NEGATIVE_W_EXT                                  0x87DC
#define GL_ZERO_EXT                                        0x87DD
#define GL_ONE_EXT                                         0x87DE
#define GL_NEGATIVE_ONE_EXT                                0x87DF
#define GL_NORMALIZED_RANGE_EXT                            0x87E0
#define GL_FULL_RANGE_EXT                                  0x87E1
#define GL_CURRENT_VERTEX_EXT                              0x87E2
#define GL_MVP_MATRIX_EXT                                  0x87E3
#define GL_VARIANT_VALUE_EXT                               0x87E4
#define GL_VARIANT_DATATYPE_EXT                            0x87E5
#define GL_VARIANT_ARRAY_STRIDE_EXT                        0x87E6
#define GL_VARIANT_ARRAY_TYPE_EXT                          0x87E7
#define GL_VARIANT_ARRAY_EXT                               0x87E8
#define GL_VARIANT_ARRAY_POINTER_EXT                       0x87E9
#define GL_INVARIANT_VALUE_EXT                             0x87EA
#define GL_INVARIANT_DATATYPE_EXT                          0x87EB
#define GL_LOCAL_CONSTANT_VALUE_EXT                        0x87EC
#define GL_LOCAL_CONSTANT_DATATYPE_EXT                     0x87ED

typedef GLvoid    (APIENTRY * PFNGLBEGINVERTEXSHADEREXTPROC) (void);
typedef GLvoid    (APIENTRY * PFNGLENDVERTEXSHADEREXTPROC) (void);
typedef GLvoid    (APIENTRY * PFNGLBINDVERTEXSHADEREXTPROC) (GLuint id);
typedef GLuint    (APIENTRY * PFNGLGENVERTEXSHADERSEXTPROC) (GLuint range);
typedef GLvoid    (APIENTRY * PFNGLDELETEVERTEXSHADEREXTPROC) (GLuint id);
typedef GLvoid    (APIENTRY * PFNGLSHADEROP1EXTPROC) (GLenum op, GLuint res, GLuint arg1);
typedef GLvoid    (APIENTRY * PFNGLSHADEROP2EXTPROC) (GLenum op, GLuint res, GLuint arg1,
                                                      GLuint arg2);
typedef GLvoid    (APIENTRY * PFNGLSHADEROP3EXTPROC) (GLenum op, GLuint res, GLuint arg1,
                                                      GLuint arg2, GLuint arg3);
typedef GLvoid    (APIENTRY * PFNGLSWIZZLEEXTPROC) (GLuint res, GLuint in, GLenum outX,
                                                    GLenum outY, GLenum outZ, GLenum outW);
typedef GLvoid    (APIENTRY * PFNGLWRITEMASKEXTPROC) (GLuint res, GLuint in, GLenum outX,
                                                      GLenum outY, GLenum outZ, GLenum outW);
typedef GLvoid    (APIENTRY * PFNGLINSERTCOMPONENTEXTPROC) (GLuint res, GLuint src, GLuint num);
typedef GLvoid    (APIENTRY * PFNGLEXTRACTCOMPONENTEXTPROC) (GLuint res, GLuint src, GLuint num);
typedef GLuint    (APIENTRY * PFNGLGENSYMBOLSEXTPROC) (GLenum dataType, GLenum storageType,
                                                       GLenum range, GLuint components);
typedef GLvoid    (APIENTRY * PFNGLSETINVARIANTEXTPROC) (GLuint id, GLenum type, GLvoid *addr);
typedef GLvoid    (APIENTRY * PFNGLSETLOCALCONSTANTEXTPROC) (GLuint id, GLenum type, GLvoid *addr);
typedef GLvoid    (APIENTRY * PFNGLVARIANTBVEXTPROC) (GLuint id, GLbyte *addr);
typedef GLvoid    (APIENTRY * PFNGLVARIANTSVEXTPROC) (GLuint id, GLshort *addr);
typedef GLvoid    (APIENTRY * PFNGLVARIANTIVEXTPROC) (GLuint id, GLint *addr);
typedef GLvoid    (APIENTRY * PFNGLVARIANTFVEXTPROC) (GLuint id, GLfloat *addr);
typedef GLvoid    (APIENTRY * PFNGLVARIANTDVEXTPROC) (GLuint id, GLdouble *addr);
typedef GLvoid    (APIENTRY * PFNGLVARIANTUBVEXTPROC) (GLuint id, GLubyte *addr);
typedef GLvoid    (APIENTRY * PFNGLVARIANTUSVEXTPROC) (GLuint id, GLushort *addr);
typedef GLvoid    (APIENTRY * PFNGLVARIANTUIVEXTPROC) (GLuint id, GLuint *addr);
typedef GLvoid    (APIENTRY * PFNGLVARIANTPOINTEREXTPROC) (GLuint id, GLenum type,
                                                           GLuint stride, GLvoid *addr);
typedef GLvoid    (APIENTRY * PFNGLENABLEVARIANTCLIENTSTATEEXTPROC) (GLuint id);
typedef GLvoid    (APIENTRY * PFNGLDISABLEVARIANTCLIENTSTATEEXTPROC) (GLuint id);
typedef GLuint    (APIENTRY * PFNGLBINDLIGHTPARAMETEREXTPROC) (GLenum light, GLenum value);
typedef GLuint    (APIENTRY * PFNGLBINDMATERIALPARAMETEREXTPROC) (GLenum face, GLenum value);
typedef GLuint    (APIENTRY * PFNGLBINDTEXGENPARAMETEREXTPROC) (GLenum unit, GLenum coord,
                                                                GLenum value);
typedef GLuint    (APIENTRY * PFNGLBINDTEXTUREUNITPARAMETEREXTPROC) (GLenum unit, GLenum value);
typedef GLuint    (APIENTRY * PFNGLBINDPARAMETEREXTPROC) (GLenum value);
typedef GLboolean (APIENTRY * PFNGLISVARIANTENABLEDEXTPROC) (GLuint id, GLenum cap);
typedef GLvoid    (APIENTRY * PFNGLGETVARIANTBOOLEANVEXTPROC) (GLuint id, GLenum value,
                                                               GLboolean *data);
typedef GLvoid    (APIENTRY * PFNGLGETVARIANTINTEGERVEXTPROC) (GLuint id, GLenum value,
                                                               GLint *data);
typedef GLvoid    (APIENTRY * PFNGLGETVARIANTFLOATVEXTPROC) (GLuint id, GLenum value,
                                                             GLfloat *data);
typedef GLvoid    (APIENTRY * PFNGLGETVARIANTPOINTERVEXTPROC) (GLuint id, GLenum value,
                                                               GLvoid **data);
typedef GLvoid    (APIENTRY * PFNGLGETINVARIANTBOOLEANVEXTPROC) (GLuint id, GLenum value,
                                                                 GLboolean *data);
typedef GLvoid    (APIENTRY * PFNGLGETINVARIANTINTEGERVEXTPROC) (GLuint id, GLenum value,
                                                                 GLint *data);
typedef GLvoid    (APIENTRY * PFNGLGETINVARIANTFLOATVEXTPROC) (GLuint id, GLenum value,
                                                               GLfloat *data);
typedef GLvoid    (APIENTRY * PFNGLGETLOCALCONSTANTBOOLEANVEXTPROC) (GLuint id, GLenum value,
                                                                     GLboolean *data);
typedef GLvoid    (APIENTRY * PFNGLGETLOCALCONSTANTINTEGERVEXTPROC) (GLuint id, GLenum value,
                                                                     GLint *data);
typedef GLvoid    (APIENTRY * PFNGLGETLOCALCONSTANTFLOATVEXTPROC) (GLuint id, GLenum value,
                                                                   GLfloat *data);

#endif /* GL_EXT_vertex_shader */

/*
** GL_EXT_bindable_uniform
**
**  Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Not Supported
**   Radeon X800+  based : Not Supported
**   Radeon X1000+ based : Not Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_EXT_bindable_uniform
#define GL_EXT_bindable_uniform                            1

#define GL_MAX_VERTEX_BINDABLE_UNIFORMS_EXT                0x8DE2
#define GL_MAX_FRAGMENT_BINDABLE_UNIFORMS_EXT              0x8DE3
#define GL_MAX_GEOMETRY_BINDABLE_UNIFORMS_EXT              0x8DE4
#define GL_MAX_BINDABLE_UNIFORM_SIZE_EXT                   0x8DED
#define GL_UNIFORM_BUFFER_BINDING_EXT                      0x8DEF
#define GL_UNIFORM_BUFFER_EXT                              0x8DEE

typedef GLvoid    (APIENTRY * PFNGLUNIFORMBUFFEREXT) (GLuint program, GLint location, GLuint buffer);
typedef GLint     (APIENTRY * PFNGLGETUNIFORMBUFFERSIZEEXT)(GLuint program, GLint location);
typedef GLintptr  (APIENTRY * PFNGLGETUNIFORMOFFSETEXT)(GLuint program, GLint location);

#endif /* GL_EXT_bindable_uniform */

/*
** GL_EXT_texture_integer
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/

#ifndef GL_EXT_texture_integer
#define GL_EXT_texture_integer                         1

#define GL_RGBA32UI_EXT                                         0x8D70
#define GL_RGB32UI_EXT                                          0x8D71
#define GL_RGBA16UI_EXT                                         0x8D76
#define GL_RGB16UI_EXT                                          0x8D77
#define GL_RGBA8UI_EXT                                          0x8D7C
#define GL_RGB8UI_EXT                                           0x8D7D
#define GL_RGBA32I_EXT                                          0x8D82
#define GL_RGB32I_EXT                                           0x8D83
#define GL_RGBA16I_EXT                                          0x8D88
#define GL_RGB16I_EXT                                           0x8D89
#define GL_RGBA8I_EXT                                           0x8D8E
#define GL_RGB8I_EXT                                            0x8D8F
#define GL_RED_INTEGER_EXT                                      0x8D94
#define GL_GREEN_INTEGER_EXT                                    0x8D95
#define GL_BLUE_INTEGER_EXT                                     0x8D96
#define GL_ALPHA_INTEGER_EXT                                    0x8D97
#define GL_RGB_INTEGER_EXT                                      0x8D98
#define GL_RGBA_INTEGER_EXT                                     0x8D99
#define GL_BGR_INTEGER_EXT                                      0x8D9A
#define GL_BGRA_INTEGER_EXT                                     0x8D9B

typedef GLvoid       (APIENTRY * PFNGLTEXPARAMETERIIVEXTPROC) (GLenum target, GLenum pname, const GLint* params);
typedef GLvoid       (APIENTRY * PFNGLTEXPARAMETERIUIVEXTPROC) (GLenum target, GLenum pname, const GLuint* params);
typedef GLvoid       (APIENTRY * PFNGLGETTEXPARAMETERIIVEXTPROC) (GLenum target, GLenum pname, GLint* params);
typedef GLvoid       (APIENTRY * PFNGLGETTEXPARAMETERIUIVEXTPROC) (GLenum target, GLenum pname, GLuint* params);
typedef GLvoid       (APIENTRY * PFNGLCLEARCOLORIIEXTPROC) (GLint red, GLint green, GLint blue, GLint alpha);
typedef GLvoid       (APIENTRY * PFNGLCLEARCOLORIUIEXTPROC) (GLuint red, GLuint green, GLuint blue, GLuint alpha);

#endif /* GL_EXT_texture_integer */

/*
** GL_EXT_texture_array
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Not Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/

#ifndef GL_EXT_texture_array
#define GL_EXT_texture_array                         1

#define GL_TEXTURE_1D_ARRAY_EXT                         0x8C18
#define GL_TEXTURE_2D_ARRAY_EXT                         0x8C1A
#define GL_PROXY_TEXTURE_1D_ARRAY_EXT                   0x8C19
#define GL_PROXY_TEXTURE_2D_ARRAY_EXT                   0x8C1B
#define GL_TEXTURE_BINDING_1D_ARRAY_EXT                 0x8C1C
#define GL_TEXTURE_BINDING_2D_ARRAY_EXT                 0x8C1D
#define GL_MAX_ARRAY_TEXTURE_LAYERS_EXT                 0x88FF
#define GL_COMPARE_REF_DEPTH_TO_TEXTURE_EXT             0x884E
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER_EXT     0x8CD4
#define GL_SAMPLER_1D_ARRAY_EXT                         0x8DC0
#define GL_SAMPLER_2D_ARRAY_EXT                         0x8DC1
#define GL_SAMPLER_1D_ARRAY_SHADOW_EXT                  0x8DC3
#define GL_SAMPLER_2D_ARRAY_SHADOW_EXT                  0x8DC4


typedef GLvoid       (APIENTRY * PFNGLFRAMEBUFFERTEXTURELAYEREXTPROC)(GLenum target, GLenum attachment,
                                                                   GLuint texture,
                                                                   GLint level, GLint layer);

#endif /* GL_ARB_texture_array */


/* 
** GL_EXT_texture_swizzle 
** 
** Support: 
** Rage 128 based : Supported 
** Radeon 7000+ based : Supported 
** Radeon 8500+ based : Supported 
** Radeon 9500+ based : Supported 
** Radeon X800+ based : Supported 
** Radeon X1000+ based : Supported 
** Radeon X2000+ based : Supported 
*/ 
#ifndef GL_EXT_texture_swizzle 
#define GL_EXT_texture_swizzle 1 
#define GL_TEXTURE_SWIZZLE_R_EXT 0x8E42 
#define GL_TEXTURE_SWIZZLE_G_EXT 0x8E43 
#define GL_TEXTURE_SWIZZLE_B_EXT 0x8E44 
#define GL_TEXTURE_SWIZZLE_A_EXT 0x8E45 
#define GL_TEXTURE_SWIZZLE_RGBA_EXT 0x8E46 
  
#endif /* GL_EXT_texture_swizzle */ 



/* 
** GL_ARB_texture_cube_map_array
** 
**  Support:
**   Rage   128     based : Not Supported
**   Radeon 7000+   based : Not Supported
**   Radeon 8500+   based : Not Supported
**   Radeon 9500+   based : Not Supported
**   Radeon X800+   based : Not Supported
**   Radeon X1000+  based : Not Supported
**   Radeon HD2000+ based : Not Supported
**   Radeon HD3000+ based : Supported
**   Radeon HD4000+ based : Supported
*/ 
#ifndef GL_ARB_texture_cube_map_array
#define GL_ARB_texture_cube_map_array 1
#define GL_TEXTURE_CUBE_MAP_ARRAY_ARB                   0x9009
#define GL_TEXTURE_BINDING_CUBE_MAP_ARRAY_ARB           0x900A
#define GL_PROXY_TEXTURE_CUBE_MAP_ARRAY_ARB             0x900B
#define GL_SAMPLER_CUBE_MAP_ARRAY_ARB                   0x900C
#define GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW_ARB            0x900D
#define GL_INT_SAMPLER_CUBE_MAP_ARRAY_ARB               0x900E 
#define GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY_ARB      0x900F 
#endif /* GL_ARB_texture_cube_map_array */

/*
** GL_EXT_texture_buffer_object
**
**  Support:
**   Rage   128     based : Not Supported
**   Radeon 7000+   based : Not Supported
**   Radeon 8500+   based : Not Supported
**   Radeon 9500+   based : Not Supported
**   Radeon X800+   based : Not Supported
**   Radeon X1000+  based : Not Supported
**   Radeon HD2000+ based : Supported
**   Radeon HD3000+ based : Supported
**   Radeon HD4000+ based : Supported
*/


#ifndef GL_EXT_texture_buffer_object
#define GL_EXT_texture_buffer_object 1

#define GL_TEXTURE_BUFFER_EXT                              0x8C2A

#define GL_MAX_TEXTURE_BUFFER_SIZE_EXT                     0x8C2B
#define GL_TEXTURE_BINDING_BUFFER_EXT                      0x8C2C
#define GL_TEXTURE_BUFFER_DATA_STORE_BINDING_EXT           0x8C2D
#define GL_TEXTURE_BUFFER_FORMAT_EXT                       0x8C2E

typedef GLvoid (APIENTRY * PFNGLTEXBUFFEREXTPROC)(GLenum target, GLenum internalformat, GLuint buffer);

#endif

/*
** GL_EXT_copy_buffer
**
**  Support:
**   Rage   128     based : Not Supported
**   Radeon 7000+   based : Not Supported
**   Radeon 8500+   based : Not Supported
**   Radeon 9500+   based : Not Supported
**   Radeon X800+   based : Not Supported
**   Radeon X1000+  based : Not Supported
**   Radeon HD2000+ based : Supported
**   Radeon HD3000+ based : Supported
**   Radeon HD4000+ based : Supported
*/
#ifndef GL_EXT_copy_buffer
#define GL_EXT_copy_buffer  1

#define GL_COPY_READ_BUFFER_EXT                                0x8F36
#define GL_COPY_WRITE_BUFFER_EXT                               0x8F37

typedef GLvoid (APIENTRY * PFNGLCOPYBUFFERSUBDATAEXTPROC)(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);

#endif /* GL_EXT_copy_buffer */

// GL_NV_explicit_multisample
#define GL_SAMPLE_POSITION_NV                              0x8E50
#define GL_SAMPLE_MASK_NV                                  0x8E51
#define GL_SAMPLE_MASK_VALUE_NV                            0x8E52
#define GL_TEXTURE_BINDING_RENDERBUFFER_NV                 0x8E53
#define GL_TEXTURE_RENDERBUFFER_DATA_STORE_BINDING_NV      0x8E54
#define GL_TEXTURE_RENDERBUFFER_NV                         0x8E55
#define GL_SAMPLER_RENDERBUFFER_NV                         0x8E56
#define GL_INT_SAMPLER_RENDERBUFFER_NV                     0x8E57
#define GL_UNSIGNED_INT_SAMPLER_RENDERBUFFER_NV            0x8E58
#define GL_MAX_SAMPLE_MASK_WORDS_NV                        0x8E59

typedef GLvoid (APIENTRY * PFNGLSAMPLEMASKINDEXEDPROC) (GLuint index, GLbitfield value);
typedef GLvoid (APIENTRY * PFNGLGETMULTISAMPLEFVPROC) (GLenum pname, GLuint index, GLfloat* val);
typedef GLvoid (APIENTRY * PFNGLTEXRENDERBUFFERPROC) (GLenum target, GLuint buffer);

// GL_ARB_texture_multisample
#define GL_SAMPLE_POSITION                                 0x8E50
#define GL_SAMPLE_MASK                                     0x8E51
#define GL_SAMPLE_MASK_VALUE                               0x8E52
#define GL_MAX_SAMPLE_MASK_WORDS                           0x8E59
#define GL_TEXTURE_2D_MULTISAMPLE                          0x9100
#define GL_TEXTURE_2D_MULTISAMPLE_ARRAY                    0x9102
#define GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY              0x9103
#define GL_MAX_COLOR_TEXTURE_SAMPLES                       0x910E
#define GL_MAX_DEPTH_TEXTURE_SAMPLES                       0x910F
#define GL_MAX_INTEGER_SAMPLES                             0x9110
#define GL_TEXTURE_BINDING_2D_MULTISAMPLE                  0x9104
#define GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY            0x9105
#define GL_TEXTURE_SAMPLES                                 0x9106
#define GL_TEXTURE_FIXED_SAMPLE_LOCATIONS                  0x9107
#define GL_SAMPLER_2D_MULTISAMPLE                          0x9108
#define GL_INT_SAMPLER_2D_MULTISAMPLE                      0x9109
#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE             0x910A
#define GL_SAMPLER_2D_MULTISAMPLE_ARRAY                    0x910B
#define GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY                0x910C
#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY       0x910D

typedef GLvoid (APIENTRY * PFNGLSAMPLEMASKIPROC) (GLuint index, GLbitfield value);
typedef GLvoid (APIENTRY * PFNGLTEXIMAGE2DMULTISAMPLEPROC) (GLenum target, GLsizei samples, GLint internalformat, 
                                                            GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
typedef GLvoid (APIENTRY * PFNGLTEXIMAGE3DMULTISAMPLEPROC) (GLenum target, GLsizei samples, GLint internalformat, 
                                                            GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations); 

// GL_EXT_timer_query
#define GL_TIME_ELAPSED_EXT               0x88BF

/*
**  IHV EXTENSIONS
*/

/*
** AMD_name_gen_delete
**
** Support: All HW supported
*/
#ifndef AMD_name_gen_delete
#define AMD_name_gen_delete                 1

#define GL_DATA_BUFFER_AMD                               0x9151
#define GL_PERFORMANCE_MONITOR_AMD                       0x9152
#define GL_QUERY_OBJECT_AMD                              0x9153
#define GL_VERTEX_ARRAY_OBJECT_AMD                       0x9154
#define GL_SAMPLER_OBJECT_AMD                            0x9155

#endif /* AMD_name_gen_delete */

/*
** AMD_debug_output
**
** Support: All HW supported
*/

#ifndef GL_AMD_debug_output
#define GL_AMD_debug_output                               1

#define GL_DEBUG_FILTER_CATEGORY_AMD                      0x9140
#define GL_DEBUG_FILTER_SEVERITY_AMD                      0x9141
#define GL_DEBUG_FILTER_ID_AMD                            0x9142

#define GL_MAX_DEBUG_MESSAGE_LENGTH_AMD                   0x9143
#define GL_MAX_DEBUG_LOGGED_MESSAGES_AMD                  0x9144
#define GL_DEBUG_LOGGED_MESSAGES_AMD                      0x9145        

#define GL_DEBUG_SEVERITY_HIGH_AMD                        0x9146
#define GL_DEBUG_SEVERITY_MEDIUM_AMD                      0x9147
#define GL_DEBUG_SEVERITY_LOW_AMD                         0x9148

#define GL_DEBUG_CATEGORY_API_ERROR_AMD                   0x9149
#define GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD               0x914A
#define GL_DEBUG_CATEGORY_DEPRECATION_AMD                 0x914B
#define GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD          0x914C
#define GL_DEBUG_CATEGORY_PERFORMANCE_AMD                 0x914D
#define GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD             0x914E
#define GL_DEBUG_CATEGORY_APPLICATION_AMD                 0x914F
#define GL_DEBUG_CATEGORY_OTHER_AMD                       0x9150

typedef GLvoid (APIENTRY * GLDEBUGPROCAMD)(GLuint id,
                                           GLenum category,
                                           GLenum severity,
                                           GLsizei length,
                                           const GLchar* message,
                                           GLvoid* userParam);

typedef GLvoid (APIENTRY * PFNGLDEBUGMESSAGEENABLEAMDPROC)(GLenum category,
                                                           GLenum severity,
                                                           GLsizei count,
                                                           const GLuint* ids,
                                                           GLboolean enabled);

typedef GLvoid (APIENTRY * PFNGLDEBUGMESSAGEINSERTAMDPROC)(GLenum category,
                                                           GLenum severity,
                                                           GLuint id,
                                                           GLsizei length, 
                                                           const GLchar* buf);

typedef GLvoid (APIENTRY * PFNGLDEBUGMESSAGECALLBACKAMDPROC)(GLvoid* param, GLvoid* userParam);

typedef GLuint (APIENTRY * PFNGETDEBUGMESSAGELOGAMDPROC)(GLuint count,
                                                         GLsizei bufsize,
                                                         GLenum* categories,
                                                         GLuint* severities,
                                                         GLuint* ids,
                                                         GLsizei* lengths, 
                                                         GLchar* message);

#endif

#ifndef GL_AMD_sample_positions
#define GL_AMD_sample_positions

#define GL_SUBSAMPLE_DISTANCE_AMD              0x883F

#endif //GL_AMD_sample_positions

/*
** HP_occlusion_test
**
** Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_HP_occlusion_test
#define GL_HP_occlusion_test                 1

#define GL_OCCLUSION_TEST_HP                 0x8165
#define GL_OCCLUSION_TEST_RESULT_HP          0x8166

#endif  /* HP_occlusion_test */

/*
** GL_NV_blend_square
**
**  Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_NV_blend_square
#define GL_NV_blend_square                   1

#endif /* GL_NV_blend_square */

/*
** GL_NV_occlusion_query
**
** Support:
**
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Not Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_NV_occlusion_query
#define GL_NV_occlusion_query              1

#define GL_PIXEL_COUNTER_BITS_NV           0x8864
#define GL_CURRENT_OCCLUSION_QUERY_ID_NV   0x8865
#define GL_PIXEL_COUNT_NV                  0x8866
#define GL_PIXEL_COUNT_AVAILABLE_NV        0x8867

typedef GLvoid    (APIENTRY * PFNGLGENOCCLUSIONQUERIESNVPROC) (GLsizei n, GLuint *ids);
typedef GLvoid    (APIENTRY * PFNGLDELETEOCCLUSIONQUERIESNVPROC) (GLsizei n, const GLuint *ids);
typedef GLboolean (APIENTRY * PFNGLISOCCLUSIONQUERYNVPROC) (GLuint id);
typedef GLvoid    (APIENTRY * PFNGLBEGINOCCLUSIONQUERYNVPROC) (GLuint id);
typedef GLvoid    (APIENTRY * PFNGLENDOCCLUSIONQUERYNVPROC) (void);
typedef GLvoid    (APIENTRY * PFNGLGETOCCLUSIONQUERYIVNVPROC) (GLuint id, GLenum pname, GLint *params);
typedef GLvoid    (APIENTRY * PFNGLGETOCCLUSIONQUERYUIVNVPROC) (GLuint id, GLenum pname, GLuint *params);

#endif /* GL_NV_occlusion_query */

/*
** GL_NV_texgen_reflection
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_NV_texgen_reflection
#define GL_NV_texgen_reflection              1

#define GL_NORMAL_MAP_NV                     0x8511
#define GL_REFLECTION_MAP_NV                 0x8512

#endif /* GL_NV_texgen_reflection */

/*
** GL_S3_s3tc
**
**  Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_S3_s3tc
#define GL_S3_s3tc                           1

#define GL_RGB_S3TC                          0x83A0
#define GL_RGB4_S3TC                         0x83A1
#define GL_RGBA_S3TC                         0x83A2
#define GL_RGBA4_S3TC                        0x83A3

#endif /* GL_S3_s3tc */

/*
** GL_SGI_color_matrix
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_SGI_color_matrix
#define GL_SGI_color_matrix                     1

#define GL_COLOR_MATRIX_SGI                     0x80B1
#define GL_COLOR_MATRIX_STACK_DEPTH_SGI         0x80B2
#define GL_MAX_COLOR_MATRIX_STACK_DEPTH_SGI     0x80B3
#define GL_POST_COLOR_MATRIX_RED_SCALE_SGI      0x80B4
#define GL_POST_COLOR_MATRIX_GREEN_SCALE_SGI    0x80B5
#define GL_POST_COLOR_MATRIX_BLUE_SCALE_SGI     0x80B6
#define GL_POST_COLOR_MATRIX_ALPHA_SCALE_SGI    0x80B7
#define GL_POST_COLOR_MATRIX_RED_BIAS_SGI       0x80B8
#define GL_POST_COLOR_MATRIX_GREEN_BIAS_SGI     0x80B9
#define GL_POST_COLOR_MATRIX_BLUE_BIAS_SGI      0x80BA
#define GL_POST_COLOR_MATRIX_ALPHA_BIAS_SGI     0x80BB

#endif /* GL_SGI_color_matrix */

/*
** GL_SGIS_generate_mipmap
**
**  Support:
**   Rage   128    based : Not Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_SGIS_generate_mipmap
#define GL_SGIS_generate_mipmap              1

#define GL_GENERATE_MIPMAP_SGIS              0x8191
#define GL_GENERATE_MIPMAP_HINT_SGIS         0x8192

#endif /* GL_SGIS_generate_mipmap */

/*
** GL_SGIS_multitexture
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_SGIS_multitexture
#define GL_SGIS_multitexture                1

#define TEXTURE0_SGIS                       0x835E
#define TEXTURE1_SGIS                       0x835F
#define GL_SELECTED_TEXTURE_SGIS            0x83C0
#define GL_SELECTED_TEXTURE_COORD_SET_SGIS  0x83C1
#define GL_SELECTED_TEXTURE_TRANSFORM_SGIS  0x83C2
#define GL_MAX_TEXTURES_SGIS                0x83C3
#define GL_MAX_TEXTURE_COORD_SETS_SGIS      0x83C4
#define GL_TEXTURE_ENV_COORD_SET_SGIS       0x83C5
#define GL_TEXTURE0_SGIS                    0x83C6
#define GL_TEXTURE1_SGIS                    0x83C7
#define GL_TEXTURE2_SGIS                    0x83C8
#define GL_TEXTURE3_SGIS                    0x83C9
#define GL_TEXTURE4_SGIS                    0x83CA
#define GL_TEXTURE5_SGIS                    0x83CB
#define GL_TEXTURE6_SGIS                    0x83CC
#define GL_TEXTURE7_SGIS                    0x83CD
#define GL_TEXTURE8_SGIS                    0x83CE
#define GL_TEXTURE9_SGIS                    0x83CF
#define GL_TEXTURE10_SGIS                   0x83D0
#define GL_TEXTURE11_SGIS                   0x83D1
#define GL_TEXTURE12_SGIS                   0x83D2
#define GL_TEXTURE13_SGIS                   0x83D3
#define GL_TEXTURE14_SGIS                   0x83D4
#define GL_TEXTURE15_SGIS                   0x83D5
#define GL_TEXTURE16_SGIS                   0x83D6
#define GL_TEXTURE17_SGIS                   0x83D7
#define GL_TEXTURE18_SGIS                   0x83D8
#define GL_TEXTURE19_SGIS                   0x83D9
#define GL_TEXTURE20_SGIS                   0x83DA
#define GL_TEXTURE21_SGIS                   0x83DB
#define GL_TEXTURE22_SGIS                   0x83DC
#define GL_TEXTURE23_SGIS                   0x83DD
#define GL_TEXTURE24_SGIS                   0x83DE
#define GL_TEXTURE25_SGIS                   0x83DF
#define GL_TEXTURE26_SGIS                   0x83E0
#define GL_TEXTURE27_SGIS                   0x83E1
#define GL_TEXTURE28_SGIS                   0x83E2
#define GL_TEXTURE29_SGIS                   0x83E3
#define GL_TEXTURE30_SGIS                   0x83E4
#define GL_TEXTURE31_SGIS                   0x83E5

typedef GLvoid (APIENTRY * PFNGLSELECTTEXTURESGISPROC) (GLenum texture);
typedef GLvoid (APIENTRY * PFNGLSELECTTEXTURETRANSFORMSGISPROC) (GLenum texture);
typedef GLvoid (APIENTRY * PFNGLSELECTTEXTURECOORDSETSGISPROC) (GLenum texture);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD1DSGISPROC) (GLenum texture, GLdouble s);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD1DVSGISPROC) (GLenum texture, const GLdouble *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD1FSGISPROC) (GLenum texture, GLfloat s);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD1FVSGISPROC) (GLenum texture, const GLfloat *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD1ISGISPROC) (GLenum texture, GLint s);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD1IVSGISPROC) (GLenum texture, const GLint *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD1SSGISPROC) (GLenum texture, GLshort s);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD1SVSGISPROC) (GLenum texture, const GLshort *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD2DSGISPROC) (GLenum texture, GLdouble s, GLdouble t);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD2DVSGISPROC) (GLenum texture, const GLdouble *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD2FSGISPROC) (GLenum texture, GLfloat s, GLfloat t);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD2FVSGISPROC) (GLenum texture, const GLfloat *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD2ISGISPROC) (GLenum texture, GLint s, GLint t);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD2IVSGISPROC) (GLenum texture, const GLint *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD2SSGISPROC) (GLenum texture, GLshort s, GLshort t);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD2SVSGISPROC) (GLenum texture, const GLshort *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD3DSGISPROC) (GLenum texture, GLdouble s, GLdouble t, GLdouble r);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD3DVSGISPROC) (GLenum texture, const GLdouble *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD3FSGISPROC) (GLenum texture, GLfloat s, GLfloat t, GLfloat r);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD3FVSGISPROC) (GLenum texture, const GLfloat *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD3ISGISPROC) (GLenum texture, GLint s, GLint t, GLint r);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD3IVSGISPROC) (GLenum texture, const GLint *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD3SSGISPROC) (GLenum texture, GLshort s, GLshort t, GLshort r);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD3SVSGISPROC) (GLenum texture, const GLshort *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD4DSGISPROC) (GLenum texture, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD4DVSGISPROC) (GLenum texture, const GLdouble *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD4FSGISPROC) (GLenum texture, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD4FVSGISPROC) (GLenum texture, const GLfloat *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD4ISGISPROC) (GLenum texture, GLint s, GLint t, GLint r, GLint q);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD4IVSGISPROC) (GLenum texture, const GLint *v);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD4SSGISPROC) (GLenum texture, GLshort s, GLshort t, GLshort r, GLshort q);
typedef GLvoid (APIENTRY * PFNGLMULTITEXCOORD4SVSGISPROC) (GLenum texture, const GLshort *v);

#endif /* GL_SGIS_multitexture */

/*
** GL_SGIS_texture_border_clamp
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_SGIS_texture_border_clamp
#define GL_SGIS_texture_border_clamp         1

#define GL_CLAMP_TO_BORDER_SGIS              0x812D

#endif

/*
** GL_SGIS_texture_edge_clamp
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_SGIS_texture_edge_clamp
#define GL_SGIS_texture_edge_clamp            1

#define GL_CLAMP_TO_EDGE_SGIS                 0x812F

#endif /* GL_SGIS_texture_edge_clamp */

/*
** GL_SGIS_texture_lod
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_SGIS_texture_lod
#define GL_SGIS_texture_lod                  1

#define GL_TEXTURE_MIN_LOD_SGIS              0x813A
#define GL_TEXTURE_MAX_LOD_SGIS              0x813B
#define GL_TEXTURE_BASE_LEVEL_SGIS           0x813C
#define GL_TEXTURE_MAX_LEVEL_SGIS            0x813D

#endif /* GL_SGIS_texture_lod */

/*
** GL_SUN_multi_draw_arrays
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_SUN_multi_draw_arrays
#define GL_SUN_multi_draw_arrays              1

typedef GLvoid (APIENTRY * PFNGLMULTIDRAWARRAYSSUNPROC) (GLenum mode, GLint *first, GLsizei *count, GLsizei primcount);
typedef GLvoid (APIENTRY * PFNGLMULTIDRAWELEMENTSSUNPROC) (GLenum mode, GLsizei *count, GLenum type, const GLvoid **indices, GLsizei primcount);

#endif /* GL_SUN_multi_draw_arrays */

/*
**  ISV EXTENSIONS
*/

/*
** GL_KTX_buffer_region
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef __linux__
#ifndef GL_KTX_buffer_region
#define GL_KTX_buffer_region                 1

#define GL_KTX_FRONT_REGION                  0x0
#define GL_KTX_BACK_REGION                   0x1
#define GL_KTX_Z_REGION                      0x2
#define GL_KTX_STENCIL_REGION                0x3

typedef GLuint (APIENTRY * PFNGLBUFFERREGIONENABLEDEXTPROC) (void);
typedef HANDLE (APIENTRY * PFNGLNEWBUFFERREGIONEXTPROC) (GLenum region);
typedef GLvoid (APIENTRY * PFNGLDELETEBUFFERREGIONEXTPROC) (HANDLE hRegion);
typedef GLvoid (APIENTRY * PFNGLREADBUFFERREGIONEXTPROC) (HANDLE hRegion, GLint x, GLint y, GLsizei width, GLsizei height);
typedef GLvoid (APIENTRY * PFNGLDRAWBUFFERREGIONEXTPROC) (HANDLE hRegion, GLint x, GLint y, GLsizei width, GLsizei height, GLint xDest, GLint yDest);

#endif /* GL_KTX_buffer_region */
#endif // !__linux__

/*
** GL_WIN_swap_hint
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef __linux__
#ifndef GL_WIN_swap_hint
#define GL_WIN_swap_hint                     1

typedef GLvoid (APIENTRY * PFNGLADDSWAPHINTRECTWINPROC) (GLint x, GLint y, GLsizei width, GLsizei height);

#endif /* GL_WIN_swap_hint */
#endif // !__linux__

/*
** GL_APPLE_client_storage
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_APPLE_client_storage
#define GL_APPLE_client_storage                 1

#define GL_UNPACK_CLIENT_STORAGE_APPLE       0x85B2

#endif /* GL_APPLE_client_storage */

/*
** GL_APPLE_texture_range
**
**  Support:
**   Rage   128    based : Supported
**   Radeon 7000+  based : Supported
**   Radeon 8500+  based : Supported
**   Radeon 9500+  based : Supported
**   Radeon X800+  based : Supported
**   Radeon X1000+ based : Supported
**   Radeon X2000+ based : Supported
*/
#ifndef GL_APPLE_texture_range
#define GL_APPLE_texture_range               1

#define GL_TEXTURE_RANGE_LENGTH_APPLE        0x85B7
#define GL_TEXTURE_RANGE_POINTER_APPLE       0x85B8
#define GL_TEXTURE_STORAGE_HINT_APPLE        0x85BC
#define GL_STORAGE_PRIVATE_APPLE             0x85BD
#define GL_STORAGE_CACHED_APPLE              0x85BE
#define GL_STORAGE_SHARED_APPLE              0x85BF

typedef GLvoid (APIENTRY * PFNGLTEXTURERANGEAPPLEPROC)(GLenum target, GLsizei length, GLvoid *pointer);
typedef GLvoid (APIENTRY * PFNGLGETTEXPARAMETERPOINTERVAPPLEPROC)(GLenum target, GLenum pname, GLvoid **params);

#endif /* GL_APPLE_texture_range */

/*
** GL_ARB_depth_clamp
**
**  Support:
**   6XX and beyond
*/
#ifndef GL_ARB_depth_clamp
#define GL_ARB_depth_clamp

#define GL_DEPTH_CLAMP                       0x864F

#endif /* GL_ARB_depth_clamp */

/*
** GL_ARB_seamless_cube_map
**
**  Support:
**   6XX and beyond
*/
#ifndef GL_ARB_seamless_cube_map
#define GL_ARB_seamless_cube_map

#define GL_TEXTURE_CUBE_MAP_SEAMLESS         0x884F

#endif /* GL_ARB_seamless_cube_map */

/*
** AMD_pinned_memory
**
** Support: All HW supported
*/
#ifndef AMD_pinned_memory
#define AMD_pinned_memory

#define GL_EXTERNAL_VIRTUAL_MEMORY_AMD      0x9160

#endif /* AMD_pinned_memory */

/*
** AMD_bus_addressable_memory
**
** Support: All Workstation HW supported
*/
#ifndef AMD_bus_addressable_memory
#define AMD_bus_addressable_memory

#define GL_BUS_ADDRESSABLE_MEMORY_AMD       0x9168
#define GL_EXTERNAL_PHYSICAL_MEMORY_AMD     0x9169

typedef GLvoid (APIENTRY * PFNGLWAITMARKERAMDPROC)  (GLuint buffer, GLuint marker);
typedef GLvoid (APIENTRY * PFNGLWRITEMARKERAMDPROC) (GLuint buffer, GLuint marker, GLuint64 offset);
typedef GLvoid (APIENTRY * PFNGLMAKEBUFFERSRESIDENTAMDPROC) (GLsizei count, GLuint* buffers, GLuint64* baddrs, GLuint64* maddrs);
typedef GLvoid (APIENTRY * PFNGLBUFFERBUSADDRESSAMDPROC)    (GLenum target, GLsizeiptr size, GLuint64 baddrs, GLuint64 maddrs);

#endif /* AMD_bus_addressable_memory */

/* ARB_debug_output */
#define GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB                   0x8242

#define GL_MAX_DEBUG_MESSAGE_LENGTH_ARB                   0x9143
#define GL_MAX_DEBUG_LOGGED_MESSAGES_ARB                  0x9144
#define GL_DEBUG_LOGGED_MESSAGES_ARB                      0x9145
#define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB           0x8243

#define GL_DEBUG_CALLBACK_FUNCTION_ARB                    0x8244
#define GL_DEBUG_CALLBACK_USER_PARAM_ARB                  0x8245

#define GL_DEBUG_SOURCE_API_ARB                           0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB                 0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER_ARB               0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY_ARB                   0x8249
#define GL_DEBUG_SOURCE_APPLICATION_ARB                   0x824A
#define GL_DEBUG_SOURCE_OTHER_ARB                         0x824B

#define GL_DEBUG_TYPE_ERROR_ARB                           0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB             0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB              0x824E
#define GL_DEBUG_TYPE_PORTABILITY_ARB                     0x824F
#define GL_DEBUG_TYPE_PERFORMANCE_ARB                     0x8250
#define GL_DEBUG_TYPE_OTHER_ARB                           0x8251

#define GL_DEBUG_SEVERITY_HIGH_ARB                        0x9146
#define GL_DEBUG_SEVERITY_MEDIUM_ARB                      0x9147
#define GL_DEBUG_SEVERITY_LOW_ARB                         0x9148

typedef GLvoid (APIENTRY * PFNGLDEBUGMESSAGECONTROLARB)(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint* ids, GLboolean enabled);
typedef GLvoid (APIENTRY * PFNGLDEBUGMESSAGEINSERTARB)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* buf);
typedef GLvoid (APIENTRY * PFNGLDEBUGMESSAGECALLBACKARB)(GLvoid* callback, GLvoid* userParam);
typedef GLuint (APIENTRY * PFNGLGETDEBUGMESSAGELOGARB)(GLuint count, GLsizei bufsize, GLenum* sources, GLenum* types, GLuint* ids, GLenum* severities, GLsizei* lengths, GLchar* messageLog);

typedef void (APIENTRY *GLDEBUGPROCARB)(GLenum source,
                                        GLenum type,
                                        GLuint id,
                                        GLenum severity,
                                        GLsizei length,
                                        const GLchar* message,
                                        GLvoid* userParam);
// end ARB_debug_output

/* AMD_array_buffer */
typedef GLint  (APIENTRY * PFNGLGETBUFFERARRAYINDEXAMDPROC)(GLuint program, const GLchar * name);
typedef GLvoid (APIENTRY * PFNGLARRAYBUFFERBINDINGAMDPROC)(GLuint program, GLuint arrayBufferIndex, GLuint arrayBufferBinding);
// END AMD_array_buffer
/*
** EXT_texture_storage
**
** Support: All HW supported
*/

#ifndef GL_EXT_texture_storage
#define GL_EXT_texture_storage                            1

typedef GLvoid (APIENTRY * PFNGLTEXSTORAGE1DEXT) (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width);
typedef GLvoid (APIENTRY * PFNGLTEXSTORAGE2DEXT) (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
typedef GLvoid (APIENTRY * PFNGLTEXSTORAGE3DEXT) (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
typedef GLvoid (APIENTRY * PFNGLTEXTURESTORAGE1DEXT) (GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width);
typedef GLvoid (APIENTRY * PFNGLTEXTURESTORAGE2DEXT) (GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
typedef GLvoid (APIENTRY * PFNGLTEXTURESTORAGE3DEXT) (GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);

#define GL_TEXTURE_IMMUTABLE_FORMAT_EXT   0x912F

#endif // end GL_EXT_texture_storage

/*AMD_compute_shader*/
#define GL_COMPUTE_SHADER_AMD                             0x6969

typedef GLvoid (APIENTRY *PFNGLDISPATCHCOMPUTEAMDPROC)(GLuint program, 
                                                       GLuint workdim, 
                                                       const GLuint* global_work_size, 
                                                       const GLuint* local_work_size);
// end AMD_compute_shader

/*AMD_transform_feedback4*/

#define GL_STREAM_RASTERIZATION_AMD                       0x9380 // strat of any_vendor_future_use, should be modified atfer the spec is released 
// end AMD_transform_feedback4

#ifdef __cplusplus
}
#endif


#endif /* __gl_ATI_h_ */
