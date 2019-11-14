#include<openssl/bio.h>
int Ckik_send(BIO* kik_connection, char* buffer, unsigned int buflen) {
    unsigned int bytes_total = 0;
    int bytes_sent = BIO_write(kik_connection, buffer, buflen);
    while(bytes_sent > 0) {
        bytes_total += bytes_sent;
        if(bytes_total >= buflen) {
            return 0;
        }
        bytes_sent = BIO_write(kik_connection, buffer+bytes_total, buflen-bytes_total);
    }
    return -1;
}

