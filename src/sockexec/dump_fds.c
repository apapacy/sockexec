#include "common.h"

void dump_fds(fd)
int fd;
{
    if(fds_tbl[fd].fd > -1) {
    fprintf(stderr,"------------------------------\n");
    fprintf(stderr,"fd: %d\n",fd);
    fprintf(stderr,"  fd:  %d\n",fds_tbl[fd].fd);
    fprintf(stderr,"  events:   %d\n",fds_tbl[fd].events);
    fprintf(stderr,"------------------------------\n");
    }
}
