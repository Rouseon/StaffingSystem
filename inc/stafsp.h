/*
 * @Descripttion: 员工管理系统服务器和客户端协议层
 * @version: 1.0
 * @Author: Lou Sheng
 * @Date: 2021-06-26 10:53:48
 * @LastEditors: Lou Sheng
 * @LastEditTime: 2021-06-29 20:19:55
 */

#ifndef __STAFSP_H__
#define __STAFSP_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define I_AM_ADMIN 1
#define I_AM_STAFF 2

#define STAFF_ITEM 11
#define RECORD_ITEM 5

//协议大小
#define DATA_LEN        5120 //数据消息
#define ADDRESS_LEN     100  //住址
#define DEPARTMENT_LEN  50   //部门
#define JOB_LEN         50   //岗位
#define CMD_LEN         50   //请求指令
#define DATE_LEN        30   //日期
#define ONLINE_LEN      30   //上班打卡记录
#define OFFLINE_LEN     30   //下班打开记录
#define PASSWORD_LEN    20   //密码
#define NAME_LEN        20   //姓名
#define SEX_LEN         10   //性别

//请求指令
#define CMD_APPLY_LOGIN  "cmd_apply_login"
#define CMD_ALLOW_LOGIN  "cmd_allow_login"
#define CMD_DONOT_LOGIN  "cmd_donot_login"

#define CMD_INSERT_STAFF "cmd_insert_staff"
#define CMD_INSERT_OK    "cmd_insert_ok"
#define CMD_INSERT_FAIL  "cmd_insert_fail"

#define CMD_DELETE_STAFF "cmd_delete_staff"
#define CMD_DELETE_OK    "cmd_delete_ok"
#define CMD_DELETE_FAIL  "cmd_delete_fail"

#define CMD_CHANGE_PASSWD       "cmd_change_passwd"
#define CMD_CHANGE_PASSWD_OK    "cmd_change_passwdok"
#define CMD_CHANGE_PASSWD_FAIL  "cmd_change_passwdfail"
#define CMD_CHANGE_AGE       "cmd_change_age"
#define CMD_CHANGE_AGE_OK    "cmd_change_age_ok"
#define CMD_CHANGE_AGE_FAIL  "cmd_change_age_fail"
#define CMD_CHANGE_ADDRES       "cmd_change_addres"
#define CMD_CHANGE_ADDRES_OK    "cmd_change_addres_ok"
#define CMD_CHANGE_ADDRES_FAIL  "cmd_change_addres_fail"
#define CMD_CHANGE_SALARY       "cmd_change_salary"
#define CMD_CHANGE_SALARY_OK    "cmd_change_salary_ok"
#define CMD_CHANGE_SALARY_FAIL  "cmd_change_salary_fail"
#define CMD_CHANGE_DEPART       "cmd_change_depart"
#define CMD_CHANGE_DEPART_OK    "cmd_change_depart_ok"
#define CMD_CHANGE_DEPART_FAIL  "cmd_change_depart_fail"
#define CMD_CHANGE_JOB       "cmd_change_job"
#define CMD_CHANGE_JOB_OK    "cmd_change_job_ok"
#define CMD_CHANGE_JOB_FAIL  "cmd_change_job_fail"
#define CMD_CHANGE_LEVEL       "cmd_change_level"
#define CMD_CHANGE_LEVEL_OK    "cmd_change_level_ok"
#define CMD_CHANGE_LEVEL_FAIL  "cmd_change_level_fail"

#define CMD_DEMAND_ONE_STAFF "cmd_demand_one_staff"
#define CMD_DEMAND_ONE_OK    "cmd_demand_one_ok"
#define CMD_DEMAND_ONE_FAIL  "cmd_demand_one_fail"
#define CMD_DEMAND_ALL_STAFF "cmd_demand_all_staff"
#define CMD_DEMAND_ALL_OK    "cmd_demand_all_ok"
#define CMD_DEMAND_ALL_FAIL  "cmd_demand_all_fail"
#define CMD_DEMAND_ONE_STAFF_RECORD "cmd_demand_one_staff_record"
#define CMD_DEMAND_ONE_OK_RECORD    "cmd_demand_one_ok_record"
#define CMD_DEMAND_ONE_FAIL_RECORD  "cmd_demand_one_fail_record"
#define CMD_DEMAND_ALL_STAFF_RECORD "cmd_demand_all_staff_record"
#define CMD_DEMAND_ALL_OK_RECORD    "cmd_demand_all_ok_record"
#define CMD_DEMAND_ALL_FAIL_RECORD  "cmd_demand_all_fail_record"

#define COERCE_QUIT    "coerce_quit"
#define CMD_QUIT       "cmd_quit"
#define CMD_QUIT_OK    "cmd_quit_ok"
#define CMD_QUIT_FAIL  "cmd_quit_fail"

//员工对象
typedef struct staff{
    char    address[ADDRESS_LEN];
    char    password[PASSWORD_LEN];
    char    department[DEPARTMENT_LEN];
    char    job[JOB_LEN];
    char    name[NAME_LEN];
    char    sex[SEX_LEN];
    float   salary;
    int     jobid;
    int     age;
    int     level;
    int     privilege;
}staff;

//考勤表对象
typedef struct record{
    int  jobid;
    int  state;  //在线状态
    char date[DATE_LEN];
    char online[ONLINE_LEN];
    char offline[OFFLINE_LEN];
}record;

//传输协议
typedef struct ptotocol{
    char    data[DATA_LEN];
    staff   staff;
    record  record;
    char    cmd[CMD_LEN];
}data;

/**
 * @name: ERR_LOG
 * @function: 打印错误信息
 */
#define ERR_LOG(err_msg) do{\
    fprintf(stderr,"error: ");\
	perror(err_msg);\
	fprintf(stderr, "%s %s %d\n", __FILE__, __func__, __LINE__);\
}while(0)

/**
 * @name: SQL_LOG
 * @function: 打印数据库错误信息
 */
#define SQL_LOG(err_msg) do{\
    fprintf(stderr,"error: %s\n", err_msg);\
    fprintf(stderr, "%s %s %d\n", __FILE__, __func__, __LINE__);\
}while(0)

#define INPUT_LOG() do{\
    printf("输入选项错误，请重新输入提供的选项（按回车继续）");\
    while(getchar()!=10);\
}while(0)

#define HANG_LOG() do{\
    printf("（按回车继续）");\
    while(getchar()!=10);\
}while(0)

#endif
