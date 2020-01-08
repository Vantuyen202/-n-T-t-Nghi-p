#include <stdbool.h>

sbit  rs485_rxtx_pin  at RC2_bit;
sbit  rs485_rxtx_pin_direction at TRISC2_bit;


sbit LCD_RS at RC1_bit;
sbit LCD_EN at RC0_bit;
sbit LCD_D4 at RB5_bit;
sbit LCD_D5 at RB4_bit;
sbit LCD_D6 at RB3_bit;
sbit LCD_D7 at RB2_bit;

sbit LCD_RS_Direction at TRISC1_bit;
sbit LCD_EN_Direction at TRISC0_bit;
sbit LCD_D4_Direction at TRISB5_bit;
sbit LCD_D5_Direction at TRISB4_bit;
sbit LCD_D6_Direction at TRISB3_bit;
sbit LCD_D7_Direction at TRISB2_bit;

sbit Relay1 at LATA5_bit;
sbit Relay2 at LATA4_bit;


sbit Relay1_Direction at TRISA5_bit;
sbit Relay2_Direction at TRISA4_bit;


sbit Digital_IN1 at LATA3_bit;
sbit Digital_IN2 at LATA2_bit;

sbit Digital_IN1_Direction at TRISA3_bit;
sbit Digital_IN2_Direction at TRISA2_bit;

sbit AM2302_Bus_In at RA0_bit;
sbit AM2302_Bus_Out at LATA0_bit;
sbit AM2302_Bus_Direction at TRISA0_bit;
// END AM2302 module connections

void GSM_Begin();
void GSM_Calling(char*);
void GSM_HangCall();
void GSM_Response();
void GSM_Response_Display();
void GSM_Msg_Read(int);
bool GSM_Wait_for_Msg();
void GSM_Msg_Display();
void GSM_Msg_Delete(unsigned int);
void GSM_Send_Msg(char* , char*);
void GSM_Delete_All_Msg();
unsigned ComposeMessage(char*Message);
void Send_Status();
void checksms();
void status_lcd(void);
void Display_Init();
void sms1(unsigned);
void processValue(unsigned , unsigned );
char AM2302_Read(unsigned *, unsigned *);
void degreeChar(char , char );
unsigned settingMessage(char* Message1);
unsigned AlarmMessage(char* Message2);
unsigned Al_Message(char* Message3);
void Send_Status1();
void Send_Status2();
void Send_Status3();
void re_485();
void temp();
void Alarm();

int t2=0,t3=0,h2=0,h3=0;
int cm=0;
int value=0;
int value_t=0,value_h=0;
unsigned char dat[10];

char str_val[160]; // pointer to the first character of a token
int M1, M2; // values extracted from string
char M3;

char AM2302_Data[5] = {0, 0, 0, 0, 0};
unsigned humidity = 0, temperature = 0;
const char degree[] = {14,10,14,0,0,0,0,0};
int cnt1=0;
int cnt2=0;
int cnt3=0;
int Alarm1=0;
// float t=0.0,h=0.0;
 unsigned short cnt=0;
 short temp_ll,temp_hh,humi_ll,humi_hh;
 int temp_l = -2, temp_h = 101, humi_l=-2,humi_h=101 ;
 unsigned char str1[8]="0";
 unsigned char str2[8]="0";
 unsigned char str3[8]="0";
 unsigned char str4[8]="0";


char txt1[]="00.0";
char txt2[]="00.0";
char txt3[]="00.0";
char txt4[]="00.0";
char lcd=0;
int sms_status=0;
int sms_status_a=0;
int sms_status_b=0;


char sms[160];
char buff[160];
char status_flag = 0;
volatile int buffer_pointer;
char Mobile_no[14];
char Mobile_no1[14]="0971550398";
char message_received[160];
int position = 0;

void Intuart(){

     RC1IE_bit = 1;
     RC2IE_bit = 1;

     TX2IE_bit = 0;
     INTCON.PEIE = 1;
     INTCON.GIE = 1;
     T1CON         = 0x21;
     TMR1IF_bit         = 0;
     TMR1H         = 0x3C;
     TMR1L         = 0xB0;
     TMR1IE_bit         = 1;
     cnt1 =   0;
     cnt2 =   0;
     cnt3 =   0;
     INTCON         = 0xC0;
}

