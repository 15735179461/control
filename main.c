/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-10     zhuangwei    first version
 */

#include <rtthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <rthw.h>
#include <drivers/pin.h>
#include "../drivers/drv_gpio.h"
#include "../libraries/ls1c_delay.h"
#include "ls1c.h" 


#define IN4 (rt_uint8_t)54  //控制电机B
#define IN3 (rt_uint8_t)56
#define IN2 (rt_uint8_t)55 //控制电机A
#define IN1 (rt_uint8_t)57

#define XLEFT	89     //循迹左传感器gpio
#define XMIDDLE  87    //循迹中间传感器gpio
#define XRIGHT	91     //循迹右传感器gpio

#define BLEFT	77      //避障左传感器gpio
#define BRIGHT	75     //避障右传感器gpio

#define key1_gpio 85  //key1的gpio口
#define key2_gpio 86  //key2的gpio口

#define time 100/2   //循迹中断时间


int isKey1Press(void);
int isKey2Press(void);
void bizhang_(void);


//引脚初始化
void pin_init(void)
{
    hw_pin_init();
    
    // 把相应 gpio 设为输出模式
    rt_pin_mode(IN4, PIN_MODE_OUTPUT);
    rt_pin_mode(IN3, PIN_MODE_OUTPUT);
    rt_pin_mode(IN2, PIN_MODE_OUTPUT);
    rt_pin_mode(IN1, PIN_MODE_OUTPUT); 
    
    //循迹gpio初始化
    rt_pin_mode(XLEFT, PIN_MODE_INPUT);
    rt_pin_mode(XRIGHT, PIN_MODE_INPUT);
    rt_pin_mode(XMIDDLE, PIN_MODE_INPUT);
    
    //避障gpio初始化
    rt_pin_mode(BLEFT, PIN_MODE_INPUT);
    rt_pin_mode(BRIGHT, PIN_MODE_INPUT);
    
    //按键gpio初始化
    rt_pin_mode(key1_gpio, PIN_MODE_INPUT); 
    rt_pin_mode(key2_gpio, PIN_MODE_INPUT); 
}

//驱动
void drive(char *str)
{

    if(strcmp(str,"forward") == 0)
    {  
        rt_kprintf("str=forward\n");
        rt_pin_write(IN1,PIN_HIGH);
        rt_pin_write(IN2,PIN_LOW);
        rt_pin_write(IN3,PIN_HIGH);
        rt_pin_write(IN4,PIN_LOW);

    }else if(strcmp(str,"back") == 0)
    { 
        rt_kprintf("str=back\n");
        rt_pin_write(IN1,PIN_LOW);
        rt_pin_write(IN2,PIN_HIGH);
        rt_pin_write(IN3,PIN_LOW);
        rt_pin_write(IN4,PIN_HIGH);

    }else if(strcmp(str,"right") == 0)
    { 
       rt_kprintf("str=right\n");
       rt_pin_write(IN1,PIN_LOW);
       rt_pin_write(IN2,PIN_HIGH);
       rt_pin_write(IN3,PIN_HIGH);
       rt_pin_write(IN4,PIN_LOW);
        

    }else if(strcmp(str,"left") == 0)
    { 
        rt_kprintf("str=left\n");
        rt_pin_write(IN1,PIN_HIGH);
        rt_pin_write(IN2,PIN_LOW);
        rt_pin_write(IN3,PIN_LOW);
        rt_pin_write(IN4,PIN_HIGH);


    }else if(strcmp(str,"stop") == 0)
    {  //AB都不转
       rt_kprintf("str=stop\n");
        rt_pin_write(IN1,PIN_HIGH);
        rt_pin_write(IN2,PIN_HIGH);
        rt_pin_write(IN3,PIN_LOW);
        rt_pin_write(IN4,PIN_LOW);
    }
}

