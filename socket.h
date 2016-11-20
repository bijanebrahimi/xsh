#ifndef _COMPLETION_H_
#define _COMPLETION_H_

#define SOCKET_PATH "/tmp/"
#define SOCKET_FMT (SOCKET_PATH "%d.sock")


struct msg_hdr {
  u_short msg_len;
  u_char  msg_version;
  u_char  msg_type;
  u_short msg_index;
  pid_t   msg_pid;
};

extern void    sck_cleanup(void);
extern int     sck_init(callback_buffer_t callback);

#endif
