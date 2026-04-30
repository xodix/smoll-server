/*
 * Compiling: gcc -nostdlib -static -fno-stack-protector o.c -o o
 */

#include <stdint.h>

// Direct syscall wrapper for x86_64
#if defined(__x86_64__)
// x86_64 Syscall Numbers
#define SYS_WRITE 1
#define SYS_CLOSE 3
#define SYS_SOCKET 41
#define SYS_ACCEPT 43
#define SYS_BIND 49
#define SYS_LISTEN 50
#define SYS_TIME 201
#define SYS_EXIT 60

static inline int64_t syscall3(int64_t nr, int64_t a1, int64_t a2, int64_t a3)
{
	int64_t ret;
	__asm__ volatile(
		"syscall"
		: "=a"(ret)
		: "0"(nr), "D"(a1), "S"(a2), "d"(a3)
		: "rcx", "r11", "memory");
	return ret;
}

#elif defined(__aarch64__)
// ARM64 Syscall Numbers (Generic Table)
#define SYS_WRITE 64
#define SYS_CLOSE 57
#define SYS_SOCKET 198
#define SYS_ACCEPT 202
#define SYS_BIND 200
#define SYS_LISTEN 201
#define SYS_TIME 160
#define SYS_EXIT 93

static inline int64_t syscall3(int64_t nr, int64_t a1, int64_t a2, int64_t a3)
{
	register int64_t x8 __asm__("x8") = nr;
	register int64_t x0 __asm__("x0") = a1;
	register int64_t x1 __asm__("x1") = a2;
	register int64_t x2 __asm__("x2") = a3;
	__asm__ volatile(
		"svc #0"
		: "+r"(x0)
		: "r"(x8), "r"(x1), "r"(x2)
		: "memory");
	return x0;
}

#else
#error "Unsupported Architecture"
#endif

static inline int64_t syscall2(int64_t nr, int64_t a1, int64_t a2)
{
	return syscall3(nr, a1, a2, 0);
}

static inline int64_t syscall1(int64_t nr, int64_t a1)
{
	return syscall3(nr, a1, 0, 0);
}

// Syscall Numbers for Linux x86_64
#define SYS_WRITE 1
#define SYS_CLOSE 3
#define SYS_SOCKET 41
#define SYS_ACCEPT 43
#define SYS_BIND 49
#define SYS_LISTEN 50
#define SYS_TIME 201
#define SYS_EXIT 60

// Entry point since we aren't using libc's crt0
void _start()
{
	// --- printDate Logic ---
	int64_t ts = syscall1(SYS_TIME, 0);
	uint64_t days = (uint64_t)ts / 86400;

	uint64_t year = 1970;
	while (1)
	{
		uint64_t leap = ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) ? 366 : 365;
		if (days < leap)
			break;
		days -= leap;
		year += 1;
	}

	uint64_t month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	uint64_t month = 0;
	while (month < 12)
	{
		uint64_t m_len = month_days[month];
		if (month == 1 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)))
			m_len = 29;
		if (days < m_len)
			break;
		days -= m_len;
		month++;
	}

	uint64_t day = days + 1;
	month += 1;

	char buf[11] = {};
	buf[0] = (char)(48 + day / 10);
	buf[1] = (char)(48 + day % 10);
	buf[2] = '.';
	buf[3] = (char)(48 + month / 10);
	buf[4] = (char)(48 + month % 10);
	buf[5] = '.';
	buf[6] = (char)(48 + year / 1000);
	buf[7] = (char)(48 + (year / 100) % 10);
	buf[8] = (char)(48 + (year / 10) % 10);
	buf[9] = (char)(48 + year % 10);
	buf[10] = '\n';

	syscall3(SYS_WRITE, 1, (int64_t)buf, 11);

	// --- Networking Logic ---
	const char *name = "Autor: Bartłomiej Deska\n";
	syscall3(SYS_WRITE, 1, (int64_t)name, 26);

	const char *portMsg = "Aplikacja nasłuchuje na porcie 3000\n";
	syscall3(SYS_WRITE, 1, (int64_t)portMsg, 37);

	int64_t sock = syscall3(SYS_SOCKET, 2, 1, 0); // AF_INET, SOCK_STREAM

	// sockaddr_in manual layout: sin_family (2), sin_port (3000 = 0x0BB8), sin_addr (0)
	uint8_t addr[16] = {0};
	addr[0] = 2;	// AF_INET
	addr[2] = 0x0B; // Port 3000 MSB
	addr[3] = 0xB8; // Port 3000 LSB

	syscall3(SYS_BIND, sock, (int64_t)addr, 16);
	syscall2(SYS_LISTEN, sock, 10);

	const char msg[] =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html;charset=utf8\r\n"
		"Content-Length: 591\r\n"
		"Connection: close\r\n\r\n"
		"<h1>Autor: Bartłomiej Deska</h1><button onclick=f(51.23,22.55)>Lublin</button>"
		"<button onclick=f(52.22,20.98)>Warszawa</button><button onclick=f(50.06,19.93)>Kraków</button>"
		"<pre id=o></pre><script>function z(e){switch(e){case 0:return\"sunny\";case 1:return\"light overcast\";"
		"case 2:return\"partial overcast\";case 3:return\"overcast\";default:return\"bad\"}}function f(e,t){"
		"fetch(`https://api.open-meteo.com/v1/forecast?latitude=${e}&longitude=${t}&current=temperature_2m,weather_code`)"
		".then(e=>e.json()).then(e=>{o.innerHTML=`${e.current.temperature_2m}°C ${z(e.current.weather_code)}`})}</script>";

	while (1)
	{
		int64_t client_fd = syscall3(SYS_ACCEPT, sock, 0, 0);
		if (client_fd >= 0)
		{
			syscall3(SYS_WRITE, client_fd, (int64_t)msg, sizeof(msg) - 1); // Length of the string
			syscall1(SYS_CLOSE, client_fd);
		}
	}
}