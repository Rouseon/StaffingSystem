/*
 * @Descripttion: 
 * @version: 1.0
 * @Author: Lou Sheng
 * @Date: 2021-06-26 13:18:54
 * @LastEditors: Lou Sheng
 * @LastEditTime: 2021-06-29 20:33:45
 */
#include "server.h"
char *ch_result_staff[STAFF_ITEM] = {"工号","密码","姓名","性别","年龄","住址","薪资","部门","岗位","职级","权限"};
char *ch_result_record[RECORD_ITEM] = {"工  号","日  期","上  班","下  班","状  态"};
char today[DATE_LEN];

typedef void (*sighandler)(int);
void handler_quit(int sig)
{
    putchar(10);
    while(waitpid(-1, NULL, WNOHANG) > 0); 
    // printf("回收子进程\n");
    close(sfd);
    exit(0);
}

/**
 * @name: client_handler
 * @function: 客户请求处理线程
 * @param {void} *arg : 传入参数：cfd、协议对象
 * @return {*}
 */
void* client_handler(void *arg)
{
	pthread_args* args = (pthread_args*)arg;
	int cfd = args->cfd;
    data databuf = args->databuf;
    sqlite3 * db = *args->db;
	pthread_detach(pthread_self());

	int recv_len;
	while(1)
	{
		bzero(databuf.cmd, CMD_LEN);
        databuf.staff.jobid = 0;
        bzero(databuf.staff.password, PASSWORD_LEN);

		recv_len = recv(cfd, &databuf, sizeof(data), 0);

		if(recv_len < 0)
		{
			ERR_LOG("recv");
			return NULL;
		}
		else if(recv_len == 0)
		{
			//printf("客户端离线\n");
			close(cfd);
			exit(1);
		}
		else
		{
            if (strcmp(databuf.cmd, CMD_APPLY_LOGIN) == 0)
            {
                //查询工号和密码是否正确
                int ret;
                if(databuf.staff.privilege == 0)
                    ret = select_jobid_password(db,databuf.staff.jobid, databuf.staff.password, I_AM_ADMIN);
                else if(databuf.staff.privilege == 1)
                    ret = select_jobid_password(db,databuf.staff.jobid, databuf.staff.password, I_AM_STAFF);
                if(ret > 0)
                {
                    //正确，允许登录
                    //printf("允许登录\n");

                    //考勤表上线记录
                    //查询是否已打卡
                    get_date(today);
                    databuf.record.jobid = databuf.staff.jobid;
                    if(isalready_onrecord(db,&databuf.record))
                    {
                         //printf("已打过上班卡！\n");
                    }
                    else
                    {                      
                        bzero(databuf.record.date,DATE_LEN);
                        get_date(databuf.record.date);
                        bzero(databuf.record.online,ONLINE_LEN);
                        get_time(databuf.record.online);
                        strcpy(databuf.record.offline,"[]");
                        databuf.record.state = 1;
                        if(insert_record_online(db,&databuf.record) == 0)
                        {
                            // printf("打卡成功！\n");    
                        }                    
                    }
                    
                    bzero(databuf.cmd, CMD_LEN);
                    strcpy(databuf.cmd, CMD_ALLOW_LOGIN);
                    if((send(cfd, &databuf, sizeof(data), 0)) < 0)
                    {
                        ERR_LOG("send");
                        return NULL;
                    }
                }
                else
                {
                    //错误，不允许登录
                    //printf("不允许登录\n");
                    bzero(databuf.cmd, CMD_LEN);
                    strcpy(databuf.cmd, CMD_DONOT_LOGIN);
                    if((send(cfd, &databuf, sizeof(data), 0)) < 0)
                    {
                        ERR_LOG("send");
                        return NULL;
                    }               
                }
            }
            else if (strcmp(databuf.cmd, CMD_INSERT_STAFF) == 0)
            {
                //插入员工信息请求
                bzero(databuf.cmd, CMD_LEN);
                if(insert_staff(db,&databuf.staff) == 0)
                {
                    strcpy(databuf.cmd, CMD_INSERT_OK);
                    //printf("添加员工成功！\n");
                }
                else
                    strcpy(databuf.cmd, CMD_INSERT_FAIL);
                if((send(cfd, &databuf, sizeof(data), 0)) < 0)
                {
                    ERR_LOG("send");
                    return NULL;
                } 
            }
            else if (strcmp(databuf.cmd, CMD_DELETE_STAFF) == 0)
            {
                //删除员工信息请求
                bzero(databuf.cmd, CMD_LEN);
                if(delete_staff(db,&databuf.staff) == 0)
                {
                    strcpy(databuf.cmd, CMD_DELETE_OK);
                    //printf("删除员工成功！\n");
                }
                else
                    strcpy(databuf.cmd, CMD_DELETE_FAIL);
                if((send(cfd, &databuf, sizeof(data), 0)) < 0)
                {
                    ERR_LOG("send");
                    return NULL;
                }                
            }
            else if (strcmp(databuf.cmd, CMD_CHANGE_PASSWD) == 0)
            {
                //修改密码请求
                bzero(databuf.cmd, CMD_LEN);
                if(change_password(db,&databuf.staff) == 0)
                {
                    strcpy(databuf.cmd, CMD_CHANGE_PASSWD_OK);
                    //printf("修改密码成功！\n");
                }
                else
                    strcpy(databuf.cmd, CMD_CHANGE_PASSWD_FAIL);
                if((send(cfd, &databuf, sizeof(data), 0)) < 0)
                {
                    ERR_LOG("send");
                    return NULL;
                }                
            }        
            else if (strcmp(databuf.cmd, CMD_CHANGE_AGE) == 0)
            {
                //修改年龄请求
                bzero(databuf.cmd, CMD_LEN);
                if(change_age(db,&databuf.staff) == 0)
                {
                    strcpy(databuf.cmd, CMD_CHANGE_AGE_OK);
                    //printf("修改年龄成功！\n");
                }
                else
                    strcpy(databuf.cmd, CMD_CHANGE_AGE_FAIL);
                if((send(cfd, &databuf, sizeof(data), 0)) < 0)
                {
                    ERR_LOG("send");
                    return NULL;
                }                
            }    
            else if (strcmp(databuf.cmd, CMD_CHANGE_ADDRES) == 0)
            {
                //修改地址请求
                bzero(databuf.cmd, CMD_LEN);
                if(change_address(db,&databuf.staff) == 0)
                {
                    strcpy(databuf.cmd, CMD_CHANGE_ADDRES_OK);
                    //printf("修改地址成功！\n");
                }
                else
                    strcpy(databuf.cmd, CMD_CHANGE_ADDRES_FAIL);
                if((send(cfd, &databuf, sizeof(data), 0)) < 0)
                {
                    ERR_LOG("send");
                    return NULL;
                }                
            }     
            else if (strcmp(databuf.cmd, CMD_CHANGE_SALARY) == 0)
            {
                //修改薪资请求
                bzero(databuf.cmd, CMD_LEN);
                if(change_salary(db,&databuf.staff) == 0)
                {
                    strcpy(databuf.cmd, CMD_CHANGE_SALARY_OK);
                    //printf("修改薪资成功！\n");
                }
                else
                    strcpy(databuf.cmd, CMD_CHANGE_SALARY_FAIL);
                if((send(cfd, &databuf, sizeof(data), 0)) < 0)
                {
                    ERR_LOG("send");
                    return NULL;
                }                
            }   
            else if (strcmp(databuf.cmd, CMD_CHANGE_DEPART) == 0)
            {
                //修改部门请求
                bzero(databuf.cmd, CMD_LEN);
                if(change_department(db,&databuf.staff) == 0)
                {
                    strcpy(databuf.cmd, CMD_CHANGE_DEPART_OK);
                    //printf("修改部门成功！\n");
                }
                else
                    strcpy(databuf.cmd, CMD_CHANGE_DEPART_FAIL);
                if((send(cfd, &databuf, sizeof(data), 0)) < 0)
                {
                    ERR_LOG("send");
                    return NULL;
                }                
            }     
            else if (strcmp(databuf.cmd, CMD_CHANGE_JOB) == 0)
            {
                //修改岗位请求
                bzero(databuf.cmd, CMD_LEN);
                if(change_job(db,&databuf.staff) == 0)
                {
                    strcpy(databuf.cmd, CMD_CHANGE_JOB_OK);
                    //printf("修改岗位成功！\n");
                }
                else
                    strcpy(databuf.cmd, CMD_CHANGE_JOB_FAIL);
                if((send(cfd, &databuf, sizeof(data), 0)) < 0)
                {
                    ERR_LOG("send");
                    return NULL;
                }                
            }          
            else if (strcmp(databuf.cmd, CMD_CHANGE_LEVEL) == 0)
            {
                //修改职级请求
                bzero(databuf.cmd, CMD_LEN);
                if(change_level(db,&databuf.staff) == 0)
                {
                    strcpy(databuf.cmd, CMD_CHANGE_LEVEL_OK);
                    //printf("修改职级成功！\n");
                }
                else
                    strcpy(databuf.cmd, CMD_CHANGE_LEVEL_FAIL);
                if((send(cfd, &databuf, sizeof(data), 0)) < 0)
                {
                    ERR_LOG("send");
                    return NULL;
                }                
            }                         
            else if (strcmp(databuf.cmd, CMD_DEMAND_ONE_STAFF) == 0)
            {
                //查询员工信息请求
                bzero(databuf.cmd, CMD_LEN);
                if(demand_one_staff(db,&databuf) > 0)
                {
                    strcpy(databuf.cmd, CMD_DEMAND_ONE_OK);
                    //printf("查询员工成功！\n");
                }
                else
                    strcpy(databuf.cmd, CMD_DEMAND_ONE_FAIL);
                if((send(cfd, &databuf, sizeof(data), 0)) < 0)
                {
                    ERR_LOG("send");
                    return NULL;
                }                
            }           
            else if (strcmp(databuf.cmd, CMD_DEMAND_ALL_STAFF) == 0)
            {
                //查询所有员工信息请求
                bzero(databuf.cmd, CMD_LEN);

                if(demand_all_staff(db,&databuf) > 0)
                {
                    strcpy(databuf.cmd, CMD_DEMAND_ALL_OK);
                    //printf("查询所有员工成功！\n");
                }
                else
                    strcpy(databuf.cmd, CMD_DEMAND_ALL_FAIL);
                if((send(cfd, &databuf, sizeof(data), 0)) < 0)
                {
                    ERR_LOG("send");
                    return NULL;
                }              
            } 
            else if (strcmp(databuf.cmd, CMD_DEMAND_ONE_STAFF_RECORD) == 0)
            {
                //查询员工考勤记录信息请求
                bzero(databuf.cmd, CMD_LEN);
                if(demand_one_staff_record(db,&databuf) > 0)
                {
                    strcpy(databuf.cmd, CMD_DEMAND_ONE_OK_RECORD);
                    //printf("查询员工考勤记录成功！\n");
                }
                else
                    strcpy(databuf.cmd, CMD_DEMAND_ONE_FAIL_RECORD);
                if((send(cfd, &databuf, sizeof(data), 0)) < 0)
                {
                    ERR_LOG("send");
                    return NULL;
                }                
            } 
            else if (strcmp(databuf.cmd, CMD_DEMAND_ALL_STAFF_RECORD) == 0)
            {
                //查询所有员工考勤记录信息请求
                bzero(databuf.cmd, CMD_LEN);

                if(demand_all_staff_record(db,&databuf) > 0)
                {
                    strcpy(databuf.cmd, CMD_DEMAND_ALL_OK_RECORD);
                    //printf("查询所有考勤记录成功！\n");
                }
                else
                    strcpy(databuf.cmd, CMD_DEMAND_ALL_FAIL_RECORD);
                if((send(cfd, &databuf, sizeof(data), 0)) < 0)
                {
                    ERR_LOG("send");
                    return NULL;
                }               
            }
            else if (strcmp(databuf.cmd, CMD_QUIT) == 0)
            {
                //员工下班打卡信息请求           
                //查询是否已打卡
                if(!isalready_offrecord(db,&databuf.record))
                {
                    //printf("已打过下班卡！\n");
                    // strcpy(databuf.cmd, CMD_QUIT_OK);
                }
                else
                {
                    databuf.record.jobid = databuf.staff.jobid;
                    bzero(databuf.record.offline,OFFLINE_LEN);
                    get_time(databuf.record.offline);
                    databuf.record.state = 0;
                    bzero(databuf.cmd, CMD_LEN);
                    if(insert_record_offline(db,&databuf.record) == 0)
                    {
                        strcpy(databuf.cmd, CMD_QUIT_OK);
                        //printf("员工下班打卡成功！\n");
                    }
                    else
                        strcpy(databuf.cmd, CMD_QUIT_FAIL);                       
                }              

                if((send(cfd, &databuf, sizeof(data), 0)) < 0)
                {
                    ERR_LOG("send");
                    return NULL;
                }
                //记忆出错，一开始以为exit即可自动回收线程
                //没加pthread_exit将导致一个客户端退出，其余客户端将失去与服务器的通信
                //（能收到但是没有内容）,接收数据recv_len=0误认为服务器离线
                pthread_exit(NULL);     
                //同理，再ctrl+c退出客户端时也会产生相同问题
                //解决：信号回收    
            }
            else if (strcmp(databuf.cmd, COERCE_QUIT) == 0)
            {
                //记忆出错，一开始以为exit即可自动回收线程
                //没加pthread_exit将导致一个客户端退出，其余客户端将失去与服务器的通信
                //（能收到但是没有内容）,接收数据recv_len=0误认为服务器离线
                pthread_exit(NULL);     
                //同理，再ctrl+c退出客户端时也会产生相同问题
                //解决：信号回收    
            }

		}
	}
}

