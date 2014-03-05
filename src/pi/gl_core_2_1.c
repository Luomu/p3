#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "gl_core_2_1.h"

#if defined(__APPLE__)
#include <mach-o/dyld.h>

static void* AppleGLGetProcAddress (const GLubyte *name)
{
  static const struct mach_header* image = NULL;
  NSSymbol symbol;
  char* symbolName;
  if (NULL == image)
  {
    image = NSAddImage("/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL", NSADDIMAGE_OPTION_RETURN_ON_ERROR);
  }
  /* prepend a '_' for the Unix C symbol mangling convention */
  symbolName = malloc(strlen((const char*)name) + 2);
  strcpy(symbolName+1, (const char*)name);
  symbolName[0] = '_';
  symbol = NULL;
  /* if (NSIsSymbolNameDefined(symbolName))
	 symbol = NSLookupAndBindSymbol(symbolName); */
  symbol = image ? NSLookupSymbolInImage(image, symbolName, NSLOOKUPSYMBOLINIMAGE_OPTION_BIND | NSLOOKUPSYMBOLINIMAGE_OPTION_RETURN_ON_ERROR) : NULL;
  free(symbolName);
  return symbol ? NSAddressOfSymbol(symbol) : NULL;
}
#endif /* __APPLE__ */

#if defined(__sgi) || defined (__sun)
#include <dlfcn.h>
#include <stdio.h>

static void* SunGetProcAddress (const GLubyte* name)
{
  static void* h = NULL;
  static void* gpa;

  if (h == NULL)
  {
    if ((h = dlopen(NULL, RTLD_LAZY | RTLD_LOCAL)) == NULL) return NULL;
    gpa = dlsym(h, "glXGetProcAddress");
  }

  if (gpa != NULL)
    return ((void*(*)(const GLubyte*))gpa)(name);
  else
    return dlsym(h, (const char*)name);
}
#endif /* __sgi || __sun */

#if defined(_WIN32)

#ifdef _MSC_VER
#pragma warning(disable: 4055)
#pragma warning(disable: 4054)
#endif

static int TestPointer(const PROC pTest)
{
	ptrdiff_t iTest;
	if(!pTest) return 0;
	iTest = (ptrdiff_t)pTest;
	
	if(iTest == 1 || iTest == 2 || iTest == 3 || iTest == -1) return 0;
	
	return 1;
}

static PROC WinGetProcAddress(const char *name)
{
	HMODULE glMod = NULL;
	PROC pFunc = wglGetProcAddress((LPCSTR)name);
	if(TestPointer(pFunc))
	{
		return pFunc;
	}
	glMod = GetModuleHandleA("OpenGL32.dll");
	return (PROC)GetProcAddress(glMod, (LPCSTR)name);
}
	
#define IntGetProcAddress(name) WinGetProcAddress(name)
#else
	#if defined(__APPLE__)
		#define IntGetProcAddress(name) AppleGLGetProcAddress(name)
	#else
		#if defined(__sgi) || defined(__sun)
			#define IntGetProcAddress(name) SunGetProcAddress(name)
		#else /* GLX */
		    #include <GL/glx.h>

			#define IntGetProcAddress(name) (*glXGetProcAddressARB)((const GLubyte*)name)
		#endif
	#endif
#endif

int ogl_ext_EXT_framebuffer_object = ogl_LOAD_FAILED;
int ogl_ext_EXT_texture_compression_s3tc = ogl_LOAD_FAILED;
int ogl_ext_KHR_debug = ogl_LOAD_FAILED;
int ogl_ext_ARB_debug_output = ogl_LOAD_FAILED;
int ogl_ext_EXT_texture_sRGB = ogl_LOAD_FAILED;
int ogl_ext_EXT_texture_filter_anisotropic = ogl_LOAD_FAILED;
int ogl_ext_ARB_framebuffer_sRGB = ogl_LOAD_FAILED;

void (CODEGEN_FUNCPTR *_ptrc_glBindFramebufferEXT)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindRenderbufferEXT)(GLenum, GLuint) = NULL;
GLenum (CODEGEN_FUNCPTR *_ptrc_glCheckFramebufferStatusEXT)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteFramebuffersEXT)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteRenderbuffersEXT)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFramebufferRenderbufferEXT)(GLenum, GLenum, GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFramebufferTexture1DEXT)(GLenum, GLenum, GLenum, GLuint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFramebufferTexture2DEXT)(GLenum, GLenum, GLenum, GLuint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFramebufferTexture3DEXT)(GLenum, GLenum, GLenum, GLuint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenFramebuffersEXT)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenRenderbuffersEXT)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenerateMipmapEXT)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetFramebufferAttachmentParameterivEXT)(GLenum, GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetRenderbufferParameterivEXT)(GLenum, GLenum, GLint *) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsFramebufferEXT)(GLuint) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsRenderbufferEXT)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRenderbufferStorageEXT)(GLenum, GLenum, GLsizei, GLsizei) = NULL;

static int Load_EXT_framebuffer_object()
{
	int numFailed = 0;
	_ptrc_glBindFramebufferEXT = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glBindFramebufferEXT");
	if(!_ptrc_glBindFramebufferEXT) numFailed++;
	_ptrc_glBindRenderbufferEXT = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glBindRenderbufferEXT");
	if(!_ptrc_glBindRenderbufferEXT) numFailed++;
	_ptrc_glCheckFramebufferStatusEXT = (GLenum (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glCheckFramebufferStatusEXT");
	if(!_ptrc_glCheckFramebufferStatusEXT) numFailed++;
	_ptrc_glDeleteFramebuffersEXT = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteFramebuffersEXT");
	if(!_ptrc_glDeleteFramebuffersEXT) numFailed++;
	_ptrc_glDeleteRenderbuffersEXT = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteRenderbuffersEXT");
	if(!_ptrc_glDeleteRenderbuffersEXT) numFailed++;
	_ptrc_glFramebufferRenderbufferEXT = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLuint))IntGetProcAddress("glFramebufferRenderbufferEXT");
	if(!_ptrc_glFramebufferRenderbufferEXT) numFailed++;
	_ptrc_glFramebufferTexture1DEXT = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLuint, GLint))IntGetProcAddress("glFramebufferTexture1DEXT");
	if(!_ptrc_glFramebufferTexture1DEXT) numFailed++;
	_ptrc_glFramebufferTexture2DEXT = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLuint, GLint))IntGetProcAddress("glFramebufferTexture2DEXT");
	if(!_ptrc_glFramebufferTexture2DEXT) numFailed++;
	_ptrc_glFramebufferTexture3DEXT = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLuint, GLint, GLint))IntGetProcAddress("glFramebufferTexture3DEXT");
	if(!_ptrc_glFramebufferTexture3DEXT) numFailed++;
	_ptrc_glGenFramebuffersEXT = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenFramebuffersEXT");
	if(!_ptrc_glGenFramebuffersEXT) numFailed++;
	_ptrc_glGenRenderbuffersEXT = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenRenderbuffersEXT");
	if(!_ptrc_glGenRenderbuffersEXT) numFailed++;
	_ptrc_glGenerateMipmapEXT = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glGenerateMipmapEXT");
	if(!_ptrc_glGenerateMipmapEXT) numFailed++;
	_ptrc_glGetFramebufferAttachmentParameterivEXT = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLint *))IntGetProcAddress("glGetFramebufferAttachmentParameterivEXT");
	if(!_ptrc_glGetFramebufferAttachmentParameterivEXT) numFailed++;
	_ptrc_glGetRenderbufferParameterivEXT = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetRenderbufferParameterivEXT");
	if(!_ptrc_glGetRenderbufferParameterivEXT) numFailed++;
	_ptrc_glIsFramebufferEXT = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsFramebufferEXT");
	if(!_ptrc_glIsFramebufferEXT) numFailed++;
	_ptrc_glIsRenderbufferEXT = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsRenderbufferEXT");
	if(!_ptrc_glIsRenderbufferEXT) numFailed++;
	_ptrc_glRenderbufferStorageEXT = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLsizei, GLsizei))IntGetProcAddress("glRenderbufferStorageEXT");
	if(!_ptrc_glRenderbufferStorageEXT) numFailed++;
	return numFailed;
}

void (CODEGEN_FUNCPTR *_ptrc_glDebugMessageCallback)(GLDEBUGPROC, const void *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDebugMessageControl)(GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDebugMessageInsert)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *) = NULL;
GLuint (CODEGEN_FUNCPTR *_ptrc_glGetDebugMessageLog)(GLuint, GLsizei, GLenum *, GLenum *, GLuint *, GLenum *, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetObjectLabel)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetObjectPtrLabel)(const void *, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetPointerv)(GLenum, GLvoid **) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glObjectLabel)(GLenum, GLuint, GLsizei, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glObjectPtrLabel)(const void *, GLsizei, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPopDebugGroup)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPushDebugGroup)(GLenum, GLuint, GLsizei, const GLchar *) = NULL;

static int Load_KHR_debug()
{
	int numFailed = 0;
	_ptrc_glDebugMessageCallback = (void (CODEGEN_FUNCPTR *)(GLDEBUGPROC, const void *))IntGetProcAddress("glDebugMessageCallback");
	if(!_ptrc_glDebugMessageCallback) numFailed++;
	_ptrc_glDebugMessageControl = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean))IntGetProcAddress("glDebugMessageControl");
	if(!_ptrc_glDebugMessageControl) numFailed++;
	_ptrc_glDebugMessageInsert = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *))IntGetProcAddress("glDebugMessageInsert");
	if(!_ptrc_glDebugMessageInsert) numFailed++;
	_ptrc_glGetDebugMessageLog = (GLuint (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLenum *, GLenum *, GLuint *, GLenum *, GLsizei *, GLchar *))IntGetProcAddress("glGetDebugMessageLog");
	if(!_ptrc_glGetDebugMessageLog) numFailed++;
	_ptrc_glGetObjectLabel = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetObjectLabel");
	if(!_ptrc_glGetObjectLabel) numFailed++;
	_ptrc_glGetObjectPtrLabel = (void (CODEGEN_FUNCPTR *)(const void *, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetObjectPtrLabel");
	if(!_ptrc_glGetObjectPtrLabel) numFailed++;
	_ptrc_glGetPointerv = (void (CODEGEN_FUNCPTR *)(GLenum, GLvoid **))IntGetProcAddress("glGetPointerv");
	if(!_ptrc_glGetPointerv) numFailed++;
	_ptrc_glObjectLabel = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLsizei, const GLchar *))IntGetProcAddress("glObjectLabel");
	if(!_ptrc_glObjectLabel) numFailed++;
	_ptrc_glObjectPtrLabel = (void (CODEGEN_FUNCPTR *)(const void *, GLsizei, const GLchar *))IntGetProcAddress("glObjectPtrLabel");
	if(!_ptrc_glObjectPtrLabel) numFailed++;
	_ptrc_glPopDebugGroup = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glPopDebugGroup");
	if(!_ptrc_glPopDebugGroup) numFailed++;
	_ptrc_glPushDebugGroup = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLsizei, const GLchar *))IntGetProcAddress("glPushDebugGroup");
	if(!_ptrc_glPushDebugGroup) numFailed++;
	return numFailed;
}

void (CODEGEN_FUNCPTR *_ptrc_glDebugMessageCallbackARB)(GLDEBUGPROCARB, const void *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDebugMessageControlARB)(GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDebugMessageInsertARB)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *) = NULL;
GLuint (CODEGEN_FUNCPTR *_ptrc_glGetDebugMessageLogARB)(GLuint, GLsizei, GLenum *, GLenum *, GLuint *, GLenum *, GLsizei *, GLchar *) = NULL;

static int Load_ARB_debug_output()
{
	int numFailed = 0;
	_ptrc_glDebugMessageCallbackARB = (void (CODEGEN_FUNCPTR *)(GLDEBUGPROCARB, const void *))IntGetProcAddress("glDebugMessageCallbackARB");
	if(!_ptrc_glDebugMessageCallbackARB) numFailed++;
	_ptrc_glDebugMessageControlARB = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean))IntGetProcAddress("glDebugMessageControlARB");
	if(!_ptrc_glDebugMessageControlARB) numFailed++;
	_ptrc_glDebugMessageInsertARB = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *))IntGetProcAddress("glDebugMessageInsertARB");
	if(!_ptrc_glDebugMessageInsertARB) numFailed++;
	_ptrc_glGetDebugMessageLogARB = (GLuint (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLenum *, GLenum *, GLuint *, GLenum *, GLsizei *, GLchar *))IntGetProcAddress("glGetDebugMessageLogARB");
	if(!_ptrc_glGetDebugMessageLogARB) numFailed++;
	return numFailed;
}

