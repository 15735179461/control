#include <rtthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <drivers/pin.h>
#include "../drivers/drv_gpio.h"
#include "../libraries/ls1c_delay.h"


#define IN4 (rt_uint8_t)54  //控制电机B
#define IN3 (rt_uint8_t)56
#define IN2 (rt_uint8_t)55 //控制电机A
#define IN1 (rt_uint8_t)57

#define LEFT	89
#define MIDDLE  87
#define RIGHT	91

#define time 100/2

void pin_init_3(void)
{
    hw_pin_init();
    
    // 把相应 gpio 设为输出模式
    rt_pin_mode(IN4, PIN_MODE_OUTPUT);
    rt_pin_mode(IN3, PIN_MODE_OUTPUT);
    rt_pin_mode(IN2, PIN_MODE_OUTPUT);
    rt_pin_mode(IN1, PIN_MODE_OUTPUT); 

}

void drive_3(char *str)
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

void XunJi(void)
{
    int SR;
	int SL;
    int MID;

    rt_kprintf("xunji!\n");
    
    rt_pin_mode(LEFT, PIN_MODE_INPUT);
    rt_pin_mode(RIGHT, PIN_MODE_INPUT);
    rt_pin_mode(MIDDLE, PIN_MODE_INPUT);
    
    while(1)
    {
        //rt_pin_mode(LEFT, PIN_MODE_INPUT);
        //rt_pin_mode(RIGHT, PIN_MODE_INPUT);
        //rt_pin_mode(MIDDLE, PIN_MODE_INPUT);
        
        SR = rt_pin_read(RIGHT);//有信号表明在白色区域，车子底板上L亮；没信号表明压在黑线上，车子底板上L灭
        SL = rt_pin_read(LEFT);//有信号表明在白色区域，车子底板上L亮；没信号表明压在黑线上，车子底板上L灭
        MID = rt_pin_read(MIDDLE);
        
        if (MID == 1)
        {

            if (SL == 0&&SR==0){
                //printf("forward\n");
                drive_3("forward");
                delay_ms(time);
            }
            else if (SL == 1&&SR == 0){
                //printf("left\n");
                drive_3("left");
                delay_ms(time);
            }
            else if (SR == 1&&SL == 0) {// 右循迹红外传感器,检测到信号，车子向左偏离轨道，向右转  
                //printf("right\n");  
                drive_3("right");
                delay_ms(time);
            }
            else {// 都是白色, 停止
               //printf("stop\n");
               drive_3("stop");
               delay_ms(time);
            }
        }     
        else if(MID == 0)
        {
            if (SL == 0 && SR == 1){
                //printf("right\n");
                drive_3("right");
                delay_ms(time);
            }
            else if(SR == 0 && SL == 1){
                //printf("left\n");
                drive_3("left");
                delay_ms(time);
            }
            else{
                //printf("stop\n");
                drive_3("STOP");
                delay_ms(time);
            }
        }
        
    }
      
}

void xunji(void)
{   
    pin_init_3();
    
    rt_kprintf("qi dong!\n");
    
    //mqtt_start();
    XunJi();  
    
}
#include <finsh.h>
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(xunji, xunji);