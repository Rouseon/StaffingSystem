/*
 * @Descripttion: 员工管理系统服务器接口
 * @version: 1.0
 * @Author: Lou Sheng
 * @Date: 2021-06-26 11:00:18
 * @LastEditors: Lou Sheng
 * @LastEditTime: 2021-06-29 19:50:09
 */
#ifndef __SERVER_H__
#define __SERVER_H__

#include "stafsp.h"
#include <sqlite3.h>
#include <unistd.h>
#include <time.h>

#define SQLLEN 512  //sql语句大小  

#define INSERT_ADMIN 1
#define DELETE_ADMIN 2
#define SHOW_ADMIN   3

#define SQL_CREATE_STAFF "create table if not exists staff(\
                        job_id int primary key,\
                        password char,\
                        name char,\
                        sex char,\
                        age int,\
                        address char,\
                        salary float,\
                        department char,\
                        job char,\
                        level int,\
                        privilege int);"

#define SQL_CREATE_RECORD "create table if not exists record(\
                        job_id int,\
                        date char,\
                        online char,\
                        offline char,\
                        state int);"

//线程传递参数
typedef struct args{
    data databuf;
    sqlite3** db;
	int cfd;
}pthread_args;

int sfd;

void* client_handler(void *arg);

void create_staffInfo(sqlite3** db);
int create_table(sqlite3* db,char* sql);
void create_staff_record_table(sqlite3* db);

int server_admin_UI(void);
void server_admin_insert_UI(staff* adminer);
void server_admin_delete_UI(sqlite3* db,staff* adminer);

int select_admin_all(sqlite3* db);
int insert_admin(sqlite3* db, staff* staff);
int delete_admin(sqlite3* db, staff* staff);
int select_jobid_password(sqlite3* db, int jobid, char * password, int whoami);

int isalready_onrecord(sqlite3* db,record* record);
int isalready_offrecord(sqlite3* db,record* record);
int insert_record_online(sqlite3* db, record* record);
int insert_record_offline(sqlite3* db, record* record);

int insert_staff(sqlite3* db, staff* staff);//增
int delete_staff(sqlite3* db, staff* staff);//删
int change_staff(sqlite3* db, staff* staff);//改
int change_password(sqlite3* db, staff* staff);
int change_age(sqlite3* db, staff* staff);
int change_address(sqlite3* db, staff* staff);
int change_salary(sqlite3* db, staff* staff);
int change_department(sqlite3* db, staff* staff);
int change_job(sqlite3* db, staff* staff);
int change_level(sqlite3* db, staff* staff);
int demand_one_staff(sqlite3* db, data* databuf);//查
int demand_all_staff(sqlite3* db, data* databuf);
int demand_one_staff_record(sqlite3* db, data* databuf);//查
int demand_all_staff_record(sqlite3* db, data* databuf);

char* get_date(char* date);
char* get_time(char* date);

typedef void (*sighandler)(int);
void handler_quit(int sig);

#endif
