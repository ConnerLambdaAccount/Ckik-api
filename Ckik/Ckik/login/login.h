#include "ids.h"
#include "crypto.h"
#include "xmpp/login.h"
#include "captcha.h"
#include "parse.h"
int Ckik_login(Ckik_conn* conn, chat_list** chats, char* jid) {

	// Read or generate device id & android id
	char device_id[33];
	char android_id[17];
	if(handle_ids(device_id, android_id) == -1) {
		Ckik_error("handle_ids", __FILE__, __LINE__);
		return -1;
	}

	// Generate passkey
	char* passkey = kik_passkey();
	
	// Create the xmpp and login
	char* xmpp = malloc(16384);
	make_login_xmpp(xmpp, device_id, android_id, passkey, 0);
	if(Ckik_send(conn->bio, xmpp, strlen(xmpp)) == -1) {
		Ckik_error("Ckik_send", __FILE__, __LINE__);
		free(passkey);
		free(xmpp);
		return -1;
	}

	// Read kik's response:
	// ack_uuid
	memset(xmpp, 0, 16384);
	if(BIO_read(conn->bio, xmpp, 16384) <= 0) {
		Ckik_error("BIO_read", __FILE__, __LINE__);
		free(passkey);
		free(xmpp);
		return -1;
	}
	
	// captcha or other error 
	memset(xmpp, 0, 16384);
	if(BIO_read(conn->bio, xmpp, 16384) <= 0) {
		Ckik_error("BIO_read", __FILE__, __LINE__);
		free(passkey);
		free(xmpp);
		return -1;
	}
	
	// Check for captcha
	if(solve_captcha(conn->bio, xmpp, device_id, android_id, passkey) != 0) {
		Ckik_error("solve_captcha", __FILE__, __LINE__);
		free(passkey);
		free(xmpp);
		return -1;
	}
	
	// Write node
	if(get_jid(xmpp, jid) != 0) {
		Ckik_error("get_jid", __FILE__, __LINE__);
	}

	// Parse chats into a linked list
	*chats = parse_chats(xmpp);
	
	// Free
	free(passkey); 
	free(xmpp);
	return 0;
} 
