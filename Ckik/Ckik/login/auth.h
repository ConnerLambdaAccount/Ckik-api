#include "hashmap.h"
int Ckik_authenticate(Ckik_conn* conn, char* jid) {
	/***************************
	* Initialize variables *****
	****************************/
	char jid_w_resource[strlen(jid) + 37];
	
	// Read device id from file
	char filename[strlen(Ckik_username)+6];
	sprintf(filename, ".%s-ids", Ckik_username);
	
	FILE* idfile = fopen(filename, "r");
	if(idfile == NULL) {
		Ckik_error("fopen", __FILE__, __LINE__);
		return -1;
	}
	char device_id[33];
	fgets(device_id, 32, idfile);
	device_id[32] = '\0';
	fclose(idfile);

	// complete jid with resource
	sprintf(jid_w_resource, "%s/CAN%s", jid, device_id); // CAN means android, CIP means iPhone

	char* timestamp = kik_timestamp();
	char* uuid = kik_uuid();
	char* passkey = kik_passkey();	

	/***************************
	* RSA sign *****************
	****************************/
	unsigned char* rsa = kik_rsa(jid, timestamp, uuid);
	if(rsa == NULL) {
		Ckik_error("kik_rsa", __FILE__, __LINE__);
		free(timestamp);
		free(uuid);
		free(passkey);
		return -1;
	}
	/**************************
	* HMAC ********************
	***************************/
	char* hmac = kik_hmac(timestamp, jid);
	if(hmac == NULL) {
		Ckik_error("kik_hmac", __FILE__, __LINE__);
		free(timestamp);
		free(uuid);
		free(passkey);
		return -1;
	}
	/**************************
	* Hashmap *****************
	***************************/
	unsigned int map_size = 11;
	table hashmap = create_table(map_size);
	add_value(0, "conn", "WIFI", hashmap);
	add_value(1, "cv", hmac, hashmap);
	add_value(2, "from", jid_w_resource, hashmap);
	add_value(3, "lang", "en_US", hashmap);
	add_value(4, "n", "1", hashmap);
	add_value(5, "p", passkey, hashmap);
	add_value(6, "sid", uuid, hashmap);
	add_value(7, "signed", rsa, hashmap);
	add_value(8, "to", "talk.kik.com", hashmap);
	add_value(9, "ts", timestamp, hashmap);
	add_value(10, "v", Ckik_version, hashmap);
	free(timestamp);
	free(uuid);
	free(passkey);
	free(rsa);
	free(hmac);

	print_table(map_size, hashmap);
	char* session_request = sort_kik_map(map_size, hashmap);
	printf("SESSIONBUFFER:\n%s\n\n", session_request);
	/***************************
	* Send & verify response ***
	****************************/
	if(Ckik_send(conn->bio, session_request, strlen(session_request)) == -1) {
		Ckik_error("Ckik_send", __FILE__, __LINE__);
		free(session_request);
		return -1;
	}
	free(session_request);

	char auth_response[64];
	memset(auth_response, 0, 64);
	if(BIO_read(conn->bio, auth_response, 64) <= 0) {
		Ckik_error("BIO_read", __FILE__, __LINE__);
		return -1;
	}
	printf("RESPONSE:\n%s\n\n", auth_response);
	if(strcmp(auth_response, "<k ok=\"1\">") != 0) {
		Ckik_error("strcmp", __FILE__, __LINE__);
		return -1;
	}
	return 0;
}
