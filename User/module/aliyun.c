/*���ļ������������ϴ������Ƶ�GM190*/
#include "aliyun.h"
#include "string.h"
#include <stdlib.h>

char aliyun_post[]="AT+ZMQPUB=1,/sys/a1Alavpm3AJ/GM190_1/thing/event/property/post,1,0,0,";
//����

char aliyun_set[]="AT+ZMQSUB=1,/sys/a1Alavpm3AJ/GM190_1/thing/service/property/set,1";
//����

char aliyun_str[]="7b226964223a22313233222c2276657273696f6e223a22312e30222c226d6574686f64223a227468696e672e6576656e742e70726f70657274792e706f7374222c22706172616d73223a";
//{"id":"123","version":"1.0","method":"thing.event.property.post","params":
char LightLux_str[]="224c696768744c7578223a";
//{"LightLux":
char AirTemp_str[]="2241697254656d70223a";
//{"AirTemp":
char AirHumi_str[]="2261697248756d69223a";
//{"airHumi":
char soilTemp_str[]="22736f696c54656d70223a";
//{"soilTemp":
char soilHumi_str[]="22736f696c48756d69223a";
//{"soilHumi":
char soilPH_str[]="22736f696c5048223a";
//{"soilPH":
char Pic_name_str[]="225069635f6e616d65223a";
//{"Pic_name":
char LightSwitch_str[]="224c69676874537769746368223a";
//{"LightSwitch":

char stater_str[]="7b";//"{"
char end_str[]="7d";//"}"

//

//�ϴ�������ʽΪaliyun_str+LightLux_str+����+end_str+end_str

int Hex[12]={30,31,32,33,34,35,36,37,38,39,40};
char minus_hex[]="2d";//����-��hex
char dot[]="2e";//С����
char comma[]="2c";//����
char hc_String[]={"2268635f6e756d626572223a"};
char xc_String[]={"2278635f6e756d626572223a"};
char hd_String[]={"2268645f6e756d626572223a"};
char mf_String[]={"226d665f6e756d626572223a"};
char qt_String[]={"2271745f6e756d626572223a"};
char ot_String[]={"226f745f6e756d626572223a"};

char Unit_Flag_String[]={"22756e69745f666c6167223a"};
char Aht_Flag_String[]={"226168745f666c6167223a"};
char Soli_Flag_String[]={"22736f6c695f666c6167223a"};
char Bh_Flag_String[]={"2262685f666c6167223a"};
char K210_Flag_String[]={""};

char temp_flag_String[]={"2254454d505f466c6167223a"};
char huml_flag_String[]={"2248554d495f466c6167223a"};
char lux_flag_String[]={"224c55585f466c6167223a"};
char s_t_flag_String[]={"22536f5f54454d505f466c6167223a"};
char s_h_flag_String[]={"22536f5f48554d495f466c6167223a"};
char s_ph_flag_String[]={"22536f5f50485f466c6167223a"};
/*
*��ǿ�������ϴ�
*������ն�
*�����ϴ�״̬
*/
uint8_t GY30_Lux_pload(int LightLux_Value)
{
	char str_cahe[128];
	char str_cahe2[512];
	char pload_str[2048];
	char LUX_String[64];
	memset(str_cahe, 0, sizeof(str_cahe));
	
	String_Joint_Int(LightLux_str,LightLux_Value,LUX_String);
	
	sprintf(str_cahe,"%s%s%s",stater_str,LUX_String,end_str);
	
	sprintf(str_cahe2,"%s%s%s",aliyun_str,str_cahe,end_str);
	//{"id":"123","version":"1.0","method":"thing.event.property.post","params":+{"LightLux_str":3}+}
	
	sprintf(pload_str,"%s%d,%s\r\n",aliyun_post,strlen(str_cahe2)/2,str_cahe2);
	//AT+ZMQPUB=1,/sys/a1Alavpm3AJ/GM190_1/thing/event/property/post,1,0,0,+�������ݵĳ��ȳ���2+hex����
	Usart2_SendString((uint8_t*)pload_str);
	
	if(strstr((char*)Rx2Buff,"OK"))
	{
		memset(Rx6Buff,0,sizeof(Rx6Buff));
		Rx6BuffCount=0;
		return 1;//�ɹ�
	}
	else
	{
		return 0;//ʧ��
	}
	
}

/*
*�����¶������ϴ�
*��������¶�
*�����ϴ�״̬
*/