/**
 * @name: create_staffInfo
 * @function: 创建员工管理系统数据库
 * @param {sqlite3} * db : 数据库句柄
 * @return {*}
 */
void create_staffInfo(sqlite3 **db)
{
    int ret;
    if((ret = sqlite3_open("./sql/staffInfo.db",db)) != SQLITE_OK)
    {
        printf("数据库打开（创建）失败，错误码：%d\n",ret);
        exit(1); 
    }
    //printf("数据库打开（创建）成功\n");
}

/**
 * @name: create_table
 * @function: 创建sqlite3表
 * @param {sqlite3} * db : 数据库句柄
 * @param {char} * sql : 数据库语句
 * @return {int} 成功返回0，失败返回-1
 */
int create_table(sqlite3* db,char* sql)
{
    char* errmsg = NULL;
 	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK)
	{
        SQL_LOG(errmsg);
		return -1;
	}   
    return 0;
}

/**
 * @name: create_staff_record_table
 * @function:创建员工信息表和考勤记录表 
 * @param {sqlite3} * db : 数据库句柄
 * @return {*}
 */
void create_staff_record_table(sqlite3* db)
{
    char sql[SQLLEN] = SQL_CREATE_STAFF;
    if(create_table(db,sql) < 0)
    {
        fprintf(stderr,"%s %s %s %d\n",__FILE__,__func__,"打开（创建）员工信息表失败",__LINE__);
		exit(1);
    }
    //printf("员工信息表打开（创建）成功\n");
	
    bzero(sql,SQLLEN);
    strcpy(sql,SQL_CREATE_RECORD);
    if(create_table(db,sql) < 0)
    {
        fprintf(stderr,"%s %s %s %d\n",__FILE__,__func__,"打开（创建）考勤记录表失败",__LINE__);
		exit(1);
	}  
    //printf("考勤记录表打开（创建）成功\n"); 
}