void (CODEGEN_FUNCPTR *_ptrc_glAccum)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glAlphaFunc)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBegin)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBitmap)(GLsizei, GLsizei, GLfloat, GLfloat, GLfloat, GLfloat, const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBlendFunc)(GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCallList)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCallLists)(GLsizei, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClear)(GLbitfield) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearAccum)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearColor)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearDepth)(GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearIndex)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearStencil)(GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClipPlane)(GLenum, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3b)(GLbyte, GLbyte, GLbyte) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3bv)(const GLbyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3d)(GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3f)(GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3i)(GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3s)(GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3ub)(GLubyte, GLubyte, GLubyte) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3ubv)(const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3ui)(GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3uiv)(const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3us)(GLushort, GLushort, GLushort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3usv)(const GLushort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4b)(GLbyte, GLbyte, GLbyte, GLbyte) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4bv)(const GLbyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4d)(GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4f)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4i)(GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4s)(GLshort, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4ub)(GLubyte, GLubyte, GLubyte, GLubyte) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4ubv)(const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4ui)(GLuint, GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4uiv)(const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4us)(GLushort, GLushort, GLushort, GLushort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4usv)(const GLushort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColorMask)(GLboolean, GLboolean, GLboolean, GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColorMaterial)(GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCopyPixels)(GLint, GLint, GLsizei, GLsizei, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCullFace)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteLists)(GLuint, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDepthFunc)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDepthMask)(GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDepthRange)(GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDisable)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawBuffer)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawPixels)(GLsizei, GLsizei, GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEdgeFlag)(GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEdgeFlagv)(const GLboolean *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEnable)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEnd)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEndList)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEvalCoord1d)(GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEvalCoord1dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEvalCoord1f)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEvalCoord1fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEvalCoord2d)(GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEvalCoord2dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEvalCoord2f)(GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEvalCoord2fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEvalMesh1)(GLenum, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEvalMesh2)(GLenum, GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEvalPoint1)(GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEvalPoint2)(GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFeedbackBuffer)(GLsizei, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFinish)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFlush)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFogf)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFogfv)(GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFogi)(GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFogiv)(GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFrontFace)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFrustum)(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
GLuint (CODEGEN_FUNCPTR *_ptrc_glGenLists)(GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetBooleanv)(GLenum, GLboolean *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetClipPlane)(GLenum, GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetDoublev)(GLenum, GLdouble *) = NULL;
GLenum (CODEGEN_FUNCPTR *_ptrc_glGetError)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetFloatv)(GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetIntegerv)(GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetLightfv)(GLenum, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetLightiv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetMapdv)(GLenum, GLenum, GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetMapfv)(GLenum, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetMapiv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetMaterialfv)(GLenum, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetMaterialiv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetPixelMapfv)(GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetPixelMapuiv)(GLenum, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetPixelMapusv)(GLenum, GLushort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetPolygonStipple)(GLubyte *) = NULL;
const GLubyte * (CODEGEN_FUNCPTR *_ptrc_glGetString)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexEnvfv)(GLenum, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexEnviv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexGendv)(GLenum, GLenum, GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexGenfv)(GLenum, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexGeniv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexImage)(GLenum, GLint, GLenum, GLenum, GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexLevelParameterfv)(GLenum, GLint, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexLevelParameteriv)(GLenum, GLint, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexParameterfv)(GLenum, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexParameteriv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glHint)(GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glIndexMask)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glIndexd)(GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glIndexdv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glIndexf)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glIndexfv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glIndexi)(GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glIndexiv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glIndexs)(GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glIndexsv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glInitNames)() = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsEnabled)(GLenum) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsList)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLightModelf)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLightModelfv)(GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLightModeli)(GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLightModeliv)(GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLightf)(GLenum, GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLightfv)(GLenum, GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLighti)(GLenum, GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLightiv)(GLenum, GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLineStipple)(GLint, GLushort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLineWidth)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glListBase)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLoadIdentity)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLoadMatrixd)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLoadMatrixf)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLoadName)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLogicOp)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMap1d)(GLenum, GLdouble, GLdouble, GLint, GLint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMap1f)(GLenum, GLfloat, GLfloat, GLint, GLint, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMap2d)(GLenum, GLdouble, GLdouble, GLint, GLint, GLdouble, GLdouble, GLint, GLint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMap2f)(GLenum, GLfloat, GLfloat, GLint, GLint, GLfloat, GLfloat, GLint, GLint, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMapGrid1d)(GLint, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMapGrid1f)(GLint, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMapGrid2d)(GLint, GLdouble, GLdouble, GLint, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMapGrid2f)(GLint, GLfloat, GLfloat, GLint, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMaterialf)(GLenum, GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMaterialfv)(GLenum, GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMateriali)(GLenum, GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMaterialiv)(GLenum, GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMatrixMode)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultMatrixd)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultMatrixf)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNewList)(GLuint, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNormal3b)(GLbyte, GLbyte, GLbyte) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNormal3bv)(const GLbyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNormal3d)(GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNormal3dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNormal3f)(GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNormal3fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNormal3i)(GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNormal3iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNormal3s)(GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNormal3sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glOrtho)(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPassThrough)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPixelMapfv)(GLenum, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPixelMapuiv)(GLenum, GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPixelMapusv)(GLenum, GLsizei, const GLushort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPixelStoref)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPixelStorei)(GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPixelTransferf)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPixelTransferi)(GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPixelZoom)(GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPointSize)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPolygonMode)(GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPolygonStipple)(const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPopAttrib)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPopMatrix)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPopName)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPushAttrib)(GLbitfield) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPushMatrix)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPushName)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos2d)(GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos2dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos2f)(GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos2fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos2i)(GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos2iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos2s)(GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos2sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos3d)(GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos3dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos3f)(GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos3fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos3i)(GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos3iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos3s)(GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos3sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos4d)(GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos4dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos4f)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos4fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos4i)(GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos4iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos4s)(GLshort, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos4sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glReadBuffer)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glReadPixels)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRectd)(GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRectdv)(const GLdouble *, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRectf)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRectfv)(const GLfloat *, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRecti)(GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRectiv)(const GLint *, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRects)(GLshort, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRectsv)(const GLshort *, const GLshort *) = NULL;
GLint (CODEGEN_FUNCPTR *_ptrc_glRenderMode)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRotated)(GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRotatef)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glScaled)(GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glScalef)(GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glScissor)(GLint, GLint, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSelectBuffer)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glShadeModel)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glStencilFunc)(GLenum, GLint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glStencilMask)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glStencilOp)(GLenum, GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord1d)(GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord1dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord1f)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord1fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord1i)(GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord1iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord1s)(GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord1sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord2d)(GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord2dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord2f)(GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord2fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord2i)(GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord2iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord2s)(GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord2sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord3d)(GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord3dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord3f)(GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord3fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord3i)(GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord3iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord3s)(GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord3sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord4d)(GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord4dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord4f)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord4fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord4i)(GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord4iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord4s)(GLshort, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord4sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexEnvf)(GLenum, GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexEnvfv)(GLenum, GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexEnvi)(GLenum, GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexEnviv)(GLenum, GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexGend)(GLenum, GLenum, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexGendv)(GLenum, GLenum, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexGenf)(GLenum, GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexGenfv)(GLenum, GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexGeni)(GLenum, GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexGeniv)(GLenum, GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexImage1D)(GLenum, GLint, GLint, GLsizei, GLint, GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexImage2D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexParameterf)(GLenum, GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexParameterfv)(GLenum, GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexParameteri)(GLenum, GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexParameteriv)(GLenum, GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTranslated)(GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTranslatef)(GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex2d)(GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex2dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex2f)(GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex2fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex2i)(GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex2iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex2s)(GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex2sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex3d)(GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex3dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex3f)(GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex3fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex3i)(GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex3iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex3s)(GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex3sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex4d)(GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex4dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex4f)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex4fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex4i)(GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex4iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex4s)(GLshort, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex4sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glViewport)(GLint, GLint, GLsizei, GLsizei) = NULL;

GLboolean (CODEGEN_FUNCPTR *_ptrc_glAreTexturesResident)(GLsizei, const GLuint *, GLboolean *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glArrayElement)(GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindTexture)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColorPointer)(GLint, GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCopyTexImage1D)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCopyTexImage2D)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCopyTexSubImage1D)(GLenum, GLint, GLint, GLint, GLint, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCopyTexSubImage2D)(GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteTextures)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDisableClientState)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawArrays)(GLenum, GLint, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawElements)(GLenum, GLsizei, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEdgeFlagPointer)(GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEnableClientState)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenTextures)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glIndexPointer)(GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glIndexub)(GLubyte) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glIndexubv)(const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glInterleavedArrays)(GLenum, GLsizei, const GLvoid *) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsTexture)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNormalPointer)(GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPolygonOffset)(GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPopClientAttrib)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPrioritizeTextures)(GLsizei, const GLuint *, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPushClientAttrib)(GLbitfield) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoordPointer)(GLint, GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexSubImage1D)(GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexSubImage2D)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexPointer)(GLint, GLenum, GLsizei, const GLvoid *) = NULL;

void (CODEGEN_FUNCPTR *_ptrc_glBlendColor)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBlendEquation)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCopyTexSubImage3D)(GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawRangeElements)(GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexImage3D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexSubImage3D)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *) = NULL;

void (CODEGEN_FUNCPTR *_ptrc_glActiveTexture)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClientActiveTexture)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCompressedTexImage1D)(GLenum, GLint, GLenum, GLsizei, GLint, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCompressedTexImage2D)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCompressedTexImage3D)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCompressedTexSubImage1D)(GLenum, GLint, GLint, GLsizei, GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCompressedTexSubImage2D)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCompressedTexSubImage3D)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetCompressedTexImage)(GLenum, GLint, GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLoadTransposeMatrixd)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLoadTransposeMatrixf)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultTransposeMatrixd)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultTransposeMatrixf)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord1d)(GLenum, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord1dv)(GLenum, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord1f)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord1fv)(GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord1i)(GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord1iv)(GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord1s)(GLenum, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord1sv)(GLenum, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord2d)(GLenum, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord2dv)(GLenum, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord2f)(GLenum, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord2fv)(GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord2i)(GLenum, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord2iv)(GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord2s)(GLenum, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord2sv)(GLenum, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord3d)(GLenum, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord3dv)(GLenum, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord3f)(GLenum, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord3fv)(GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord3i)(GLenum, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord3iv)(GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord3s)(GLenum, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord3sv)(GLenum, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord4d)(GLenum, GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord4dv)(GLenum, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord4f)(GLenum, GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord4fv)(GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord4i)(GLenum, GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord4iv)(GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord4s)(GLenum, GLshort, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord4sv)(GLenum, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSampleCoverage)(GLfloat, GLboolean) = NULL;

void (CODEGEN_FUNCPTR *_ptrc_glBlendFuncSeparate)(GLenum, GLenum, GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFogCoordPointer)(GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFogCoordd)(GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFogCoorddv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFogCoordf)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFogCoordfv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiDrawArrays)(GLenum, const GLint *, const GLsizei *, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiDrawElements)(GLenum, const GLsizei *, GLenum, const GLvoid *const*, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPointParameterf)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPointParameterfv)(GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPointParameteri)(GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPointParameteriv)(GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3b)(GLbyte, GLbyte, GLbyte) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3bv)(const GLbyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3d)(GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3f)(GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3i)(GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3s)(GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3ub)(GLubyte, GLubyte, GLubyte) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3ubv)(const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3ui)(GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3uiv)(const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3us)(GLushort, GLushort, GLushort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3usv)(const GLushort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColorPointer)(GLint, GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos2d)(GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos2dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos2f)(GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos2fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos2i)(GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos2iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos2s)(GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos2sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos3d)(GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos3dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos3f)(GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos3fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos3i)(GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos3iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos3s)(GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos3sv)(const GLshort *) = NULL;

void (CODEGEN_FUNCPTR *_ptrc_glBeginQuery)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindBuffer)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBufferData)(GLenum, GLsizeiptr, const GLvoid *, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBufferSubData)(GLenum, GLintptr, GLsizeiptr, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteBuffers)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteQueries)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEndQuery)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenBuffers)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenQueries)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetBufferParameteriv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetBufferPointerv)(GLenum, GLenum, GLvoid **) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetBufferSubData)(GLenum, GLintptr, GLsizeiptr, GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetQueryObjectiv)(GLuint, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetQueryObjectuiv)(GLuint, GLenum, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetQueryiv)(GLenum, GLenum, GLint *) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsBuffer)(GLuint) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsQuery)(GLuint) = NULL;
void * (CODEGEN_FUNCPTR *_ptrc_glMapBuffer)(GLenum, GLenum) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glUnmapBuffer)(GLenum) = NULL;

void (CODEGEN_FUNCPTR *_ptrc_glAttachShader)(GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindAttribLocation)(GLuint, GLuint, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBlendEquationSeparate)(GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCompileShader)(GLuint) = NULL;
GLuint (CODEGEN_FUNCPTR *_ptrc_glCreateProgram)() = NULL;
GLuint (CODEGEN_FUNCPTR *_ptrc_glCreateShader)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteProgram)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteShader)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDetachShader)(GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDisableVertexAttribArray)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawBuffers)(GLsizei, const GLenum *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEnableVertexAttribArray)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetActiveAttrib)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetActiveUniform)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetAttachedShaders)(GLuint, GLsizei, GLsizei *, GLuint *) = NULL;
GLint (CODEGEN_FUNCPTR *_ptrc_glGetAttribLocation)(GLuint, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetProgramInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetProgramiv)(GLuint, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetShaderInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetShaderSource)(GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetShaderiv)(GLuint, GLenum, GLint *) = NULL;
GLint (CODEGEN_FUNCPTR *_ptrc_glGetUniformLocation)(GLuint, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetUniformfv)(GLuint, GLint, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetUniformiv)(GLuint, GLint, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetVertexAttribPointerv)(GLuint, GLenum, GLvoid **) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetVertexAttribdv)(GLuint, GLenum, GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetVertexAttribfv)(GLuint, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetVertexAttribiv)(GLuint, GLenum, GLint *) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsProgram)(GLuint) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsShader)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLinkProgram)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glShaderSource)(GLuint, GLsizei, const GLchar *const*, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glStencilFuncSeparate)(GLenum, GLenum, GLint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glStencilMaskSeparate)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glStencilOpSeparate)(GLenum, GLenum, GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform1f)(GLint, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform1fv)(GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform1i)(GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform1iv)(GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform2f)(GLint, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform2fv)(GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform2i)(GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform2iv)(GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform3f)(GLint, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform3fv)(GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform3i)(GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform3iv)(GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform4f)(GLint, GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform4fv)(GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform4i)(GLint, GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform4iv)(GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix2fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix3fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix4fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUseProgram)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glValidateProgram)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib1d)(GLuint, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib1dv)(GLuint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib1f)(GLuint, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib1fv)(GLuint, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib1s)(GLuint, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib1sv)(GLuint, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib2d)(GLuint, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib2dv)(GLuint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib2f)(GLuint, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib2fv)(GLuint, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib2s)(GLuint, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib2sv)(GLuint, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib3d)(GLuint, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib3dv)(GLuint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib3f)(GLuint, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib3fv)(GLuint, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib3s)(GLuint, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib3sv)(GLuint, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Nbv)(GLuint, const GLbyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Niv)(GLuint, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Nsv)(GLuint, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Nub)(GLuint, GLubyte, GLubyte, GLubyte, GLubyte) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Nubv)(GLuint, const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Nuiv)(GLuint, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Nusv)(GLuint, const GLushort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4bv)(GLuint, const GLbyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4d)(GLuint, GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4dv)(GLuint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4f)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4fv)(GLuint, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4iv)(GLuint, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4s)(GLuint, GLshort, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4sv)(GLuint, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4ubv)(GLuint, const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4uiv)(GLuint, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4usv)(GLuint, const GLushort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *) = NULL;

void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix2x3fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix2x4fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix3x2fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix3x4fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix4x2fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix4x3fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;