void main(void)
{
    int is_msg_arrived;
    OSCCON=0x72;
    ANSELA=0;
    ANSELB=0;
    ANSELC=0;
    buffer_pointer = 0;
    memset(message_received, 0, 80);

    Relay1 = 0;
    Relay2 = 0;

    Relay1_Direction = 0;
    Relay2_Direction = 0;

    Digital_IN1_Direction = 1;
    Digital_IN2_Direction = 1;

    Digital_IN1=1;
    Digital_IN2=1;

    TRISA1_bit=0;
    RA1_bit=0;

    UART1_Init(9600);
    UARt2_Init(14400);
    Delay_ms(100);
    RS485Master_Init();
    dat[0] = 0;//0b00100000;
    dat[1] = 0;
    dat[2] = 0;
    dat[4] = 0;
    dat[5] = 0;
    dat[6] = 0;

    Intuart();
    dat[4] = 0;
    dat[5] = 0;
    Display_Init();
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Delay_ms(2000); // doi khoi dong module sim
    GSM_Begin();
    Lcd_Cmd(_LCD_CLEAR);
    while(1)
    {
    status_lcd();
  if(lcd==0){
    re_485();
    RS485Master_Send(dat,3,1);
  if(cnt1>=20)
   {
  if (AM2302_Read(&humidity, &temperature) == 0){                 // Display AM2302_Read sensor values via LCD
       processValue(humidity, temperature);                       // Display AM2302_Read sensor values via LCD
        }
    else
    {
    Lcd_Out(1,1,"Sensor1 NC    ");
    Lcd_Out(2,1,"              ");
    }
    cnt1=0;
   }
    ////////////cai dat alarm///////////////
     if (Button(&PORTB, 1, 100, 0)&&Alarm1==0) {
       Alarm1=1;
       }
     if (Button(&PORTB, 1, 100, 0)&&Alarm1==1) {
       Alarm1=0;
       }
     if(Alarm1==0){
      Lcd_Out(2,15,"AL:OFF");
      }
     if(Alarm1==1){
      Lcd_Out(2,15,"AL: ON");
      }
      /////////////////IN1_Alarm///////////////////////
      if(Digital_IN1==1)
      {
        RA1_bit=1;
        if(sms_status_a==0){
        Send_Status3();
       }
        sms_status_a=1;
        }
       if(Digital_IN1==0)
      {
        RA1_bit=0;
        sms_status_a=0;
        }
        ////////////////////IN2_alarm////////////////
        if(Digital_IN2==0)
      {
        if(sms_status_b==0){
        Relay1=1;
        Send_Status3();
        delay_ms(3000);
        Relay1=0;
       }
        sms_status_b=1;
        }
       if(Digital_IN2==1)
      {
        sms_status_b=0;
        }
        ///////////////////Temp_humi_alarm/////////////
      if(EEPROM_Read(0x11)>=EEPROM_Read(0x00) && Alarm1==1 && sms_status==0)    //TEMP >=TEMP_HH
      {
       Send_Status2();
       if(Relay1==1){
       delay_ms(3000);
       Relay1=0;}
       sms_status=1;
      }
      if(EEPROM_Read(0x11)<=EEPROM_Read(0x01) && Alarm1==1&& sms_status==0)       //TEMP <=TEMP_HH
      {
       Send_Status2();
        if(Relay1==1){
       delay_ms(3000);
       Relay1=0;}
       sms_status=1;
      }
      if(EEPROM_Read(0x10)>=EEPROM_Read(0x02) && Alarm1==1&& sms_status==0)      // HUMI>=HUMI_HH
      {
       Send_Status2();
        if(Relay1==1){
       delay_ms(3000);
       Relay1=0;}
       if(Relay2==0){
       Relay2=1;
       }
       sms_status=1;
      }
      if(EEPROM_Read(0x10)<=EEPROM_Read(0x03) && Alarm1==1&& sms_status==0)    //   HUMI<=HUMI_HH
      {
       Send_Status2();
        if(Relay2==1){
       delay_ms(3000);
       Relay1=0;}
       if(Relay2==1){
       Relay2=0;
       }
       sms_status=1;
      }
      if(Alarm1==0&& sms_status==1)
      {
       Relay1=0;
       sms_status=0;
      }
      if(cnt2>=6000) //300s
      {
      if(sms_status==1)
      {
       sms_status=0;
       }
       cnt2=0;
      }
      if(status_flag==1){
      is_msg_arrived = GSM_Wait_for_Msg();
             if(is_msg_arrived== true)
             {   Lcd_Out(1,16,"MSG:1");
                 delay_ms(1000);
                 Lcd_Cmd(_LCD_CLEAR);
                 Lcd_Out(1,1,"New message");
                 Delay_ms(500);
                 Lcd_Cmd(_LCD_CLEAR);
                 GSM_Msg_Read(position);
                 Delay_ms(2000);
                 checksms();
                 Lcd_Cmd(_LCD_CLEAR);
                 GSM_Msg_Delete(position);
                 Lcd_Out(1,1,"Clear msg");
                 GSM_Response();
                 Delay_ms(1000);
                 }
                 is_msg_arrived=0;
                 status_flag=0;
                 Lcd_Cmd(_LCD_CLEAR);
                 }
                Lcd_Out(1,16,"MSG:0");
                memset(Mobile_no, 0, 14);
                memset(message_received, 0, 80);     //sao chép ký tu
                while( dat[4] != 255 );
                {
                dat[4]=0;
                 }
        }
    }

}