/*
*ʶ�������Ϣ�ϴ�
*����:�����������ݣ��۷䣬���������ѣ��ȳ棬�Ĳ���������������
*�����ϴ�״̬
*/
uint8_t Insect_pload(int Mf_Value,int Hd_Value,int Qt_Value,int Hc_Value,int Xc_Value,int Ot_Value,int Pic_Value)
{
	char str_cahe[513];
	char str_cahe2[1024];
	char pload_str[2048];
	
	char Mf_String[64],Hd_String[64],Qt_String[64],Hc_String[64],Xc_String[64],Ot_String[64],Pic_String[64];
	String_Joint_Int(mf_String,Mf_Value,Mf_String);
	String_Joint_Int(hd_String,Hd_Value,Hd_String);
	String_Joint_Int(qt_String,Qt_Value,Qt_String);
	String_Joint_Int(hc_String,Hc_Value,Hc_String);
	String_Joint_Int(xc_String,Xc_Value,Xc_String);
	String_Joint_Int(ot_String,Ot_Value,Ot_String);
	String_Joint_Int(Pic_name_str,Pic_Value,Pic_String);

	
	sprintf(str_cahe,"%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",stater_str,Mf_String,comma,Hd_String,comma,Qt_String,comma,Hc_String,comma,Xc_String,comma,Ot_String,comma,Pic_String,end_str);
	
	sprintf(str_cahe2,"%s%s%s",aliyun_str,str_cahe,end_str);
	sprintf(pload_str,"%s%d,%s\r\n",aliyun_post,strlen(str_cahe2)/2,str_cahe2);
	Usart2_SendString((uint8_t*)pload_str);
	
	if(strstr((char*)Rx2Buff,"OK"))
	{
		memset(Rx6Buff,0,sizeof(Rx6Buff));
		Rx6BuffCount=0;
		return 1;//�ɹ�
	}
	else
	{
		return 0;//ʧ��
	}

}

/*
*������ʪ�ȴ�������Ϣ�ϴ�
*����:�����¶ȡ�����ʪ��
*�����ϴ�״̬
*/
uint8_t Aht_pload(double Air_Temp_Value,double Air_Humi_Value)
{
	char str_cahe[512];
	char str_cahe2[1024];
	char pload_str[2048];
	
	char Temp_String[64],Humi_String[64];
	String_Joint_Double(AirTemp_str,Air_Temp_Value,Temp_String);
	String_Joint_Double(AirHumi_str,Air_Humi_Value,Humi_String);
	
	sprintf(str_cahe,"%s%s%s%s%s",stater_str,Temp_String,comma,Humi_String,end_str);
	
	sprintf(str_cahe2,"%s%s%s",aliyun_str,str_cahe,end_str);
	sprintf(pload_str,"%s%d,%s\r\n",aliyun_post,strlen(str_cahe2)/2,str_cahe2);
	Usart2_SendString((uint8_t*)pload_str);
	
	if(strstr((char*)Rx2Buff,"OK"))
	{
		memset(Rx6Buff,0,sizeof(Rx6Buff));
		Rx6BuffCount=0;
		return 1;//�ɹ�
	}
	else
	{
		return 0;//ʧ��
	}

}

/*
*������������Ϣ�ϴ�
*����:�����¶ȡ�����ʪ�ȡ�����ph
*�����ϴ�״̬
*/
uint8_t Soil_pload(double Soil_Temp_Value,double Soil_Humi_Value,double Soil_Ph_Value)
{
	char str_cahe[512];
	char str_cahe2[1024];
	char pload_str[2048];
	
	char Temp_String[64],Humi_String[64],Ph_String[64];
	String_Joint_Double(soilTemp_str,Soil_Temp_Value,Temp_String);
	String_Joint_Double(soilHumi_str,Soil_Humi_Value,Humi_String);
	String_Joint_Double(soilPH_str,Soil_Ph_Value,Ph_String);
	
	sprintf(str_cahe,"%s%s%s%s%s%s%s",stater_str,Temp_String,comma,Humi_String,comma,Ph_String,end_str);
	
	sprintf(str_cahe2,"%s%s%s",aliyun_str,str_cahe,end_str);
	sprintf(pload_str,"%s%d,%s\r\n",aliyun_post,strlen(str_cahe2)/2,str_cahe2);
	Usart2_SendString((uint8_t*)pload_str);
	
	if(strstr((char*)Rx2Buff,"OK"))
	{
		memset(Rx6Buff,0,sizeof(Rx6Buff));
		Rx6BuffCount=0;
		return 1;//�ɹ�
	}
	else
	{
		return 0;//ʧ��
	}

}