static int Load_Version_2_1()
{
	int numFailed = 0;
	_ptrc_glAccum = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat))IntGetProcAddress("glAccum");
	if(!_ptrc_glAccum) numFailed++;
	_ptrc_glAlphaFunc = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat))IntGetProcAddress("glAlphaFunc");
	if(!_ptrc_glAlphaFunc) numFailed++;
	_ptrc_glBegin = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glBegin");
	if(!_ptrc_glBegin) numFailed++;
	_ptrc_glBitmap = (void (CODEGEN_FUNCPTR *)(GLsizei, GLsizei, GLfloat, GLfloat, GLfloat, GLfloat, const GLubyte *))IntGetProcAddress("glBitmap");
	if(!_ptrc_glBitmap) numFailed++;
	_ptrc_glBlendFunc = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glBlendFunc");
	if(!_ptrc_glBlendFunc) numFailed++;
	_ptrc_glCallList = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glCallList");
	if(!_ptrc_glCallList) numFailed++;
	_ptrc_glCallLists = (void (CODEGEN_FUNCPTR *)(GLsizei, GLenum, const GLvoid *))IntGetProcAddress("glCallLists");
	if(!_ptrc_glCallLists) numFailed++;
	_ptrc_glClear = (void (CODEGEN_FUNCPTR *)(GLbitfield))IntGetProcAddress("glClear");
	if(!_ptrc_glClear) numFailed++;
	_ptrc_glClearAccum = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glClearAccum");
	if(!_ptrc_glClearAccum) numFailed++;
	_ptrc_glClearColor = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glClearColor");
	if(!_ptrc_glClearColor) numFailed++;
	_ptrc_glClearDepth = (void (CODEGEN_FUNCPTR *)(GLdouble))IntGetProcAddress("glClearDepth");
	if(!_ptrc_glClearDepth) numFailed++;
	_ptrc_glClearIndex = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glClearIndex");
	if(!_ptrc_glClearIndex) numFailed++;
	_ptrc_glClearStencil = (void (CODEGEN_FUNCPTR *)(GLint))IntGetProcAddress("glClearStencil");
	if(!_ptrc_glClearStencil) numFailed++;
	_ptrc_glClipPlane = (void (CODEGEN_FUNCPTR *)(GLenum, const GLdouble *))IntGetProcAddress("glClipPlane");
	if(!_ptrc_glClipPlane) numFailed++;
	_ptrc_glColor3b = (void (CODEGEN_FUNCPTR *)(GLbyte, GLbyte, GLbyte))IntGetProcAddress("glColor3b");
	if(!_ptrc_glColor3b) numFailed++;
	_ptrc_glColor3bv = (void (CODEGEN_FUNCPTR *)(const GLbyte *))IntGetProcAddress("glColor3bv");
	if(!_ptrc_glColor3bv) numFailed++;
	_ptrc_glColor3d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble))IntGetProcAddress("glColor3d");
	if(!_ptrc_glColor3d) numFailed++;
	_ptrc_glColor3dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glColor3dv");
	if(!_ptrc_glColor3dv) numFailed++;
	_ptrc_glColor3f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat))IntGetProcAddress("glColor3f");
	if(!_ptrc_glColor3f) numFailed++;
	_ptrc_glColor3fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glColor3fv");
	if(!_ptrc_glColor3fv) numFailed++;
	_ptrc_glColor3i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint))IntGetProcAddress("glColor3i");
	if(!_ptrc_glColor3i) numFailed++;
	_ptrc_glColor3iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glColor3iv");
	if(!_ptrc_glColor3iv) numFailed++;
	_ptrc_glColor3s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort, GLshort))IntGetProcAddress("glColor3s");
	if(!_ptrc_glColor3s) numFailed++;
	_ptrc_glColor3sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glColor3sv");
	if(!_ptrc_glColor3sv) numFailed++;
	_ptrc_glColor3ub = (void (CODEGEN_FUNCPTR *)(GLubyte, GLubyte, GLubyte))IntGetProcAddress("glColor3ub");
	if(!_ptrc_glColor3ub) numFailed++;
	_ptrc_glColor3ubv = (void (CODEGEN_FUNCPTR *)(const GLubyte *))IntGetProcAddress("glColor3ubv");
	if(!_ptrc_glColor3ubv) numFailed++;
	_ptrc_glColor3ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint))IntGetProcAddress("glColor3ui");
	if(!_ptrc_glColor3ui) numFailed++;
	_ptrc_glColor3uiv = (void (CODEGEN_FUNCPTR *)(const GLuint *))IntGetProcAddress("glColor3uiv");
	if(!_ptrc_glColor3uiv) numFailed++;
	_ptrc_glColor3us = (void (CODEGEN_FUNCPTR *)(GLushort, GLushort, GLushort))IntGetProcAddress("glColor3us");
	if(!_ptrc_glColor3us) numFailed++;
	_ptrc_glColor3usv = (void (CODEGEN_FUNCPTR *)(const GLushort *))IntGetProcAddress("glColor3usv");
	if(!_ptrc_glColor3usv) numFailed++;
	_ptrc_glColor4b = (void (CODEGEN_FUNCPTR *)(GLbyte, GLbyte, GLbyte, GLbyte))IntGetProcAddress("glColor4b");
	if(!_ptrc_glColor4b) numFailed++;
	_ptrc_glColor4bv = (void (CODEGEN_FUNCPTR *)(const GLbyte *))IntGetProcAddress("glColor4bv");
	if(!_ptrc_glColor4bv) numFailed++;
	_ptrc_glColor4d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glColor4d");
	if(!_ptrc_glColor4d) numFailed++;
	_ptrc_glColor4dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glColor4dv");
	if(!_ptrc_glColor4dv) numFailed++;
	_ptrc_glColor4f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glColor4f");
	if(!_ptrc_glColor4f) numFailed++;
	_ptrc_glColor4fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glColor4fv");
	if(!_ptrc_glColor4fv) numFailed++;
	_ptrc_glColor4i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint, GLint))IntGetProcAddress("glColor4i");
	if(!_ptrc_glColor4i) numFailed++;
	_ptrc_glColor4iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glColor4iv");
	if(!_ptrc_glColor4iv) numFailed++;
	_ptrc_glColor4s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort, GLshort, GLshort))IntGetProcAddress("glColor4s");
	if(!_ptrc_glColor4s) numFailed++;
	_ptrc_glColor4sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glColor4sv");
	if(!_ptrc_glColor4sv) numFailed++;
	_ptrc_glColor4ub = (void (CODEGEN_FUNCPTR *)(GLubyte, GLubyte, GLubyte, GLubyte))IntGetProcAddress("glColor4ub");
	if(!_ptrc_glColor4ub) numFailed++;
	_ptrc_glColor4ubv = (void (CODEGEN_FUNCPTR *)(const GLubyte *))IntGetProcAddress("glColor4ubv");
	if(!_ptrc_glColor4ubv) numFailed++;
	_ptrc_glColor4ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint, GLuint))IntGetProcAddress("glColor4ui");
	if(!_ptrc_glColor4ui) numFailed++;
	_ptrc_glColor4uiv = (void (CODEGEN_FUNCPTR *)(const GLuint *))IntGetProcAddress("glColor4uiv");
	if(!_ptrc_glColor4uiv) numFailed++;
	_ptrc_glColor4us = (void (CODEGEN_FUNCPTR *)(GLushort, GLushort, GLushort, GLushort))IntGetProcAddress("glColor4us");
	if(!_ptrc_glColor4us) numFailed++;
	_ptrc_glColor4usv = (void (CODEGEN_FUNCPTR *)(const GLushort *))IntGetProcAddress("glColor4usv");
	if(!_ptrc_glColor4usv) numFailed++;
	_ptrc_glColorMask = (void (CODEGEN_FUNCPTR *)(GLboolean, GLboolean, GLboolean, GLboolean))IntGetProcAddress("glColorMask");
	if(!_ptrc_glColorMask) numFailed++;
	_ptrc_glColorMaterial = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glColorMaterial");
	if(!_ptrc_glColorMaterial) numFailed++;
	_ptrc_glCopyPixels = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLsizei, GLsizei, GLenum))IntGetProcAddress("glCopyPixels");
	if(!_ptrc_glCopyPixels) numFailed++;
	_ptrc_glCullFace = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glCullFace");
	if(!_ptrc_glCullFace) numFailed++;
	_ptrc_glDeleteLists = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei))IntGetProcAddress("glDeleteLists");
	if(!_ptrc_glDeleteLists) numFailed++;
	_ptrc_glDepthFunc = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glDepthFunc");
	if(!_ptrc_glDepthFunc) numFailed++;
	_ptrc_glDepthMask = (void (CODEGEN_FUNCPTR *)(GLboolean))IntGetProcAddress("glDepthMask");
	if(!_ptrc_glDepthMask) numFailed++;
	_ptrc_glDepthRange = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble))IntGetProcAddress("glDepthRange");
	if(!_ptrc_glDepthRange) numFailed++;
	_ptrc_glDisable = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glDisable");
	if(!_ptrc_glDisable) numFailed++;
	_ptrc_glDrawBuffer = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glDrawBuffer");
	if(!_ptrc_glDrawBuffer) numFailed++;
	_ptrc_glDrawPixels = (void (CODEGEN_FUNCPTR *)(GLsizei, GLsizei, GLenum, GLenum, const GLvoid *))IntGetProcAddress("glDrawPixels");
	if(!_ptrc_glDrawPixels) numFailed++;
	_ptrc_glEdgeFlag = (void (CODEGEN_FUNCPTR *)(GLboolean))IntGetProcAddress("glEdgeFlag");
	if(!_ptrc_glEdgeFlag) numFailed++;
	_ptrc_glEdgeFlagv = (void (CODEGEN_FUNCPTR *)(const GLboolean *))IntGetProcAddress("glEdgeFlagv");
	if(!_ptrc_glEdgeFlagv) numFailed++;
	_ptrc_glEnable = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glEnable");
	if(!_ptrc_glEnable) numFailed++;
	_ptrc_glEnd = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glEnd");
	if(!_ptrc_glEnd) numFailed++;
	_ptrc_glEndList = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glEndList");
	if(!_ptrc_glEndList) numFailed++;
	_ptrc_glEvalCoord1d = (void (CODEGEN_FUNCPTR *)(GLdouble))IntGetProcAddress("glEvalCoord1d");
	if(!_ptrc_glEvalCoord1d) numFailed++;
	_ptrc_glEvalCoord1dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glEvalCoord1dv");
	if(!_ptrc_glEvalCoord1dv) numFailed++;
	_ptrc_glEvalCoord1f = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glEvalCoord1f");
	if(!_ptrc_glEvalCoord1f) numFailed++;
	_ptrc_glEvalCoord1fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glEvalCoord1fv");
	if(!_ptrc_glEvalCoord1fv) numFailed++;
	_ptrc_glEvalCoord2d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble))IntGetProcAddress("glEvalCoord2d");
	if(!_ptrc_glEvalCoord2d) numFailed++;
	_ptrc_glEvalCoord2dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glEvalCoord2dv");
	if(!_ptrc_glEvalCoord2dv) numFailed++;
	_ptrc_glEvalCoord2f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat))IntGetProcAddress("glEvalCoord2f");
	if(!_ptrc_glEvalCoord2f) numFailed++;
	_ptrc_glEvalCoord2fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glEvalCoord2fv");
	if(!_ptrc_glEvalCoord2fv) numFailed++;
	_ptrc_glEvalMesh1 = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint))IntGetProcAddress("glEvalMesh1");
	if(!_ptrc_glEvalMesh1) numFailed++;
	_ptrc_glEvalMesh2 = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLint))IntGetProcAddress("glEvalMesh2");
	if(!_ptrc_glEvalMesh2) numFailed++;
	_ptrc_glEvalPoint1 = (void (CODEGEN_FUNCPTR *)(GLint))IntGetProcAddress("glEvalPoint1");
	if(!_ptrc_glEvalPoint1) numFailed++;
	_ptrc_glEvalPoint2 = (void (CODEGEN_FUNCPTR *)(GLint, GLint))IntGetProcAddress("glEvalPoint2");
	if(!_ptrc_glEvalPoint2) numFailed++;
	_ptrc_glFeedbackBuffer = (void (CODEGEN_FUNCPTR *)(GLsizei, GLenum, GLfloat *))IntGetProcAddress("glFeedbackBuffer");
	if(!_ptrc_glFeedbackBuffer) numFailed++;
	_ptrc_glFinish = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glFinish");
	if(!_ptrc_glFinish) numFailed++;
	_ptrc_glFlush = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glFlush");
	if(!_ptrc_glFlush) numFailed++;
	_ptrc_glFogf = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat))IntGetProcAddress("glFogf");
	if(!_ptrc_glFogf) numFailed++;
	_ptrc_glFogfv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLfloat *))IntGetProcAddress("glFogfv");
	if(!_ptrc_glFogfv) numFailed++;
	_ptrc_glFogi = (void (CODEGEN_FUNCPTR *)(GLenum, GLint))IntGetProcAddress("glFogi");
	if(!_ptrc_glFogi) numFailed++;
	_ptrc_glFogiv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLint *))IntGetProcAddress("glFogiv");
	if(!_ptrc_glFogiv) numFailed++;
	_ptrc_glFrontFace = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glFrontFace");
	if(!_ptrc_glFrontFace) numFailed++;
	_ptrc_glFrustum = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glFrustum");
	if(!_ptrc_glFrustum) numFailed++;
	_ptrc_glGenLists = (GLuint (CODEGEN_FUNCPTR *)(GLsizei))IntGetProcAddress("glGenLists");
	if(!_ptrc_glGenLists) numFailed++;
	_ptrc_glGetBooleanv = (void (CODEGEN_FUNCPTR *)(GLenum, GLboolean *))IntGetProcAddress("glGetBooleanv");
	if(!_ptrc_glGetBooleanv) numFailed++;
	_ptrc_glGetClipPlane = (void (CODEGEN_FUNCPTR *)(GLenum, GLdouble *))IntGetProcAddress("glGetClipPlane");
	if(!_ptrc_glGetClipPlane) numFailed++;
	_ptrc_glGetDoublev = (void (CODEGEN_FUNCPTR *)(GLenum, GLdouble *))IntGetProcAddress("glGetDoublev");
	if(!_ptrc_glGetDoublev) numFailed++;
	_ptrc_glGetError = (GLenum (CODEGEN_FUNCPTR *)())IntGetProcAddress("glGetError");
	if(!_ptrc_glGetError) numFailed++;
	_ptrc_glGetFloatv = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat *))IntGetProcAddress("glGetFloatv");
	if(!_ptrc_glGetFloatv) numFailed++;
	_ptrc_glGetIntegerv = (void (CODEGEN_FUNCPTR *)(GLenum, GLint *))IntGetProcAddress("glGetIntegerv");
	if(!_ptrc_glGetIntegerv) numFailed++;
	_ptrc_glGetLightfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLfloat *))IntGetProcAddress("glGetLightfv");
	if(!_ptrc_glGetLightfv) numFailed++;
	_ptrc_glGetLightiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetLightiv");
	if(!_ptrc_glGetLightiv) numFailed++;
	_ptrc_glGetMapdv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLdouble *))IntGetProcAddress("glGetMapdv");
	if(!_ptrc_glGetMapdv) numFailed++;
	_ptrc_glGetMapfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLfloat *))IntGetProcAddress("glGetMapfv");
	if(!_ptrc_glGetMapfv) numFailed++;
	_ptrc_glGetMapiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetMapiv");
	if(!_ptrc_glGetMapiv) numFailed++;
	_ptrc_glGetMaterialfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLfloat *))IntGetProcAddress("glGetMaterialfv");
	if(!_ptrc_glGetMaterialfv) numFailed++;
	_ptrc_glGetMaterialiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetMaterialiv");
	if(!_ptrc_glGetMaterialiv) numFailed++;
	_ptrc_glGetPixelMapfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat *))IntGetProcAddress("glGetPixelMapfv");
	if(!_ptrc_glGetPixelMapfv) numFailed++;
	_ptrc_glGetPixelMapuiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint *))IntGetProcAddress("glGetPixelMapuiv");
	if(!_ptrc_glGetPixelMapuiv) numFailed++;
	_ptrc_glGetPixelMapusv = (void (CODEGEN_FUNCPTR *)(GLenum, GLushort *))IntGetProcAddress("glGetPixelMapusv");
	if(!_ptrc_glGetPixelMapusv) numFailed++;
	_ptrc_glGetPolygonStipple = (void (CODEGEN_FUNCPTR *)(GLubyte *))IntGetProcAddress("glGetPolygonStipple");
	if(!_ptrc_glGetPolygonStipple) numFailed++;
	_ptrc_glGetString = (const GLubyte * (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glGetString");
	if(!_ptrc_glGetString) numFailed++;
	_ptrc_glGetTexEnvfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLfloat *))IntGetProcAddress("glGetTexEnvfv");
	if(!_ptrc_glGetTexEnvfv) numFailed++;
	_ptrc_glGetTexEnviv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetTexEnviv");
	if(!_ptrc_glGetTexEnviv) numFailed++;
	_ptrc_glGetTexGendv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLdouble *))IntGetProcAddress("glGetTexGendv");
	if(!_ptrc_glGetTexGendv) numFailed++;
	_ptrc_glGetTexGenfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLfloat *))IntGetProcAddress("glGetTexGenfv");
	if(!_ptrc_glGetTexGenfv) numFailed++;
	_ptrc_glGetTexGeniv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetTexGeniv");
	if(!_ptrc_glGetTexGeniv) numFailed++;
	_ptrc_glGetTexImage = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLenum, GLvoid *))IntGetProcAddress("glGetTexImage");
	if(!_ptrc_glGetTexImage) numFailed++;
	_ptrc_glGetTexLevelParameterfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLfloat *))IntGetProcAddress("glGetTexLevelParameterfv");
	if(!_ptrc_glGetTexLevelParameterfv) numFailed++;
	_ptrc_glGetTexLevelParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLint *))IntGetProcAddress("glGetTexLevelParameteriv");
	if(!_ptrc_glGetTexLevelParameteriv) numFailed++;
	_ptrc_glGetTexParameterfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLfloat *))IntGetProcAddress("glGetTexParameterfv");
	if(!_ptrc_glGetTexParameterfv) numFailed++;
	_ptrc_glGetTexParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetTexParameteriv");
	if(!_ptrc_glGetTexParameteriv) numFailed++;
	_ptrc_glHint = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glHint");
	if(!_ptrc_glHint) numFailed++;
	_ptrc_glIndexMask = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIndexMask");
	if(!_ptrc_glIndexMask) numFailed++;
	_ptrc_glIndexd = (void (CODEGEN_FUNCPTR *)(GLdouble))IntGetProcAddress("glIndexd");
	if(!_ptrc_glIndexd) numFailed++;
	_ptrc_glIndexdv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glIndexdv");
	if(!_ptrc_glIndexdv) numFailed++;
	_ptrc_glIndexf = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glIndexf");
	if(!_ptrc_glIndexf) numFailed++;
	_ptrc_glIndexfv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glIndexfv");
	if(!_ptrc_glIndexfv) numFailed++;
	_ptrc_glIndexi = (void (CODEGEN_FUNCPTR *)(GLint))IntGetProcAddress("glIndexi");
	if(!_ptrc_glIndexi) numFailed++;
	_ptrc_glIndexiv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glIndexiv");
	if(!_ptrc_glIndexiv) numFailed++;
	_ptrc_glIndexs = (void (CODEGEN_FUNCPTR *)(GLshort))IntGetProcAddress("glIndexs");
	if(!_ptrc_glIndexs) numFailed++;
	_ptrc_glIndexsv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glIndexsv");
	if(!_ptrc_glIndexsv) numFailed++;
	_ptrc_glInitNames = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glInitNames");
	if(!_ptrc_glInitNames) numFailed++;
	_ptrc_glIsEnabled = (GLboolean (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glIsEnabled");
	if(!_ptrc_glIsEnabled) numFailed++;
	_ptrc_glIsList = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsList");
	if(!_ptrc_glIsList) numFailed++;
	_ptrc_glLightModelf = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat))IntGetProcAddress("glLightModelf");
	if(!_ptrc_glLightModelf) numFailed++;
	_ptrc_glLightModelfv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLfloat *))IntGetProcAddress("glLightModelfv");
	if(!_ptrc_glLightModelfv) numFailed++;
	_ptrc_glLightModeli = (void (CODEGEN_FUNCPTR *)(GLenum, GLint))IntGetProcAddress("glLightModeli");
	if(!_ptrc_glLightModeli) numFailed++;
	_ptrc_glLightModeliv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLint *))IntGetProcAddress("glLightModeliv");
	if(!_ptrc_glLightModeliv) numFailed++;
	_ptrc_glLightf = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLfloat))IntGetProcAddress("glLightf");
	if(!_ptrc_glLightf) numFailed++;
	_ptrc_glLightfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLfloat *))IntGetProcAddress("glLightfv");
	if(!_ptrc_glLightfv) numFailed++;
	_ptrc_glLighti = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint))IntGetProcAddress("glLighti");
	if(!_ptrc_glLighti) numFailed++;
	_ptrc_glLightiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLint *))IntGetProcAddress("glLightiv");
	if(!_ptrc_glLightiv) numFailed++;
	_ptrc_glLineStipple = (void (CODEGEN_FUNCPTR *)(GLint, GLushort))IntGetProcAddress("glLineStipple");
	if(!_ptrc_glLineStipple) numFailed++;
	_ptrc_glLineWidth = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glLineWidth");
	if(!_ptrc_glLineWidth) numFailed++;
	_ptrc_glListBase = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glListBase");
	if(!_ptrc_glListBase) numFailed++;
	_ptrc_glLoadIdentity = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glLoadIdentity");
	if(!_ptrc_glLoadIdentity) numFailed++;
	_ptrc_glLoadMatrixd = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glLoadMatrixd");
	if(!_ptrc_glLoadMatrixd) numFailed++;
	_ptrc_glLoadMatrixf = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glLoadMatrixf");
	if(!_ptrc_glLoadMatrixf) numFailed++;
	_ptrc_glLoadName = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glLoadName");
	if(!_ptrc_glLoadName) numFailed++;
	_ptrc_glLogicOp = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glLogicOp");
	if(!_ptrc_glLogicOp) numFailed++;
	_ptrc_glMap1d = (void (CODEGEN_FUNCPTR *)(GLenum, GLdouble, GLdouble, GLint, GLint, const GLdouble *))IntGetProcAddress("glMap1d");
	if(!_ptrc_glMap1d) numFailed++;
	_ptrc_glMap1f = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat, GLfloat, GLint, GLint, const GLfloat *))IntGetProcAddress("glMap1f");
	if(!_ptrc_glMap1f) numFailed++;
	_ptrc_glMap2d = (void (CODEGEN_FUNCPTR *)(GLenum, GLdouble, GLdouble, GLint, GLint, GLdouble, GLdouble, GLint, GLint, const GLdouble *))IntGetProcAddress("glMap2d");
	if(!_ptrc_glMap2d) numFailed++;
	_ptrc_glMap2f = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat, GLfloat, GLint, GLint, GLfloat, GLfloat, GLint, GLint, const GLfloat *))IntGetProcAddress("glMap2f");
	if(!_ptrc_glMap2f) numFailed++;
	_ptrc_glMapGrid1d = (void (CODEGEN_FUNCPTR *)(GLint, GLdouble, GLdouble))IntGetProcAddress("glMapGrid1d");
	if(!_ptrc_glMapGrid1d) numFailed++;
	_ptrc_glMapGrid1f = (void (CODEGEN_FUNCPTR *)(GLint, GLfloat, GLfloat))IntGetProcAddress("glMapGrid1f");
	if(!_ptrc_glMapGrid1f) numFailed++;
	_ptrc_glMapGrid2d = (void (CODEGEN_FUNCPTR *)(GLint, GLdouble, GLdouble, GLint, GLdouble, GLdouble))IntGetProcAddress("glMapGrid2d");
	if(!_ptrc_glMapGrid2d) numFailed++;
	_ptrc_glMapGrid2f = (void (CODEGEN_FUNCPTR *)(GLint, GLfloat, GLfloat, GLint, GLfloat, GLfloat))IntGetProcAddress("glMapGrid2f");
	if(!_ptrc_glMapGrid2f) numFailed++;
	_ptrc_glMaterialf = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLfloat))IntGetProcAddress("glMaterialf");
	if(!_ptrc_glMaterialf) numFailed++;
	_ptrc_glMaterialfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLfloat *))IntGetProcAddress("glMaterialfv");
	if(!_ptrc_glMaterialfv) numFailed++;
	_ptrc_glMateriali = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint))IntGetProcAddress("glMateriali");
	if(!_ptrc_glMateriali) numFailed++;
	_ptrc_glMaterialiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLint *))IntGetProcAddress("glMaterialiv");
	if(!_ptrc_glMaterialiv) numFailed++;
	_ptrc_glMatrixMode = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glMatrixMode");
	if(!_ptrc_glMatrixMode) numFailed++;
	_ptrc_glMultMatrixd = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glMultMatrixd");
	if(!_ptrc_glMultMatrixd) numFailed++;
	_ptrc_glMultMatrixf = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glMultMatrixf");
	if(!_ptrc_glMultMatrixf) numFailed++;
	_ptrc_glNewList = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum))IntGetProcAddress("glNewList");
	if(!_ptrc_glNewList) numFailed++;
	_ptrc_glNormal3b = (void (CODEGEN_FUNCPTR *)(GLbyte, GLbyte, GLbyte))IntGetProcAddress("glNormal3b");
	if(!_ptrc_glNormal3b) numFailed++;
	_ptrc_glNormal3bv = (void (CODEGEN_FUNCPTR *)(const GLbyte *))IntGetProcAddress("glNormal3bv");
	if(!_ptrc_glNormal3bv) numFailed++;
	_ptrc_glNormal3d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble))IntGetProcAddress("glNormal3d");
	if(!_ptrc_glNormal3d) numFailed++;
	_ptrc_glNormal3dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glNormal3dv");
	if(!_ptrc_glNormal3dv) numFailed++;
	_ptrc_glNormal3f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat))IntGetProcAddress("glNormal3f");
	if(!_ptrc_glNormal3f) numFailed++;
	_ptrc_glNormal3fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glNormal3fv");
	if(!_ptrc_glNormal3fv) numFailed++;
	_ptrc_glNormal3i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint))IntGetProcAddress("glNormal3i");
	if(!_ptrc_glNormal3i) numFailed++;
	_ptrc_glNormal3iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glNormal3iv");
	if(!_ptrc_glNormal3iv) numFailed++;
	_ptrc_glNormal3s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort, GLshort))IntGetProcAddress("glNormal3s");
	if(!_ptrc_glNormal3s) numFailed++;
	_ptrc_glNormal3sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glNormal3sv");
	if(!_ptrc_glNormal3sv) numFailed++;
	_ptrc_glOrtho = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glOrtho");
	if(!_ptrc_glOrtho) numFailed++;
	_ptrc_glPassThrough = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glPassThrough");
	if(!_ptrc_glPassThrough) numFailed++;
	_ptrc_glPixelMapfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, const GLfloat *))IntGetProcAddress("glPixelMapfv");
	if(!_ptrc_glPixelMapfv) numFailed++;
	_ptrc_glPixelMapuiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, const GLuint *))IntGetProcAddress("glPixelMapuiv");
	if(!_ptrc_glPixelMapuiv) numFailed++;
	_ptrc_glPixelMapusv = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, const GLushort *))IntGetProcAddress("glPixelMapusv");
	if(!_ptrc_glPixelMapusv) numFailed++;
	_ptrc_glPixelStoref = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat))IntGetProcAddress("glPixelStoref");
	if(!_ptrc_glPixelStoref) numFailed++;
	_ptrc_glPixelStorei = (void (CODEGEN_FUNCPTR *)(GLenum, GLint))IntGetProcAddress("glPixelStorei");
	if(!_ptrc_glPixelStorei) numFailed++;
	_ptrc_glPixelTransferf = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat))IntGetProcAddress("glPixelTransferf");
	if(!_ptrc_glPixelTransferf) numFailed++;
	_ptrc_glPixelTransferi = (void (CODEGEN_FUNCPTR *)(GLenum, GLint))IntGetProcAddress("glPixelTransferi");
	if(!_ptrc_glPixelTransferi) numFailed++;
	_ptrc_glPixelZoom = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat))IntGetProcAddress("glPixelZoom");
	if(!_ptrc_glPixelZoom) numFailed++;
	_ptrc_glPointSize = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glPointSize");
	if(!_ptrc_glPointSize) numFailed++;
	_ptrc_glPolygonMode = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glPolygonMode");
	if(!_ptrc_glPolygonMode) numFailed++;
	_ptrc_glPolygonStipple = (void (CODEGEN_FUNCPTR *)(const GLubyte *))IntGetProcAddress("glPolygonStipple");
	if(!_ptrc_glPolygonStipple) numFailed++;
	_ptrc_glPopAttrib = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glPopAttrib");
	if(!_ptrc_glPopAttrib) numFailed++;
	_ptrc_glPopMatrix = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glPopMatrix");
	if(!_ptrc_glPopMatrix) numFailed++;
	_ptrc_glPopName = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glPopName");
	if(!_ptrc_glPopName) numFailed++;
	_ptrc_glPushAttrib = (void (CODEGEN_FUNCPTR *)(GLbitfield))IntGetProcAddress("glPushAttrib");
	if(!_ptrc_glPushAttrib) numFailed++;
	_ptrc_glPushMatrix = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glPushMatrix");
	if(!_ptrc_glPushMatrix) numFailed++;
	_ptrc_glPushName = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glPushName");
	if(!_ptrc_glPushName) numFailed++;
	_ptrc_glRasterPos2d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble))IntGetProcAddress("glRasterPos2d");
	if(!_ptrc_glRasterPos2d) numFailed++;
	_ptrc_glRasterPos2dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glRasterPos2dv");
	if(!_ptrc_glRasterPos2dv) numFailed++;
	_ptrc_glRasterPos2f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat))IntGetProcAddress("glRasterPos2f");
	if(!_ptrc_glRasterPos2f) numFailed++;
	_ptrc_glRasterPos2fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glRasterPos2fv");
	if(!_ptrc_glRasterPos2fv) numFailed++;
	_ptrc_glRasterPos2i = (void (CODEGEN_FUNCPTR *)(GLint, GLint))IntGetProcAddress("glRasterPos2i");
	if(!_ptrc_glRasterPos2i) numFailed++;
	_ptrc_glRasterPos2iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glRasterPos2iv");
	if(!_ptrc_glRasterPos2iv) numFailed++;
	_ptrc_glRasterPos2s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort))IntGetProcAddress("glRasterPos2s");
	if(!_ptrc_glRasterPos2s) numFailed++;
	_ptrc_glRasterPos2sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glRasterPos2sv");
	if(!_ptrc_glRasterPos2sv) numFailed++;
	_ptrc_glRasterPos3d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble))IntGetProcAddress("glRasterPos3d");
	if(!_ptrc_glRasterPos3d) numFailed++;
	_ptrc_glRasterPos3dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glRasterPos3dv");
	if(!_ptrc_glRasterPos3dv) numFailed++;
	_ptrc_glRasterPos3f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat))IntGetProcAddress("glRasterPos3f");
	if(!_ptrc_glRasterPos3f) numFailed++;
	_ptrc_glRasterPos3fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glRasterPos3fv");
	if(!_ptrc_glRasterPos3fv) numFailed++;
	_ptrc_glRasterPos3i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint))IntGetProcAddress("glRasterPos3i");
	if(!_ptrc_glRasterPos3i) numFailed++;
	_ptrc_glRasterPos3iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glRasterPos3iv");
	if(!_ptrc_glRasterPos3iv) numFailed++;
	_ptrc_glRasterPos3s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort, GLshort))IntGetProcAddress("glRasterPos3s");
	if(!_ptrc_glRasterPos3s) numFailed++;
	_ptrc_glRasterPos3sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glRasterPos3sv");
	if(!_ptrc_glRasterPos3sv) numFailed++;
	_ptrc_glRasterPos4d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glRasterPos4d");
	if(!_ptrc_glRasterPos4d) numFailed++;
	_ptrc_glRasterPos4dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glRasterPos4dv");
	if(!_ptrc_glRasterPos4dv) numFailed++;
	_ptrc_glRasterPos4f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glRasterPos4f");
	if(!_ptrc_glRasterPos4f) numFailed++;
	_ptrc_glRasterPos4fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glRasterPos4fv");
	if(!_ptrc_glRasterPos4fv) numFailed++;
	_ptrc_glRasterPos4i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint, GLint))IntGetProcAddress("glRasterPos4i");
	if(!_ptrc_glRasterPos4i) numFailed++;
	_ptrc_glRasterPos4iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glRasterPos4iv");
	if(!_ptrc_glRasterPos4iv) numFailed++;
	_ptrc_glRasterPos4s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort, GLshort, GLshort))IntGetProcAddress("glRasterPos4s");
	if(!_ptrc_glRasterPos4s) numFailed++;
	_ptrc_glRasterPos4sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glRasterPos4sv");
	if(!_ptrc_glRasterPos4sv) numFailed++;
	_ptrc_glReadBuffer = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glReadBuffer");
	if(!_ptrc_glReadBuffer) numFailed++;
	_ptrc_glReadPixels = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid *))IntGetProcAddress("glReadPixels");
	if(!_ptrc_glReadPixels) numFailed++;
	_ptrc_glRectd = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glRectd");
	if(!_ptrc_glRectd) numFailed++;
	_ptrc_glRectdv = (void (CODEGEN_FUNCPTR *)(const GLdouble *, const GLdouble *))IntGetProcAddress("glRectdv");
	if(!_ptrc_glRectdv) numFailed++;
	_ptrc_glRectf = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glRectf");
	if(!_ptrc_glRectf) numFailed++;
	_ptrc_glRectfv = (void (CODEGEN_FUNCPTR *)(const GLfloat *, const GLfloat *))IntGetProcAddress("glRectfv");
	if(!_ptrc_glRectfv) numFailed++;
	_ptrc_glRecti = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint, GLint))IntGetProcAddress("glRecti");
	if(!_ptrc_glRecti) numFailed++;
	_ptrc_glRectiv = (void (CODEGEN_FUNCPTR *)(const GLint *, const GLint *))IntGetProcAddress("glRectiv");
	if(!_ptrc_glRectiv) numFailed++;
	_ptrc_glRects = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort, GLshort, GLshort))IntGetProcAddress("glRects");
	if(!_ptrc_glRects) numFailed++;
	_ptrc_glRectsv = (void (CODEGEN_FUNCPTR *)(const GLshort *, const GLshort *))IntGetProcAddress("glRectsv");
	if(!_ptrc_glRectsv) numFailed++;
	_ptrc_glRenderMode = (GLint (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glRenderMode");
	if(!_ptrc_glRenderMode) numFailed++;
	_ptrc_glRotated = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glRotated");
	if(!_ptrc_glRotated) numFailed++;
	_ptrc_glRotatef = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glRotatef");
	if(!_ptrc_glRotatef) numFailed++;
	_ptrc_glScaled = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble))IntGetProcAddress("glScaled");
	if(!_ptrc_glScaled) numFailed++;
	_ptrc_glScalef = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat))IntGetProcAddress("glScalef");
	if(!_ptrc_glScalef) numFailed++;
	_ptrc_glScissor = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glScissor");
	if(!_ptrc_glScissor) numFailed++;
	_ptrc_glSelectBuffer = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glSelectBuffer");
	if(!_ptrc_glSelectBuffer) numFailed++;
	_ptrc_glShadeModel = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glShadeModel");
	if(!_ptrc_glShadeModel) numFailed++;
	_ptrc_glStencilFunc = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLuint))IntGetProcAddress("glStencilFunc");
	if(!_ptrc_glStencilFunc) numFailed++;
	_ptrc_glStencilMask = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glStencilMask");
	if(!_ptrc_glStencilMask) numFailed++;
	_ptrc_glStencilOp = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum))IntGetProcAddress("glStencilOp");
	if(!_ptrc_glStencilOp) numFailed++;
	_ptrc_glTexCoord1d = (void (CODEGEN_FUNCPTR *)(GLdouble))IntGetProcAddress("glTexCoord1d");
	if(!_ptrc_glTexCoord1d) numFailed++;
	_ptrc_glTexCoord1dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glTexCoord1dv");
	if(!_ptrc_glTexCoord1dv) numFailed++;
	_ptrc_glTexCoord1f = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glTexCoord1f");
	if(!_ptrc_glTexCoord1f) numFailed++;
	_ptrc_glTexCoord1fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glTexCoord1fv");
	if(!_ptrc_glTexCoord1fv) numFailed++;
	_ptrc_glTexCoord1i = (void (CODEGEN_FUNCPTR *)(GLint))IntGetProcAddress("glTexCoord1i");
	if(!_ptrc_glTexCoord1i) numFailed++;
	_ptrc_glTexCoord1iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glTexCoord1iv");
	if(!_ptrc_glTexCoord1iv) numFailed++;
	_ptrc_glTexCoord1s = (void (CODEGEN_FUNCPTR *)(GLshort))IntGetProcAddress("glTexCoord1s");
	if(!_ptrc_glTexCoord1s) numFailed++;
	_ptrc_glTexCoord1sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glTexCoord1sv");
	if(!_ptrc_glTexCoord1sv) numFailed++;
	_ptrc_glTexCoord2d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble))IntGetProcAddress("glTexCoord2d");
	if(!_ptrc_glTexCoord2d) numFailed++;
	_ptrc_glTexCoord2dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glTexCoord2dv");
	if(!_ptrc_glTexCoord2dv) numFailed++;
	_ptrc_glTexCoord2f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat))IntGetProcAddress("glTexCoord2f");
	if(!_ptrc_glTexCoord2f) numFailed++;
	_ptrc_glTexCoord2fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glTexCoord2fv");
	if(!_ptrc_glTexCoord2fv) numFailed++;
	_ptrc_glTexCoord2i = (void (CODEGEN_FUNCPTR *)(GLint, GLint))IntGetProcAddress("glTexCoord2i");
	if(!_ptrc_glTexCoord2i) numFailed++;
	_ptrc_glTexCoord2iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glTexCoord2iv");
	if(!_ptrc_glTexCoord2iv) numFailed++;
	_ptrc_glTexCoord2s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort))IntGetProcAddress("glTexCoord2s");
	if(!_ptrc_glTexCoord2s) numFailed++;
	_ptrc_glTexCoord2sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glTexCoord2sv");
	if(!_ptrc_glTexCoord2sv) numFailed++;
	_ptrc_glTexCoord3d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble))IntGetProcAddress("glTexCoord3d");
	if(!_ptrc_glTexCoord3d) numFailed++;
	_ptrc_glTexCoord3dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glTexCoord3dv");
	if(!_ptrc_glTexCoord3dv) numFailed++;
	_ptrc_glTexCoord3f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat))IntGetProcAddress("glTexCoord3f");
	if(!_ptrc_glTexCoord3f) numFailed++;
	_ptrc_glTexCoord3fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glTexCoord3fv");
	if(!_ptrc_glTexCoord3fv) numFailed++;
	_ptrc_glTexCoord3i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint))IntGetProcAddress("glTexCoord3i");
	if(!_ptrc_glTexCoord3i) numFailed++;
	_ptrc_glTexCoord3iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glTexCoord3iv");
	if(!_ptrc_glTexCoord3iv) numFailed++;
	_ptrc_glTexCoord3s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort, GLshort))IntGetProcAddress("glTexCoord3s");
	if(!_ptrc_glTexCoord3s) numFailed++;
	_ptrc_glTexCoord3sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glTexCoord3sv");
	if(!_ptrc_glTexCoord3sv) numFailed++;
	_ptrc_glTexCoord4d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glTexCoord4d");
	if(!_ptrc_glTexCoord4d) numFailed++;
	_ptrc_glTexCoord4dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glTexCoord4dv");
	if(!_ptrc_glTexCoord4dv) numFailed++;
	_ptrc_glTexCoord4f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glTexCoord4f");
	if(!_ptrc_glTexCoord4f) numFailed++;
	_ptrc_glTexCoord4fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glTexCoord4fv");
	if(!_ptrc_glTexCoord4fv) numFailed++;
	_ptrc_glTexCoord4i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint, GLint))IntGetProcAddress("glTexCoord4i");
	if(!_ptrc_glTexCoord4i) numFailed++;
	_ptrc_glTexCoord4iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glTexCoord4iv");
	if(!_ptrc_glTexCoord4iv) numFailed++;
	_ptrc_glTexCoord4s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort, GLshort, GLshort))IntGetProcAddress("glTexCoord4s");
	if(!_ptrc_glTexCoord4s) numFailed++;
	_ptrc_glTexCoord4sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glTexCoord4sv");
	if(!_ptrc_glTexCoord4sv) numFailed++;
	_ptrc_glTexEnvf = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLfloat))IntGetProcAddress("glTexEnvf");
	if(!_ptrc_glTexEnvf) numFailed++;
	_ptrc_glTexEnvfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLfloat *))IntGetProcAddress("glTexEnvfv");
	if(!_ptrc_glTexEnvfv) numFailed++;
	_ptrc_glTexEnvi = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint))IntGetProcAddress("glTexEnvi");
	if(!_ptrc_glTexEnvi) numFailed++;
	_ptrc_glTexEnviv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLint *))IntGetProcAddress("glTexEnviv");
	if(!_ptrc_glTexEnviv) numFailed++;
	_ptrc_glTexGend = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLdouble))IntGetProcAddress("glTexGend");
	if(!_ptrc_glTexGend) numFailed++;
	_ptrc_glTexGendv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLdouble *))IntGetProcAddress("glTexGendv");
	if(!_ptrc_glTexGendv) numFailed++;
	_ptrc_glTexGenf = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLfloat))IntGetProcAddress("glTexGenf");
	if(!_ptrc_glTexGenf) numFailed++;
	_ptrc_glTexGenfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLfloat *))IntGetProcAddress("glTexGenfv");
	if(!_ptrc_glTexGenfv) numFailed++;
	_ptrc_glTexGeni = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint))IntGetProcAddress("glTexGeni");
	if(!_ptrc_glTexGeni) numFailed++;
	_ptrc_glTexGeniv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLint *))IntGetProcAddress("glTexGeniv");
	if(!_ptrc_glTexGeniv) numFailed++;
	_ptrc_glTexImage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLsizei, GLint, GLenum, GLenum, const GLvoid *))IntGetProcAddress("glTexImage1D");
	if(!_ptrc_glTexImage1D) numFailed++;
	_ptrc_glTexImage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *))IntGetProcAddress("glTexImage2D");
	if(!_ptrc_glTexImage2D) numFailed++;
	_ptrc_glTexParameterf = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLfloat))IntGetProcAddress("glTexParameterf");
	if(!_ptrc_glTexParameterf) numFailed++;
	_ptrc_glTexParameterfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLfloat *))IntGetProcAddress("glTexParameterfv");
	if(!_ptrc_glTexParameterfv) numFailed++;
	_ptrc_glTexParameteri = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint))IntGetProcAddress("glTexParameteri");
	if(!_ptrc_glTexParameteri) numFailed++;
	_ptrc_glTexParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLint *))IntGetProcAddress("glTexParameteriv");
	if(!_ptrc_glTexParameteriv) numFailed++;
	_ptrc_glTranslated = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble))IntGetProcAddress("glTranslated");
	if(!_ptrc_glTranslated) numFailed++;
	_ptrc_glTranslatef = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat))IntGetProcAddress("glTranslatef");
	if(!_ptrc_glTranslatef) numFailed++;
	_ptrc_glVertex2d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble))IntGetProcAddress("glVertex2d");
	if(!_ptrc_glVertex2d) numFailed++;
	_ptrc_glVertex2dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glVertex2dv");
	if(!_ptrc_glVertex2dv) numFailed++;
	_ptrc_glVertex2f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat))IntGetProcAddress("glVertex2f");
	if(!_ptrc_glVertex2f) numFailed++;
	_ptrc_glVertex2fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glVertex2fv");
	if(!_ptrc_glVertex2fv) numFailed++;
	_ptrc_glVertex2i = (void (CODEGEN_FUNCPTR *)(GLint, GLint))IntGetProcAddress("glVertex2i");
	if(!_ptrc_glVertex2i) numFailed++;
	_ptrc_glVertex2iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glVertex2iv");
	if(!_ptrc_glVertex2iv) numFailed++;
	_ptrc_glVertex2s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort))IntGetProcAddress("glVertex2s");
	if(!_ptrc_glVertex2s) numFailed++;
	_ptrc_glVertex2sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glVertex2sv");
	if(!_ptrc_glVertex2sv) numFailed++;
	_ptrc_glVertex3d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble))IntGetProcAddress("glVertex3d");
	if(!_ptrc_glVertex3d) numFailed++;
	_ptrc_glVertex3dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glVertex3dv");
	if(!_ptrc_glVertex3dv) numFailed++;
	_ptrc_glVertex3f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat))IntGetProcAddress("glVertex3f");
	if(!_ptrc_glVertex3f) numFailed++;
	_ptrc_glVertex3fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glVertex3fv");
	if(!_ptrc_glVertex3fv) numFailed++;
	_ptrc_glVertex3i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint))IntGetProcAddress("glVertex3i");
	if(!_ptrc_glVertex3i) numFailed++;
	_ptrc_glVertex3iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glVertex3iv");
	if(!_ptrc_glVertex3iv) numFailed++;
	_ptrc_glVertex3s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort, GLshort))IntGetProcAddress("glVertex3s");
	if(!_ptrc_glVertex3s) numFailed++;
	_ptrc_glVertex3sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glVertex3sv");
	if(!_ptrc_glVertex3sv) numFailed++;
	_ptrc_glVertex4d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glVertex4d");
	if(!_ptrc_glVertex4d) numFailed++;
	_ptrc_glVertex4dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glVertex4dv");
	if(!_ptrc_glVertex4dv) numFailed++;
	_ptrc_glVertex4f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glVertex4f");
	if(!_ptrc_glVertex4f) numFailed++;
	_ptrc_glVertex4fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glVertex4fv");
	if(!_ptrc_glVertex4fv) numFailed++;
	_ptrc_glVertex4i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint, GLint))IntGetProcAddress("glVertex4i");
	if(!_ptrc_glVertex4i) numFailed++;
	_ptrc_glVertex4iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glVertex4iv");
	if(!_ptrc_glVertex4iv) numFailed++;
	_ptrc_glVertex4s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort, GLshort, GLshort))IntGetProcAddress("glVertex4s");
	if(!_ptrc_glVertex4s) numFailed++;
	_ptrc_glVertex4sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glVertex4sv");
	if(!_ptrc_glVertex4sv) numFailed++;
	_ptrc_glViewport = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glViewport");
	if(!_ptrc_glViewport) numFailed++;
	_ptrc_glAreTexturesResident = (GLboolean (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *, GLboolean *))IntGetProcAddress("glAreTexturesResident");
	if(!_ptrc_glAreTexturesResident) numFailed++;
	_ptrc_glArrayElement = (void (CODEGEN_FUNCPTR *)(GLint))IntGetProcAddress("glArrayElement");
	if(!_ptrc_glArrayElement) numFailed++;
	_ptrc_glBindTexture = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glBindTexture");
	if(!_ptrc_glBindTexture) numFailed++;
	_ptrc_glColorPointer = (void (CODEGEN_FUNCPTR *)(GLint, GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glColorPointer");
	if(!_ptrc_glColorPointer) numFailed++;
	_ptrc_glCopyTexImage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLint))IntGetProcAddress("glCopyTexImage1D");
	if(!_ptrc_glCopyTexImage1D) numFailed++;
	_ptrc_glCopyTexImage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint))IntGetProcAddress("glCopyTexImage2D");
	if(!_ptrc_glCopyTexImage2D) numFailed++;
	_ptrc_glCopyTexSubImage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLint, GLsizei))IntGetProcAddress("glCopyTexSubImage1D");
	if(!_ptrc_glCopyTexSubImage1D) numFailed++;
	_ptrc_glCopyTexSubImage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glCopyTexSubImage2D");
	if(!_ptrc_glCopyTexSubImage2D) numFailed++;
	_ptrc_glDeleteTextures = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteTextures");
	if(!_ptrc_glDeleteTextures) numFailed++;
	_ptrc_glDisableClientState = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glDisableClientState");
	if(!_ptrc_glDisableClientState) numFailed++;
	_ptrc_glDrawArrays = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLsizei))IntGetProcAddress("glDrawArrays");
	if(!_ptrc_glDrawArrays) numFailed++;
	_ptrc_glDrawElements = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, const GLvoid *))IntGetProcAddress("glDrawElements");
	if(!_ptrc_glDrawElements) numFailed++;
	_ptrc_glEdgeFlagPointer = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLvoid *))IntGetProcAddress("glEdgeFlagPointer");
	if(!_ptrc_glEdgeFlagPointer) numFailed++;
	_ptrc_glEnableClientState = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glEnableClientState");
	if(!_ptrc_glEnableClientState) numFailed++;
	_ptrc_glGenTextures = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenTextures");
	if(!_ptrc_glGenTextures) numFailed++;
	_ptrc_glGetPointerv = (void (CODEGEN_FUNCPTR *)(GLenum, GLvoid **))IntGetProcAddress("glGetPointerv");
	if(!_ptrc_glGetPointerv) numFailed++;
	_ptrc_glIndexPointer = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glIndexPointer");
	if(!_ptrc_glIndexPointer) numFailed++;
	_ptrc_glIndexub = (void (CODEGEN_FUNCPTR *)(GLubyte))IntGetProcAddress("glIndexub");
	if(!_ptrc_glIndexub) numFailed++;
	_ptrc_glIndexubv = (void (CODEGEN_FUNCPTR *)(const GLubyte *))IntGetProcAddress("glIndexubv");
	if(!_ptrc_glIndexubv) numFailed++;
	_ptrc_glInterleavedArrays = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glInterleavedArrays");
	if(!_ptrc_glInterleavedArrays) numFailed++;
	_ptrc_glIsTexture = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsTexture");
	if(!_ptrc_glIsTexture) numFailed++;
	_ptrc_glNormalPointer = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glNormalPointer");
	if(!_ptrc_glNormalPointer) numFailed++;
	_ptrc_glPolygonOffset = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat))IntGetProcAddress("glPolygonOffset");
	if(!_ptrc_glPolygonOffset) numFailed++;
	_ptrc_glPopClientAttrib = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glPopClientAttrib");
	if(!_ptrc_glPopClientAttrib) numFailed++;
	_ptrc_glPrioritizeTextures = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *, const GLfloat *))IntGetProcAddress("glPrioritizeTextures");
	if(!_ptrc_glPrioritizeTextures) numFailed++;
	_ptrc_glPushClientAttrib = (void (CODEGEN_FUNCPTR *)(GLbitfield))IntGetProcAddress("glPushClientAttrib");
	if(!_ptrc_glPushClientAttrib) numFailed++;
	_ptrc_glTexCoordPointer = (void (CODEGEN_FUNCPTR *)(GLint, GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glTexCoordPointer");
	if(!_ptrc_glTexCoordPointer) numFailed++;
	_ptrc_glTexSubImage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const GLvoid *))IntGetProcAddress("glTexSubImage1D");
	if(!_ptrc_glTexSubImage1D) numFailed++;
	_ptrc_glTexSubImage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *))IntGetProcAddress("glTexSubImage2D");
	if(!_ptrc_glTexSubImage2D) numFailed++;
	_ptrc_glVertexPointer = (void (CODEGEN_FUNCPTR *)(GLint, GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glVertexPointer");
	if(!_ptrc_glVertexPointer) numFailed++;
	_ptrc_glBlendColor = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glBlendColor");
	if(!_ptrc_glBlendColor) numFailed++;
	_ptrc_glBlendEquation = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glBlendEquation");
	if(!_ptrc_glBlendEquation) numFailed++;
	_ptrc_glCopyTexSubImage3D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glCopyTexSubImage3D");
	if(!_ptrc_glCopyTexSubImage3D) numFailed++;
	_ptrc_glDrawRangeElements = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid *))IntGetProcAddress("glDrawRangeElements");
	if(!_ptrc_glDrawRangeElements) numFailed++;
	_ptrc_glTexImage3D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *))IntGetProcAddress("glTexImage3D");
	if(!_ptrc_glTexImage3D) numFailed++;
	_ptrc_glTexSubImage3D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *))IntGetProcAddress("glTexSubImage3D");
	if(!_ptrc_glTexSubImage3D) numFailed++;
	_ptrc_glActiveTexture = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glActiveTexture");
	if(!_ptrc_glActiveTexture) numFailed++;
	_ptrc_glClientActiveTexture = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glClientActiveTexture");
	if(!_ptrc_glClientActiveTexture) numFailed++;
	_ptrc_glCompressedTexImage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLsizei, GLint, GLsizei, const GLvoid *))IntGetProcAddress("glCompressedTexImage1D");
	if(!_ptrc_glCompressedTexImage1D) numFailed++;
	_ptrc_glCompressedTexImage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *))IntGetProcAddress("glCompressedTexImage2D");
	if(!_ptrc_glCompressedTexImage2D) numFailed++;
	_ptrc_glCompressedTexImage3D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *))IntGetProcAddress("glCompressedTexImage3D");
	if(!_ptrc_glCompressedTexImage3D) numFailed++;
	_ptrc_glCompressedTexSubImage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLsizei, GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glCompressedTexSubImage1D");
	if(!_ptrc_glCompressedTexSubImage1D) numFailed++;
	_ptrc_glCompressedTexSubImage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glCompressedTexSubImage2D");
	if(!_ptrc_glCompressedTexSubImage2D) numFailed++;
	_ptrc_glCompressedTexSubImage3D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glCompressedTexSubImage3D");
	if(!_ptrc_glCompressedTexSubImage3D) numFailed++;
	_ptrc_glGetCompressedTexImage = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLvoid *))IntGetProcAddress("glGetCompressedTexImage");
	if(!_ptrc_glGetCompressedTexImage) numFailed++;
	_ptrc_glLoadTransposeMatrixd = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glLoadTransposeMatrixd");
	if(!_ptrc_glLoadTransposeMatrixd) numFailed++;
	_ptrc_glLoadTransposeMatrixf = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glLoadTransposeMatrixf");
	if(!_ptrc_glLoadTransposeMatrixf) numFailed++;
	_ptrc_glMultTransposeMatrixd = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glMultTransposeMatrixd");
	if(!_ptrc_glMultTransposeMatrixd) numFailed++;
	_ptrc_glMultTransposeMatrixf = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glMultTransposeMatrixf");
	if(!_ptrc_glMultTransposeMatrixf) numFailed++;
	_ptrc_glMultiTexCoord1d = (void (CODEGEN_FUNCPTR *)(GLenum, GLdouble))IntGetProcAddress("glMultiTexCoord1d");
	if(!_ptrc_glMultiTexCoord1d) numFailed++;
	_ptrc_glMultiTexCoord1dv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLdouble *))IntGetProcAddress("glMultiTexCoord1dv");
	if(!_ptrc_glMultiTexCoord1dv) numFailed++;
	_ptrc_glMultiTexCoord1f = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat))IntGetProcAddress("glMultiTexCoord1f");
	if(!_ptrc_glMultiTexCoord1f) numFailed++;
	_ptrc_glMultiTexCoord1fv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLfloat *))IntGetProcAddress("glMultiTexCoord1fv");
	if(!_ptrc_glMultiTexCoord1fv) numFailed++;
	_ptrc_glMultiTexCoord1i = (void (CODEGEN_FUNCPTR *)(GLenum, GLint))IntGetProcAddress("glMultiTexCoord1i");
	if(!_ptrc_glMultiTexCoord1i) numFailed++;
	_ptrc_glMultiTexCoord1iv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLint *))IntGetProcAddress("glMultiTexCoord1iv");
	if(!_ptrc_glMultiTexCoord1iv) numFailed++;
	_ptrc_glMultiTexCoord1s = (void (CODEGEN_FUNCPTR *)(GLenum, GLshort))IntGetProcAddress("glMultiTexCoord1s");
	if(!_ptrc_glMultiTexCoord1s) numFailed++;
	_ptrc_glMultiTexCoord1sv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLshort *))IntGetProcAddress("glMultiTexCoord1sv");
	if(!_ptrc_glMultiTexCoord1sv) numFailed++;
	_ptrc_glMultiTexCoord2d = (void (CODEGEN_FUNCPTR *)(GLenum, GLdouble, GLdouble))IntGetProcAddress("glMultiTexCoord2d");
	if(!_ptrc_glMultiTexCoord2d) numFailed++;
	_ptrc_glMultiTexCoord2dv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLdouble *))IntGetProcAddress("glMultiTexCoord2dv");
	if(!_ptrc_glMultiTexCoord2dv) numFailed++;
	_ptrc_glMultiTexCoord2f = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat, GLfloat))IntGetProcAddress("glMultiTexCoord2f");
	if(!_ptrc_glMultiTexCoord2f) numFailed++;
	_ptrc_glMultiTexCoord2fv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLfloat *))IntGetProcAddress("glMultiTexCoord2fv");
	if(!_ptrc_glMultiTexCoord2fv) numFailed++;
	_ptrc_glMultiTexCoord2i = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint))IntGetProcAddress("glMultiTexCoord2i");
	if(!_ptrc_glMultiTexCoord2i) numFailed++;
	_ptrc_glMultiTexCoord2iv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLint *))IntGetProcAddress("glMultiTexCoord2iv");
	if(!_ptrc_glMultiTexCoord2iv) numFailed++;
	_ptrc_glMultiTexCoord2s = (void (CODEGEN_FUNCPTR *)(GLenum, GLshort, GLshort))IntGetProcAddress("glMultiTexCoord2s");
	if(!_ptrc_glMultiTexCoord2s) numFailed++;
	_ptrc_glMultiTexCoord2sv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLshort *))IntGetProcAddress("glMultiTexCoord2sv");
	if(!_ptrc_glMultiTexCoord2sv) numFailed++;
	_ptrc_glMultiTexCoord3d = (void (CODEGEN_FUNCPTR *)(GLenum, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glMultiTexCoord3d");
	if(!_ptrc_glMultiTexCoord3d) numFailed++;
	_ptrc_glMultiTexCoord3dv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLdouble *))IntGetProcAddress("glMultiTexCoord3dv");
	if(!_ptrc_glMultiTexCoord3dv) numFailed++;
	_ptrc_glMultiTexCoord3f = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glMultiTexCoord3f");
	if(!_ptrc_glMultiTexCoord3f) numFailed++;
	_ptrc_glMultiTexCoord3fv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLfloat *))IntGetProcAddress("glMultiTexCoord3fv");
	if(!_ptrc_glMultiTexCoord3fv) numFailed++;
	_ptrc_glMultiTexCoord3i = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint))IntGetProcAddress("glMultiTexCoord3i");
	if(!_ptrc_glMultiTexCoord3i) numFailed++;
	_ptrc_glMultiTexCoord3iv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLint *))IntGetProcAddress("glMultiTexCoord3iv");
	if(!_ptrc_glMultiTexCoord3iv) numFailed++;
	_ptrc_glMultiTexCoord3s = (void (CODEGEN_FUNCPTR *)(GLenum, GLshort, GLshort, GLshort))IntGetProcAddress("glMultiTexCoord3s");
	if(!_ptrc_glMultiTexCoord3s) numFailed++;
	_ptrc_glMultiTexCoord3sv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLshort *))IntGetProcAddress("glMultiTexCoord3sv");
	if(!_ptrc_glMultiTexCoord3sv) numFailed++;
	_ptrc_glMultiTexCoord4d = (void (CODEGEN_FUNCPTR *)(GLenum, GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glMultiTexCoord4d");
	if(!_ptrc_glMultiTexCoord4d) numFailed++;
	_ptrc_glMultiTexCoord4dv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLdouble *))IntGetProcAddress("glMultiTexCoord4dv");
	if(!_ptrc_glMultiTexCoord4dv) numFailed++;
	_ptrc_glMultiTexCoord4f = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glMultiTexCoord4f");
	if(!_ptrc_glMultiTexCoord4f) numFailed++;
	_ptrc_glMultiTexCoord4fv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLfloat *))IntGetProcAddress("glMultiTexCoord4fv");
	if(!_ptrc_glMultiTexCoord4fv) numFailed++;
	_ptrc_glMultiTexCoord4i = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLint))IntGetProcAddress("glMultiTexCoord4i");
	if(!_ptrc_glMultiTexCoord4i) numFailed++;
	_ptrc_glMultiTexCoord4iv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLint *))IntGetProcAddress("glMultiTexCoord4iv");
	if(!_ptrc_glMultiTexCoord4iv) numFailed++;
	_ptrc_glMultiTexCoord4s = (void (CODEGEN_FUNCPTR *)(GLenum, GLshort, GLshort, GLshort, GLshort))IntGetProcAddress("glMultiTexCoord4s");
	if(!_ptrc_glMultiTexCoord4s) numFailed++;
	_ptrc_glMultiTexCoord4sv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLshort *))IntGetProcAddress("glMultiTexCoord4sv");
	if(!_ptrc_glMultiTexCoord4sv) numFailed++;
	_ptrc_glSampleCoverage = (void (CODEGEN_FUNCPTR *)(GLfloat, GLboolean))IntGetProcAddress("glSampleCoverage");
	if(!_ptrc_glSampleCoverage) numFailed++;
	_ptrc_glBlendFuncSeparate = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLenum))IntGetProcAddress("glBlendFuncSeparate");
	if(!_ptrc_glBlendFuncSeparate) numFailed++;
	_ptrc_glFogCoordPointer = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glFogCoordPointer");
	if(!_ptrc_glFogCoordPointer) numFailed++;
	_ptrc_glFogCoordd = (void (CODEGEN_FUNCPTR *)(GLdouble))IntGetProcAddress("glFogCoordd");
	if(!_ptrc_glFogCoordd) numFailed++;
	_ptrc_glFogCoorddv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glFogCoorddv");
	if(!_ptrc_glFogCoorddv) numFailed++;
	_ptrc_glFogCoordf = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glFogCoordf");
	if(!_ptrc_glFogCoordf) numFailed++;
	_ptrc_glFogCoordfv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glFogCoordfv");
	if(!_ptrc_glFogCoordfv) numFailed++;
	_ptrc_glMultiDrawArrays = (void (CODEGEN_FUNCPTR *)(GLenum, const GLint *, const GLsizei *, GLsizei))IntGetProcAddress("glMultiDrawArrays");
	if(!_ptrc_glMultiDrawArrays) numFailed++;
	_ptrc_glMultiDrawElements = (void (CODEGEN_FUNCPTR *)(GLenum, const GLsizei *, GLenum, const GLvoid *const*, GLsizei))IntGetProcAddress("glMultiDrawElements");
	if(!_ptrc_glMultiDrawElements) numFailed++;
	_ptrc_glPointParameterf = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat))IntGetProcAddress("glPointParameterf");
	if(!_ptrc_glPointParameterf) numFailed++;
	_ptrc_glPointParameterfv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLfloat *))IntGetProcAddress("glPointParameterfv");
	if(!_ptrc_glPointParameterfv) numFailed++;
	_ptrc_glPointParameteri = (void (CODEGEN_FUNCPTR *)(GLenum, GLint))IntGetProcAddress("glPointParameteri");
	if(!_ptrc_glPointParameteri) numFailed++;
	_ptrc_glPointParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLint *))IntGetProcAddress("glPointParameteriv");
	if(!_ptrc_glPointParameteriv) numFailed++;
	_ptrc_glSecondaryColor3b = (void (CODEGEN_FUNCPTR *)(GLbyte, GLbyte, GLbyte))IntGetProcAddress("glSecondaryColor3b");
	if(!_ptrc_glSecondaryColor3b) numFailed++;
	_ptrc_glSecondaryColor3bv = (void (CODEGEN_FUNCPTR *)(const GLbyte *))IntGetProcAddress("glSecondaryColor3bv");
	if(!_ptrc_glSecondaryColor3bv) numFailed++;
	_ptrc_glSecondaryColor3d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble))IntGetProcAddress("glSecondaryColor3d");
	if(!_ptrc_glSecondaryColor3d) numFailed++;
	_ptrc_glSecondaryColor3dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glSecondaryColor3dv");
	if(!_ptrc_glSecondaryColor3dv) numFailed++;
	_ptrc_glSecondaryColor3f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat))IntGetProcAddress("glSecondaryColor3f");
	if(!_ptrc_glSecondaryColor3f) numFailed++;
	_ptrc_glSecondaryColor3fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glSecondaryColor3fv");
	if(!_ptrc_glSecondaryColor3fv) numFailed++;
	_ptrc_glSecondaryColor3i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint))IntGetProcAddress("glSecondaryColor3i");
	if(!_ptrc_glSecondaryColor3i) numFailed++;
	_ptrc_glSecondaryColor3iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glSecondaryColor3iv");
	if(!_ptrc_glSecondaryColor3iv) numFailed++;
	_ptrc_glSecondaryColor3s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort, GLshort))IntGetProcAddress("glSecondaryColor3s");
	if(!_ptrc_glSecondaryColor3s) numFailed++;
	_ptrc_glSecondaryColor3sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glSecondaryColor3sv");
	if(!_ptrc_glSecondaryColor3sv) numFailed++;
	_ptrc_glSecondaryColor3ub = (void (CODEGEN_FUNCPTR *)(GLubyte, GLubyte, GLubyte))IntGetProcAddress("glSecondaryColor3ub");
	if(!_ptrc_glSecondaryColor3ub) numFailed++;
	_ptrc_glSecondaryColor3ubv = (void (CODEGEN_FUNCPTR *)(const GLubyte *))IntGetProcAddress("glSecondaryColor3ubv");
	if(!_ptrc_glSecondaryColor3ubv) numFailed++;
	_ptrc_glSecondaryColor3ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint))IntGetProcAddress("glSecondaryColor3ui");
	if(!_ptrc_glSecondaryColor3ui) numFailed++;
	_ptrc_glSecondaryColor3uiv = (void (CODEGEN_FUNCPTR *)(const GLuint *))IntGetProcAddress("glSecondaryColor3uiv");
	if(!_ptrc_glSecondaryColor3uiv) numFailed++;
	_ptrc_glSecondaryColor3us = (void (CODEGEN_FUNCPTR *)(GLushort, GLushort, GLushort))IntGetProcAddress("glSecondaryColor3us");
	if(!_ptrc_glSecondaryColor3us) numFailed++;
	_ptrc_glSecondaryColor3usv = (void (CODEGEN_FUNCPTR *)(const GLushort *))IntGetProcAddress("glSecondaryColor3usv");
	if(!_ptrc_glSecondaryColor3usv) numFailed++;
	_ptrc_glSecondaryColorPointer = (void (CODEGEN_FUNCPTR *)(GLint, GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glSecondaryColorPointer");
	if(!_ptrc_glSecondaryColorPointer) numFailed++;
	_ptrc_glWindowPos2d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble))IntGetProcAddress("glWindowPos2d");
	if(!_ptrc_glWindowPos2d) numFailed++;
	_ptrc_glWindowPos2dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glWindowPos2dv");
	if(!_ptrc_glWindowPos2dv) numFailed++;
	_ptrc_glWindowPos2f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat))IntGetProcAddress("glWindowPos2f");
	if(!_ptrc_glWindowPos2f) numFailed++;
	_ptrc_glWindowPos2fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glWindowPos2fv");
	if(!_ptrc_glWindowPos2fv) numFailed++;
	_ptrc_glWindowPos2i = (void (CODEGEN_FUNCPTR *)(GLint, GLint))IntGetProcAddress("glWindowPos2i");
	if(!_ptrc_glWindowPos2i) numFailed++;
	_ptrc_glWindowPos2iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glWindowPos2iv");
	if(!_ptrc_glWindowPos2iv) numFailed++;
	_ptrc_glWindowPos2s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort))IntGetProcAddress("glWindowPos2s");
	if(!_ptrc_glWindowPos2s) numFailed++;
	_ptrc_glWindowPos2sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glWindowPos2sv");
	if(!_ptrc_glWindowPos2sv) numFailed++;
	_ptrc_glWindowPos3d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble))IntGetProcAddress("glWindowPos3d");
	if(!_ptrc_glWindowPos3d) numFailed++;
	_ptrc_glWindowPos3dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glWindowPos3dv");
	if(!_ptrc_glWindowPos3dv) numFailed++;
	_ptrc_glWindowPos3f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat))IntGetProcAddress("glWindowPos3f");
	if(!_ptrc_glWindowPos3f) numFailed++;
	_ptrc_glWindowPos3fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glWindowPos3fv");
	if(!_ptrc_glWindowPos3fv) numFailed++;
	_ptrc_glWindowPos3i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint))IntGetProcAddress("glWindowPos3i");
	if(!_ptrc_glWindowPos3i) numFailed++;
	_ptrc_glWindowPos3iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glWindowPos3iv");
	if(!_ptrc_glWindowPos3iv) numFailed++;
	_ptrc_glWindowPos3s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort, GLshort))IntGetProcAddress("glWindowPos3s");
	if(!_ptrc_glWindowPos3s) numFailed++;
	_ptrc_glWindowPos3sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glWindowPos3sv");
	if(!_ptrc_glWindowPos3sv) numFailed++;
	_ptrc_glBeginQuery = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glBeginQuery");
	if(!_ptrc_glBeginQuery) numFailed++;
	_ptrc_glBindBuffer = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glBindBuffer");
	if(!_ptrc_glBindBuffer) numFailed++;
	_ptrc_glBufferData = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizeiptr, const GLvoid *, GLenum))IntGetProcAddress("glBufferData");
	if(!_ptrc_glBufferData) numFailed++;
	_ptrc_glBufferSubData = (void (CODEGEN_FUNCPTR *)(GLenum, GLintptr, GLsizeiptr, const GLvoid *))IntGetProcAddress("glBufferSubData");
	if(!_ptrc_glBufferSubData) numFailed++;
	_ptrc_glDeleteBuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteBuffers");
	if(!_ptrc_glDeleteBuffers) numFailed++;
	_ptrc_glDeleteQueries = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteQueries");
	if(!_ptrc_glDeleteQueries) numFailed++;
	_ptrc_glEndQuery = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glEndQuery");
	if(!_ptrc_glEndQuery) numFailed++;
	_ptrc_glGenBuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenBuffers");
	if(!_ptrc_glGenBuffers) numFailed++;
	_ptrc_glGenQueries = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenQueries");
	if(!_ptrc_glGenQueries) numFailed++;
	_ptrc_glGetBufferParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetBufferParameteriv");
	if(!_ptrc_glGetBufferParameteriv) numFailed++;
	_ptrc_glGetBufferPointerv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLvoid **))IntGetProcAddress("glGetBufferPointerv");
	if(!_ptrc_glGetBufferPointerv) numFailed++;
	_ptrc_glGetBufferSubData = (void (CODEGEN_FUNCPTR *)(GLenum, GLintptr, GLsizeiptr, GLvoid *))IntGetProcAddress("glGetBufferSubData");
	if(!_ptrc_glGetBufferSubData) numFailed++;
	_ptrc_glGetQueryObjectiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetQueryObjectiv");
	if(!_ptrc_glGetQueryObjectiv) numFailed++;
	_ptrc_glGetQueryObjectuiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint *))IntGetProcAddress("glGetQueryObjectuiv");
	if(!_ptrc_glGetQueryObjectuiv) numFailed++;
	_ptrc_glGetQueryiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetQueryiv");
	if(!_ptrc_glGetQueryiv) numFailed++;
	_ptrc_glIsBuffer = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsBuffer");
	if(!_ptrc_glIsBuffer) numFailed++;
	_ptrc_glIsQuery = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsQuery");
	if(!_ptrc_glIsQuery) numFailed++;
	_ptrc_glMapBuffer = (void * (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glMapBuffer");
	if(!_ptrc_glMapBuffer) numFailed++;
	_ptrc_glUnmapBuffer = (GLboolean (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glUnmapBuffer");
	if(!_ptrc_glUnmapBuffer) numFailed++;
	_ptrc_glAttachShader = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glAttachShader");
	if(!_ptrc_glAttachShader) numFailed++;
	_ptrc_glBindAttribLocation = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, const GLchar *))IntGetProcAddress("glBindAttribLocation");
	if(!_ptrc_glBindAttribLocation) numFailed++;
	_ptrc_glBlendEquationSeparate = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glBlendEquationSeparate");
	if(!_ptrc_glBlendEquationSeparate) numFailed++;
	_ptrc_glCompileShader = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glCompileShader");
	if(!_ptrc_glCompileShader) numFailed++;
	_ptrc_glCreateProgram = (GLuint (CODEGEN_FUNCPTR *)())IntGetProcAddress("glCreateProgram");
	if(!_ptrc_glCreateProgram) numFailed++;
	_ptrc_glCreateShader = (GLuint (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glCreateShader");
	if(!_ptrc_glCreateShader) numFailed++;
	_ptrc_glDeleteProgram = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glDeleteProgram");
	if(!_ptrc_glDeleteProgram) numFailed++;
	_ptrc_glDeleteShader = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glDeleteShader");
	if(!_ptrc_glDeleteShader) numFailed++;
	_ptrc_glDetachShader = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glDetachShader");
	if(!_ptrc_glDetachShader) numFailed++;
	_ptrc_glDisableVertexAttribArray = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glDisableVertexAttribArray");
	if(!_ptrc_glDisableVertexAttribArray) numFailed++;
	_ptrc_glDrawBuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLenum *))IntGetProcAddress("glDrawBuffers");
	if(!_ptrc_glDrawBuffers) numFailed++;
	_ptrc_glEnableVertexAttribArray = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glEnableVertexAttribArray");
	if(!_ptrc_glEnableVertexAttribArray) numFailed++;
	_ptrc_glGetActiveAttrib = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *))IntGetProcAddress("glGetActiveAttrib");
	if(!_ptrc_glGetActiveAttrib) numFailed++;
	_ptrc_glGetActiveUniform = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *))IntGetProcAddress("glGetActiveUniform");
	if(!_ptrc_glGetActiveUniform) numFailed++;
	_ptrc_glGetAttachedShaders = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLsizei *, GLuint *))IntGetProcAddress("glGetAttachedShaders");
	if(!_ptrc_glGetAttachedShaders) numFailed++;
	_ptrc_glGetAttribLocation = (GLint (CODEGEN_FUNCPTR *)(GLuint, const GLchar *))IntGetProcAddress("glGetAttribLocation");
	if(!_ptrc_glGetAttribLocation) numFailed++;
	_ptrc_glGetProgramInfoLog = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetProgramInfoLog");
	if(!_ptrc_glGetProgramInfoLog) numFailed++;
	_ptrc_glGetProgramiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetProgramiv");
	if(!_ptrc_glGetProgramiv) numFailed++;
	_ptrc_glGetShaderInfoLog = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetShaderInfoLog");
	if(!_ptrc_glGetShaderInfoLog) numFailed++;
	_ptrc_glGetShaderSource = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetShaderSource");
	if(!_ptrc_glGetShaderSource) numFailed++;
	_ptrc_glGetShaderiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetShaderiv");
	if(!_ptrc_glGetShaderiv) numFailed++;
	_ptrc_glGetUniformLocation = (GLint (CODEGEN_FUNCPTR *)(GLuint, const GLchar *))IntGetProcAddress("glGetUniformLocation");
	if(!_ptrc_glGetUniformLocation) numFailed++;
	_ptrc_glGetUniformfv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLfloat *))IntGetProcAddress("glGetUniformfv");
	if(!_ptrc_glGetUniformfv) numFailed++;
	_ptrc_glGetUniformiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint *))IntGetProcAddress("glGetUniformiv");
	if(!_ptrc_glGetUniformiv) numFailed++;
	_ptrc_glGetVertexAttribPointerv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLvoid **))IntGetProcAddress("glGetVertexAttribPointerv");
	if(!_ptrc_glGetVertexAttribPointerv) numFailed++;
	_ptrc_glGetVertexAttribdv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLdouble *))IntGetProcAddress("glGetVertexAttribdv");
	if(!_ptrc_glGetVertexAttribdv) numFailed++;
	_ptrc_glGetVertexAttribfv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLfloat *))IntGetProcAddress("glGetVertexAttribfv");
	if(!_ptrc_glGetVertexAttribfv) numFailed++;
	_ptrc_glGetVertexAttribiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetVertexAttribiv");
	if(!_ptrc_glGetVertexAttribiv) numFailed++;
	_ptrc_glIsProgram = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsProgram");
	if(!_ptrc_glIsProgram) numFailed++;
	_ptrc_glIsShader = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsShader");
	if(!_ptrc_glIsShader) numFailed++;
	_ptrc_glLinkProgram = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glLinkProgram");
	if(!_ptrc_glLinkProgram) numFailed++;
	_ptrc_glShaderSource = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLchar *const*, const GLint *))IntGetProcAddress("glShaderSource");
	if(!_ptrc_glShaderSource) numFailed++;
	_ptrc_glStencilFuncSeparate = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint, GLuint))IntGetProcAddress("glStencilFuncSeparate");
	if(!_ptrc_glStencilFuncSeparate) numFailed++;
	_ptrc_glStencilMaskSeparate = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glStencilMaskSeparate");
	if(!_ptrc_glStencilMaskSeparate) numFailed++;
	_ptrc_glStencilOpSeparate = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLenum))IntGetProcAddress("glStencilOpSeparate");
	if(!_ptrc_glStencilOpSeparate) numFailed++;
	_ptrc_glUniform1f = (void (CODEGEN_FUNCPTR *)(GLint, GLfloat))IntGetProcAddress("glUniform1f");
	if(!_ptrc_glUniform1f) numFailed++;
	_ptrc_glUniform1fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLfloat *))IntGetProcAddress("glUniform1fv");
	if(!_ptrc_glUniform1fv) numFailed++;
	_ptrc_glUniform1i = (void (CODEGEN_FUNCPTR *)(GLint, GLint))IntGetProcAddress("glUniform1i");
	if(!_ptrc_glUniform1i) numFailed++;
	_ptrc_glUniform1iv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLint *))IntGetProcAddress("glUniform1iv");
	if(!_ptrc_glUniform1iv) numFailed++;
	_ptrc_glUniform2f = (void (CODEGEN_FUNCPTR *)(GLint, GLfloat, GLfloat))IntGetProcAddress("glUniform2f");
	if(!_ptrc_glUniform2f) numFailed++;
	_ptrc_glUniform2fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLfloat *))IntGetProcAddress("glUniform2fv");
	if(!_ptrc_glUniform2fv) numFailed++;
	_ptrc_glUniform2i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint))IntGetProcAddress("glUniform2i");
	if(!_ptrc_glUniform2i) numFailed++;
	_ptrc_glUniform2iv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLint *))IntGetProcAddress("glUniform2iv");
	if(!_ptrc_glUniform2iv) numFailed++;
	_ptrc_glUniform3f = (void (CODEGEN_FUNCPTR *)(GLint, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glUniform3f");
	if(!_ptrc_glUniform3f) numFailed++;
	_ptrc_glUniform3fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLfloat *))IntGetProcAddress("glUniform3fv");
	if(!_ptrc_glUniform3fv) numFailed++;
	_ptrc_glUniform3i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint, GLint))IntGetProcAddress("glUniform3i");
	if(!_ptrc_glUniform3i) numFailed++;
	_ptrc_glUniform3iv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLint *))IntGetProcAddress("glUniform3iv");
	if(!_ptrc_glUniform3iv) numFailed++;
	_ptrc_glUniform4f = (void (CODEGEN_FUNCPTR *)(GLint, GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glUniform4f");
	if(!_ptrc_glUniform4f) numFailed++;
	_ptrc_glUniform4fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLfloat *))IntGetProcAddress("glUniform4fv");
	if(!_ptrc_glUniform4fv) numFailed++;
	_ptrc_glUniform4i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint, GLint, GLint))IntGetProcAddress("glUniform4i");
	if(!_ptrc_glUniform4i) numFailed++;
	_ptrc_glUniform4iv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLint *))IntGetProcAddress("glUniform4iv");
	if(!_ptrc_glUniform4iv) numFailed++;
	_ptrc_glUniformMatrix2fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix2fv");
	if(!_ptrc_glUniformMatrix2fv) numFailed++;
	_ptrc_glUniformMatrix3fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix3fv");
	if(!_ptrc_glUniformMatrix3fv) numFailed++;
	_ptrc_glUniformMatrix4fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix4fv");
	if(!_ptrc_glUniformMatrix4fv) numFailed++;
	_ptrc_glUseProgram = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glUseProgram");
	if(!_ptrc_glUseProgram) numFailed++;
	_ptrc_glValidateProgram = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glValidateProgram");
	if(!_ptrc_glValidateProgram) numFailed++;
	_ptrc_glVertexAttrib1d = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble))IntGetProcAddress("glVertexAttrib1d");
	if(!_ptrc_glVertexAttrib1d) numFailed++;
	_ptrc_glVertexAttrib1dv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLdouble *))IntGetProcAddress("glVertexAttrib1dv");
	if(!_ptrc_glVertexAttrib1dv) numFailed++;
	_ptrc_glVertexAttrib1f = (void (CODEGEN_FUNCPTR *)(GLuint, GLfloat))IntGetProcAddress("glVertexAttrib1f");
	if(!_ptrc_glVertexAttrib1f) numFailed++;
	_ptrc_glVertexAttrib1fv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLfloat *))IntGetProcAddress("glVertexAttrib1fv");
	if(!_ptrc_glVertexAttrib1fv) numFailed++;
	_ptrc_glVertexAttrib1s = (void (CODEGEN_FUNCPTR *)(GLuint, GLshort))IntGetProcAddress("glVertexAttrib1s");
	if(!_ptrc_glVertexAttrib1s) numFailed++;
	_ptrc_glVertexAttrib1sv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLshort *))IntGetProcAddress("glVertexAttrib1sv");
	if(!_ptrc_glVertexAttrib1sv) numFailed++;
	_ptrc_glVertexAttrib2d = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble, GLdouble))IntGetProcAddress("glVertexAttrib2d");
	if(!_ptrc_glVertexAttrib2d) numFailed++;
	_ptrc_glVertexAttrib2dv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLdouble *))IntGetProcAddress("glVertexAttrib2dv");
	if(!_ptrc_glVertexAttrib2dv) numFailed++;
	_ptrc_glVertexAttrib2f = (void (CODEGEN_FUNCPTR *)(GLuint, GLfloat, GLfloat))IntGetProcAddress("glVertexAttrib2f");
	if(!_ptrc_glVertexAttrib2f) numFailed++;
	_ptrc_glVertexAttrib2fv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLfloat *))IntGetProcAddress("glVertexAttrib2fv");
	if(!_ptrc_glVertexAttrib2fv) numFailed++;
	_ptrc_glVertexAttrib2s = (void (CODEGEN_FUNCPTR *)(GLuint, GLshort, GLshort))IntGetProcAddress("glVertexAttrib2s");
	if(!_ptrc_glVertexAttrib2s) numFailed++;
	_ptrc_glVertexAttrib2sv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLshort *))IntGetProcAddress("glVertexAttrib2sv");
	if(!_ptrc_glVertexAttrib2sv) numFailed++;
	_ptrc_glVertexAttrib3d = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glVertexAttrib3d");
	if(!_ptrc_glVertexAttrib3d) numFailed++;
	_ptrc_glVertexAttrib3dv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLdouble *))IntGetProcAddress("glVertexAttrib3dv");
	if(!_ptrc_glVertexAttrib3dv) numFailed++;
	_ptrc_glVertexAttrib3f = (void (CODEGEN_FUNCPTR *)(GLuint, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glVertexAttrib3f");
	if(!_ptrc_glVertexAttrib3f) numFailed++;
	_ptrc_glVertexAttrib3fv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLfloat *))IntGetProcAddress("glVertexAttrib3fv");
	if(!_ptrc_glVertexAttrib3fv) numFailed++;
	_ptrc_glVertexAttrib3s = (void (CODEGEN_FUNCPTR *)(GLuint, GLshort, GLshort, GLshort))IntGetProcAddress("glVertexAttrib3s");
	if(!_ptrc_glVertexAttrib3s) numFailed++;
	_ptrc_glVertexAttrib3sv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLshort *))IntGetProcAddress("glVertexAttrib3sv");
	if(!_ptrc_glVertexAttrib3sv) numFailed++;
	_ptrc_glVertexAttrib4Nbv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLbyte *))IntGetProcAddress("glVertexAttrib4Nbv");
	if(!_ptrc_glVertexAttrib4Nbv) numFailed++;
	_ptrc_glVertexAttrib4Niv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLint *))IntGetProcAddress("glVertexAttrib4Niv");
	if(!_ptrc_glVertexAttrib4Niv) numFailed++;
	_ptrc_glVertexAttrib4Nsv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLshort *))IntGetProcAddress("glVertexAttrib4Nsv");
	if(!_ptrc_glVertexAttrib4Nsv) numFailed++;
	_ptrc_glVertexAttrib4Nub = (void (CODEGEN_FUNCPTR *)(GLuint, GLubyte, GLubyte, GLubyte, GLubyte))IntGetProcAddress("glVertexAttrib4Nub");
	if(!_ptrc_glVertexAttrib4Nub) numFailed++;
	_ptrc_glVertexAttrib4Nubv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLubyte *))IntGetProcAddress("glVertexAttrib4Nubv");
	if(!_ptrc_glVertexAttrib4Nubv) numFailed++;
	_ptrc_glVertexAttrib4Nuiv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLuint *))IntGetProcAddress("glVertexAttrib4Nuiv");
	if(!_ptrc_glVertexAttrib4Nuiv) numFailed++;
	_ptrc_glVertexAttrib4Nusv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLushort *))IntGetProcAddress("glVertexAttrib4Nusv");
	if(!_ptrc_glVertexAttrib4Nusv) numFailed++;
	_ptrc_glVertexAttrib4bv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLbyte *))IntGetProcAddress("glVertexAttrib4bv");
	if(!_ptrc_glVertexAttrib4bv) numFailed++;
	_ptrc_glVertexAttrib4d = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glVertexAttrib4d");
	if(!_ptrc_glVertexAttrib4d) numFailed++;
	_ptrc_glVertexAttrib4dv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLdouble *))IntGetProcAddress("glVertexAttrib4dv");
	if(!_ptrc_glVertexAttrib4dv) numFailed++;
	_ptrc_glVertexAttrib4f = (void (CODEGEN_FUNCPTR *)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glVertexAttrib4f");
	if(!_ptrc_glVertexAttrib4f) numFailed++;
	_ptrc_glVertexAttrib4fv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLfloat *))IntGetProcAddress("glVertexAttrib4fv");
	if(!_ptrc_glVertexAttrib4fv) numFailed++;
	_ptrc_glVertexAttrib4iv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLint *))IntGetProcAddress("glVertexAttrib4iv");
	if(!_ptrc_glVertexAttrib4iv) numFailed++;
	_ptrc_glVertexAttrib4s = (void (CODEGEN_FUNCPTR *)(GLuint, GLshort, GLshort, GLshort, GLshort))IntGetProcAddress("glVertexAttrib4s");
	if(!_ptrc_glVertexAttrib4s) numFailed++;
	_ptrc_glVertexAttrib4sv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLshort *))IntGetProcAddress("glVertexAttrib4sv");
	if(!_ptrc_glVertexAttrib4sv) numFailed++;
	_ptrc_glVertexAttrib4ubv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLubyte *))IntGetProcAddress("glVertexAttrib4ubv");
	if(!_ptrc_glVertexAttrib4ubv) numFailed++;
	_ptrc_glVertexAttrib4uiv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLuint *))IntGetProcAddress("glVertexAttrib4uiv");
	if(!_ptrc_glVertexAttrib4uiv) numFailed++;
	_ptrc_glVertexAttrib4usv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLushort *))IntGetProcAddress("glVertexAttrib4usv");
	if(!_ptrc_glVertexAttrib4usv) numFailed++;
	_ptrc_glVertexAttribPointer = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *))IntGetProcAddress("glVertexAttribPointer");
	if(!_ptrc_glVertexAttribPointer) numFailed++;
	_ptrc_glUniformMatrix2x3fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix2x3fv");
	if(!_ptrc_glUniformMatrix2x3fv) numFailed++;
	_ptrc_glUniformMatrix2x4fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix2x4fv");
	if(!_ptrc_glUniformMatrix2x4fv) numFailed++;
	_ptrc_glUniformMatrix3x2fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix3x2fv");
	if(!_ptrc_glUniformMatrix3x2fv) numFailed++;
	_ptrc_glUniformMatrix3x4fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix3x4fv");
	if(!_ptrc_glUniformMatrix3x4fv) numFailed++;
	_ptrc_glUniformMatrix4x2fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix4x2fv");
	if(!_ptrc_glUniformMatrix4x2fv) numFailed++;
	_ptrc_glUniformMatrix4x3fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix4x3fv");
	if(!_ptrc_glUniformMatrix4x3fv) numFailed++;
	return numFailed;
}

