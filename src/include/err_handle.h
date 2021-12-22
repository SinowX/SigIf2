// Error Handler
// 错误简单处理函数
#ifndef ERR_HANDLE_H
#define ERR_HANDLE_H
#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <cstring>

void pFatalError(const char* str)
{
  printf("Fatal: %s\n",str);
  abort();
}
void pFatalErrno(const int num)
{
  printf("Fatal: %s\n",strerror(num));
  abort();
}
void pWarningError(const char* str)
{
  printf("Warning: %s\n",str);
}
void pWarningErrno(const int num)
{
  printf("Warning: %s\n",strerror(num));
}



#endif
