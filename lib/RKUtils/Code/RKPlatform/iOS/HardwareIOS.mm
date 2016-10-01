#include "Platform/Hardware.h"
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#include <sys/sysctl.h>
#include "RK.h"
#include "RKVector.h"
#include "RKMatrix.h"
#include "RKDevice.h"

#define _HARDWARE_PRINT_BENCHMARK_INFO 0

using namespace CasualCore;

Hardware::CpuRating Hardware::_PerformCpuBenchmark()
{
  static const unsigned int iterationsCount = 22;
  static const unsigned int arraySize = 1024;
  
  int array[arraySize]; // filling array with pseudo-random content
  for (int i = 0; i < arraySize; ++i)
  {
    array[i] = (123456 * i + i / 10) % arraySize;
  }
  int data = 0;
  
  
  /* // assembly below roughly corresponds to:
  for (int i = iterationsCount; i != 0; --i)
  {
    data = data * data;
    while (data > arraySize)
    {
      data -= arraySize;
    }
    data += array[data];
    data += 1;
  }*/
  const CFAbsoluteTime timeBefore = CFAbsoluteTimeGetCurrent();
  
#if defined(__arm64__)
    return CPU_HIGH;
#else
    
  asm volatile (
                "mov r0, %0 \n\t" // r0 contains data
                "mov r1, %1 \n\t" // r1 contains loop counter
                "L_MAIN_LOOP: \n\t"
                
                // r0 = r0 * r0
                "mul r0, r0 \n\t"
                
                // while (r0 >= arraySize) r0 -= arraySize
                "L_MOD_LOOP: \n\t"
                "cmp r0, %3 \n\t"
                "blo L_MOD_LOOP_END \n\t"
                "sub r0, %3 \n\t"
                "b L_MOD_LOOP \n\t"
                "L_MOD_LOOP_END: \n\t"
                
                // r0 += array[r0] + 1
                "ldr r2, [%2,r0] \n\t"
                "add r0, r2 \n\t"
                "add r0, r1 \n\t"
                
                "subs r1, #1 \n\t"
                "bne L_MAIN_LOOP \n\t"
                "mov %0, r0 \n\t"
                : "+r"(data)
                : "r"(iterationsCount), "r"(array), "r"(arraySize)
                : "r0", "r1", "r2" );
#endif
  const CFAbsoluteTime timeAfter = CFAbsoluteTimeGetCurrent();
  const CFAbsoluteTime timeDelta = timeAfter - timeBefore;
  
#if _HARDWARE_PRINT_BENCHMARK_INFO
  RKLOG("CPU TIME: %f", (float)timeDelta);
#endif
  
  // variation up to 10ms =(
  
  // 3gs             - 105ms
  // ipod3           - 100ms O_o?
  // A4 (4,ipod4)    -  78ms
  // A4 (ipad1)      -  60ms
  // A5 (4s,ipod5)   -  50ms
  // A5 (ipad2,mini) -  40ms
  // A5X(ipad3)      -  40ms
  // A6 (5,5c)       -  35ms
  // A6X(ipad4)      -  32ms
  // A7 (5s)         -  25ms
  
  if (timeDelta > 0.055f) // iphone4 and lower are CPU_LOW
  {
    return CPU_LOW;
  }
  else if (timeDelta > 0.037f) // iphone4s and most ipads are CPU_MED
  {
    return CPU_MEDIUM;
  }
  return CPU_HIGH; // iphone5 and later are CPU_HIGH
}

static inline void glCheck() {
  GLuint error = glGetError();
  if (error != GL_NO_ERROR) {
    RKASSERT(false, "GL error: 0x%x", error);
  }
}

static GLuint _CreateShader(GLuint type, const char* text)
{
  GLuint shader = glCreateShader(type); glCheck();
  glShaderSource(shader, 1, &text, NULL); glCheck();
  glCompileShader(shader); glCheck();
  
  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status); glCheck();
  if (status != GL_TRUE)
  {
    char log[1024];
    GLint logSize;
    glGetShaderInfoLog(shader, ARRAYSIZE(log), &logSize, log); glCheck();
    RKASSERT(false, "Compilation error:\n%s", log);
  }
  
  return shader;
}

static CFTimeInterval _GpuBenchmarkDraw(uint32 primitiveCount, uint32 drawCount)
{
  glClear(GL_COLOR_BUFFER_BIT); glCheck();
  const CFAbsoluteTime timeBefore = CFAbsoluteTimeGetCurrent();
  // because glDrawArrays seems to write to disc if it's compiling shaders  
  if (RKDevice_HasEnoughDiscSpace())
  {
    for (unsigned int i = 0; i < drawCount; ++i)
    {
      glDrawArrays(GL_TRIANGLE_FAN, 0, primitiveCount); glCheck();
    }
  }  
  glFinish(); glCheck();
  const CFAbsoluteTime timeAfter = CFAbsoluteTimeGetCurrent();
  const CFTimeInterval timeDelta = timeAfter - timeBefore;
  return timeDelta;
}