typedef int (*PFN_LOADFUNCPOINTERS)();
typedef struct ogl_StrToExtMap_s
{
	char *extensionName;
	int *extensionVariable;
	PFN_LOADFUNCPOINTERS LoadExtension;
} ogl_StrToExtMap;

static ogl_StrToExtMap ExtensionMap[7] = {
	{"GL_EXT_framebuffer_object", &ogl_ext_EXT_framebuffer_object, Load_EXT_framebuffer_object},
	{"GL_EXT_texture_compression_s3tc", &ogl_ext_EXT_texture_compression_s3tc, NULL},
	{"GL_KHR_debug", &ogl_ext_KHR_debug, Load_KHR_debug},
	{"GL_ARB_debug_output", &ogl_ext_ARB_debug_output, Load_ARB_debug_output},
	{"GL_EXT_texture_sRGB", &ogl_ext_EXT_texture_sRGB, NULL},
	{"GL_EXT_texture_filter_anisotropic", &ogl_ext_EXT_texture_filter_anisotropic, NULL},
	{"GL_ARB_framebuffer_sRGB", &ogl_ext_ARB_framebuffer_sRGB, NULL},
};

static int g_extensionMapSize = 7;

static ogl_StrToExtMap *FindExtEntry(const char *extensionName)
{
	int loop;
	ogl_StrToExtMap *currLoc = ExtensionMap;
	for(loop = 0; loop < g_extensionMapSize; ++loop, ++currLoc)
	{
		if(strcmp(extensionName, currLoc->extensionName) == 0)
			return currLoc;
	}
	
	return NULL;
}

