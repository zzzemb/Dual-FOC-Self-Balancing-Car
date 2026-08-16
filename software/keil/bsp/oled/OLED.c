#include "main.h"
#include "OLED.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>

#define OLED_USE_SW_I2C	


#define OLED_SCL            GPIO_PIN_13 
#define OLED_SDA            GPIO_PIN_14 
#define OLED_SCL_GPIO_Port  GPIOC
#define OLED_SDA_GPIO_Port  GPIOC

#ifdef OLED_USE_HW_I2C
#define OLED_I2C            hi2c1
extern  I2C_HandleTypeDef   hi2c1;	
#endif

#define OLED_ADDRESS 0x3C << 1	

#define OLED_I2C_TIMEOUT 10
#define Delay_time 3

uint8_t OLED_DisplayBuf[8][128];

#ifdef OLED_USE_SW_I2C

void OLED_W_SCL(uint8_t BitValue)
{
	
	HAL_GPIO_WritePin(OLED_SCL_GPIO_Port, OLED_SCL, (GPIO_PinState)BitValue);
    for (volatile uint16_t i = 0; i < Delay_time; i++){
       
    }
}

void OLED_W_SDA(uint8_t BitValue)
{

	HAL_GPIO_WritePin(OLED_SDA_GPIO_Port, OLED_SDA, (GPIO_PinState)BitValue);
	
	for (volatile uint16_t i = 0; i < Delay_time; i++){
       
    }
}
#endif

void OLED_GPIO_Init(void)
{
    uint32_t i, j;

    for (i = 0; i < 1000; i++) {
        for (j = 0; j < 1000; j++)
            ;
    }
#ifdef OLED_USE_SW_I2C
    __HAL_RCC_GPIOC_CLK_ENABLE();		
    __HAL_RCC_GPIOA_CLK_ENABLE();       
	GPIO_InitTypeDef GPIO_InitStruct = {0};            
 	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;	       
    GPIO_InitStruct.Pull = GPIO_PULLUP;                
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  
	GPIO_InitStruct.Pin = SW_I2C_SDA_Pin;                
 	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SW_I2C_SCL_Pin;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	OLED_W_SCL(1);
	OLED_W_SDA(1);
#endif
}

void OLED_I2C_Start(void)
{
#ifdef OLED_USE_SW_I2C
	OLED_W_SDA(1);		
	OLED_W_SCL(1);		
	OLED_W_SDA(0);		
	OLED_W_SCL(0);		
#endif
}

void OLED_I2C_Stop(void)
{
#ifdef OLED_USE_SW_I2C
	OLED_W_SDA(0);	
	OLED_W_SCL(1);	
	OLED_W_SDA(1);	
#endif
}


void OLED_I2C_SendByte(uint8_t Byte)
{
#ifdef OLED_USE_SW_I2C
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		OLED_W_SDA(!!(Byte & (0x80 >> i)));
		OLED_W_SCL(1);	
		OLED_W_SCL(0);	
	}
	OLED_W_SCL(1);		
	OLED_W_SCL(0);
#endif
}

void OLED_WriteCommand(uint8_t Command)
{
#ifdef OLED_USE_SW_I2C
    OLED_I2C_Start();           
	OLED_I2C_SendByte(0x78);		
	OLED_I2C_SendByte(0x00);	
    OLED_I2C_SendByte(Command); 
    OLED_I2C_Stop();           
#elif defined(OLED_USE_HW_I2C)
    uint8_t TxData[2] = {0x00, Command};
    HAL_I2C_Master_Transmit(&OLED_I2C, OLED_ADDRESS, (uint8_t*)TxData, 2, OLED_I2C_TIMEOUT);
#endif 
}

void OLED_WriteData(uint8_t *Data, uint8_t Count)
{
    uint8_t i;
#ifdef OLED_USE_SW_I2C
    OLED_I2C_Start();       
	OLED_I2C_SendByte(0x78);		

    OLED_I2C_SendByte(0x40); 

    for (i = 0; i < Count; i++) {
        OLED_I2C_SendByte(Data[i]);
    }
    OLED_I2C_Stop();
#elif defined(OLED_USE_HW_I2C)
    uint8_t TxData[Count + 1]; 
    TxData[0] = 0x40; 
    for (i = 0; i < Count; i++) {
        TxData[i + 1] = Data[i];
    }
    HAL_I2C_Master_Transmit(&OLED_I2C, OLED_ADDRESS, (uint8_t*)TxData, Count + 1, OLED_I2C_TIMEOUT);
#endif    
}


