#ifndef __GET_INFO_API_H__
#define __GET_INFO_API_H__

#include <unistd.h>

int usr_get_name(char *buffer, size_t size);

int uts_get_sysname(char **ret_buf);
int uts_get_nodename(char **ret_buf);
int uts_get_release(char **ret_buf);
int uts_get_version(char **ret_buf);
int uts_get_machine(char **ret_buf);

#endif /* __GET_INFO_API_H__ */