/**
 * @name: server_admin_UI
 * @function: 员工管理系统服务器界面
 * @param {*}
 * @return {*} 1：添加管理员 2：删除管理员
 */
int server_admin_UI(void)
{
    int select = 0;
    printf("————————————————————————————\n");
    printf("*****员工管理系统服务器*****\n");
    printf("————————————————————————————\n");
    printf("1.添加管理员\n");
    printf("2.删除管理员\n"); 
    printf("3.显示管理员\n");
    printf("请输入选项> ");
    scanf("%d",&select);
    getchar();
    return select;
}

/**
 * @name: server_admin_insert_UI
 * @function: 添加管理员信息界面
 * @param {staff*} : staff对象
 * @return {*}
 */
void server_admin_insert_UI(staff* adminer)
{
    printf("————————————————————————————\n");
    printf("*****员工管理系统服务器*****\n");
    printf("————————————————————————————\n");
    printf(">>>>>>>>>添加管理员<<<<<<<<<\n");
    printf("————————————————————————————\n");
    printf("请输入工号> ");
    scanf("%d",&adminer->jobid);
    getchar();

    bzero(adminer->password,PASSWORD_LEN);
    printf("请输入密码> ");
    fgets(adminer->password, PASSWORD_LEN, stdin);
    adminer->password[strlen(adminer->password)-1] = 0;

    bzero(adminer->name,NAME_LEN);
    printf("请输入姓名> ");
    fgets(adminer->name, NAME_LEN, stdin);
    adminer->name[strlen(adminer->name)-1] = 0;

    bzero(adminer->sex,SEX_LEN);
    printf("请输入性别> ");
    fgets(adminer->sex, SEX_LEN, stdin);
    adminer->sex[strlen(adminer->sex)-1] = 0;

    printf("请输入年龄> ");
    scanf("%d",&adminer->age);
    getchar();

    bzero(adminer->address,ADDRESS_LEN);
    printf("请输入住址> ");
    fgets(adminer->address, ADDRESS_LEN, stdin);
    adminer->address[strlen(adminer->address)-1] = 0;

    printf("请输入薪资> ");
    scanf("%f",&adminer->salary);
    getchar();

    bzero(adminer->department,DEPARTMENT_LEN);
    printf("请输入部门> ");
    fgets(adminer->department, DEPARTMENT_LEN, stdin);
    adminer->department[strlen(adminer->department)-1] = 0;

    bzero(adminer->job,JOB_LEN);
    printf("请输入岗位> ");
    fgets(adminer->job, JOB_LEN, stdin);
    adminer->job[strlen(adminer->job)-1] = 0;

    printf("请输入职级> ");
    scanf("%d",&adminer->level);
    getchar();
    
    // printf("\n请输入权限> ");
    // scanf("%d",&adminer->privilege);
    // getchar();
}