void OLED_Init(void)
{
    OLED_GPIO_Init(); 

    OLED_WriteCommand(0xAE);

    OLED_WriteCommand(0xD5);
    OLED_WriteCommand(0x80);

    OLED_WriteCommand(0xA8);
    OLED_WriteCommand(0x3F);

    OLED_WriteCommand(0xD3);
    OLED_WriteCommand(0x00);

    OLED_WriteCommand(0x40);

    OLED_WriteCommand(0xA1);

    OLED_WriteCommand(0xC8);

    OLED_WriteCommand(0xDA);
    OLED_WriteCommand(0x12);

    OLED_WriteCommand(0x81);
    OLED_WriteCommand(0xCF);

    OLED_WriteCommand(0xD9);
    OLED_WriteCommand(0xF1);

    OLED_WriteCommand(0xDB);
    OLED_WriteCommand(0x30);

    OLED_WriteCommand(0xA4);

    OLED_WriteCommand(0xA6);

    OLED_WriteCommand(0x8D);
    OLED_WriteCommand(0x14);

    OLED_WriteCommand(0xAF);

    OLED_Clear();  
    OLED_Update(); 
}

void OLED_SetCursor(uint8_t Page, uint8_t X)
{
    OLED_WriteCommand(0xB0 | Page);              
    OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4)); 
    OLED_WriteCommand(0x00 | (X & 0x0F));        
}

uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1; 
    while (Y--)         
    {
        Result *= X; 
    }
    return Result;
}

uint8_t OLED_pnpoly(uint8_t nvert, int16_t *vertx, int16_t *verty, int16_t testx, int16_t testy)
{
    int16_t i, j, c = 0;

    for (i = 0, j = nvert - 1; i < nvert; j = i++) {
        if (((verty[i] > testy) != (verty[j] > testy)) &&
            (testx < (vertx[j] - vertx[i]) * (testy - verty[i]) / (verty[j] - verty[i]) + vertx[i])) {
            c = !c;
        }
    }
    return c;
}

uint8_t OLED_IsInAngle(int16_t X, int16_t Y, int16_t StartAngle, int16_t EndAngle)
{
    int16_t PointAngle;
    PointAngle = atan2(Y, X) / 3.14 * 180; 
    if (StartAngle < EndAngle)             
    {

        if (PointAngle >= StartAngle && PointAngle <= EndAngle) {
            return 1;
        }
    } else 
    {
         
        if (PointAngle >= StartAngle || PointAngle <= EndAngle) {
            return 1;
        }
    }
    return 0; 
}

void OLED_Update(void)
{
    uint8_t j;
     
    for (j = 0; j < 8; j++) {
         
        OLED_SetCursor(j, 0);
         
        OLED_WriteData(OLED_DisplayBuf[j], 128);
    }
}

void OLED_UpdateArea(uint8_t X, uint8_t Y, uint8_t Width, uint8_t Height)
{
    uint8_t j;

     
    if (X > 127) { return; }
    if (Y > 63) { return; }
    if (X + Width > 128) { Width = 128 - X; }
    if (Y + Height > 64) { Height = 64 - Y; }

     
     
    for (j = Y / 8; j < (Y + Height - 1) / 8 + 1; j++) {
         
        OLED_SetCursor(j, X);
         
        OLED_WriteData(&OLED_DisplayBuf[j][X], Width);
    }
}

void OLED_Clear(void)
{
    uint8_t i, j;
    for (j = 0; j < 8; j++) 
    {
        for (i = 0; i < 128; i++) 
        {
            OLED_DisplayBuf[j][i] = 0x00; 
        }
    }
}

void OLED_ClearArea(uint8_t X, uint8_t Y, uint8_t Width, uint8_t Height)
{
    uint8_t i, j;

     
    if (X > 127) { return; }
    if (Y > 63) { return; }
    if (X + Width > 128) { Width = 128 - X; }
    if (Y + Height > 64) { Height = 64 - Y; }

    for (j = Y; j < Y + Height; j++) 
    {
        for (i = X; i < X + Width; i++) 
        {
            OLED_DisplayBuf[j / 8][i] &= ~(0x01 << (j % 8)); 
        }
    }
}