static void ClearExtensionVars()
{
	ogl_ext_EXT_framebuffer_object = ogl_LOAD_FAILED;
	ogl_ext_EXT_texture_compression_s3tc = ogl_LOAD_FAILED;
	ogl_ext_KHR_debug = ogl_LOAD_FAILED;
	ogl_ext_ARB_debug_output = ogl_LOAD_FAILED;
	ogl_ext_EXT_texture_sRGB = ogl_LOAD_FAILED;
	ogl_ext_EXT_texture_filter_anisotropic = ogl_LOAD_FAILED;
	ogl_ext_ARB_framebuffer_sRGB = ogl_LOAD_FAILED;
}


static void LoadExtByName(const char *extensionName)
{
	ogl_StrToExtMap *entry = NULL;
	entry = FindExtEntry(extensionName);
	if(entry)
	{
		if(entry->LoadExtension)
		{
			int numFailed = entry->LoadExtension();
			if(numFailed == 0)
			{
				*(entry->extensionVariable) = ogl_LOAD_SUCCEEDED;
			}
			else
			{
				*(entry->extensionVariable) = ogl_LOAD_SUCCEEDED + numFailed;
			}
		}
		else
		{
			*(entry->extensionVariable) = ogl_LOAD_SUCCEEDED;
		}
	}
}