/**
 * @name: server_admin_delete_UI
 * @function: 删除管理员信息界面
 * @param {sqlite3} * db : 数据库句柄
 * @param {staff*} : staff对象
 * @return {*}
 */
void server_admin_delete_UI(sqlite3* db,staff* adminer)
{
    putchar(10);
    // printf("————————————————————————————\n");
    // printf("*****员工管理系统服务器*****\n");
    printf("————————————————————————————\n");
    printf(">>>>>>>>>删除管理员<<<<<<<<<\n");
    printf("————————————————————————————\n");
    printf("请输入想要删除的管理员工号> ");
    scanf("%d",&adminer->jobid);
    getchar();
}

/**
 * @name: insert_admin
 * @function: 增加管理员到表staff
 * @param {sqlite3} * db : 数据库句柄
 * @param {staff} * staff : staff对象
 * @return {*}
 */
int insert_admin(sqlite3* db, staff* staff)
{
    char sql[SQLLEN] = "";
    sprintf(sql,"insert into staff values(\"%d\",\"%s\",\"%s\",\"%s\",\"%d\",\"%s\",\"%f\",\"%s\",\"%s\",\"%d\",\"%d\");",staff->jobid,staff->password,staff->name,staff->sex,staff->age,staff->address,staff->salary,staff->department,staff->job,staff->level,0);
	char *errmsg = NULL;
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK)
	{
        printf("添加管理员失败！请重新添加！\n");
		SQL_LOG(errmsg);
        HANG_LOG();
		return -1;
	}
	return 0;
}

