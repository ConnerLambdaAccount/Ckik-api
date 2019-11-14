void make_login_xmpp(char* xmpp, char* device_id, char* android_id, char* passkey, char* captcha_solution) {
	char* uuid = kik_uuid();
	if(captcha_solution == NULL) {
		sprintf(xmpp,
    		"<iq type=\"set\" id=\"%s\">"
        	"<query xmlns=\"jabber:iq:register\">"
        	"<username>%s</username>"
        	"<passkey-u>%s</passkey-u>"
        	"<device-id>%s</device-id>"
        	"<install-referrer>utm_source=google-play&amp;utm_medium=organic</install-referrer>"
        	"<operator>310260</operator>"
        	"<install-date>150000000000</install-date>"
        	"<device-type>android</device-type>"
        	"<brand>generic</brand>"
        	"<logins-since-install>1</logins-since-install>"
        	"<version>%s</version>"
        	"<lang>en_US</lang>"
        	"<android-sdk>26</android-sdk>"
        	"<registrations-since-install>0</registrations-since-install>"
        	"<prefix>CAN</prefix>"
        	"<android-id>%s</android-id>"
        	"<model>moto g(6) - 8.0.0 - API 26 - 1080x1920</model>"
        	"</query>"
        	"</iq>", uuid, Ckik_username, passkey, device_id, Ckik_version, android_id);
	} else {
        sprintf(xmpp,
            "<iq type=\"set\" id=\"%s\">"
            "<query xmlns=\"jabber:iq:register\">"
            "<username>%s</username>"
            "<passkey-u>%s</passkey-u>"
            "<device-id>%s</device-id>"
            "<install-referrer>utm_source=google-play&amp;utm_medium=organic</install-referrer>"
            "<operator>310260</operator>"
            "<install-date>150000000000</install-date>"
            "<device-type>android</device-type>"
            "<brand>generic</brand>"
            "<logins-since-install>1</logins-since-install>"
            "<version>%s</version>"
            "<lang>en_US</lang>"
            "<android-sdk>26</android-sdk>"
            "<registrations-since-install>0</registrations-since-install>"
            "<prefix>CAN</prefix>"
            "<android-id>%s</android-id>"
            "<model>moto g(6) - 8.0.0 - API 26 - 1080x1920</model>"
            "<challenge>"
			"<response>%s</response>"
			"</challenge>"
			"</query>"
            "</iq>", uuid, Ckik_username, passkey, device_id, Ckik_version, android_id, captcha_solution);
	}
	free(uuid);
}
