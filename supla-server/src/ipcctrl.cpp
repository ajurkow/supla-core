/*
 ============================================================================
 Name        : ipcctrl.cpp
 Author      : Przemyslaw Zygmunt p.zygmunt@acsoftware.pl [AC SOFTWARE]
 Version     : 1.0
 Copyright   : GPLv2
 ============================================================================
 */


#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <string.h>

#include "ipcctrl.h"
#include "sthread.h"
#include "log.h"
#include "user.h"

const char hello[] = "SUPLA SERVER CTRL\n";
const char cmd_is_iodev_connected[] = "IS-IODEV-CONNECTED:";
const char cmd_user_reconnect[] = "USER-RECONNECT:";

svr_ipcctrl::svr_ipcctrl(int sfd) {
	this->sfd = sfd;

	this->eh = eh_init();
	eh_add_fd(eh, sfd);
}

bool svr_ipcctrl::match_command(const char *cmd, int len) {

	if ( len > (int)strlen(cmd)
						 && memcmp(buffer, cmd, strlen(cmd)) == 0
						 && buffer[len-1] == '\n' ) {

		buffer[len-1] = 0;
		return true;
	}

	return false;
}

void svr_ipcctrl::send_result(const char *result) {

	snprintf(buffer, 255, "%s\n", result);
	send(sfd, buffer, strlen(buffer), 0);

}

void svr_ipcctrl::send_result(const char *result, int i) {

	snprintf(buffer, 255, "%s%i\n", result, i);
	send(sfd, buffer, strlen(buffer), 0);

}


void svr_ipcctrl::execute(void *sthread) {

	if ( sfd == -1 )
		return;

	int len;

	struct timeval last_action;
	gettimeofday(&last_action, NULL);

	send(sfd, hello, sizeof(hello), 0);

	while(sthread_isterminated(sthread) == 0) {
		eh_wait(eh, 1000000);

		if ( (len = recv(sfd, buffer, sizeof(buffer), 0)) != 0 ) {

			if ( len > 0 ) {

				buffer[255] = 0;

				if ( match_command(cmd_is_iodev_connected, len) ) {

					int UserID = 0;
					int DeviceID = 0;
					sscanf (&buffer[strlen(cmd_is_iodev_connected)], "%i,%i", &UserID, &DeviceID);

					if ( UserID
						 && DeviceID
						 && supla_user::is_device_online(UserID, DeviceID) ) {
						send_result("CONNECTED:",DeviceID);
					} else {
						send_result("DISCONNECTED:",DeviceID);
					}
				} else if ( match_command(cmd_user_reconnect, len) ) {

					int UserID = 0;
					sscanf (&buffer[strlen(cmd_user_reconnect)], "%i", &UserID);

					if ( UserID
						 && supla_user::reconnect(UserID) ) {
						send_result("OK:", UserID);
					} else {
						send_result("USER_UNKNOWN:", UserID);
					}
				}


			}

		} else {
			sthread_terminate(sthread);
		}


		struct timeval now;
		gettimeofday(&now, NULL);

		if ( now.tv_sec-last_action.tv_sec >= 5 ) {
			sthread_terminate(sthread);
			break;
		}
	};

}

svr_ipcctrl::~svr_ipcctrl() {
	if ( sfd != -1 )
		close(sfd);

	eh_free(eh);
}