/**
 * @name: delete_admin
 * @function: 根据主键jobid删除管理员
 * @param {sqlite3} * db : 数据库句柄
 * @param {staff} * staff : staff对象
 */
int delete_admin(sqlite3* db, staff* staff)
{
    char sql[SQLLEN] = "";
    sprintf(sql,"delete from staff where job_id = \"%d\";",staff->jobid);
	char *errmsg = NULL;
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK)
	{
        printf("删除管理员失败！请查看并重新输入正确的工号！\n");
		SQL_LOG(errmsg);
        HANG_LOG();
		return -1;
	}
	return 0;   
}

/**
 * @name: isalready_onrecord
 * @function: 查询是否已打上班卡
 * @param {*}
 * @return {*} 0：未打卡 
 */
int isalready_onrecord(sqlite3* db,record* record)
{
    char sql[SQLLEN] = "";
    sprintf(sql,"select * from record where date=\"%s\" and job_id=\"%d\" and state=\"%d\";",today,record->jobid,0);
	char** result;
	int row,column;
	char* errmsg = NULL;
	if(sqlite3_get_table(db,sql,&result,&row,&column,&errmsg) != SQLITE_OK)
	{
		SQL_LOG(errmsg);
		return -1;
	}
    // printf("isalready_onrecord row = %d",row);
    return row;
}

/**
 * @name: isalready_offrecord
 * @function: 查询是否已打下班卡
 * @param {*}
 * @return {*} 1：未打卡 
 */
int isalready_offrecord(sqlite3* db,record* record)
{
    char sql[SQLLEN] = "";
    sprintf(sql,"select * from record where date=\"%s\" and offline=\"%s\" and job_id=\"%d\";",today,"[]",record->jobid);
	char** result;
	int row,column;
	char* errmsg = NULL;
	if(sqlite3_get_table(db,sql,&result,&row,&column,&errmsg) != SQLITE_OK)
	{
		SQL_LOG(errmsg);
		return -1;
	}
    // printf("isalready_offrecord row = %d",row);
    return row;
}

/**
 * @name: insert_record_online
 * @function: 添加上班记录
 * @param {*}
 * @return {*}
 */
int insert_record_online(sqlite3* db, record* record)
{
    char sql[SQLLEN] = "";
    sprintf(sql,"insert into record values(\"%d\",\"%s\",\"%s\",\"%s\",\"%d\");",record->jobid,record->date,record->online,record->offline,record->state);
	char *errmsg = NULL;
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK)
	{
        //printf("添加上班记录失败！请重新添加！\n");
		SQL_LOG(errmsg);
		return -1;
	}
	return 0;  
}
/**
 * @name: insert_record_offline
 * @function: 添加下班记录
 * @param {*}
 * @return {*}
 */
int insert_record_offline(sqlite3* db, record* record)
{
    char sql[SQLLEN] = "";
    sprintf(sql,"update record set offline=\"%s\", state=\"%d\" where job_id=\"%d\";",record->offline,0,record->jobid);
	char *errmsg = NULL;
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK)
	{
        //printf("添加下班记录失败！请重新添加！\n");
		SQL_LOG(errmsg);
		return -1;
	}
	return 0;  
}

/**
 * @name: select_admin_all
 * @function: 显示所有的管理员信息
 * @param {sqlite3} * db : 数据库句柄
 * @return {*}
 */
int select_admin_all(sqlite3* db)
{
    char sql[SQLLEN] = "";
    sprintf(sql,"select * from staff where privilege=\"%d\";",0);
	char** result;
	int row,column;
	char* errmsg = NULL;
	if(sqlite3_get_table(db,sql,&result,&row,&column,&errmsg) != SQLITE_OK)
	{
		SQL_LOG(errmsg);
		return -1;
	}
	int i=0,j=0,k=0;
    char *item[STAFF_ITEM];
    printf("————————————————————————————\n");
    printf("        管理员信息表:\n");
    printf("****************************\n");
    for(i = 0; i <= row; i++)//行 = 字符段1行 + 记录段row行
    {
        for(j = 0; j < column; j++)//列
        {
            if(i == 0)
                item[k] = ch_result_staff[k];
            else
                printf("\t%s: %s\n",item[j],result[k]);
            k++;
        }
        if(i > 0 && i < row)
            printf("****************************\n");
    }
    printf("____________________________\n");
	return row;    
}