static void ProcExtsFromExtString(const char *strExtList)
{
	size_t iExtListLen = strlen(strExtList);
	const char *strExtListEnd = strExtList + iExtListLen;
	const char *strCurrPos = strExtList;
	char strWorkBuff[256];

	while(*strCurrPos)
	{
		/*Get the extension at our position.*/
		int iStrLen = 0;
		const char *strEndStr = strchr(strCurrPos, ' ');
		int iStop = 0;
		if(strEndStr == NULL)
		{
			strEndStr = strExtListEnd;
			iStop = 1;
		}

		iStrLen = (int)((ptrdiff_t)strEndStr - (ptrdiff_t)strCurrPos);

		if(iStrLen > 255)
			return;

		strncpy(strWorkBuff, strCurrPos, iStrLen);
		strWorkBuff[iStrLen] = '\0';

		LoadExtByName(strWorkBuff);

		strCurrPos = strEndStr + 1;
		if(iStop) break;
	}
}

int ogl_LoadFunctions()
{
	int numFailed = 0;
	ClearExtensionVars();
	
	_ptrc_glGetString = (const GLubyte * (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glGetString");
	if(!_ptrc_glGetString) return ogl_LOAD_FAILED;
	
	ProcExtsFromExtString((const char *)_ptrc_glGetString(GL_EXTENSIONS));
	numFailed = Load_Version_2_1();
	
	if(numFailed == 0)
		return ogl_LOAD_SUCCEEDED;
	else
		return ogl_LOAD_SUCCEEDED + numFailed;
}

static int g_major_version = 0;
static int g_minor_version = 0;

static void ParseVersionFromString(int *pOutMajor, int *pOutMinor, const char *strVersion)
{
	const char *strDotPos = NULL;
	int iLength = 0;
	char strWorkBuff[10];
	*pOutMinor = 0;
	*pOutMajor = 0;

	strDotPos = strchr(strVersion, '.');
	if(!strDotPos)
		return;

	iLength = (int)((ptrdiff_t)strDotPos - (ptrdiff_t)strVersion);
	strncpy(strWorkBuff, strVersion, iLength);
	strWorkBuff[iLength] = '\0';

	*pOutMajor = atoi(strWorkBuff);
	strDotPos = strchr(strVersion + iLength + 1, ' ');
	if(!strDotPos)
	{
		/*No extra data. Take the whole rest of the string.*/
		strcpy(strWorkBuff, strVersion + iLength + 1);
	}
	else
	{
		/*Copy only up until the space.*/
		int iLengthMinor = (int)((ptrdiff_t)strDotPos - (ptrdiff_t)strVersion);
		iLengthMinor = iLengthMinor - (iLength + 1);
		strncpy(strWorkBuff, strVersion + iLength + 1, iLengthMinor);
		strWorkBuff[iLengthMinor] = '\0';
	}

	*pOutMinor = atoi(strWorkBuff);
}

static void GetGLVersion()
{
	ParseVersionFromString(&g_major_version, &g_minor_version, (const char*)glGetString(GL_VERSION));
}

int ogl_GetMajorVersion()
{
	if(g_major_version == 0)
		GetGLVersion();
	return g_major_version;
}

int ogl_GetMinorVersion()
{
	if(g_major_version == 0) //Yes, check the major version to get the minor one.
		GetGLVersion();
	return g_minor_version;
}

int ogl_IsVersionGEQ(int majorVersion, int minorVersion)
{
	if(g_major_version == 0)
		GetGLVersion();
		
	if(majorVersion > g_major_version) return 1;
	if(majorVersion < g_major_version) return 0;
	if(minorVersion >= g_minor_version) return 1;
	return 0;
}

