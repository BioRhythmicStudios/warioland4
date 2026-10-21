#include "port/os.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

long PortOsReadFile(const char *path, void *buffer, size_t size, int *missing)
{
    int fd = open(path, O_RDONLY | O_CLOEXEC);
    size_t done = 0;
    *missing = 0;
    if (fd < 0) { *missing = errno == ENOENT; return -1; }
    while (done < size) {
        ssize_t n = read(fd, (char *)buffer + done, size - done);
        if (n < 0 && errno == EINTR) continue;
        if (n < 0) { close(fd); return -1; }
        if (!n) break;
        done += (size_t)n;
    }
    if (close(fd)) return -1;
    return (long)done;
}

int PortOsWriteFileAtomic(const char *path, const void *data, size_t size, char *error, size_t errorSize)
{
    static unsigned sequence;
    char temporary[4352];
    int fd = -1, reason = ENAMETOOLONG;
    size_t done = 0;
    for (int attempt = 0; attempt < 16; ++attempt) {
        int n = snprintf(temporary, sizeof(temporary), "%s.tmp.%ld.%u", path, (long)getpid(), ++sequence);
        if (n < 0 || (size_t)n >= sizeof(temporary)) goto failed;
        fd = open(temporary, O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC, 0600);
        if (fd >= 0) break;
        reason = errno;
        if (reason != EEXIST) goto failed;
    }
    if (fd < 0) goto failed;
    while (done < size) {
        ssize_t n = write(fd, (const char *)data + done, size - done);
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0) { reason = n < 0 ? errno : EIO; goto close_failed; }
        done += (size_t)n;
    }
    if (fsync(fd)) { reason = errno; goto close_failed; }
    if (close(fd)) { fd = -1; reason = errno; goto failed; }
    fd = -1;
    if (rename(temporary, path)) { reason = errno; goto failed; }
    if (errorSize) error[0] = 0;
    return 0;
close_failed:
    close(fd);
failed:
    // Retain only our exclusively created temporary file as failure evidence.
    if (errorSize) snprintf(error, errorSize, "Linux error %d: %s", reason, strerror(reason));
    return -1;
}