/**
 * @name: select_jobid_password
 * @function: 查询工号密码是否正确
 * @param {sqlite3} * db : 数据库句柄
 * @return {*}
 */
int select_jobid_password(sqlite3* db, int jobid, char * password, int whoami)
{
    char sql[SQLLEN] = "";
    if(whoami == I_AM_ADMIN)
        sprintf(sql,"select * from staff where job_id = \"%d\" and password = \"%s\" and privilege = \"%d\";",jobid,password,0);
	else 
        sprintf(sql,"select * from staff where job_id = \"%d\" and password = \"%s\" and privilege = \"%d\";",jobid,password,1);
    char** result;
	int row,column;
	char* errmsg = NULL;
	if(sqlite3_get_table(db,sql,&result,&row,&column,&errmsg) != SQLITE_OK)
	{
		SQL_LOG(errmsg);
		return -1;
	}

	return row;     
}

/**
 * @name: insert_admin
 * @function: 增加员工到表staff
 * @param {sqlite3} * db : 数据库句柄
 * @param {staff} * staff : staff对象
 * @return {*}
 */
int insert_staff(sqlite3* db, staff* staff)
{
    char sql[SQLLEN] = "";
    sprintf(sql,"insert into staff values(\"%d\",\"%s\",\"%s\",\"%s\",\"%d\",\"%s\",\"%f\",\"%s\",\"%s\",\"%d\",\"%d\");",staff->jobid,staff->password,staff->name,staff->sex,staff->age,staff->address,staff->salary,staff->department,staff->job,staff->level,1);
	char *errmsg = NULL;
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK)
	{
        //printf("添加员工失败！请重新添加！\n");
		SQL_LOG(errmsg);
		return -1;
	}
	return 0;
}

/**
 * @name: delete_staff
 * @function: 从表staff中删除员工
 * @param {sqlite3} * db : 数据库句柄
 * @param {staff} * staff : staff对象
 * @return {*}
 */
int delete_staff(sqlite3* db, staff* staff)
{
    char sql[SQLLEN] = "";
    sprintf(sql,"delete from staff where job_id = \"%d\";",staff->jobid);
	char *errmsg = NULL;
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK)
	{
        //printf("删除员工失败！请查看并重新输入正确的工号！\n");
		SQL_LOG(errmsg);
		return -1;
	}
	return 0;
}

/**
 * @name: change_staff
 * @function: 修改表staff中员工
 * @param {sqlite3} * db : 数据库句柄
 * @param {staff} * staff : staff对象
 * @return {*}
 */
int change_staff(sqlite3* db, staff* staff)
{
    char sql[SQLLEN] = "";
    sprintf(sql,"update staff set password=\"%s\", age=\"%d\", address=\"%s\", salary=\"%f\", department=\"%s\", job=\"%s\", level=\"%d\" where job_id = \"%d\";",staff->password,staff->age,staff->address,staff->salary,staff->department,staff->job,staff->level,staff->jobid);
	char *errmsg = NULL;
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK)
	{
        //printf("修改员工信息失败！\n");
		SQL_LOG(errmsg);
		return -1;
	}
	return 0;
}

/**
 * @name: change_password
 * @function: 修改表staff中员工
 * @param {sqlite3} * db : 数据库句柄
 * @param {staff} * staff : staff对象
 * @return {*}
 */
int change_password(sqlite3* db, staff* staff)
{
    char sql[SQLLEN] = "";
    sprintf(sql,"update staff set password=\"%s\" where job_id = \"%d\";",staff->password,staff->jobid);
	char *errmsg = NULL;
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK)
	{
        //printf("修改信息失败！\n");
		SQL_LOG(errmsg);
		return -1;
	}
	return 0;
}

/**
 * @name: change_age
 * @function: 修改表staff中员工
 * @param {sqlite3} * db : 数据库句柄
 * @param {staff} * staff : staff对象
 * @return {*}
 */
int change_age(sqlite3* db, staff* staff)
{
    char sql[SQLLEN] = "";
    sprintf(sql,"update staff set age=\"%d\" where job_id = \"%d\";",staff->age,staff->jobid);
	char *errmsg = NULL;
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK)
	{
        printf("修改信息失败！\n");
		SQL_LOG(errmsg);
		return -1;
	}
	return 0;
}

/**
 * @name: change_address
 * @function: 修改表staff中员工
 * @param {sqlite3} * db : 数据库句柄
 * @param {staff} * staff : staff对象
 * @return {*}
 */
