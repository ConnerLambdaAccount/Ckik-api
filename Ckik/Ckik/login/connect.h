#include<string.h>
#include<openssl/bio.h>
#include<openssl/ssl.h>
#include<openssl/err.h>

// Necessary so we can free both these before the end of the program
typedef struct {
	BIO* bio;
	SSL_CTX* ssl_ctx;
} Ckik_conn;

Ckik_conn* Ckik_connect(void) {
    // Init openssl
    OpenSSL_add_all_algorithms();

	SSL_CTX* ctx = malloc(sizeof(SSL_CTX*));
    ctx = SSL_CTX_new(SSLv23_client_method());
    if(!SSL_CTX_load_verify_locations(ctx, NULL, "/etc/ssl/certs/")) {
        Ckik_error("SSL_CTX_load_verify_locations", __FILE__, __LINE__);
        SSL_CTX_free(ctx);
		return NULL;
    }
    BIO* kik_connection = malloc(sizeof(BIO*));
    kik_connection = BIO_new_ssl_connect(ctx);
    SSL* ssl_pointer;
	BIO_get_ssl(kik_connection, &ssl_pointer);
    SSL_set_mode(ssl_pointer, SSL_MODE_AUTO_RETRY);
    BIO_set_conn_hostname(kik_connection, "talk1110an.kik.com:5223");
    if(BIO_do_connect(kik_connection) <= 0) {
        Ckik_error("BIO_do_connect", __FILE__, __LINE__);
        SSL_CTX_free(ctx);
		BIO_free_all(kik_connection);
		return NULL;
    }
    if(SSL_get_verify_result(ssl_pointer) != X509_V_OK) {
        Ckik_error("SSL_get_verify_result", __FILE__, __LINE__);
        SSL_CTX_free(ctx);
		BIO_free_all(kik_connection);
		return NULL;
    }

	// Verify kik oks connection
	if(Ckik_send(kik_connection, "<k anon=\"\">", 11) == -1) {
		SSL_CTX_free(ctx);
		BIO_free_all(kik_connection);
		return NULL;
	}
	char response[11];
	response[10] = '\0';
	BIO_read(kik_connection, response, 11);
	if(strcmp(response, "<k ok=\"1\">") != 0) {
		Ckik_error("strcmp", __FILE__, __LINE__);
		SSL_CTX_free(ctx);
		BIO_free_all(kik_connection);
		return NULL;
	}

	// Return struct with pointers to ctx and bio objects
	Ckik_conn* ret = malloc(sizeof(Ckik_conn));
	ret->bio = kik_connection;
	ret->ssl_ctx = ctx;
	return ret;
}

void Ckik_disconnect(Ckik_conn* conn) {
	SSL_CTX_free(conn->ssl_ctx);
    BIO_free_all(conn->bio);
	free(conn);
}

int Ckik_reconnect(Ckik_conn* conn) {
	Ckik_send(conn->bio, "</k>", 4);
	BIO_reset(conn->bio);
    if(BIO_do_connect(conn->bio) <= 0) {
        Ckik_error("BIO_do_connect", __FILE__, __LINE__);
        return -1;
    }
	return 0;
}