void re_485() {
   cm=dat[2];
   value=(dat[0]*10)+dat[1];
   if(cm==1)
   {
   value_t=dat[0];
   EEPROM_Write(0x11,value_t);
   Lcd_out(4,1,"Temp2: ");
   txt1[0]=(value/100)%10+48;
   txt1[1]=(value/10)%10+48;
   txt1[3]=(value%10)+48;
   lcd_out(4,8,txt1);
   Lcd_Out_CP(" C");
   }
   delay_ms(100);
   if(cm==2)
   {
   value_h=dat[0];
   EEPROM_Write(0x10,value_h);
   Lcd_out(3,1,"HUMI2: ");
   txt2[0]=(value/100)%10+48;
   txt2[1]=(value/10)%10+48;
   txt2[3]=(value%10)+48;
   lcd_out(3,8,txt2);
   Lcd_Out_CP(" %");
   }
}

void Interrupt()
{
    RS485Master_Receive(dat);

    if(PIR1.RC1IF)
    {

        buff[buffer_pointer] = RCREG1;
        buffer_pointer++;

        if(RCSTA1.OERR)
        {
            RCSTA1.CREN = 0;
            NOP();
            RCSTA1.CREN = 1;
        }
        status_flag=1;
    }

     if (TMR1IF_bit){
    cnt1++;   //hien thi
    cnt2++;   // lap lai canh bao
  //  cnt3++;
    TMR1IF_bit = 0;
    TMR1H         = 0x3C;
    TMR1L         = 0xB0;

  }
}
////////////////////////////////////////////SIM800/////////////////////////////
void checksms(){
 strcpy(str_val,strtok(message_received, "#")); //take the first value
    M3 = str_val;
    strcpy(str_val,strtok(0, "#")); //take the secound value
    M1 = atoi(str_val); //convert string to INT
    strcpy(str_val,strtok(0, "#")); //take the third value
    M2 = atoi(str_val); //convert string to INT
    if(strstr( message_received,"SETTEMP")){
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1,1,"SETING TEMP");
    EEPROM_Write(0x01,M1);  //nhiet do thap
    EEPROM_Write(0x00,M2);   //nhiet do cao
    Send_Status1();
    Delay_ms(500);
}
    if(strstr( message_received,"SETHUMI")){
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1,1,"SETING HUMI");
    EEPROM_Write(0x03,M1); //do am thap
    EEPROM_Write(0x02,M2); //do am cao
    Send_Status1();
    Delay_ms(500);
}
    if(strstr( message_received,"SETTING?")){
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1,1,"SETING?");
    Send_Status1();
    Delay_ms(500);
}
    if(strstr( message_received,"CALL")){
    GSM_Calling(Mobile_no1);
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1,1,"Calling...");
    Delay_ms(15000);
    GSM_HangCall();
    Lcd_Out(1,1,"Hang Call");
    Delay_ms(500);
}
  else if(strstr(message_received,"INFOR?")){
//    Relay1=1;
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1,1,"Send infor");
    Send_Status();
    Delay_ms(500);
}
 else if(strstr(message_received,"ALARM ON")){
    Alarm1=1;
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1,1,"ALARM ON");
  //  Send_Status();
    Delay_ms(500);
}
 else if(strstr(message_received,"ALARM OFF")){
    Alarm1=0;
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1,1,"ALARM OFF");
   // Send_Status();
    Delay_ms(500);
}
    else if(strstr(message_received,"TB1ON")){
    Relay1=1;
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1,1,"TB1 ON");
    Send_Status();
    Delay_ms(500);
}
   else if(strstr(message_received,"TB1OFF")){
   Relay1=0;
   Lcd_Cmd(_LCD_CLEAR);
   Lcd_Out(1,1,"TB1 OFF");
   Send_Status();
   Delay_ms(500);
   }
   else if(strstr(message_received,"TB2ON")){
   Relay2=1;
   Lcd_Cmd(_LCD_CLEAR);
   Lcd_Out(1,1,"TB2 ON");
   Send_Status();
   Delay_ms(500);
  }
   else if(strstr(message_received,"TB2OFF")){
   Relay2=0;
   Lcd_Cmd(_LCD_CLEAR);
   Lcd_Out(1,1,"TB2 OFF");
   Send_Status();
   Delay_ms(500);
   }
}