int change_address(sqlite3* db, staff* staff)
{
    char sql[SQLLEN] = "";
    sprintf(sql,"update staff set address=\"%s\" where job_id = \"%d\";",staff->address,staff->jobid);
	char *errmsg = NULL;
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK)
	{
        //printf("修改员工信息失败！\n");
		SQL_LOG(errmsg);
		return -1;
	}
	return 0;
}

/**
 * @name: change_salary
 * @function: 修改表staff中员工
 * @param {sqlite3} * db : 数据库句柄
 * @param {staff} * staff : staff对象
 * @return {*}
 */
int change_salary(sqlite3* db, staff* staff)
{
    char sql[SQLLEN] = "";
    sprintf(sql,"update staff set salary=\"%f\" where job_id = \"%d\";",staff->salary,staff->jobid);
	char *errmsg = NULL;
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK)
	{
        //printf("修改员工信息失败！\n");
		SQL_LOG(errmsg);
		return -1;
	}
	return 0;
}

/**
 * @name: change_department
 * @function: 修改表staff中员工
 * @param {sqlite3} * db : 数据库句柄
 * @param {staff} * staff : staff对象
 * @return {*}
 */
int change_department(sqlite3* db, staff* staff)
{
    char sql[SQLLEN] = "";
    sprintf(sql,"update staff set department=\"%s\" where job_id = \"%d\";",staff->department,staff->jobid);
	char *errmsg = NULL;
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK)
	{
        //printf("修改员工信息失败！\n");
		SQL_LOG(errmsg);
		return -1;
	}
	return 0;
}

/**
 * @name: change_job
 * @function: 修改表staff中员工
 * @param {sqlite3} * db : 数据库句柄
 * @param {staff} * staff : staff对象
 * @return {*}
 */
int change_job(sqlite3* db, staff* staff)
{
    char sql[SQLLEN] = "";
    sprintf(sql,"update staff set job=\"%s\" where job_id = \"%d\";",staff->job,staff->jobid);
	char *errmsg = NULL;
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK)
	{
        //printf("修改员工信息失败！\n");
		SQL_LOG(errmsg);
		return -1;
	}
	return 0;
}

/**
 * @name: change_level
 * @function: 修改表staff中员工
 * @param {sqlite3} * db : 数据库句柄
 * @param {staff} * staff : staff对象
 * @return {*}
 */
int change_level(sqlite3* db, staff* staff)
{
    char sql[SQLLEN] = "";
    sprintf(sql,"update staff set level=\"%d\" where job_id = \"%d\";",staff->level,staff->jobid);
	char *errmsg = NULL;
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK)
	{
        //printf("修改员工信息失败！\n");
		SQL_LOG(errmsg);
		return -1;
	}
	return 0;
}

/**
 * @name: demand_one_staff
 * @function: 查询某位员工信息
 * @param {sqlite3} * db : 数据库句柄
 * @return {*}
 */
int demand_one_staff(sqlite3* db, data* databuf)
{
    char sql[SQLLEN] = "";
    sprintf(sql,"select * from staff where job_id=\"%d\";",databuf->staff.jobid);
	char** result;
	int row,column;
	char* errmsg = NULL;
	if(sqlite3_get_table(db,sql,&result,&row,&column,&errmsg) != SQLITE_OK)
	{
		SQL_LOG(errmsg);
		return -1;
	}

	int i=0,j=0,k=0,l=0;
    char *item[STAFF_ITEM];
    char data[DATA_LEN];
    // printf("____________________________\n");
    // printf("          员工信息表:\n");
    // printf("****************************\n");
    bzero(databuf->data,DATE_LEN);
    for(i = 0; i <= row; i++)//行 = 字符段1行 + 记录段row行
    {
        for(j = 0; j < column; j++)//列
        {
            if(i == 0)
                item[k] = ch_result_staff[k];
            else
            {              
                // printf("\t%s: %s\n",item[j],result[k]);
                
                sprintf(data,"\t%s: %s\n",item[j],result[k]);
                strcat(databuf->data,data);
                
                // printf("%s",databuf->data);
                l++;
            }
            k++;          
        }
        if(i > 0 && i < row)
        {
            // printf("****************************\n");
            sprintf(data,"****************************\n");
            strcat(databuf->data,data);
        }
    }
    // printf("____________________________\n");
	return row;   
}

/**
 * @name: demand_all_staff
 * @function: 查询所有员工信息
 * @param {sqlite3} * db : 数据库句柄
 * @return {*}
 */
