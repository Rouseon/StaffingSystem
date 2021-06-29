/*
 * @Descripttion: 员工管理系统客户端接口
 * @version: 1.0
 * @Author: Lou Sheng
 * @Date: 2021-06-26 11:00:08
 * @LastEditors: Lou Sheng
 * @LastEditTime: 2021-06-29 19:54:42
 */
#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "stafsp.h"

#define INSERT_STAFF 1
#define DELETE_STAFF 2
#define CHANGE_STAFF 3
#define DEMAND_STAFF 4
#define DEMAND_RECORD 5
#define QUIT          6

#define STAFF_DEMAND  1
#define STAFF_CHANGE  2
#define RECORD_DEMAND 3
#define STAFF_QUIT    4

#define CHANGE_PASSWORD 1
#define CHANGE_AGE      2
#define CHANGE_ADDRESS  3
#define CHANGE_SALARY   4
#define CHANGE_DEPART   5
#define CHANGE_JOB      6
#define CHANGE_LEVEL    7
#define CHANGE_FINISH   8
#define SELFCH_FINISH   4

#define DEMAND_ONE  1
#define DEMAND_ALL  2

int sfd;

int user_select_UI(void);
void admin_UI(void);
void staff_UI(void);
int login_UI(data * databuf,int sfd,int whoami);

void admin_work_UI(data * databuf,int sfd);
void staff_work_UI(data * databuf,int sfd);

void insert_staff(staff * staff);
void delete_staff(staff * staff);
void change_staff(data * databuf,int sfd);
void change_self(data * databuf, int sfd);
void demand_staff(data * databuf);
void display_staff(data * databuf);
void demand_record(data * databuf);
void display_record(data * databuf);
void quit(staff * staff);
typedef void (*sighandler)(int);
void handler_quit(int sig);


#endif