//循迹
void xunji_(void)
{
    int SR;
	int SL;
    int MID;

    rt_kprintf("xunji!\n");
    
    
    while(1)
    {   

        if (1 == isKey1Press())
        {
            bizhang_();
            return;
        }
        //isKey2Press();
        
        //有信号表明在白色区域，车子底板上L亮；没信号表明压在黑线上，车子底板上L灭
        SR = rt_pin_read(XRIGHT);
        SL = rt_pin_read(XLEFT);
        MID = rt_pin_read(XMIDDLE);
        
        if (MID == 1)
        {

            if (SL == 0&&SR==0)
            {
                drive("forward");
                delay_ms(time);
            }
            else if (SL == 1&&SR == 0)
            {
                drive("left");
                delay_ms(time);
            }
            else if (SR == 1&&SL == 0) 
            {// 右循迹红外传感器,检测到信号，车子向左偏离轨道，向右转  
                drive("right");
                delay_ms(time);
            }
            else \
            {// 都是白色, 停止
               
               drive("stop");
               delay_ms(time);
            }
        }   
        else if(MID == 0)
        {
            if (SL == 0 && SR == 1)
            {
                //printf("right\n");
                drive("right");
                delay_ms(time);
            }
            else if(SR == 0 && SL == 1)
            {
                //printf("left\n");
                drive("left");
                delay_ms(time);
            }
            else
            {
                //printf("stop\n");
                drive("STOP");
                delay_ms(time);
            }
        }
        
        
    }
      
}

//避障
void bizhang_(void)
{
    int SR;
	int SL;

    rt_kprintf("bizhang!\n");
    
    while(1)
    {
        if (2 == isKey2Press())
        {
            xunji_();
            return;
        }
        //isKey1Press();
        //isKey2Press();
        
        //有信号表明在白色区域，车子底板上L亮；没信号表明压在黑线上，车子底板上L灭
        SR = rt_pin_read(BRIGHT);
        SL = rt_pin_read(BLEFT);
        
        if (SL == 1&&SR == 1)
        {  // 前面没有障碍物，小车前进

            drive("forward");
        }
        else if (SL == 0&&SR == 1)
        { // 右侧有障碍物，左转
            
            drive("stop");
            rt_thread_delay(0.2 * RT_TICK_PER_SECOND);
            drive("right");
            rt_thread_delay(0.1 * RT_TICK_PER_SECOND);     
        }
        else if (SR == 0&&SL == 1) 
        {// 左侧有障碍物，右转
        
            drive("stop");
            rt_thread_delay(0.2 * RT_TICK_PER_SECOND);
            drive("left");
            rt_thread_delay(0.1 * RT_TICK_PER_SECOND);
        }
        else if (SR == 0&&SL == 0) 
        {// 两边同时有障碍物，后退一段然后，左转
            drive("stop");
            drive("back");
            rt_thread_delay(0.1 * RT_TICK_PER_SECOND); 
            drive("stop");
            drive("left");
        }
        else 
        {// 其他
            drive("stop");
        }

        //isKey1Press();
        
    }
      
}

//判断按键key1按下
int isKey1Press(void)
{
    //rt_thread_delay(RT_TICK_PER_SECOND/100);
    if (0 == rt_pin_read(key1_gpio))
    {
        rt_thread_delay(RT_TICK_PER_SECOND/100);
        printf("key1已经按下,启动bizhang\n");
        bizhang_();
        return 1;         
    }
    else
    {
        return -1;
    }

}

//判断按键key2按下
int isKey2Press(void)
{
    //rt_thread_delay(RT_TICK_PER_SECOND/100);
    if (0 == rt_pin_read(key2_gpio))
    {
        rt_thread_delay(RT_TICK_PER_SECOND/100);
        printf("key2已经按下,启动xunji\n");
        xunji_();
        return 2;
    }
    else
    {
        return -1;
    }
}




int main(int argc, char** argv)
{   

#ifdef XPT2046_USING_TOUCH
	/*触摸屏使用SPI总线SPI1 CS0  初始化*/
		rtgui_touch_hw_init("spi10");
#endif
	rt_kprintf("Loongson系统初始化完成！\n");
#if (defined RT_USING_RTGUI) && (defined RTGUI_USING_DEMO)
#ifdef USING_DEMO
extern int test_guidemo(void);
	test_guidemo();
#endif
#ifdef USING_UI_BUTTON
	ui_button();
#endif
#endif
   
    pin_init();
    while(1)
    {
        rt_thread_delay(RT_TICK_PER_SECOND/100);
        isKey1Press();
        isKey2Press();
    }

    return 0;
}