unsigned ComposeMessage(char* Message){

  Message[0] = '\0';
  strcat(Message, "INFO:");
  strcat(Message, "\r\n");
  if (Alarm1==1)  // Digital_IN1
  { strcat(Message, " AL - ON"); }
  else
  { strcat(Message, " AL - OFF"); }
  strcat(Message, "\r\n");
  if (Relay1)
  { strcat(Message, " TB1 - ON");  }
  else
  { strcat(Message, " TB1 - OFF"); }
  strcat(Message, "\r\n");

  if (Relay2)
  { strcat(Message, " TB2 - ON");  }
  else
  { strcat(Message, " TB2 - OFF"); }
  strcat(Message, "\r\n");

  if (Button(&PORTA, 3, 100, 1))  // Digital_IN1
  { strcat(Message, " IN1 - OPEN"); }
  else
  { strcat(Message, " IN1 - CLOSE"); }
  strcat(Message, "\r\n");
  if (Button(&PORTA, 2, 100, 1))  // Digital_IN2
  { strcat(Message, " IN2 - CLOSE"); }
  else
  { strcat(Message, " IN2 - OPEN"); }
  strcat(Message, "\r\n");

    if (AM2302_Read(&humidity, &temperature) == 0)
  {
  processValue(humidity, temperature);

  strcat(Message, " TEMP 1: ");
  strcat(Message, txt3);    // Add Humidity data
  strcat(Message, " C");
  strcat(Message, "\r\n");

  strcat(Message, " HUMI 1: ");
  strcat(Message, txt4);    // Add Humidity data
  strcat(Message, " %");
  strcat(Message, "\r\n");
   }
   else
  {
  strcat(Message, "Sensor 1 Error");
   strcat(Message, "\r\n");
  }
  strcat(Message, " TEMP 1: ");
  strcat(Message, txt1);    // Add Humidity data
  strcat(Message, " C");
  strcat(Message, "\r\n");
  strcat(Message, " HUMI 2: ");
  strcat(Message, txt2);    // Add Humidity data
  strcat(Message, " %");
  strcat(Message, "\r\n");
  strcat(Message, "End.");
  strcat(Message, "\r\n");
  return strlen(Message);
}

unsigned settingMessage(char* Message1){
  char txt[3];
  Message1[0] = '\0';
  strcat(Message1, "SETTING:");
  strcat(Message1, "\r\n");

  strcat(Message1, " Temp_H:");
  t2=EEPROM_Read(0x00);
  ShortToStr(t2,txt);
  strcat(Message1, txt);
  strcat(Message1, " C");
  strcat(Message1, "\r\n");

  strcat(Message1, " Temp_L:");
  t3=EEPROM_Read(0x01);
  ShortToStr(t3,txt);
  strcat(Message1, txt);
  strcat(Message1, " C");
  strcat(Message1, "\r\n");

  strcat(Message1, " Humi_H:");
  h2=EEPROM_Read(0x02);
  ShortToStr(h2,txt);
  strcat(Message1, txt);
  strcat(Message1, " %");
  strcat(Message1, "\r\n");

  strcat(Message1, " Humi_L:");
  h3=EEPROM_Read(0x03);
  ShortToStr(h3,txt);
  strcat(Message1, txt);
  strcat(Message1, " %");
  strcat(Message1, "\r\n");

  strcat(Message1, "End.");
  strcat(Message1, "\r\n");
  return strlen(Message1);
}

