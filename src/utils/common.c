#include "common.h"

char *execFolder;

char *logPath;
FILE *logFile;

char *fsJoinPath_(const char *base, ...) {
	size_t      len = strlen(base);
	const char *next;

	/* Get the length of the joined path */
	va_list args;
	va_start(args, base);
	while ((next = va_arg(args, const char*)) != NULL)
		len += strlen(next) + 1;
	va_end(args);

	/* Allocate memory and construct the path */
	char *buf = new(char, len + 1);
	strcpy(buf, base);

	va_start(args, base);
	while ((next = va_arg(args, const char*)) != NULL) {
		strcat(buf, PATH_SEP);
		strcat(buf, next);
	}
	va_end(args);

	return buf;
}

#if defined(PLATFORM_LINUX) || defined(PLATFORM_UNIX)
#	include <unistd.h>
#elif defined(PLATFORM_WINDOWS)
#	include <windows.h>
#endif

static char *fsFindExecFolder(void) {
	char *buf = new(char, PATH_MAX);

#if defined(PLATFORM_LINUX) || defined(PLATFORM_UNIX)
	ssize_t len = readlink("/proc/self/exe", buf, PATH_MAX);
	if (len != -1)
		buf[len] = '\0';
	else
		strcpy(buf, argv[0]);
#elif defined(PLATFORM_APPLE)
	uint32_t len = PATH_MAX;
	if (_NSGetExecutablePath(buf, &len) != 0)
		strcpy(buf, argv[0]);
#elif defined(PLATFORM_WINDOWS)
	GetModuleFileName(NULL, buf, PATH_MAX);
#else
	strcpy(buf, argv[0]);
#endif

#if defined(PLATFORM_LINUX)
	ssize_t i = len;
#elif defined(PLATFORM_APPLE)
	uint32_t i = len;
#elif defined(PLATFORM_WINDOWS)
	size_t i = len;
#endif

	while (i --> 0) {
		if (buf[i] == '/' || buf[i] == '\\') {
			buf[i] = '\0';
			break;
		}
	}

	return buf;
}

const char *fsGetExecFolder(void) {
	return execFolder == NULL? (execFolder = fsFindExecFolder()) : execFolder;
}

int Log_setup(const char *path, bool append) {
	logFile = fopen(path, append? "a" : "w");
	if (logFile == NULL)
		return -1;

	logPath = xstrdup(path);

	time_t timer    = time(NULL);
	struct tm* info = localtime(&timer);
	char   date[16];
	strftime(date, 16, "%Y-%m-%d", info);
	fprintf(logFile, "==================== %s ====================\n", date);

	return 0;
}

void Log_cleanup(void) {
	if (logFile == NULL)
		return;

	fclose(logFile);
	xfree(logPath);

	logPath = NULL;
	logFile = NULL;
}

const char *logTypeTitle[] = {
	[LogInfo]  = "INFO",
	[LogWarn]  = "WARN",
	[LogError] = "ERROR",
};

void Log_from(LogType type, const char *path, int line, const char *fmt, ...) {
	if (logFile == NULL)
		return;

	char    msg[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(msg, sizeof(msg), fmt, args);
	va_end(args);

	time_t timer    = time(NULL);
	struct tm* info = localtime(&timer);

	fprintf(logFile, "%02d:%02d:%02d [%s] %s:%i: %s\n",
	        info->tm_hour, info->tm_min, info->tm_sec, logTypeTitle[type], path, line, msg);
}

void fatalFrom(const char *path, int line, const char *fmt, ...) {
	char    msg[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(msg, sizeof(msg), fmt, args);
	va_end(args);
	Log_from(LogError, path, line, msg);

	const char *desc = "Fatal error, please check the logs.";

    size_t len = snprintf(NULL, 0, "%s\n%s", desc, logPath);
    char  *buf = new(char, len + 1);
    sprintf(buf, "%s\n%s", desc, logPath);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal error", buf, NULL);

	exit(EXIT_FAILURE);
}

void *xalloc(size_t bytes) {
	assert(bytes > 0);

	void *ptr = malloc(bytes);
	if (ptr == NULL)
		fatal("Memory allocation failed");

	return ptr;
}

void *xrealloc(void *ptr, size_t bytes) {
	assert(ptr != NULL);
	assert(bytes > 0);

	ptr = realloc(ptr, bytes);
	if (ptr == NULL)
		fatal("Memory reallocation failed");

	return ptr;
}

void xfree(void *ptr) {
	assert(ptr != NULL);
	free(ptr);
}

char *xstrdup(const char *str) {
	char *duped = new(char, strlen(str) + 1);
	strcpy(duped, str);
	return duped;
}