void OLED_Reverse(void)
{
    uint8_t i, j;
    for (j = 0; j < 8; j++) 
    {
        for (i = 0; i < 128; i++) 
        {
            OLED_DisplayBuf[j][i] ^= 0xFF; 
        }
    }
}

void OLED_ReverseArea(uint8_t X, uint8_t Y, uint8_t Width, uint8_t Height)
{
    uint8_t i, j;

     
    if (X > 127) { return; }
    if (Y > 63) { return; }
    if (X + Width > 128) { Width = 128 - X; }
    if (Y + Height > 64) { Height = 64 - Y; }

    for (j = Y; j < Y + Height; j++) 
    {
        for (i = X; i < X + Width; i++) 
        {
            OLED_DisplayBuf[j / 8][i] ^= 0x01 << (j % 8); 
        }
    }
}

void OLED_ShowChar(uint8_t X, uint8_t Y, char Char, uint8_t FontSize)
{
    if (FontSize == OLED_8X16) 
    {
         
        OLED_ShowImage(X, Y, 8, 16, OLED_F8x16[Char - ' ']);
    } else if (FontSize == OLED_6X8) 
    {
         
        OLED_ShowImage(X, Y, 6, 8, OLED_F6x8[Char - ' ']);
    }
}

void OLED_ShowString(uint8_t X, uint8_t Y, char *String, uint8_t FontSize)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i++) 
    {
         
        OLED_ShowChar(X + i * FontSize, Y, String[i], FontSize);
    }
}

void OLED_ShowNum(uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize)
{
    uint8_t i;
    for (i = 0; i < Length; i++) 
    {
         
         
         
        OLED_ShowChar(X + i * FontSize, Y, Number / OLED_Pow(10, Length - i - 1) % 10 + '0', FontSize);
    }
}

