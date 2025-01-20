/* 
 */
#ifndef LIBUSBCDC_H
#define LIBUSBCDC_H

#include <stdarg.h>//?
#include <stdlib.h>//?
#include <string.h>//?
#include <stdbool.h>//?

void usb_start(void);//ok
bool usb_ready(void);//ok

int usb_getc(void);
//int usb_getline(char *buf,unsigned maxbuf);
//return getline(buf,bufsiz,usb_getc,usb_putc); getline.h+c
void usb_putc(char ch);
void usb_puts(const char *buf);//while(*buf) usb_putc(*buf++)
void usb_write(const char *buf,unsigned bytes);

//int usb_vprintf(const char *format,va_list ap);
//int usb_printf(const char *format,...);


#endif /* LIBUSBCDC_H */