unsigned AlarmMessage(char* Message2){
   char txt1[3],txt2[3],txt3[3],txt4[3];
   Message2[0] = '\0';
  strcat(Message2, "Alarm:");
  strcat(Message2, "\r\n");
  
  if(EEPROM_Read(0x11)>=EEPROM_Read(0x00)){
  strcat(Message2, " Temp: ");
  ShortToStr(EEPROM_Read(0x11),txt1);
  strcat(Message2, txt1);
  strcat(Message2, " C");
  strcat(Message2,">= Temp_H: ");
  ShortToStr(EEPROM_Read(0x00),txt2);
  strcat(Message2, txt2);
  strcat(Message2, " C");
  strcat(Message2, "\r\n");
  }
  if(EEPROM_Read(0x11)<=EEPROM_Read(0x01)){
  strcat(Message2, " Temp: ");
  ShortToStr(EEPROM_Read(0x11),txt1);
  strcat(Message2, txt1);
  strcat(Message2, " C");
  strcat(Message2,"<= Temp_L: ");
  ShortToStr(EEPROM_Read(0x01),txt2);
  strcat(Message2, txt2);
  strcat(Message2, " C");
  strcat(Message2, "\r\n");
  }
  if(EEPROM_Read(0x10)>=EEPROM_Read(0x02)){
  strcat(Message2, " Humi: ");
  ShortToStr(EEPROM_Read(0x10),txt3);
  strcat(Message2, txt3);
  strcat(Message2, " %");
  strcat(Message2,">= Humi_H: ");
  ShortToStr(EEPROM_Read(0x02),txt4);
  strcat(Message2, txt4);
  strcat(Message2, " %");
  strcat(Message2, "\r\n");
  }
  if(EEPROM_Read(0x10)<=EEPROM_Read(0x03)){
  strcat(Message2, " Humi: ");
  ShortToStr(EEPROM_Read(0x10),txt3);
  strcat(Message2, txt3);
  strcat(Message2, " %");
  strcat(Message2,"<= Humi_L: ");
  ShortToStr(EEPROM_Read(0x03),txt4);
  strcat(Message2, txt4);
  strcat(Message2, " %");
  strcat(Message2, "\r\n");
  }
  strcat(Message2, "End.");
  strcat(Message2, "\r\n");
  return strlen(Message2);
}
unsigned Al_Message(char* Message3){

  Message3[0] = '\0';
  strcat(Message3, "Status_INPUT:");
  strcat(Message3, "\r\n");
  if (Button(&PORTA, 3, 100, 1))  // Digital_IN1
  { strcat(Message3, " IN1 - OPEN"); }
  else
  { strcat(Message3, " IN1 - CLOSE"); }
  strcat(Message3, "\r\n");
  if (Button(&PORTA, 2, 100, 1))  // Digital_IN2
  { strcat(Message3, " IN2 - CLOSE"); }
  else
  { strcat(Message3, " IN2 - OPEN"); }
  strcat(Message3, "\r\n");
  
  strcat(Message3, "End.");
  strcat(Message3, "\r\n");
  return strlen(Message3);
}

void Send_Status(){
 ComposeMessage(sms);
 GSM_Send_Msg(Mobile_no1,sms);
}

void Send_Status1(){
 settingMessage(sms);
 GSM_Send_Msg(Mobile_no1,sms);
 cnt2=0;
}
void Send_Status2(){
 Relay1=1;
 AlarmMessage(sms);
 GSM_Send_Msg(Mobile_no1,sms);
 cnt2=0;
}
void Send_Status3(){
 Al_Message(sms);
 GSM_Send_Msg(Mobile_no1,sms);
}


void GSM_Begin()
{

        while(1)
        {
                LCD_Cmd(0xc0);
                UART1_Write_Text("ATE0\r"); /* send ATE0 to check module is ready or not */
                UART1_Write(0x0D);
                Delay_ms(500);
                if(strstr(buff,"OK"))
                {
                        GSM_Response();
                        memset(buff,0,160);
                        break;
                }
                else
                {
                        Lcd_Out_CP("Error");
                     //     break;
                }
        }

        Lcd_Cmd(_LCD_CLEAR);
        Lcd_Out(1,1,"Text Mode");
        LCD_Cmd(0xc0);
        UART1_Write_Text("AT+CMGF=1\r"); /* select message format as text */
        UART1_Write(0x0D);
        GSM_Response();
}

