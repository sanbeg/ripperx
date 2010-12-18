#ifndef CDDBP_H
#define CDDBP_H

/* the following functions are active, meaning that they themselves will
	communicate directly to the server through the FILE* which is given */
#if 0
#define DEBUG
#endif

#include "common.h"

int cddbp_signon(FILE *);
int cddbp_handshake(FILE *, const char *clientname, const char *version);
int cddbp_query(FILE *, const char *disk_id, int tracknum,
                long int offsets[ tracknum ], int duration, int *matches,
                char ***category_buffer, char ***title_buffer,
                char ***id_buffer);
int cddbp_read(FILE *, const char *category, const char *disk_id,
               char **result_buffer);

int http_query(const char *server, int port, const char *URL,
               const char *disk_cd, int tracknum, long int offsets[ tracknum ],
               int duration, int *matches,
               char ***category_buffer, char ***title_buffer,
               char ***id_buffer, const char *client, const char *version);

int http_query_proxy(const char *server, int port, const char *proxy_server, int proxy_port, const char *URL,
                     const char *cd_id, int tracknum, long int offset[ tracknum ],
                     int duration, int *matches,
                     char ***category_buffer, char ***title_buffer,
                     char ***id_buffer, const char *client, const char *version);

int http_read(const char *server, int port, const char *URL,
              const char *category, const char *disk_id,
              char **result_buffer, const char *client, const char *version);
int http_read_proxy(const char *server, int port, const char *proxy_server, int proxy_port, const char *URL,
                    const char *category, const char *disk_id,
                    char **result_buffer, const char *client, const char *version);

void cddbp_signoff(FILE *);

#endif