int demand_all_staff(sqlite3* db, data* databuf)
{
    char sql[SQLLEN] = "";
    sprintf(sql,"select * from staff;");
	char** result;
	int row,column;
	char* errmsg = NULL;
	if(sqlite3_get_table(db,sql,&result,&row,&column,&errmsg) != SQLITE_OK)
	{
		SQL_LOG(errmsg);
		return -1;
	}

	int i=0,j=0,k=0,l=0;
    char *item[STAFF_ITEM];
    char data[DATA_LEN];
    // printf("____________________________\n");
    // printf("          员工信息表:\n");
    // printf("****************************\n");
    bzero(databuf->data,DATE_LEN);
    for(i = 0; i <= row; i++)//行 = 字符段1行 + 记录段row行
    {
        for(j = 0; j < column; j++)//列
        {
            if(i == 0)
                item[k] = ch_result_staff[k];
            else
            {             
                // printf("\t%s: %s\n",item[j],result[k]);
                
                sprintf(data,"\t%s: %s\n",item[j],result[k]);
                strcat(databuf->data,data);
                
                // printf("%s",databuf->data);
                l++;
            }
            k++;          
        }
        if(i > 0 && i < row)
        {
            // printf("****************************\n");
            sprintf(data,"****************************\n");
            strcat(databuf->data,data);
        }
    }
    // printf("____________________________\n");
	return row;    
}

/**
 * @name: demand_one_staff_record
 * @function: 查询某位员工考情记录信息
 * @param {sqlite3} * db : 数据库句柄
 * @return {*}
 */
int demand_one_staff_record(sqlite3* db, data* databuf)
{
    char sql[SQLLEN] = "";
    sprintf(sql,"select * from record where job_id=\"%d\";",databuf->staff.jobid);
	char** result;
	int row,column;
	char* errmsg = NULL;
	if(sqlite3_get_table(db,sql,&result,&row,&column,&errmsg) != SQLITE_OK)
	{
		SQL_LOG(errmsg);
		return -1;
	}

	int i=0,j=0,k=0,l=0;
    char *item[STAFF_ITEM];
    char data[DATA_LEN];
    // printf("____________________________\n");
    // printf("          考情记录表:\n");
    // printf("****************************\n");
    bzero(databuf->data,DATE_LEN);
    for(i = 0; i <= row; i++)//行 = 字符段1行 + 记录段row行
    {
        for(j = 0; j < column; j++)//列
        {
            if(i == 0)
                item[k] = ch_result_record[k];
            else
            {              
                // printf("\t%s: %s\n",item[j],result[k]);
                
                sprintf(data,"\t%s: %s\n",item[j],result[k]);
                strcat(databuf->data,data);
                
                // printf("%s",databuf->data);
                l++;
            }
            k++;          
        }
        if(i > 0 && i < row)
        {
            // printf("****************************\n");
            sprintf(data,"****************************\n");
            strcat(databuf->data,data);
        }
    }
    // printf("____________________________\n");
	return row;   
}

/**
 * @name: demand_all_staff_record
 * @function: 查询所有考情记录信息
 * @param {sqlite3} * db : 数据库句柄
 * @return {*}
 */
int demand_all_staff_record(sqlite3* db, data* databuf)
{
    char sql[SQLLEN] = "";
    sprintf(sql,"select * from record;");
	char** result;
	int row,column;
	char* errmsg = NULL;
	if(sqlite3_get_table(db,sql,&result,&row,&column,&errmsg) != SQLITE_OK)
	{
		SQL_LOG(errmsg);
		return -1;
	}

	int i=0,j=0,k=0,l=0;
    char *item[STAFF_ITEM];
    char data[DATA_LEN];
    // printf("____________________________\n");
    // printf("          考情记录表:\n");
    // printf("****************************\n");
    bzero(databuf->data,DATE_LEN);
    for(i = 0; i <= row; i++)//行 = 字符段1行 + 记录段row行
    {
        for(j = 0; j < column; j++)//列
        {
            if(i == 0)
                item[k] = ch_result_record[k];
            else
            {               
                // printf("\t%s: %s\n",item[j],result[k]);
                
                sprintf(data,"\t%s: %s\n",item[j],result[k]);
                strcat(databuf->data,data);
                
                // printf("%s",databuf->data);
                l++;
            }
            k++;          
        }
        if(i > 0 && i < row)
        {
            // printf("****************************\n");
            sprintf(data,"****************************\n");
            strcat(databuf->data,data);
        }
    }
    // printf("____________________________\n");
	return row;    
}


/**
 * @name: get_time
 * @function: 获取当前日期时间
 * @param {*}
 * @return {*}
 */
char* get_time(char* date)
{
    time_t t;
    time(&t); 
    struct tm *tm = localtime(&t);
    strftime(date,DATE_LEN,"[%H:%M:%S]",tm); 
    return date;
}


/**
 * @name: get_date
 * @function: 获取当前日期
 * @param {*}
 * @return {*}
 */
char* get_date(char* date)
{
    time_t t;
    time(&t); 
    struct tm *tm = localtime(&t);
    strftime(date,DATE_LEN,"[%Y-%m-%d]",tm); 
    return date;
}