void GSM_Msg_Delete(unsigned int position)
{
char delete_cmd[20];
buffer_pointer=0;

      /* delete message at specified position */
        sprintl(delete_cmd,"AT+CMGD=%d\r",position);
        UART1_Write_Text(delete_cmd);
        UART1_Write(0x0D);
}

void GSM_Delete_All_Msg()
{
   UART1_Write_Text("AT+CMGDA=\"DEL ALL\"\r"); /* delete all messages of SIM */
   UART1_Write(0x0D);
}

bool GSM_Wait_for_Msg()
{
        char msg_location[4];
        int i;
        Delay_ms(500);
        buffer_pointer=0;

        while(1)
        {
           /*eliminate "\r \n" which is start of string */

           if(buff[buffer_pointer]=='\r' || buff[buffer_pointer]== '\n'){
                        buffer_pointer++;
                }
                else
                        break;
        }

        /* "CMTI:" to check if any new message received */

        if(strstr(buff,"CMTI:")){
                while(buff[buffer_pointer]!= ',')
                {
                        buffer_pointer++;
                }
                buffer_pointer++;

                i=0;
                while(buff[buffer_pointer]!= '\r')
                {
                        msg_location[i]=buff[buffer_pointer];
                        buffer_pointer++;
                        i++;
                }

                /* convert string of position to integer value */
                position = atoi(msg_location);

                memset(buff,0,strlen(buff));
                buffer_pointer=0;

                return true;
        }
        else
        {
                return false;
        }
}


void GSM_Send_Msg(char *num,char *sms)
{
        char sms_buffer[35];
        buffer_pointer=0;
        sprintl(sms_buffer,"AT+CMGS=\"%s\"\r",num);
        UART1_Write_Text(sms_buffer); /*send command AT+CMGS="Mobile No."\r */
        UART1_Write(0x0D);
        Delay_ms(200);
        while(1)
        {
                if(buff[buffer_pointer]==0x3E) /* wait for '>' character*/
                {
                   buffer_pointer = 0;
                   memset(buff,0,strlen(buff));
                   UART1_Write_Text(sms); /* send msg to given no. */
                   UART1_Write(0x0D);
                   UART1_Write(0x1A); /* send Ctrl+Z */
                   break;
                }
                buffer_pointer++;
        }
        Delay_ms(300);
        buffer_pointer = 0;
        memset(buff,0,strlen(buff));
        memset(sms_buffer,0,strlen(sms_buffer));
}

void GSM_Calling(char *Mob_no)
{
   char call[20];
   sprintl(call,"ATD%s;\r",Mob_no);
   UART1_Write_Text(call);        /* send command ATD<Mobile_No>; for calling*/
   UART1_Write(0x0D);
}

void GSM_HangCall()
{
   Lcd_Cmd(_LCD_CLEAR);
   UART1_Write_Text("ATH\r");        /*send command ATH\r to hang call*/
   UART1_Write(0x0D);
}

void GSM_Response()
{       int i;
        unsigned int timeout=0;
        int CRLF_Found=0;
        char CRLF_buff[2];
        int Response_Length=0;
        while(1)
        {
                if(timeout>=60000)          /*if timeout occur then return */
                return;
                Response_Length = strlen(buff);
                if(Response_Length)
                {
                        Delay_ms(2);
                        timeout++;
                        if(Response_Length==strlen(buff))
                        {
                                for(i=0;i<Response_Length;i++)
                                {
                                        memmove(CRLF_buff,CRLF_buff+1,1);
                                        CRLF_buff[1]=buff[i];
                                        if(strncmp(CRLF_buff,"\r\n",2))
                                        {
                                            if(CRLF_Found++==2)                                                                    /* search for \r\n in string */
                                             {
                                                GSM_Response_Display();
                                                return;
                                             }
                                        }

                                }
                                CRLF_Found = 0;

                        }

                }
                Delay_ms(1);
                timeout++;
        }
        status_flag=0;
}

