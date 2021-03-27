#include <stdio.h>
#include <sys/stat.h>
#include "s3c_uart.h"

#ifdef CONFIG_MANGO_24
#define ALLOC_BASE	0x32000000
#elif defined(CONFIG_MANGO_64)
#define ALLOC_BASE	0x52000000
#endif

void *_sbrk(ptrdiff_t __incr)
{
	static unsigned long alloc_base = ALLOC_BASE;
	unsigned long ret;
	if (__incr >= 0)
	{
		ret = alloc_base;
		alloc_base += __incr;
		alloc_base += 3;
		alloc_base &= ~(0x3);
	} 
	else
	{
		__incr = - (__incr);
		__incr += 3;
		__incr &= ~(0x3);
		alloc_base -= __incr;
		ret = alloc_base;
	}
	return (void *)ret;
}

int __srget_r(struct _reent *rnt, FILE *stream)
{
	char buf;
	mango_uart_read(&buf, 1, 0);
	return (int)buf;
}

_READ_WRITE_RETURN_TYPE _write(int __fd, const void *__buf, size_t __nbyte)
{
	return mango_uart_write(__buf, __nbyte);
}
 

int _close(int __fildes)
{
	return 0;
}

int _isatty(int __fildes)
{
	return 1;
} 

_off_t _lseek(int __fildes, _off_t __offset, int __whence)
{
	return 0;
}

_READ_WRITE_RETURN_TYPE _read(int __fd, void *__buf, size_t __nbyte)
{
	return mango_uart_read(__buf, __nbyte, 0);
}

int _fstat(int __fd, struct stat *__sbuf)
{
	memset(__sbuf, 0, sizeof(struct stat));
	__sbuf->st_dev = __fd;
	__sbuf->st_ino = __fd;
	if (__fd == 0){
		__sbuf->st_mode = S_IFCHR | S_IREAD;
	} else if (__fd == 1) {
		__sbuf->st_mode = S_IFCHR | S_IWRITE;
	}
	return 0;
}

 
