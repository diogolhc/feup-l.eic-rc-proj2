#define FTP_DATA_PORT 20
#define FTP_COMMAND_PORT 21

int ftp_setup(char *host);

int ftp_login(int socket_fd, char *user, char *pass);

int ftp_download_file(int socket_fd, char *host, char *path);

int ftp_close(int socket_fd);