CFTimeInterval _GpuBenchmarkInCurrentFramebuffer()
{
  static const unsigned int iterationsCount = 32;
  static const char* vertexShaderText =
  "attribute vec4 pos;                \n"
  "attribute vec4 color;              \n"
  "varying lowp vec4 vcolor;          \n"
  "uniform highp mat4 mat;            \n"
  "                                   \n"
  "void main() {                      \n"
  "  vcolor = color;                  \n"
  "  gl_Position = pos;               \n"
  "  gl_Position = mat * gl_Position; \n" // repeat this line if you want to make it slower
  "  gl_Position = mat * gl_Position; \n"
  "  gl_Position = mat * gl_Position; \n"
  "  gl_Position = mat * gl_Position; \n"
  "  gl_Position = mat * gl_Position; \n"
  "  gl_Position = mat * gl_Position; \n"
  "  gl_Position = mat * gl_Position; \n"
  "  gl_Position = mat * gl_Position; \n"
  "  gl_Position = mat * gl_Position; \n"
  "  gl_Position = mat * gl_Position; \n"
  "  gl_Position = mat * gl_Position; \n"
  "  gl_Position = mat * gl_Position; \n"
  "  gl_Position = mat * gl_Position; \n"
  "  gl_Position = mat * gl_Position; \n"
  "  gl_Position = mat * gl_Position; \n"
  "}                                  \n";
  static const char* pixelShaderText =
  "varying lowp vec4 vcolor;               \n"
  "uniform highp mat4 mat;                 \n"
  "                                        \n"
  "void main() {                           \n"
  "  gl_FragColor    = vcolor;             \n"
  "  gl_FragColor    = mat * gl_FragColor; \n" // repeat this line if you want to make it slower
  "  gl_FragColor    = mat * gl_FragColor; \n"
  "  gl_FragColor    = mat * gl_FragColor; \n"
  "  gl_FragColor    = mat * gl_FragColor; \n"
  "  gl_FragColor    = mat * gl_FragColor; \n"
  "  gl_FragColor    = mat * gl_FragColor; \n"
  "  gl_FragColor    = mat * gl_FragColor; \n"
  "  gl_FragColor    = mat * gl_FragColor; \n"
  "  gl_FragColor    = mat * gl_FragColor; \n"
  "  gl_FragColor    = mat * gl_FragColor; \n"
  "  gl_FragColor    = mat * gl_FragColor; \n"
  "  gl_FragColor    = mat * gl_FragColor; \n"
  "  gl_FragColor    = mat * gl_FragColor; \n"
  "  gl_FragColor    = mat * gl_FragColor; \n"
  "  gl_FragColor    = mat * gl_FragColor; \n"
  "  gl_FragColor    = mat * gl_FragColor; \n"
  "  gl_FragColor    = mat * gl_FragColor; \n"
  "}                                       \n";
  
  GLuint vertexShader = _CreateShader(GL_VERTEX_SHADER, vertexShaderText);
  GLuint pixelShader = _CreateShader(GL_FRAGMENT_SHADER, pixelShaderText);
  GLuint shaderProgram = glCreateProgram(); glCheck();
  GLint shaderProgramLinkStatus;
  glAttachShader(shaderProgram, vertexShader); glCheck();
  glAttachShader(shaderProgram, pixelShader); glCheck();
  glBindAttribLocation(shaderProgram, 0, "pos"); glCheck();
  glBindAttribLocation(shaderProgram, 1, "color"); glCheck();
  glLinkProgram(shaderProgram); glCheck();
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &shaderProgramLinkStatus); glCheck();
  RKASSERT(shaderProgramLinkStatus == GL_TRUE, "Link failed");
  glUseProgram(shaderProgram); glCheck();
  
  GLuint uniformLocation = glGetUniformLocation(shaderProgram, "mat"); glCheck();
  RKMatrix matrix = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1,
  };
  glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, (float*)&matrix); glCheck();
  
  RKVector data[8];
  data[0].Set(-1,-1, 0);
  data[2].Set(-1, 1, 0);
  data[4].Set( 1, 1, 0);
  data[6].Set( 1,-1, 0);
  data[1].Set(float(rand()) / RAND_MAX, float(rand()) / RAND_MAX, float(rand()) / RAND_MAX);
  data[3].Set(float(rand()) / RAND_MAX, float(rand()) / RAND_MAX, float(rand()) / RAND_MAX);
  data[5].Set(float(rand()) / RAND_MAX, float(rand()) / RAND_MAX, float(rand()) / RAND_MAX);
  data[7].Set(float(rand()) / RAND_MAX, float(rand()) / RAND_MAX, float(rand()) / RAND_MAX);
  glEnableVertexAttribArray(0); glCheck();
  glEnableVertexAttribArray(1); glCheck();
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), data); glCheck();
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const char*)data + 4 * sizeof(float)); glCheck();
  
#if _HARDWARE_PRINT_BENCHMARK_INFO
  const CFAbsoluteTime totalTimeBefore = CFAbsoluteTimeGetCurrent();
#endif
  
  // two calls to warm up the GPU, starting from third call it gives consistent results;
  _GpuBenchmarkDraw(ARRAYSIZE(data) / 2, iterationsCount);
  _GpuBenchmarkDraw(ARRAYSIZE(data) / 2, iterationsCount);
  
  // actual benchmark
  CFTimeInterval duration = _GpuBenchmarkDraw(ARRAYSIZE(data) / 2, iterationsCount);
  
