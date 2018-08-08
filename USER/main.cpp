#include "string.h"
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "hall.h"
#include "rfid.h"
#include "can_interface.h"
#include "stmflash.h"
#include "myiic.h"
#include "cp2532.h"
#include "timer.h"
#include "beeper.h"
#include "lock.h"
#include "qr_code.h"

static void init_exti(void);
static void sys_indicator(void);

static void init()
{
    sys_tick_init();
    NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    uart_1_init(9600);	 	//串口1初始化为9600
    uart_2_init(9600);      //串口2初始化为9600
    uart_3_init(9600);      //串口3初始化为9600   
    LED.led_init();			     //LED端口初始化
    rfid_init();
    printf("RFID Driver version:%s\r\n", SW_VERSION);
    init_exti();
    i2c_init();    
    beeper_init(200,50);    
    tim2_int_init(499,7199);    // timer to control locks
    
    get_rfid_in_flash(rfid_in_flash);
    get_password_in_flash(psss_word_in_flash);

    return;
}

int main(void)
{
    init();  
   
    while(1)
    {
        rfid_task();
        touch_key_task();
        all_qr_data_task();
        can_protocol();
        beeper_task();
        sys_indicator();               
    }
}


static void init_exti(void)
{
    EXTI_InitTypeDef exit_init_structure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource4);
    exit_init_structure.EXTI_Line = EXTI_Line4;
    exit_init_structure.EXTI_Mode = EXTI_Mode_Interrupt;
    exit_init_structure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    exit_init_structure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&exit_init_structure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource6);
    exit_init_structure.EXTI_Line = EXTI_Line6;
    EXTI_Init(&exit_init_structure);
    
//    //----  cp2532 touch key int  ----//
//    GPIO_InitTypeDef GPIO_InitStructure;
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);	//使能GPIOG时钟
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入
//    GPIO_Init(GPIOG, &GPIO_InitStructure);  //初始化PG11
//    //----  cp2532 touch key int  ----//
//    GPIO_EXTILineConfig(GPIO_PortSourceGPIOG, GPIO_PinSource11);
//    exit_init_structure.EXTI_Line = EXTI_Line11;
//    EXTI_Init(&exit_init_structure);
//    
//    //----  cp2532 touch key int  ----//
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure); 
    

    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); 

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); 
}


#define INDICATOR_LED_PERIOD    500/SYSTICK_PERIOD

extern void set_beeper_low(void);
static void sys_indicator(void)
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
 
        
        // ----  test code for lock ----//
//        if(cnt % 6 == 1)
//        {
//        }
    }
    
}
