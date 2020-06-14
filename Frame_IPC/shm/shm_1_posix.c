
#include <sys/file.h>  
#include <sys/mman.h>  
#include <fcntl.h>  
#include <sys/stat.h>  
#include <sys/wait.h>  
#include "util.h"  
  
void error_and_die(const char *msg) {  
    perror(msg);  
    exit(EXIT_FAILURE);  
}  
  
int main()  
{  
    int ret;  
    const size_t region_size = sysconf(_SC_PAGE_SIZE);  
    const char *string = "This is a test for share memory!/n";  
    int fd = shm_open(SHARE_MEMORY, O_CREAT | O_TRUNC | O_RDWR, 0666);  
    if (-1 == fd)  
    {  
        error_and_die("shm_open");  
    }  
  
    ret = ftruncate(fd, region_size);  
    if (ret)  
    {  
        error_and_die("ftruncate");  
    }  
  
    void *ptr = mmap(0, region_size, PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0);  
    if (ptr == MAP_FAILED)  
    {  
        error_and_die("mmap");  
    }  
  
    close(fd);  
    //*(int *)ptr = 0x55aa;  
    memcpy(ptr, string, (strlen(string) + 1));  
  
    sleep(10);  
  
    ret = munmap(ptr, region_size);  
    if (ret)  
        error_and_die("munmap");  
  
    ret = shm_unlink(SHARE_MEMORY);  
    if (ret)  
        error_and_die("shm_unlink");  
    return 0;  
  
  
}