#if _HARDWARE_PRINT_BENCHMARK_INFO
  const CFAbsoluteTime totalTimeAfter = CFAbsoluteTimeGetCurrent();
  RKLOG("GPU benchmark total (don't rely on this value): %f", (float)(totalTimeAfter - totalTimeBefore));
#endif
  
  glDeleteProgram(shaderProgram); glCheck();
  glDeleteShader(pixelShader); glCheck();
  glDeleteShader(vertexShader); glCheck();
  
  return duration;
}

static CFTimeInterval _GpuBenchmarkInCurrentContext()
{
  static const unsigned int textureWidth = 256;
  static const unsigned int textureHeight = 256;
  
  GLuint texture = 0;
  glGenTextures(1, &texture); glCheck(); RKASSERT(texture != 0, "OpenGL was not initialized");
  glBindTexture(GL_TEXTURE_2D, texture); glCheck();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0); glCheck();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); glCheck();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); glCheck();
  
  GLuint framebuffer;
  glGenFramebuffers(1, &framebuffer); glCheck();
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); glCheck();
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0); glCheck();
  RKASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete"); glCheck();
  glViewport(0, 0, textureWidth, textureHeight); glCheck();
  
  CFTimeInterval duration = _GpuBenchmarkInCurrentFramebuffer();
  
  glDeleteFramebuffers(1, &framebuffer); glCheck();
  glDeleteTextures(1, &texture); glCheck();
  
  return duration;
}

Hardware::GpuRating Hardware::_PerformGpuBenchmark()
{
  EAGLContext* oldContext = [[EAGLContext currentContext] retain];
  EAGLContext* context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
  [EAGLContext setCurrentContext:context];

  CFTimeInterval duration = _GpuBenchmarkInCurrentContext();
  
  [EAGLContext setCurrentContext:oldContext];
  [oldContext release];
  [context release];
  
#if _HARDWARE_PRINT_BENCHMARK_INFO
  RKLOG("GPU TIME: %f", (float)duration);
#endif
  
  // almost no variation, quite consistent, +-0.1ms

  // 3gs                - 40  ms
  // A4 (ipod4)         - 40  ms
  // ipod3              - 36  ms O_o?
  // A4 (4,ipad1)       - 36  ms
  // A5 (ipod5)         -  4.8ms
  // A5 (4s,ipad2,mini) -  4  ms
  // A5X(ipad3)         -  2.2ms
  // A6 (5,5c)          -  2.8ms
  // A6X(ipad4)         -  1.7ms
  // A7 (5s)            -  1.5ms
  
  const CGRect displayRect = [[UIScreen mainScreen] bounds];
  const CGFloat displayScale = [[UIScreen mainScreen] scale];
  const float pixelCount = (displayRect.size.width * displayScale) * (displayRect.size.height * displayScale);
  const float normalizedDuration = duration * (pixelCount / 1000.f);
  
  // normalized by resolution
  
  // device	         time	pixels	time * pixels
  // A4 (ipad1)      36	  786432	28311552
  // A4 (ipod4)      40	  588800	23552000
  // A4 (4)       	 36	  588800	21196800
  // A5X(ipad3)       2.2	3145728	6920602
  // 3gs             40	  153600	6144000
  // ipod3           36	  153600	5529600
  // A6X(ipad4)       1.7	3145728	5347738
  // A5 (ipod5)       4.8	727040	3489792
  // A5 (ipad2,mini)  4	  786432	3145728
  // A5 (4s) 	        4	  588800	2355200
  // A6 (5,5c)        2.8	727040	2035712
  // A7 (5s)          1.5	727040	1090560
  
  if (normalizedDuration > 10.f) // iphone4 and first ipad
  {
    return GPU_LOW;
  }
  else if (normalizedDuration > 2.8f) // hd ipads
  {
    return GPU_MEDIUM;
  }
  return GPU_HIGH; // iphon5 and higher
}

Hardware::MemRating Hardware::_GetMemoryRating() 
{
  unsigned long long memoryAmountBytes = [NSProcessInfo processInfo].physicalMemory;
  int memoryAmountMb = memoryAmountBytes / (1024 * 1024);
  if (memoryAmountMb < 300) { // 3gs, iPod3, iPod4, iPad1
    return MEM_LOW;
  }
  if (memoryAmountMb < 600) { // iphone4*, ipad2/mini
    return MEM_MEDIUM;
  }
  return MEM_HIGH; // iphone5, ipad3/4
}

uint32 Hardware::_GetCpuCoreCount()
{
  return [NSProcessInfo processInfo].processorCount;
}

RKString Hardware::_GetDeviceIdString()
{
  char machine[256];
  size_t size = ARRAYSIZE(machine);
  sysctlbyname("hw.machine", machine, &size, 0, 0);
  return machine;
}

RKString Hardware::_GetCacheFolder()
{
  NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
  NSString* path = [paths objectAtIndex:0];
  return [path UTF8String];
}
