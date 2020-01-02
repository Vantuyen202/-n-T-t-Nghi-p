#include <stdint.h>
#define Data_Out LATB5_bit         /* assign Port pin for data*/
#define Data_In RB5_bit/* read data from Port pin*/
#define Data_Dir TRISB5_bit /* Port direction */
//uint8_t T_byte1, T_byte2, RH_byte1, RH_byte2, CheckSum, time_read ;
//uint16_t Temp, RH;

#define CS PORTC.B1
#define DIN_Pin PORTC.B0
#define CLK_Pin PORTC.B2

sbit  rs485_rxtx_pin at RA4_bit;             // set transcieve pin
sbit  rs485_rxtx_pin_direction at TRISA4_bit;   // set transcieve pin direction

//unsigned int RH_Decimal,RH_Integral,T_Decimal,T_Integral;
//char Checksum;
//int RH=0;
//int T=0;
int comando_mestre = 0;
int comando_mestre1 = 0;
int comando_mestre4 = 0;
unsigned char dat[20];
// AM2302 module connections
sbit AM2302_Bus_In at RB5_bit;
sbit AM2302_Bus_Out at LATB5_bit;
sbit AM2302_Bus_Direction at TRISB5_bit;

unsigned short cnt;

void SPI_Write_Byte(unsigned short num)
{
unsigned short t, Mask, Flag;
  CLK_Pin = 0;
  Mask = 128;
 for (t=0; t<8; t++)
 {
  Flag = num & Mask;
  if(Flag == 0)
  {
   DIN_Pin = 0;
  }
  else
  {
  DIN_Pin = 1;
  }
  CLK_Pin = 1;
  CLK_Pin = 0;
  Mask = Mask >> 1;
 }
}

void max7219(char adres, char deger)

{

  CS=0;
  SPI_Write_Byte(adres);
  SPI_Write_Byte(deger);
  CS=1;
}
void max7219_init()

{

  max7219(0x0C,0x01);          // che do

  max7219(0x09,0xFFFF);//0x0F); // so led 7 thanh giai ma

  max7219(0x0A,0x09);//0x00    // do sang

  max7219(0x0B,0x07); //0x00    // so led 7 thanh dc quet

  max7219(0x0F,0x01);       //         chon thanh ghi che do hien thi test

  Delay_ms(100);
  max7219(0x0F,0x00);             // tat hien thi test

  //max7219(0x01,0x00);            // giu nguyen 1 thanh ghi de test

}


// END AM2302 module connections

char AM2302_Data[5] = {0, 0, 0, 0, 0};
unsigned humidity = 0, temperature = 0;
const char degree[] = {14,10,14,0,0,0,0,0};
void degreeChar(char pos_row, char pos_char) {
  char i;
    Lcd_Cmd(64);
    for (i = 0; i<=7; i++) Lcd_Chr_CP(degree[i]);
    Lcd_Cmd(_LCD_RETURN_HOME);
    Lcd_Chr(pos_row, pos_char, 0);
}
//Get Sensor values
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
    Delay_us(80);
  }

  while (AM2302_Bus_In) {  //response to high time
    Delay_us(50);
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

void processValue1 (unsigned humidity, unsigned temperature) {



  if(humidity>=1000)
  {
   max7219(5,(humidity/1000)%10);
   max7219(6,(humidity/100)%10);
   max7219(7,(humidity/10)%10+0x80);
   max7219(8,(humidity%10));
   }
  else
  {
   max7219(5,15);
   max7219(6,(humidity/100)%10);
   max7219(7,(humidity/10)%10+0x80);
   max7219(8,(humidity%10));
   }

  if(temperature&0x8000)
  {
   max7219(1,0x11);
   max7219(2,(temperature/100)%10);
   max7219(3,(temperature/10)%10+0x80);
   max7219(4,(temperature%10));
  }
  else  max7219(1,15);
   temperature&=0x7FFF;
   max7219(2,(temperature/100)%10);
   max7219(3,(temperature/10)%10+0x80);
   max7219(4,(temperature%10));

}

 void dislay(){


           max7219(1,0);
           max7219(2,0);
           max7219(3,0);
           max7219(4,0);

           max7219(5,0);
           max7219(6,0);
           max7219(7,0);
           max7219(8,0);
  }
void InitTimer1(){
  T1CON         = 0x21;
  RCIE_bit = 1;                      // enable interrupt on UART1 receive
  TXIE_bit = 0;                      // disable interrupt on UART1 transmit
  //PEIE_bit = 1;                      // enable peripheral interrupts
//  GIE_bit = 1;
  TMR1IF_bit         = 0;
  TMR1H         = 0x3C;
  TMR1L         = 0xB0;
  TMR1IE_bit         = 1;

  cnt =   0;
  INTCON         = 0xC0;
}
void interrupt() {
 RS485Slave_Receive(dat);
  if (TMR1IF_bit){
  cnt++;
    TMR1IF_bit = 0;
    TMR1H         = 0x3C;
    TMR1L         = 0xB0;
    //Enter your code here
  }
}
void main() {
   int i;
  ANSELB = 0;                        // Configure AN pins as digital I/O
  ANSELA = 0;
  OSCCON=0b01110010;
  TRISC0_bit=0;
  TRISC1_bit=0;
  TRISC2_bit=0;
  RC0_bit=0;
  RC1_bit=0;
  RC2_bit=0;


  UART1_Init(14400);                  // initialize UART1 module
  Delay_ms(100);
  RS485Slave_Init( 160 );              // Intialize MCU as slave, address 160

  //RCIE_bit = 1;                      // enable interrupt on UART1 receive
  //TXIE_bit = 0;                      // disable interrupt on UART1 transmit
  //PEIE_bit = 1;                      // enable peripheral interrupts
 // GIE_bit = 1;                       // enable all interrupts
  
  InitTimer1();
  dat[4] = 0;                        // ensure that message received flag is 0
  dat[5] = 0;                        // ensure that message received flag is 0
  dat[6] = 0;                        // ensure that error flag is 0
  
   max7219_init();
   dislay();
  while (1) {
  if(cnt>=20)
  {
   if( dat[4] ) //Recebeu alguma mensagem
  {
    dat[4] = 0;
   if (AM2302_Read(&humidity, &temperature) == 0)                 // Display AM2302_Read sensor values via LCD
     {
  processValue1(humidity, temperature);                         // Display AM2302_Read sensor values via LCD
  comando_mestre=humidity/10;
//  comando_mestre=comando_mestre;
  dat[0]=comando_mestre;//=RH;
  comando_mestre1=humidity%10;
  dat[1]=comando_mestre1;//=RH;
  dat[2]=2;
  RS485Slave_Send(dat, 3 );         //Envia 3 bytes via RS485 ao master
  
   comando_mestre=temperature/10;
//  comando_mestre=comando_mestre;
  dat[0]=comando_mestre;//=RH;
  comando_mestre1=temperature%10;
  dat[1]=comando_mestre1;//=RH;
  dat[2]=1;
  RS485Slave_Send(dat, 3 );
  cnt=0;
  }
  }
 // else
 // {
  //delay_ms(2000);
  // dat[4]=1;
  }
  }
}