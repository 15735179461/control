#include <rtthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <drivers/pin.h>
#include "../drivers/drv_gpio.h"

#define IN4 (rt_uint8_t)54  //控制电机B
#define IN3 (rt_uint8_t)56
#define IN2 (rt_uint8_t)55 //控制电机A
#define IN1 (rt_uint8_t)57


#define LEFT	77
#define RIGHT	75


//static rt_thread_t tid1 = RT_NULL;
//static rt_thread_t tid2 = RT_NULL;


void drive_2(char *str)
{

   
    if(strcmp(str,"forward") == 0){  //AB电机都正转
        rt_kprintf("str=forward\n");

        rt_pin_write(IN1,PIN_HIGH);
        rt_pin_write(IN2,PIN_LOW);
        rt_pin_write(IN3,PIN_HIGH);
        rt_pin_write(IN4,PIN_LOW);

    }else if(strcmp(str,"back") == 0){ //AB电机都倒转
        rt_kprintf("str=back\n");

        rt_pin_write(IN1,PIN_LOW);
        rt_pin_write(IN2,PIN_HIGH);
        rt_pin_write(IN3,PIN_LOW);
        rt_pin_write(IN4,PIN_HIGH);

    }else if(strcmp(str,"right") == 0){ //A电机不动，B电机正转

       rt_kprintf("str=right\n");
       rt_pin_write(IN1,PIN_LOW);
       rt_pin_write(IN2,PIN_HIGH);
       rt_pin_write(IN3,PIN_HIGH);
       rt_pin_write(IN4,PIN_LOW);
        

    }else if(strcmp(str,"left") == 0){ //A电机正转，B电机不转
        rt_kprintf("str=left\n");
        rt_pin_write(IN1,PIN_HIGH);
        rt_pin_write(IN2,PIN_LOW);
        rt_pin_write(IN3,PIN_LOW);
        rt_pin_write(IN4,PIN_HIGH);


    }else if(strcmp(str,"stop") == 0){  //AB都不转

       rt_kprintf("str=stop\n");

        rt_pin_write(IN1,PIN_HIGH);
        rt_pin_write(IN2,PIN_HIGH);
        rt_pin_write(IN3,PIN_LOW);
        rt_pin_write(IN4,PIN_LOW);

    }

}

void pin_init_2(void)
{
    hw_pin_init();
    
    // 把相应 gpio 设为输出模式
    rt_pin_mode(IN4, PIN_MODE_OUTPUT);
    rt_pin_mode(IN3, PIN_MODE_OUTPUT);
    rt_pin_mode(IN2, PIN_MODE_OUTPUT);
    rt_pin_mode(IN1, PIN_MODE_OUTPUT); 

    rt_pin_mode(LEFT, PIN_MODE_INPUT);
    rt_pin_mode(RIGHT, PIN_MODE_INPUT);
}

void BiZhang(void)
{
    int SR;
	int SL;

    rt_kprintf("bizhang!\n");
    

    
    while(1)
    {
        SR = rt_pin_read(RIGHT);//有信号表明在白色区域，车子底板上L亮；没信号表明压在黑线上，车子底板上L灭
        SL = rt_pin_read(LEFT);//有信号表明在白色区域，车子底板上L亮；没信号表明压在黑线上，车子底板上L灭
        
        if (SL == 1&&SR==1){  // 前面没有障碍物，小车前进

            drive_2("forward");
        }
        else if (SL == 0&&SR == 1){ // 右侧有障碍物，左转
            
            drive_2("stop");
            rt_thread_delay(0.2 * RT_TICK_PER_SECOND);
            drive_2("right");
            rt_thread_delay(0.1 * RT_TICK_PER_SECOND);     
        }
        else if (SR == 0&&SL == 1) {// 左侧有障碍物，右转
        
            drive_2("stop");
            rt_thread_delay(0.2 * RT_TICK_PER_SECOND);
            drive_2("left");
            rt_thread_delay(0.1 * RT_TICK_PER_SECOND);
        }
        else if (SR == 0&&SL == 0) {// 两边同时有障碍物，后退一段然后，左转
            drive_2("stop");
            drive_2("back");
            rt_thread_delay(0.1 * RT_TICK_PER_SECOND); 
            drive_2("stop");
            drive_2("left");
        }
        else {// 其他
            drive_2("stop");
        }
    }
      
}



void bizhang(void)
{   
    pin_init_2();
    rt_kprintf("qi dong!\n");
    //mqtt_start();
    BiZhang();   
}


#include <finsh.h>
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(bizhang, bizhang);
