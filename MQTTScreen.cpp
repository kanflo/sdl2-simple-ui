#include <SDL.h>
#include <MQTTScreen.h>
#include <iostream>
#include <assert.h>

static int mqttScreenThread(void *ptr)
{
	MQTTScreen *screen = (MQTTScreen*) ptr;
	while(1) {
		int rc = screen->loop();
		if (rc) {
			switch(rc) {

				case MOSQ_ERR_SUCCESS:
					printf("MQTT:success\n");
					break;
				case MOSQ_ERR_INVAL:
					printf("MQTT:input parameters were invalid.\n");
					break;
				case MOSQ_ERR_NOMEM:
					printf("MQTT:out of memory condition occurred.\n");
					break;
				case MOSQ_ERR_NO_CONN:
					printf("MQTT:client isn’t connected to a broker.\n");
					break;
				case MOSQ_ERR_CONN_LOST:
					printf("MQTT:connection to the broker was lost.\n");
					break;
				case MOSQ_ERR_PROTOCOL:
					printf("MQTT:protocol error communicating with the broker.\n");
					break;
				case MOSQ_ERR_ERRNO:
					printf("errno : %d\n", errno);
					break;
				default:
					printf("MQTT:unknown error %d\n", rc);
			}
			printf("Notice: MQTT reconnect\n");
			screen->reconnect();
		}
	}
	return 0;
}

MQTTScreen::MQTTScreen(UIGUI *ui, string id, string host, uint16_t port, uint32_t command) : UIScreen(ui), mosquittopp(id.c_str())
{
	int keepalive = 60;
	mHost = host;
	mPort = port;
	mIsConnected = false;
	mCommand = command;
	mThread = SDL_CreateThread(mqttScreenThread, "mqttScreenThread", (void*) this);
	/*mosquittopp::*/connect(mHost.c_str(), mPort, keepalive);
}

MQTTScreen::~MQTTScreen()
{
}

bool MQTTScreen::handleEvent(SDL_Event *event)
{
//	printf("MQTTScreen::HandleEvent %d\n", event->type);
	if (event->type == getEventType()) {
		return false;
	} else {
		return UIScreen::handleEvent(event);
	}
	return false;
}

void MQTTScreen::on_connect(int rc)
{
	printf("MQTT Connected with code %d.\n", rc);
	if(rc == 0){
		mIsConnected = true;
		/* Only attempt to subscribe on a successful connect. */
	}
}

void MQTTScreen::on_message(const struct mosquitto_message *message)
{
//	printf("Topic:%s | %s [%d]\n", message->topic, (char*) message->payload, message->mid);
	SDL_Event event;
	SDL_zero(event);
	event.type = getEventType();
	event.user.code = mCommand;
	event.user.data1 = (void*) strdup((const char*) message->topic);
	event.user.data2 = (void*) strdup((const char*) message->payload);
	SDL_PushEvent(&event);
}

void MQTTScreen::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
	printf("MQTT subscription succeeded.\n");
}