void GSM_Response_Display()
{
int lcd_pointer = 0;
buffer_pointer = 0;
        while(1)
        {
          /* search for \r\n in string */
          if(buff[buffer_pointer]== '\r' || buff[buffer_pointer]== '\n')
                {
                        buffer_pointer++;
                }
                else
                        break;
        }


        LCD_Cmd(0xc0);
        while(buff[buffer_pointer]!='\r')             /* display response till "\r" */
        {
                Lcd_Chr_CP(buff[buffer_pointer]);
                buffer_pointer++;
                lcd_pointer++;
                if(lcd_pointer==15)        /* check for end of LCD line */
                  LCD_Cmd(0x80);
        }
        buffer_pointer = 0;
        memset(buff,0,strlen(buff));
}

void GSM_Msg_Read(int position)
{
        char read_cmd[10];
        sprintl(read_cmd,"AT+CMGR=%d\r",position);
        UART1_Write_Text(read_cmd);        /* read message at specified location/position */
        UART1_Write(0x0D);
        GSM_Msg_Display();        /* display message */
}

void GSM_Msg_Display()
{       int i,j;
        Delay_ms(500);
        if(!(strstr(buff,"+CMGR")))        /*check for +CMGR response */
        {
                Lcd_Out(1,1,"No message");
        }
        else
        {
                buffer_pointer = 0;

                while(1)
                {
                        /*wait till \r\n not over*/

                        if(buff[buffer_pointer]=='\r' || buff[buffer_pointer]== 'n')                                         {
                                buffer_pointer++;
                        }
                        else
                                break;
                }

                /* search for 1st ',' to get mobile no.*/
                while(buff[buffer_pointer]!=',')
                {
                        buffer_pointer++;
                }
                buffer_pointer = buffer_pointer+2;

                /* extract mobile no. of message sender */
                for(i=0;i<=12;i++)
                {
                        Mobile_no[i] = buff[buffer_pointer];
                        buffer_pointer++;
                }

                do
                {
                        buffer_pointer++;
                }while(buff[buffer_pointer-1]!= '\n');

                LCD_Cmd(0xC0);
                j=0;

                /* display and save message */
                while(buff[buffer_pointer]!= '\r' && j<31)
                {
                                Lcd_Chr_CP(buff[buffer_pointer]);
                                message_received[j]=buff[buffer_pointer];

                                buffer_pointer++;
                                j++;
                                if(j==16)
                                        LCD_Cmd(0x80); /* display on 1st line */
                }

                buffer_pointer = 0;
                memset(buff,0,strlen(buff));
        }
        status_flag = 0;
}
 ///////////////////////////////////////Cam bien/////////////////////////////


char AM2302_Read(unsigned *humidity, unsigned *temperature) {
  char i = 0, j = 1;
  char timeout = 0;
  char sensor_byte;
  AM2302_Bus_Out = 0;

  AM2302_Bus_Direction = 1;                         //Set AM2302_Bus as input
  AM2302_Bus_Direction = 0;                         //Set AM2302_Bus as output
  AM2302_Bus_Out = 0;                               //Host the start signal down time min: 0.8ms, typ: 1ms, max: 20ms
  Delay_ms(18);
  AM2302_Bus_Out = 1;                               //Set AM2302_Bus HIGH
  Delay_us(20);                                       //Delay 20us
  AM2302_Bus_Direction = 1;                         //Set AM2302_Bus as input

  // Bus master has released time min: 20us, typ: 30us, max: 200us
  timeout = 30;
  while (AM2302_Bus_In == 1) {
    Delay_us(1);
    if (!timeout--) {
      return 1;
    } //ERROR: Sensor not responding
  }

  // AM2302 response signal min: 75us, typ: 80us, max: 85us
  while (!AM2302_Bus_In) { //response to low time
  //  Delay_us(80);
  }

  while (AM2302_Bus_In) {  //response to high time
    //Delay_us(50);
  }

  /*
   * time in us:            min  typ  max
   * signal 0 high time:    22   26   30 (bit=0)
   * signal 1 high time:    68   70   75 (bit=1)
   * signal 0,1 down time:  48   50   55
   */

  i = 0; //get 5 byte
  for (i = 0; i < 5; i++) {
    j = 1;
    for (j = 1; j <= 8; j++) { //get 8 bits from sensor
      while (!AM2302_Bus_In) { //signal "0", "1" low time
        Delay_us(1);
      }
      Delay_us(30);
      sensor_byte <<= 1;       //add new lower byte
      if (AM2302_Bus_In) {     //if sda high after 30us => bit=1 else bit=0
        sensor_byte |= 1;
        delay_us(45);
        while (AM2302_Bus_In) {
          Delay_us(1);
        }
      }
    }
    AM2302_Data[i] = sensor_byte;
  }

  *humidity = (AM2302_Data[0] << 8) + AM2302_Data[1];
  *temperature = (AM2302_Data[2] << 8) + AM2302_Data[3];

  return 0;
}


