const char* Ckik_username = "your username here";
const char* Ckik_password = "your password here";

#define CKIK_VERSION_11_1
#include "Ckik/kik.h"


int main() {
	// This will all be in a wrapper for a login function

	// Variables
	Ckik_conn* conn;
	chat_list* chats;

	// Connect
	conn = Ckik_connect();
	if(conn == NULL) {
		Ckik_disconnect(conn);
		return -1;
	}

	// Login
	char jid[strlen(Ckik_username) + 4 + strlen("@talk.kik.com") + 1];
	if(Ckik_login(conn, &chats, jid) != 0) {
		Ckik_disconnect(conn);
		free_list(chats);
		return -1;
	}
	
	// Reconnect before authentication
	if(Ckik_reconnect(conn) != 0) {
		free_list(chats);
		return -1;
	}
	// Authenticate
	if(Ckik_authenticate(conn, jid) != 0) {
		free_list(chats);
		Ckik_disconnect(conn);
		return -1;
	}

	free_list(chats);
	Ckik_disconnect(conn);
	return 0;
}