/*
*�豸״̬�ϴ�
*����״̬:�豸״̬����״̬��aht״̬��bh1750������״̬����Ƶģ��״̬
*�����ϴ�״̬
*/
uint8_t State_pload(int Unit_Value,int Light_Value,int Aht_Value,int Bh_Value,int Soli_Value,int K210_Value)
{
	char str_cahe[512];
	char str_cahe2[1024];
	char pload_str[2048];

	memset(str_cahe, 0, sizeof(str_cahe));

	
	sprintf(str_cahe,"%s%s%d%s%s%d%s%s%d%s%s%d%s%s%d%s",stater_str,Unit_Flag_String,Hex[Unit_Value],comma,/**/LightSwitch_str,Hex[Light_Value],comma,/**/Aht_Flag_String,Hex[Aht_Value],comma,/**/Bh_Flag_String,Hex[Bh_Value],comma,/**/Soli_Flag_String,Hex[Soli_Value],end_str);
		
	sprintf(str_cahe2,"%s%s%s",aliyun_str,str_cahe,end_str);
	sprintf(pload_str,"%s%d,%s\r\n",aliyun_post,strlen(str_cahe2)/2,str_cahe2);
	Usart2_SendString((uint8_t*)pload_str);
	
	if(strstr((char*)Rx2Buff,"OK"))
	{
		memset(Rx6Buff,0,sizeof(Rx6Buff));
		Rx6BuffCount=0;
		return 1;//�ɹ�
	}
	else
	{
		return 0;//ʧ��
	}
	
}
/*
*�豸����״̬�ϴ�
*����״̬:�豸״̬����״̬��aht״̬��bh1750������״̬����Ƶģ��״̬
*�����ϴ�״̬
*/
uint8_t Number_State_pload(int TEMP_Value,int HUML_Value,int LUX_Value,int S_T_Value,int S_H_Value,int S_PH_Value)
{
	char str_cahe[512];
	char str_cahe2[1024];
	char pload_str[2048];

	memset(str_cahe, 0, sizeof(str_cahe));

	sprintf(str_cahe,"%s%s%d%s%s%d%s%s%d%s%s%d%s%s%d%s%s%d%s",stater_str,temp_flag_String,Hex[TEMP_Value],comma,/**/huml_flag_String,Hex[HUML_Value],comma,/**/lux_flag_String,Hex[LUX_Value],comma,/**/s_t_flag_String,Hex[S_T_Value],comma,/**/s_h_flag_String,Hex[S_H_Value],comma,/**/s_ph_flag_String,Hex[S_PH_Value],end_str);
		
	sprintf(str_cahe2,"%s%s%s",aliyun_str,str_cahe,end_str);
	sprintf(pload_str,"%s%d,%s\r\n",aliyun_post,strlen(str_cahe2)/2,str_cahe2);
	Usart2_SendString((uint8_t*)pload_str);
	
	if(strstr((char*)Rx2Buff,"OK"))
	{
		memset(Rx6Buff,0,sizeof(Rx6Buff));
		Rx6BuffCount=0;
		return 1;//�ɹ�
	}
	else
	{
		return 0;//ʧ��
	}
	
}

/*
*������������
*����������λ��*/
uint8_t Int_Digit(int IntNumber)
{
	uint8_t digit=0;
	if(IntNumber<0 && IntNumber>=-9)
	{
		digit=2;
	}
	else if(IntNumber<=-10 && IntNumber>=-99)
	{
		digit=3;
	}
	else if(IntNumber>=0 && IntNumber<=9)
	{
		digit=1;
	}
	else if(IntNumber>=10 && IntNumber<=99)
	{
		digit=2;
	}
	else if(IntNumber>=100 && IntNumber<=999)
	{
		digit=3;
	}
	else if(IntNumber>=1000 && IntNumber<=9999)
	{
		digit=4;
	}
	return digit;
}


