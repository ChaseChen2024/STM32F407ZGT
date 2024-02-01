#ifndef _ALIYUN_H_
#define _ALIYUN_H_


#include "user_porc.h"

void String_Joint_Int(char* Str_Str,int Int_Value,char* str_cahe_return);
void String_Joint_Double(char* Str_Str,double Double_Value,char* str_cahe_return);
uint8_t Int_Digit(int IntNumber);

uint8_t GY30_Lux_pload(int LightLux_Value);
uint8_t Insect_pload(int Mf_Value,int Hd_Value,int Qt_Value,int Hc_Value,int Xc_Value,int Ot_Value,int Pic_Value);//识别数量上传
uint8_t State_pload(int Unit_Value,int Light_Value,int Aht_Value,int Bh_Value,int Soli_Value,int K210_Value);
uint8_t Soil_pload(double Soil_Temp_Value,double Soil_Humi_Value,double Soil_Ph_Value);//土壤传感器采集数据上传
uint8_t Aht_pload(double Air_Temp_Value,double Air_Humi_Value);//空气温湿度传感器信息上传
uint8_t Number_State_pload(int TEMP_Value,int HUML_Value,int LUX_Value,int S_T_Value,int S_H_Value,int S_PH_Value);

#endif //_ALIYUN_H_