void processValue(unsigned humidity, unsigned temperature) {
  if(humidity>=1000)  Lcd_Chr(1,7,(humidity/1000) +48);
  else  Lcd_Out(1,7," ");
  txt4[0]= humidity/100%10 +48;
  txt4[1]= (humidity/10)%10 +48;
  txt4[3]= humidity%10 +48;
  Lcd_Out(1,1,"HUMI1:");
  Lcd_Out(1,8,txt4);
  Lcd_Chr(1,13,0x25);

  if(temperature&0x8000) Lcd_Out(2,7,"-");
  else  Lcd_Out(2,7," ");
  temperature&=0x7FFF;
  txt3[0]= temperature/100 +48;
  txt3[1]= (temperature/10)%10 +48;
  txt3[3]= temperature%10 +48;
  Lcd_Out(2,1,"TEMP1:");
  Lcd_Out(2,8,txt3);
  Lcd_Out_CP(" C");
}

void status_lcd()
{
      /////////////// lcd=1////////////////////////
if(lcd==1){
  if (Button(&PORTB, 0, 100, 0)) {               // Detect logical one

     temp_hh++;
     if(temp_hh==temp_h)
     {
      temp_hh=0;
     }
     EEPROM_Write(0x00,temp_hh);

    }
    if (Button(&PORTB, 1, 100, 0)) {               // Detect logical one
     temp_ll--;
     if(temp_ll==temp_l)
     {
      temp_ll=100;
     }
     EEPROM_Write(0x01,temp_ll);
    }
  temp_hh=EEPROM_Read(0x00);
  ShortToStr(temp_hh,str1);
  temp_ll=EEPROM_Read(0x01);
  ShortToStr(temp_ll,str2);
  Lcd_Out(1,1,"TEMP HH:");
  Lcd_Out(2,1,"TEMP LL:");
  Lcd_Out(1,9,str1);
  Lcd_Out(2,9,str2);
  }
    /////////////// lcd=2////////////////////////
  if(lcd==2){
  if (Button(&PORTB, 0, 100, 0)) {               // Detect logical one

     humi_hh++;
     if(humi_hh==humi_h)
     {
      humi_hh=0;
     }
     EEPROM_Write(0x02,humi_hh);

    }
    if (Button(&PORTB, 1, 100, 0)) {               // Detect logical one
     humi_ll--;
     if(humi_ll==humi_l)
     {
      humi_ll=100;
     }
     EEPROM_Write(0x03,humi_ll);
    }
  humi_hh=EEPROM_Read(0x02);
  ShortToStr(humi_hh,str3);
  humi_ll=EEPROM_Read(0x03);
  ShortToStr(humi_ll,str4);
  Lcd_Out(1,1,"HUMI HH:");
  Lcd_Out(2,1,"HUMI LL:");
  Lcd_Out(1,9,str3);
  Lcd_Out(2,9,str4);
  }
  ///////////////xet trang thai lcd////////////////////////
    if (Button(&PORTC, 5, 150, 0) && lcd==0) {               // Detect logical one
       Lcd_Cmd(_LCD_CLEAR);
       Lcd_Cmd(_LCD_CURSOR_OFF);
       lcd=1;
    }
    if (Button(&PORTC, 5, 150, 0) && lcd==1) {               // Detect logical one
      Lcd_Cmd(_LCD_CLEAR);
      Lcd_Cmd(_LCD_CURSOR_OFF);
      lcd=2;
    }
    if (Button(&PORTC, 5, 150, 0) && lcd==2) {               // Detect logical one
      Lcd_Cmd(_LCD_CLEAR);
      Lcd_Cmd(_LCD_CURSOR_OFF);
      lcd=0;
    }
 }
void Display_Init(){
  LCD_Init();
  LCD_Cmd(_LCD_CLEAR);
  Lcd_Cmd(_LCD_CURSOR_OFF);
  Lcd_Out(3,3,"VAN TUYEN");
  Lcd_Out(4,3,"  GTVT ");
  delay_ms(2000);
}