/*
*ƴ�Ӻ���С��������ƴ����Ҫ���ϴ�����
*���룺��ƾ����ַ�����double����,���ص�����

*/
void String_Joint_Double(char* Str_Str,double Double_Value,char* str_cahe_return)
{
	char str_cahe[128];
	int Int_Value,Decimals_Vale;
	memset(str_cahe, 0, sizeof(str_cahe));
	
	Int_Value=(int)Double_Value;//����λ
	Decimals_Vale=Double_Value*10-Int_Value*10;//С��λ*10
	
	if(Int_Value>=0)
	{
		switch(Int_Digit(Int_Value))
		{
			case 1:
				sprintf(str_cahe,"%s%d%s%d",Str_Str,Hex[Int_Value],dot,Hex[Decimals_Vale]);
				//"LightLux_str":1.1
				// "LightLux_str":     +         1      +    .    +     1       
				break;
			case 2:
				sprintf(str_cahe,"%s%d%d%s%d",Str_Str,Hex[Int_Value/10],Hex[Int_Value%10],dot,Hex[Decimals_Vale]);

				//"LightLux_str":10.1
				break;
			case 3:
				sprintf(str_cahe,"%s%d%d%d%s%d",Str_Str,Hex[Int_Value/100],Hex[Int_Value/10%10],Hex[Int_Value%10],dot,Hex[Decimals_Vale]);
				//{"LightLux_str":100.1
				break;
			case 4:
				sprintf(str_cahe,"%s%d%d%d%d%s%d",Str_Str,Hex[Int_Value/1000],Hex[Int_Value/100%10],Hex[Int_Value/10%10],Hex[Int_Value%10],dot,Hex[Decimals_Vale]);
				//"LightLux_str":1000
				break;
		}
	}
	else
	{
		Int_Value=abs(Int_Value);
		switch(Int_Digit(Int_Value))
		{
			case 1:
				sprintf(str_cahe,"%s%s%d%s%d",Str_Str,minus_hex,Hex[Int_Value],dot,Hex[Decimals_Vale]);
			
				break;
			case 2:
				sprintf(str_cahe,"%s%s%d%d%s%d",Str_Str,minus_hex,Hex[Int_Value/10],Hex[Int_Value%10],dot,Hex[Decimals_Vale]);

				break;
			case 3:
				sprintf(str_cahe,"%s%s%d%d%d%s%d",Str_Str,minus_hex,Hex[Int_Value/100],Hex[Int_Value/10%10],Hex[Int_Value%10],dot,Hex[Decimals_Vale]);
				
				break;
			case 4:
				sprintf(str_cahe,"%s%s%d%d%d%d%s%d",Str_Str,minus_hex,Hex[Int_Value/1000],Hex[Int_Value/100%10],Hex[Int_Value/10%10],Hex[Int_Value%10],dot,Hex[Decimals_Vale]);
				
				break;
		}
	}
	strcpy(str_cahe_return,str_cahe);
}
/*
*ƴ�Ӻ���������ƴ����Ҫ���ϴ�����
*���룺��ƾ����ַ�����int����,���ص�����

*/
void String_Joint_Int(char* Str_Str,int Int_Value,char* str_cahe_return)
{
	char str_cahe[128];
	memset(str_cahe, 0, sizeof(str_cahe));
	
	if(Int_Value>=0)
	{
		switch(Int_Digit(Int_Value))
		{
			case 1:
				sprintf(str_cahe,"%s%d",Str_Str,Hex[Int_Value]);
				//"LightLux_str":1
				// "LightLux_str":     +         1        
				break;
			case 2:
				sprintf(str_cahe,"%s%d%d",Str_Str,Hex[Int_Value/10],Hex[Int_Value%10]);

				//"LightLux_str":10.1
				break;
			case 3:
				sprintf(str_cahe,"%s%d%d%d",Str_Str,Hex[Int_Value/100],Hex[Int_Value/10%10],Hex[Int_Value%10]);
				//{"LightLux_str":100.1
				break;
			case 4:
				sprintf(str_cahe,"%s%d%d%d%d",Str_Str,Hex[Int_Value/1000],Hex[Int_Value/100%10],Hex[Int_Value/10%10],Hex[Int_Value%10]);
				//"LightLux_str":1000
				break;
		}
	}
	else
	{
		Int_Value=abs(Int_Value);
		switch(Int_Digit(Int_Value))
		{
			case 1:
				sprintf(str_cahe,"%s%s%d",Str_Str,minus_hex,Hex[Int_Value]);
			
				break;
			case 2:
				sprintf(str_cahe,"%s%s%d%d",Str_Str,minus_hex,Hex[Int_Value/10],Hex[Int_Value%10]);

				break;
			case 3:
				sprintf(str_cahe,"%s%s%d%d%d",Str_Str,minus_hex,Hex[Int_Value/100],Hex[Int_Value/10%10],Hex[Int_Value%10]);
				
				break;
			case 4:
				sprintf(str_cahe,"%s%s%d%d%d%d",Str_Str,minus_hex,Hex[Int_Value/1000],Hex[Int_Value/100%10],Hex[Int_Value/10%10],Hex[Int_Value%10]);
				
				break;
		}
	}
	strcpy(str_cahe_return,str_cahe);
}