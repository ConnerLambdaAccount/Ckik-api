int solve_captcha(BIO* bio, char* xmpp, char* device_id, char* android_id, char* passkey) {
	char* captcha = strstr(xmpp, "<captcha-url>");
	
	// Solve captcha loop
	while(captcha != NULL) {
		// Found captcha, format for printing
		captcha += 13;
		char* captcha_end = strstr(xmpp, "</captcha-url>");
		if(captcha_end == NULL) {
			break;
		}
		*captcha_end = '\0';
		
		// Print
		printf("Captcha URL (\"How to solve\" on github):\n%s&callback_url=https://kik.com/captcha-url\n", captcha);
		
		// Get solution from user
		printf("\nEnter solution:\n");
		char solution[4096];
		if(fgets(solution, 4095, stdin) == NULL) {
			// No input / fgets() failed
			Ckik_error("fgets()", __FILE__, __LINE__);
		} else {
			// Got solution, time to craft xmpp:
			make_login_xmpp(xmpp, device_id, android_id, passkey, solution);
			
			// Send xmpp
			if(Ckik_send(bio, xmpp, strlen(xmpp)) == -1) {
				Ckik_error("Ckik_send", __FILE__, __LINE__);
				return -1;
			}

			// Read response (ack uuid)
			memset(xmpp, 0, 16384);
			if(BIO_read(bio, xmpp, 16384) <= 0) {
				Ckik_error("BIO_read", __FILE__, __LINE__);
				return -2;
			}

			// Read response
			memset(xmpp, 0, 16384);
			if(BIO_read(bio, xmpp, 16384) <= 0) {
				Ckik_error("BIO_read", __FILE__, __LINE__);
				return -3;
			}
			captcha = strstr(xmpp, "<captcha-url>");
		
		}

	}
		
	// search substrings if no captcha
	if(strstr(xmpp, "not-registered") != NULL || strstr(xmpp, "password-mismatch") != NULL) {
		fprintf(stderr, "User not registered or password incorrect\n");
		return -4;
	} else if(strstr(xmpp, "type=\"error\"") != NULL) {
		fprintf(stderr, "Kik gave unexpected response:\n%s\n", xmpp);
		return -5;
	}
	return 0;
}