void OLED_ShowSignedNum(uint8_t X, uint8_t Y, int32_t Number, uint8_t Length, uint8_t FontSize)
{
    uint8_t i;
    uint32_t Number1;

    if (Number >= 0) 
    {
        OLED_ShowChar(X, Y, '+', FontSize); 
        Number1 = Number;                   
    } else                                  
    {
        OLED_ShowChar(X, Y, '-', FontSize); 
        Number1 = -Number;                  
    }

    for (i = 0; i < Length; i++) 
    {
         
         
         
        OLED_ShowChar(X + (i + 1) * FontSize, Y, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0', FontSize);
    }
}

void OLED_ShowHexNum(uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize)
{
    uint8_t i, SingleNumber;
    for (i = 0; i < Length; i++) 
    {
         
        SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;

        if (SingleNumber < 10) 
        {
             
             
            OLED_ShowChar(X + i * FontSize, Y, SingleNumber + '0', FontSize);
        } else 
        {
             
             
            OLED_ShowChar(X + i * FontSize, Y, SingleNumber - 10 + 'A', FontSize);
        }
    }
}

void OLED_ShowBinNum(uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize)
{
    uint8_t i;
    for (i = 0; i < Length; i++) 
    {
         
         
         
        OLED_ShowChar(X + i * FontSize, Y, Number / OLED_Pow(2, Length - i - 1) % 2 + '0', FontSize);
    }
}

void OLED_ShowFloatNum(uint8_t X, uint8_t Y, double Number, uint8_t IntLength, uint8_t FraLength, uint8_t FontSize)
{
    uint32_t PowNum, IntNum, FraNum;

    if (Number >= 0) 
    {
        OLED_ShowChar(X, Y, '+', FontSize); 
    } else                                  
    {
        OLED_ShowChar(X, Y, '-', FontSize); 
        Number = -Number;                   
    }

     
    IntNum = Number;                  
    Number -= IntNum;                 
    PowNum = OLED_Pow(10, FraLength); 
    FraNum = round(Number * PowNum);  
    IntNum += FraNum / PowNum;        

     
    OLED_ShowNum(X + FontSize, Y, IntNum, IntLength, FontSize);

     
    OLED_ShowChar(X + (IntLength + 1) * FontSize, Y, '.', FontSize);

     
    OLED_ShowNum(X + (IntLength + 2) * FontSize, Y, FraNum, FraLength, FontSize);
}

void OLED_ShowImage(uint8_t X, uint8_t Y, uint8_t Width, uint8_t Height, const uint8_t *Image)
{
    uint8_t i, j;

     
    if (X > 127) { return; }
    if (Y > 63) { return; }

     
    OLED_ClearArea(X, Y, Width, Height);

     
     
    for (j = 0; j < (Height - 1) / 8 + 1; j++) {
         
        for (i = 0; i < Width; i++) {
             
            if (X + i > 127) { break; }
            if (Y / 8 + j > 7) { return; }

             
            OLED_DisplayBuf[Y / 8 + j][X + i] |= Image[j * Width + i] << (Y % 8);

             
             
            if (Y / 8 + j + 1 > 7) { continue; }

             
            OLED_DisplayBuf[Y / 8 + j + 1][X + i] |= Image[j * Width + i] >> (8 - Y % 8);
        }
    }
}

void OLED_Printf(uint8_t X, uint8_t Y, uint8_t FontSize, char *format, ...)
{
    char String[30];                         
    va_list arg;                             
    va_start(arg, format);                   
    vsprintf(String, format, arg);           
    va_end(arg);                             
    OLED_ShowString(X, Y, String, FontSize); 
}

void OLED_DrawPoint(uint8_t X, uint8_t Y)
{
     
    if (X > 127) { return; }
    if (Y > 63) { return; }

     
    OLED_DisplayBuf[Y / 8][X] |= 0x01 << (Y % 8);
}

uint8_t OLED_GetPoint(uint8_t X, uint8_t Y)
{
     
    if (X > 127) { return 0; }
    if (Y > 63) { return 0; }

     
    if (OLED_DisplayBuf[Y / 8][X] & 0x01 << (Y % 8)) {
        return 1; 
    }

    return 0; 
}

void OLED_DrawLine(uint8_t X0, uint8_t Y0, uint8_t X1, uint8_t Y1)
{
    int16_t x, y, dx, dy, d, incrE, incrNE, temp;
    int16_t x0 = X0, y0 = Y0, x1 = X1, y1 = Y1;
    uint8_t yflag = 0, xyflag = 0;

    if (y0 == y1) 
    {
         
        if (x0 > x1) {
            temp = x0;
            x0   = x1;
            x1   = temp;
        }

         
        for (x = x0; x <= x1; x++) {
            OLED_DrawPoint(x, y0); 
        }
    } else if (x0 == x1) 
    {
         
        if (y0 > y1) {
            temp = y0;
            y0   = y1;
            y1   = temp;
        }

         
        for (y = y0; y <= y1; y++) {
            OLED_DrawPoint(x0, y); 
        }
    } else 
    {
        if (x0 > x1) 
        {
             
             
            temp = x0;
            x0   = x1;
            x1   = temp;
            temp = y0;
            y0   = y1;
            y1   = temp;
        }

        if (y0 > y1) 
        {
             
             
            y0 = -y0;
            y1 = -y1;

             
            yflag = 1;
        }

        if (y1 - y0 > x1 - x0) 
        {
             
             
            temp = x0;
            x0   = y0;
            y0   = temp;
            temp = x1;
            x1   = y1;
            y1   = temp;

             
            xyflag = 1;
        }

         
         
        dx     = x1 - x0;
        dy     = y1 - y0;
        incrE  = 2 * dy;
        incrNE = 2 * (dy - dx);
        d      = 2 * dy - dx;
        x      = x0;
        y      = y0;

         
        if (yflag && xyflag) {
            OLED_DrawPoint(y, -x);
        } else if (yflag) {
            OLED_DrawPoint(x, -y);
        } else if (xyflag) {
            OLED_DrawPoint(y, x);
        } else {
            OLED_DrawPoint(x, y);
        }

        while (x < x1) 
        {
            x++;
            if (d < 0) 
            {
                d += incrE;
            } else 
            {
                y++;
                d += incrNE;
            }

             
            if (yflag && xyflag) {
                OLED_DrawPoint(y, -x);
            } else if (yflag) {
                OLED_DrawPoint(x, -y);
            } else if (xyflag) {
                OLED_DrawPoint(y, x);
            } else {
                OLED_DrawPoint(x, y);
            }
        }
    }
}

void OLED_DrawRectangle(uint8_t X, uint8_t Y, uint8_t Width, uint8_t Height, uint8_t IsFilled)
{
    uint8_t i, j;
    if (!IsFilled) 
    {
         
        for (i = X; i < X + Width; i++) {
            OLED_DrawPoint(i, Y);
            OLED_DrawPoint(i, Y + Height - 1);
        }
         
        for (i = Y; i < Y + Height; i++) {
            OLED_DrawPoint(X, i);
            OLED_DrawPoint(X + Width - 1, i);
        }
    } else 
    {
         
        for (i = X; i < X + Width; i++) {
             
            for (j = Y; j < Y + Height; j++) {
                 
                OLED_DrawPoint(i, j);
            }
        }
    }
}

void OLED_DrawTriangle(uint8_t X0, uint8_t Y0, uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2, uint8_t IsFilled)
{
    uint8_t minx = X0, miny = Y0, maxx = X0, maxy = Y0;
    uint8_t i, j;
    int16_t vx[] = {X0, X1, X2};
    int16_t vy[] = {Y0, Y1, Y2};

    if (!IsFilled) 
    {
         
        OLED_DrawLine(X0, Y0, X1, Y1);
        OLED_DrawLine(X0, Y0, X2, Y2);
        OLED_DrawLine(X1, Y1, X2, Y2);
    } else 
    {
         
        if (X1 < minx) { minx = X1; }
        if (X2 < minx) { minx = X2; }
        if (Y1 < miny) { miny = Y1; }
        if (Y2 < miny) { miny = Y2; }

         
        if (X1 > maxx) { maxx = X1; }
        if (X2 > maxx) { maxx = X2; }
        if (Y1 > maxy) { maxy = Y1; }
        if (Y2 > maxy) { maxy = Y2; }

         
         
         
        for (i = minx; i <= maxx; i++) {
             
            for (j = miny; j <= maxy; j++) {
                 
                 
                if (OLED_pnpoly(3, vx, vy, i, j)) { OLED_DrawPoint(i, j); }
            }
        }
    }
}

void OLED_DrawCircle(uint8_t X, uint8_t Y, uint8_t Radius, uint8_t IsFilled)
{
    int16_t x, y, d, j;

    d = 1 - Radius;
    x = 0;
    y = Radius;

     
    OLED_DrawPoint(X + x, Y + y);
    OLED_DrawPoint(X - x, Y - y);
    OLED_DrawPoint(X + y, Y + x);
    OLED_DrawPoint(X - y, Y - x);

    if (IsFilled) 
    {
         
        for (j = -y; j < y; j++) {
             
            OLED_DrawPoint(X, Y + j);
        }
    }

    while (x < y) 
    {
        x++;
        if (d < 0) 
        {
            d += 2 * x + 1;
        } else 
        {
            y--;
            d += 2 * (x - y) + 1;
        }

         
        OLED_DrawPoint(X + x, Y + y);
        OLED_DrawPoint(X + y, Y + x);
        OLED_DrawPoint(X - x, Y - y);
        OLED_DrawPoint(X - y, Y - x);
        OLED_DrawPoint(X + x, Y - y);
        OLED_DrawPoint(X + y, Y - x);
        OLED_DrawPoint(X - x, Y + y);
        OLED_DrawPoint(X - y, Y + x);

        if (IsFilled) 
        {
             
            for (j = -y; j < y; j++) {
                 
                OLED_DrawPoint(X + x, Y + j);
                OLED_DrawPoint(X - x, Y + j);
            }

             
            for (j = -x; j < x; j++) {
                 
                OLED_DrawPoint(X - y, Y + j);
                OLED_DrawPoint(X + y, Y + j);
            }
        }
    }
}

void OLED_DrawEllipse(uint8_t X, uint8_t Y, uint8_t A, uint8_t B, uint8_t IsFilled)
{
    int16_t x, y, j;
    int16_t a = A, b = B;
    float d1, d2;

     

    x  = 0;
    y  = b;
    d1 = b * b + a * a * (-b + 0.5);

    if (IsFilled) 
    {
         
        for (j = -y; j < y; j++) {
             
            OLED_DrawPoint(X, Y + j);
            OLED_DrawPoint(X, Y + j);
        }
    }

     
    OLED_DrawPoint(X + x, Y + y);
    OLED_DrawPoint(X - x, Y - y);
    OLED_DrawPoint(X - x, Y + y);
    OLED_DrawPoint(X + x, Y - y);

     
    while (b * b * (x + 1) < a * a * (y - 0.5)) {
        if (d1 <= 0) 
        {
            d1 += b * b * (2 * x + 3);
        } else 
        {
            d1 += b * b * (2 * x + 3) + a * a * (-2 * y + 2);
            y--;
        }
        x++;

        if (IsFilled) 
        {
             
            for (j = -y; j < y; j++) {
                 
                OLED_DrawPoint(X + x, Y + j);
                OLED_DrawPoint(X - x, Y + j);
            }
        }

         
        OLED_DrawPoint(X + x, Y + y);
        OLED_DrawPoint(X - x, Y - y);
        OLED_DrawPoint(X - x, Y + y);
        OLED_DrawPoint(X + x, Y - y);
    }

     
    d2 = b * b * (x + 0.5) * (x + 0.5) + a * a * (y - 1) * (y - 1) - a * a * b * b;

    while (y > 0) {
        if (d2 <= 0) 
        {
            d2 += b * b * (2 * x + 2) + a * a * (-2 * y + 3);
            x++;

        } else 
        {
            d2 += a * a * (-2 * y + 3);
        }
        y--;

        if (IsFilled) 
        {
             
            for (j = -y; j < y; j++) {
                 
                OLED_DrawPoint(X + x, Y + j);
                OLED_DrawPoint(X - x, Y + j);
            }
        }

         
        OLED_DrawPoint(X + x, Y + y);
        OLED_DrawPoint(X - x, Y - y);
        OLED_DrawPoint(X - x, Y + y);
        OLED_DrawPoint(X + x, Y - y);
    }
}

void OLED_DrawArc(uint8_t X, uint8_t Y, uint8_t Radius, int16_t StartAngle, int16_t EndAngle, uint8_t IsFilled)
{
    int16_t x, y, d, j;

     

    d = 1 - Radius;
    x = 0;
    y = Radius;

     
    if (OLED_IsInAngle(x, y, StartAngle, EndAngle)) { OLED_DrawPoint(X + x, Y + y); }
    if (OLED_IsInAngle(-x, -y, StartAngle, EndAngle)) { OLED_DrawPoint(X - x, Y - y); }
    if (OLED_IsInAngle(y, x, StartAngle, EndAngle)) { OLED_DrawPoint(X + y, Y + x); }
    if (OLED_IsInAngle(-y, -x, StartAngle, EndAngle)) { OLED_DrawPoint(X - y, Y - x); }

    if (IsFilled) 
    {
         
        for (j = -y; j < y; j++) {
             
            if (OLED_IsInAngle(0, j, StartAngle, EndAngle)) { OLED_DrawPoint(X, Y + j); }
        }
    }

    while (x < y) 
    {
        x++;
        if (d < 0) 
        {
            d += 2 * x + 1;
        } else 
        {
            y--;
            d += 2 * (x - y) + 1;
        }

         
        if (OLED_IsInAngle(x, y, StartAngle, EndAngle)) { OLED_DrawPoint(X + x, Y + y); }
        if (OLED_IsInAngle(y, x, StartAngle, EndAngle)) { OLED_DrawPoint(X + y, Y + x); }
        if (OLED_IsInAngle(-x, -y, StartAngle, EndAngle)) { OLED_DrawPoint(X - x, Y - y); }
        if (OLED_IsInAngle(-y, -x, StartAngle, EndAngle)) { OLED_DrawPoint(X - y, Y - x); }
        if (OLED_IsInAngle(x, -y, StartAngle, EndAngle)) { OLED_DrawPoint(X + x, Y - y); }
        if (OLED_IsInAngle(y, -x, StartAngle, EndAngle)) { OLED_DrawPoint(X + y, Y - x); }
        if (OLED_IsInAngle(-x, y, StartAngle, EndAngle)) { OLED_DrawPoint(X - x, Y + y); }
        if (OLED_IsInAngle(-y, x, StartAngle, EndAngle)) { OLED_DrawPoint(X - y, Y + x); }

        if (IsFilled) 
        {
             
            for (j = -y; j < y; j++) {
                 
                if (OLED_IsInAngle(x, j, StartAngle, EndAngle)) { OLED_DrawPoint(X + x, Y + j); }
                if (OLED_IsInAngle(-x, j, StartAngle, EndAngle)) { OLED_DrawPoint(X - x, Y + j); }
            }

             
            for (j = -x; j < x; j++) {
                 
                if (OLED_IsInAngle(-y, j, StartAngle, EndAngle)) { OLED_DrawPoint(X - y, Y + j); }
                if (OLED_IsInAngle(y, j, StartAngle, EndAngle)) { OLED_DrawPoint(X + y, Y + j); }
            }
        }
    }
}
