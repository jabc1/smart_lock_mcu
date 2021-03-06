#include "led.h"

LEDClass LED;

void LEDClass::led_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(LED_RCC_APB2, ENABLE);	 //使能PC端口时钟

    GPIO_InitStructure.GPIO_Pin = LED_PIN;				 //LED0-->PC.9 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIOC.9
    GPIO_SetBits(LED_PORT,LED_PIN);						 //PA.9 输出高
}

void LEDClass::led_on(void)
{
    GPIO_SetBits(LED_PORT,LED_PIN);
}
 
void LEDClass::led_off(void)
{
    GPIO_ResetBits(LED_PORT,LED_PIN);
}


#define INDICATOR_LED_PERIOD    500/SYSTICK_PERIOD

void sys_indicator(void)
{
    static u16 cnt = 0;
    static u32 start_tick = 0;
    if(get_tick() - start_tick >= INDICATOR_LED_PERIOD)
    {
        cnt++;
        
        if(cnt % 2 == 1)
        {
            LED.led_on();
        }
        else
        {
            LED.led_off();  
        }
        start_tick = get_tick();      
    }    
}
