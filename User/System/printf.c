#include "main.h"
/*
 * ref: https://github.com/cccriscv/mini-riscv-os/blob/master/05-Preemptive/lib.c
 */

static int _vsnprintf(char * out, size_t n, const char* s, va_list vl)
{
	int format = 0, i;
	int longarg = 0;
	size_t pos = 0;
	for (; *s; s++) {
		if (format) {
			switch(*s) {
			case 'l': {
				longarg = 1;
				break;
			}
			case 'p': {
				longarg = 1;
				if (out && pos < n) {
					out[pos] = '0';
				}
				pos++;
				if (out && pos < n) {
					out[pos] = 'x';
				}
				pos++;
			}
			case 'x': {
				long num = longarg ? va_arg(vl, long) : va_arg(vl, int);
				int hexdigits = 2*(longarg ? sizeof(long) : sizeof(int))-1;
				for(i = hexdigits; i >= 0; i--) {
					int d = (num >> (4*i)) & 0xF;
					if (out && pos < n) {
						out[pos] = (d < 10 ? '0'+d : 'a'+d-10);
					}
					pos++;
				}
				longarg = 0;
				format = 0;
				break;
			}
			case 'd': {
				long num = longarg ? va_arg(vl, long) : va_arg(vl, int);
				long digits = 1, nn;
				
				if (num < 0) {
					num = -num;
					if (out && pos < n) {
						out[pos] = '-';
					}
					pos++;
				}
				
				for (nn = num; nn /= 10; digits++);
				for (i = digits-1; i >= 0; i--) {
					if (out && pos + i < n) {
						out[pos + i] = '0' + (num % 10);
					}
					num /= 10;
				}
				pos += digits;
				longarg = 0;
				format = 0;
				break;
			}
			case 's': {
				const char* s2 = va_arg(vl, const char*);
				while (*s2) {
					if (out && pos < n) {
						out[pos] = *s2;
					}
					pos++;
					s2++;
				}
				longarg = 0;
				format = 0;
				break;
			}
			case 'c': {
				if (out && pos < n) {
					out[pos] = (char)va_arg(vl,int);
				}
				pos++;
				longarg = 0;
				format = 0;
				break;
			}
			case 'h': { // 处理 %hhu (unsigned char) 和 %hu (unsigned short)
                if (*(s + 1) == 'h') { // 检测 %hhu (unsigned char)
                    s++;
                    unsigned char num = (unsigned char)va_arg(vl, int); // `va_arg` 以 int 形式返回，需要转换为 unsigned char
                    unsigned char digits = 1, nn;
                    for (nn = num; nn /= 10; digits++);
                    for (i = digits - 1; i >= 0; i--) {
                        if (out && pos + i < n) {
                            out[pos + i] = '0' + (num % 10);
                        }
                        num /= 10;
                    }
                    pos += digits;
                } else if (*(s + 1) == 'd') { // 处理 %hd (short 有符号整数)
					s++; // 跳过 'd'
					short num = (short)va_arg(vl, int);
					int digits = 1, nn;

					if (num < 0) {
						num = -num;
						if (out && pos < n) {
							out[pos] = '-';
						}
						pos++;
					}

					for (nn = num; nn /= 10; digits++);
					for (i = digits - 1; i >= 0; i--) {
						if (out && pos + i < n) {
							out[pos + i] = '0' + (num % 10);
						}
						num /= 10;
					}
					pos += digits;
				} else { // 处理 %hu (unsigned short)
                    unsigned short num = (unsigned short)va_arg(vl, int);
                    unsigned short digits = 1, nn;
                    for (nn = num; nn /= 10; digits++);
                    for (i = digits - 1; i >= 0; i--) {
                        if (out && pos + i < n) {
                            out[pos + i] = '0' + (num % 10);
                        }
                        num /= 10;
                    }
                    pos += digits;
                }
                format = 0;
                break;
            }
			case 'f': {  // 添加 %lf 支持
				double num = va_arg(vl, double);
				if (num < 0) {
					num = -num;
					if (out && pos < n) {
						out[pos] = '-';
					}
					pos++;
				}
				
				long int_part = (long)num;  // 整数部分
				double frac_part = num - int_part;  // 小数部分
				
				// 处理整数部分
				long digits = 1, nn;
				for (nn = int_part; nn /= 10; digits++);
				for (i = digits - 1; i >= 0; i--) {
					if (out && pos + i < n) {
						out[pos + i] = '0' + (int_part % 10);
					}
					int_part /= 10;
				}
				pos += digits;

				// 处理小数部分
				if (out && pos < n) {
					out[pos] = '.';
				}
				pos++;

				for (i = 0; i < 6; i++) {  // 默认保留 6 位小数
					frac_part *= 10;
					int digit = (int)frac_part;
					if (out && pos < n) {
						out[pos] = '0' + digit;
					}
					pos++;
					frac_part -= digit;
				}

				format = 0;
				break;
			}
			default:
				break;
			}
		} else if (*s == '%') {
			format = 1;
		} else {
			if (out && pos < n) {
				out[pos] = *s;
			}
			pos++;
		}
    	}
	if (out && pos < n) {
		out[pos] = 0;
	} else if (out && n) {
		out[n-1] = 0;
	}
	return pos;
}

static char out_buf[1000]; // buffer for _vprintf()

static int _vprintf(const char* s, va_list vl)
{
	int res = _vsnprintf(NULL, -1, s, vl);
	if (res+1 >= sizeof(out_buf)) {
		MC_fputs("error: output string size overflow\n");
		while(1) {}
	}
	_vsnprintf(out_buf, res + 1, s, vl);
	MC_fputs(out_buf);
	return res;
}

int MC_printf(const char* s, ...)
{
	int res = 0;
	va_list vl;
	va_start(vl, s);
	res = _vprintf(s, vl);
	va_end(vl);
	return res;
}

void panic(char *s)
{
	MC_printf("panic: ");
	MC_printf(s);
	MC_printf("\n");
	while(1){};
}
