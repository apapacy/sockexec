#include "common.h"

int client_write(conn_id,except)
int conn_id;
int except;
{
    if(except) {
        goto client_write_close;
    }
    int bytes_sent = 0;
    int bytes_to_send;
    char *outgoing_buffer;

    if(conn_tbl[conn_id].client_out_buffer.len <= 0)
    {
        /* make sure everything is reading data */
        fds_tbl[conn_tbl[conn_id].client].events = IOPAUSE_READ;
        fds_tbl[conn_tbl[conn_id].child_stdout_fd].events = IOPAUSE_READ;
        fds_tbl[conn_tbl[conn_id].child_stderr_fd].events = IOPAUSE_READ;
        return 1;
    }

    outgoing_buffer = conn_tbl[conn_id].client_out_buffer.s + conn_tbl[conn_id].client_out_buffer_pos;
    bytes_to_send = conn_tbl[conn_id].client_out_buffer.len - conn_tbl[conn_id].client_out_buffer_pos;

    if(debug)
    {
        fprintf(stderr,"Connection %d: sending %d bytes to client\n",conn_id,bytes_to_send);
    }

    bytes_sent = fd_send(conn_tbl[conn_id].client,outgoing_buffer,bytes_to_send,0);

    if(debug)
    {
        fprintf(stderr,"Connection %d: sent %d bytes to client\n",conn_id,bytes_sent);
    }

    if(bytes_sent < 0)
    {
        goto client_write_close;
    }

    conn_tbl[conn_id].client_out_buffer_pos += bytes_sent;

    if(conn_tbl[conn_id].client_out_buffer_pos == conn_tbl[conn_id].client_out_buffer.len)
    {
        client_write_close:
        conn_tbl[conn_id].client_out_buffer_pos = 0;
        stralloc_free(&(conn_tbl[conn_id].client_out_buffer));
        /* done sending data */
        if(!close_connection(conn_id,0,0)) {
            /* connection didn't close (child pid still running) */
            fds_tbl[conn_tbl[conn_id].client].events = IOPAUSE_READ;
            fds_tbl[conn_tbl[conn_id].child_stdout_fd].events = IOPAUSE_READ;
            fds_tbl[conn_tbl[conn_id].child_stderr_fd].events = IOPAUSE_READ;
        }
        return 1;
    }
    return 1;
}

