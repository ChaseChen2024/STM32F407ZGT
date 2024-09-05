/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "user.h"
#include "shell_port.h"
//#define MQTT_TASK 1
#include "MQTTClient.h"

#define EXAMPLE_PRODUCT_KEY            "a1tICtvohbz"
#define EXAMPLE_DEVICE_NAME            "f407_mqtt_chase"
#define EXAMPLE_DEVICE_SECRET       "deb7b39b25f6ca12097889bde615b532"

const char *subTopic = "/"EXAMPLE_PRODUCT_KEY"/"EXAMPLE_DEVICE_NAME"/user/get";
//	const char *pubTopic = "/"EXAMPLE_PRODUCT_KEY"/"EXAMPLE_DEVICE_NAME"/user/update";
const char *attribute_report_pubTopic = "/sys/"EXAMPLE_PRODUCT_KEY"/"EXAMPLE_DEVICE_NAME"/thing/event/property/post";
const char *attribute_report_subTopic = "/sys/"EXAMPLE_PRODUCT_KEY"/"EXAMPLE_DEVICE_NAME"/thing/event/property/post_reply";

const char *attribute_set_subTopic = "/sys/"EXAMPLE_PRODUCT_KEY"/"EXAMPLE_DEVICE_NAME"/thing/service/property/set";

MQTTClient aiot_client;
unsigned int msgid = 0;

extern int aiotMqttSign(const char *productKey, const char *deviceName, const char *deviceSecret, 
                     	char clientId[150], char username[65], char password[65]);

void messageArrived(MessageData* data)
{
//	MQTTMessage* message = data->message;
	printf("Message arrived on topic %.*s: %.*s\n", data->topicName->lenstring.len, data->topicName->lenstring.data,
		data->message->payloadlen, data->message->payload);
}

static void prvMQTTEchoTask(void *pvParameters)
{
	
	/* connect to m2m.eclipse.org, subscribe to a topic, send and receive messages regularly every 1 sec */
	
	Network network;
	unsigned char sendbuf[1000], readbuf[1000];
	int rc = 0, 
		count = 0;
	MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
	// printf("\r\nprvMQTTEchoTask--------------------------------------\r\n");
	char *host = EXAMPLE_PRODUCT_KEY".iot-as-mqtt.cn-shanghai.aliyuncs.com";
	short port = 1883;

	

	char clientId[150] = {0};
	char username[65] = {0};
	char password[65] = {0};

	if ((rc = aiotMqttSign(EXAMPLE_PRODUCT_KEY, EXAMPLE_DEVICE_NAME, EXAMPLE_DEVICE_SECRET, clientId, username, password) < 0)) {
		printf("aiotMqttSign -%0x4x\n", -rc);
		return -1;
	}
	// printf("clientid: %s\r\n", clientId);
	// printf("username: %s\r\n", username);
	// printf("password: %s\r\n", password);


	pvParameters = 0;
	NetworkInit(&network);
	// printf("\r\nNetworkInit\r\n");
	MQTTClientInit(&aiot_client, &network, 30000, sendbuf, sizeof(sendbuf), readbuf, sizeof(readbuf));
	// printf("\r\nMQTTClientInit\r\n");

//	network.defaultMessageHandler = messageArrived;
	if ((rc = NetworkConnect(&network, host, port)) != 0)
		printf("Return code from network connect is %d\n", rc);

	// printf("\r\nNetworkConnect,rc:%d\r\n",rc);
#if defined(MQTT_TASK)
	if ((rc = MQTTStartTask(&aiot_client)) != pdPASS)
		printf("Return code from start tasks is %d\n", rc);
#endif
	connectData.willFlag = 0;
	connectData.MQTTVersion = 3;
	connectData.clientID.cstring = clientId;
	connectData.username.cstring = username;
	connectData.password.cstring = password;
	connectData.keepAliveInterval = 60*10;
	connectData.cleansession = 0;
	if ((rc = MQTTConnect(&aiot_client, &connectData)) != 0)
		printf("Return code from MQTT connect is %d--------------\r\n", rc);
	else
		printf("MQTT Connected %d-------------------\r\n", rc);

	if ((rc = MQTTSubscribe(&aiot_client, attribute_report_subTopic, 1, messageArrived)) != 0)
		printf("Return code from MQTT subscribe is %d\r\n", rc);

	if ((rc = MQTTSubscribe(&aiot_client, attribute_set_subTopic, 1, messageArrived)) != 0)
		printf("Return code from MQTT subscribe is %d\r\n", rc);

	if ((rc = MQTTSubscribe(&aiot_client, subTopic, 1, messageArrived)) != 0)
		printf("Return code from MQTT subscribe is %d\r\n", rc);
	// unsigned int msgid = 0;
	char pub_payload[128]={0};
	
	MQTTMessage msg = {0};
	msg.qos = QOS1;		
	sprintf(pub_payload,"{\"id\":\"1\",\"version\":\"1.0\",\"params\":{\"LightSwitch\":%d}}",1);
	msg.id = ++msgid;
	msg.payload = pub_payload;
	msg.payloadlen = sizeof(pub_payload);
	rc = MQTTPublish(&aiot_client, attribute_report_pubTopic, &msg);
	printf("MQTTPublish %d, msgid %d\n", rc, msgid);
	while (1)
	{
//		printf("---------------------------\r\n");
		// vTaskDelay(1000);
		rc = MQTTYield(&aiot_client, 1000);
		if(MQTT_SUCCESS != rc) break;	
	}
	printf("MQTTvTaskDelete,rc:%d\r\n", rc);
	vTaskDelete(NULL);
	/* do not return */
}


void vStartMQTTTasks(uint16_t usTaskStackSize, UBaseType_t uxTaskPriority)
{
	BaseType_t x = 0L;
	printf("\r\nvStartMQTTTasks--------------------------------------\r\n");
	xTaskCreate(prvMQTTEchoTask,	/* The function that implements the task. */
			"MQTTEcho0",			/* Just a text name for the task to aid debugging. */
			usTaskStackSize,	/* The stack size is defined in FreeRTOSIPConfig.h. */
			(void *)x,		/* The task parameter, not used in this case. */
			uxTaskPriority,		/* The priority assigned to the task is defined in FreeRTOSConfig.h. */
			NULL);				/* The task handle is not used. */
}
/*-----------------------------------------------------------*/

int mqtt_send(int i)
{
    MQTTMessage msg = {0};
	int rc =0;
	
	char pub_payload[128]={0};
	msg.qos = QOS0;		
	sprintf(pub_payload,"{\"id\":\"1\",\"version\":\"1.0\",\"params\":{\"LightSwitch\":%d}}",i);
	Usart_SendString(USART3,"\r\n");
	Usart_SendString(USART3,pub_payload);
	Usart_SendString(USART3,"\r\n");
	msg.id = ++msgid;
	msg.payload = pub_payload;
	msg.payloadlen = sizeof(pub_payload);
	rc = MQTTPublish(&aiot_client, attribute_report_pubTopic, &msg);
	printf("----------------MQTTPublish %d, msgid %d----------------\r\n", rc, msgid);
    
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), mqttsend, mqtt_send, test1);

