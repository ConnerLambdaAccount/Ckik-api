// Write 3 letter node to node param
int get_jid(char* xmpp, char* jid) {
	char* jid_start = strstr(xmpp, "<node>");
	if(jid_start == NULL) { 
		Ckik_error("strstr", __FILE__, __LINE__);
		return -1; 
	}
	jid_start += 6;	// <node>
	int node_len = strlen(Ckik_username) + 4;
	for(int i=0;i<node_len;i++) {
		jid[i] = jid_start[i];
	}
	jid[node_len] = '\0';
	strcat(jid, "@talk.kik.com");
	return 0;
}

// Parse chats from login response into a linked list
chat_list* parse_chats(char* xmpp) {

	// Find start and finish of chat_list_bins xml
	char* clb_start = strstr(xmpp, "<record pk=\"chat_list_bins\">");
	char* clb_end = strstr(clb_start, "</record>");
	if(clb_start == NULL || clb_end == NULL) {
		Ckik_error("strstr", __FILE__, __LINE__);
		return NULL;
	}
	clb_start += strlen("<record pk=\"chat_list_bins\">");
	
	// Determine length of chat_list_bins xml
	long long clb_strlen = (clb_end - clb_start);
	
	// Determine length of padding to add to base64 of chat_list_bins xml
	int pad_len = clb_strlen % 4;

	// Copy base64 into new char array with space for padding fix
	int total_clb_len = clb_strlen + pad_len;
	char clb[total_clb_len];
	memcpy(clb, clb_start, clb_strlen);

	// Fix Invalid Padding
	for(int i=clb_strlen;i<total_clb_len;i++) {
		clb[i] = '=';
	}
	clb[total_clb_len] = '\0';

	// Decode base64 from clb into decoded_clb
	unsigned char decoded_cl[strlen(clb) - (strlen(clb) / 4)+1];
	if(EVP_DecodeBlock(decoded_cl, clb, total_clb_len) == -1) {
		Ckik_error("EVP_DecodeBlock", __FILE__, __LINE__);
		return NULL;
	}


	// TODO: THIS NEEDS TO BE MADE A LOT NEATER
	// decoded_cl is 6bytes:chat:6bytes:chat:6bytes:chat...
	// Parse decoded_cl chat into chatname, add it to chats_list
	char chatname[64];
	int cn_index = 0;
	chat_list* head = NULL;
	for(int i=6;i<strlen(decoded_cl);i++) {
		// End of chat marked by 0x0a
		if(decoded_cl[i] == 0x0a) {
			chatname[cn_index] = '\0';
			// Add chat to linked list
			if(head == NULL) {
				head = add_chat(chatname, NULL);
			} else {
				add_chat(chatname, head);
			}
			cn_index = 0;
			i += 5;
		} else {
			chatname[cn_index] = decoded_cl[i];
			cn_index++;
			// If this is the last chat in the decoded_cl, it wont have 0x0a at the end, only NULL terminator
			if(i+1 == strlen(decoded_cl)) {
				chatname[cn_index] = '\0';
				// Add chat to linked list
				if(head == NULL) {
					head = add_chat(chatname, NULL);
				} else {
					add_chat(chatname, head);
				}
			}
		}
	}
	return head;
}
