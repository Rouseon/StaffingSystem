/*
 * @Descripttion: 
 * @version: 1.1
 * @Author: Lou Sheng
 * @Date: 2021-06-26 13:19:42
 * @LastEditors: Lou Sheng
 * @LastEditTime: 2021-06-29 19:56:07
 */
#include "client.h"

static int input_jobid;

typedef void (*sighandler)(int);
void handler_quit(int sig)
{
    data databuf;
    bzero(databuf.cmd,CMD_LEN);
    strcpy(databuf.cmd,COERCE_QUIT); 
    if((send(sfd, &databuf, sizeof(data), 0)) < 0)
    {
        ERR_LOG("send");
        return;
    }
    putchar(10);
    close(sfd);
    exit(0);
}
/**
 * @name: user_select_UI
 * @function: 选择用户界面
 * @param {*}
 * @return {*} 1：管理员 2：员工
 */
int user_select_UI(void)
{
    system("clear");
    int whoami = 0;
    printf("————————————————————————————\n");
    printf("********员工管理系统********\n");
    printf("————————————————————————————\n");
    printf(">>>>>>>>>>用户选择<<<<<<<<<<\n");
    printf("————————————————————————————\n");
    printf("1.我是管理员\n");
    printf("2.我是员工\n"); 
    printf("请输入选项> ");
    scanf("%d",&whoami);
    getchar();
    return whoami;
}

/**
 * @name: admin_UI
 * @function: 管理员登陆UI
 * @param {*}
 * @return {*}
 */
void admin_UI(void)
{
    system("clear");
    printf("————————————————————————————\n");
    printf("********员工管理系统********\n");
    printf("————————————————————————————\n");
    printf(">>>>>>>>>管理员登陆<<<<<<<<<\n");
    printf("————————————————————————————\n");
}

/**
 * @name: staff_UI
 * @function: 员工登陆
 * @param {*}
 * @return {*}
 */
void staff_UI(void)
{
    system("clear");
    printf("————————————————————————————\n");
    printf("********员工管理系统********\n");
    printf("————————————————————————————\n");
    printf(">>>>>>>>>>员工登陆<<<<<<<<<<\n");
    printf("————————————————————————————\n");
}

/**
 * @name: login_UI
 * @function: 登录界面
 * @param {data*} data协议对象
 * @param {int} sfd
 * @param {int} whoami 管理员 or 员工 选择
 * @return {*}
 */
int login_UI(data * databuf,int sfd,int whoami)
{
    switch (whoami)
    {
    case I_AM_ADMIN:
        admin_UI();
        databuf->staff.privilege = 0;
        break;
    case I_AM_STAFF:
        staff_UI();
        databuf->staff.privilege = 1;
        break;
    default:
        ERR_LOG("参数3错误!");  
        return -1;
    }

    databuf->staff.jobid = 0;
    printf("请输入工号> ");
    scanf("%d",&databuf->staff.jobid);
    getchar();
    input_jobid = databuf->staff.jobid;
    
    bzero(databuf->staff.password, PASSWORD_LEN);
    printf("请输入密码> ");
    fgets(databuf->staff.password, PASSWORD_LEN, stdin);
    databuf->staff.password[strlen(databuf->staff.password)-1] = 0;

    bzero(databuf->cmd,CMD_LEN);
    strcpy(databuf->cmd,CMD_APPLY_LOGIN);  

    databuf->record.state = 0;

    if((send(sfd, databuf, sizeof(data), 0)) < 0)
    {
        ERR_LOG("send");
        return -1;
    }
	int recv_len;
    static int login_count = 3;
	while(1)
	{
        // printf("login_count = %d\n",login_count);
        if(login_count > 1)
        {
            bzero(databuf->cmd, CMD_LEN);
            recv_len = recv(sfd, databuf, sizeof(data), 0);
            if(recv_len < 0)
            {
                ERR_LOG("recv");
                return -1;
            }
            else if(recv_len == 0)
            {
                printf("服务器离线 %d\n",__LINE__);
                close(sfd);
                exit(1);
            }
            else
            {
                //不允许登录
                if(strcmp(databuf->cmd, CMD_DONOT_LOGIN) == 0)
                {
                    printf("工号或密码错误，请重新输入！（按回车继续）\n");
                    while(getchar()!=10);
                    login_count--;
                    return -1;
                }
                else if(strcmp(databuf->cmd, CMD_ALLOW_LOGIN) == 0)
                {
                    //允许登录
                    printf("登录成功！\n");
                    return 0;
                }
            }
        }
        else
        {
            printf("输入次数过多，工号被锁定，请稍后再试!\n");
            bzero(databuf->cmd,CMD_LEN);
            strcpy(databuf->cmd,COERCE_QUIT); 
            if((send(sfd, databuf, sizeof(data), 0)) < 0)
            {
                ERR_LOG("send");
                return;
            }
            close(sfd);
            exit(1);
            // return -1;
        }
	}
    return 0;
}

/**
 * @name: admin_work_UI
 * @function: 管理员 增、删、改、查
 *            客户端无创建管理员的权限，管理员的创建由服务器完成
 * @param {data} *
 * @param {int} sfd
 * @return {*}
 */
void admin_work_UI(data * databuf,int sfd)
{
    int recv_len;
    while (1)
    {
        system("clear");
        int item;
        printf("————————————————————————————\n");
        printf("********员工管理系统********\n");
        printf("————————————————————————————\n");
        printf(">>>>>>>>管理员工作台<<<<<<<<\n");
        printf("————————————————————————————\n");
        printf("1.增加员工信息\n");
        printf("2.删除员工信息\n"); 
        printf("3.修改员工信息\n");
        printf("4.查询员工信息\n");
        printf("5.查询员工考勤\n"); 
        printf("6.退出管理系统\n");
        printf("请输入选项> ");
        scanf("%d",&item);
        getchar();   

        switch (item)
        {
        case INSERT_STAFF:
            insert_staff(&databuf->staff);

            bzero(databuf->cmd,CMD_LEN);
            strcpy(databuf->cmd,CMD_INSERT_STAFF); 
            if((send(sfd, databuf, sizeof(data), 0)) < 0)
            {
                ERR_LOG("send");
                return;
            }
            
            bzero(databuf->cmd, CMD_LEN);
            recv_len = recv(sfd, databuf, sizeof(data), 0);
            if(recv_len < 0)
            {
                ERR_LOG("recv");
                return;
            }
            else if(recv_len == 0)
            {
                printf("服务器离线 %d\n",__LINE__);
                close(sfd);
                exit(1);
            }
            else
            {
                //添加员工成功
                if(strcmp(databuf->cmd,CMD_INSERT_OK) == 0)
                {
                    printf("添加员工成功！\n");
                    HANG_LOG();
                }
                //添加员工失败
                else if(strcmp(databuf->cmd,CMD_INSERT_FAIL) == 0)
                {
                    printf("添加员工失败！\n");
                    HANG_LOG();
                }
            }           
            break;
        case DELETE_STAFF:
            delete_staff(&databuf->staff);

            bzero(databuf->cmd,CMD_LEN);
            strcpy(databuf->cmd,CMD_DELETE_STAFF); 
            if((send(sfd, databuf, sizeof(data), 0)) < 0)
            {
                ERR_LOG("send");
                return;
            }
            
            bzero(databuf->cmd, CMD_LEN);
            recv_len = recv(sfd, databuf, sizeof(data), 0);
            if(recv_len < 0)
            {
                ERR_LOG("recv");
                return;
            }
            else if(recv_len == 0)
            {
                printf("服务器离线 %d\n",__LINE__);
                close(sfd);
                exit(1);
            }
            else
            {
                //删除员工成功
                if(strcmp(databuf->cmd,CMD_DELETE_OK) == 0)
                {
                    printf("删除员工成功！\n");
                    HANG_LOG();
                }
                //删除员工失败
                else if(strcmp(databuf->cmd,CMD_DELETE_FAIL) == 0)
                {
                    printf("删除员工失败！\n");
                    HANG_LOG();
                }
            }           
            break;
        case CHANGE_STAFF:
            change_staff(databuf,sfd);         
            break;
        case DEMAND_STAFF:
            bzero(databuf->cmd,CMD_LEN);
            demand_staff(databuf);

            if((send(sfd, databuf, sizeof(data), 0)) < 0)
            {
                ERR_LOG("send");
                return;
            }
            
            bzero(databuf->cmd, CMD_LEN);
            recv_len = recv(sfd, databuf, sizeof(data), 0);
            if(recv_len < 0)
            {
                ERR_LOG("recv");
                return;
            }
            else if(recv_len == 0)
            {
                printf("服务器离线 %d\n",__LINE__);
                close(sfd);
                exit(1);
            }
            else
            {
                //查询员工成功
                if(strcmp(databuf->cmd,CMD_DEMAND_ONE_OK) == 0)
                {
                    printf("查询员工成功！\n");
                    //显示某位员工
                    display_staff(databuf);
                }
                else if(strcmp(databuf->cmd,CMD_DEMAND_ALL_OK) == 0)
                {
                    printf("查询全部员工成功！\n");
                    //显示全部员工
                    display_staff(databuf);              
                }
                //查询员工失败
                else if(strcmp(databuf->cmd,CMD_DEMAND_ONE_FAIL) == 0 || strcmp(databuf->cmd,CMD_DEMAND_ALL_FAIL) == 0)
                {
                    printf("查询员工失败！系统中无员工，请添加后查询！\n");
                    HANG_LOG();
                }                
            }            
            break;    
        case DEMAND_RECORD:
            bzero(databuf->cmd,CMD_LEN);
            demand_record(databuf);

            if((send(sfd, databuf, sizeof(data), 0)) < 0)
            {
                ERR_LOG("send");
                return;
            }
            
            bzero(databuf->cmd, CMD_LEN);
            recv_len = recv(sfd, databuf, sizeof(data), 0);
            if(recv_len < 0)
            {
                ERR_LOG("recv");
                return;
            }
            else if(recv_len == 0)
            {
                printf("服务器离线 %d\n",__LINE__);
                close(sfd);
                exit(1);
            }
            else
            {
                //查询考勤记录成功
                if(strcmp(databuf->cmd,CMD_DEMAND_ONE_OK_RECORD) == 0)
                {
                    printf("查询员工考勤记录成功！\n");
                    //显示某位员工考勤记录
                    display_record(databuf);
                }
                else if(strcmp(databuf->cmd,CMD_DEMAND_ALL_OK_RECORD) == 0)
                {
                    printf("查询全部员工考勤记录成功！\n");
                    //显示全部考勤记录
                    display_record(databuf);                   
                }
                //查询员工失败
                else if(strcmp(databuf->cmd,CMD_DEMAND_ONE_FAIL_RECORD) == 0 || strcmp(databuf->cmd,CMD_DEMAND_ALL_FAIL_RECORD) == 0)
                {
                    printf("查询考勤记录失败！系统中无员工，请添加后查询！\n");
                    HANG_LOG();
                }                
            }             
            break; 
        case QUIT:
            quit(&databuf->staff);

            bzero(databuf->cmd,CMD_LEN);
            strcpy(databuf->cmd,CMD_QUIT); 
            if((send(sfd, databuf, sizeof(data), 0)) < 0)
            {
                ERR_LOG("send");
                return;
            }
            
            bzero(databuf->cmd, CMD_LEN);
            recv_len = recv(sfd, databuf, sizeof(data), 0);
            if(recv_len < 0)
            {
                ERR_LOG("recv");
                return;
            }
            else if(recv_len == 0)
            {
                printf("服务器离线 %d\n",__LINE__);
                close(sfd);
                exit(1);
            }
            else
            {
                //下班打卡成功
                if(strcmp(databuf->cmd,CMD_QUIT_OK) == 0)
                {
                    printf("下班打卡成功！\n");
                    HANG_LOG();
                }
                //下班打卡失败
                else if(strcmp(databuf->cmd,CMD_QUIT_FAIL) == 0)
                {
                    printf("下班打卡失败！\n");
                    HANG_LOG();
                }
                else
                {
                    printf("已经打过下班卡了！\n");
                    HANG_LOG();
                }
            } 
            close(sfd);
            exit(0);
            break; 
        default:
            INPUT_LOG();
        }
    }
}

/**
 * @name: staff_work_UI
 * @function: 员工 改（自己）、查
 *            员工管理系统不是面向大众的，员工没有自己创建工号的权限，员工的创建由管理员完成
 * @param {data} *
 * @param {int} sfd
 * @return {*}
 */
void staff_work_UI(data * databuf,int sfd)
{
    system("clear");
    int recv_len;
    while (1)
    {
        system("clear");
        int item;
        printf("————————————————————————————\n");
        printf("********员工管理系统********\n");
        printf("————————————————————————————\n");
        printf(">>>>>>>>员工工作台<<<<<<<<<<\n");
        printf("————————————————————————————\n");
        printf("1.查看基本信息\n");
        printf("2.修改基本信息\n"); 
        printf("3.查看考勤信息\n");
        printf("4.退出管理系统\n");
        printf("请输入选项> ");
        scanf("%d",&item);
        getchar();   

        switch (item)
        {
        case STAFF_DEMAND:
            databuf->staff.jobid = input_jobid;
            bzero(databuf->cmd,CMD_LEN);
            strcpy(databuf->cmd,CMD_DEMAND_ONE_STAFF);

            if((send(sfd, databuf, sizeof(data), 0)) < 0)
            {
                ERR_LOG("send");
                return;
            }
            
            bzero(databuf->cmd, CMD_LEN);
            recv_len = recv(sfd, databuf, sizeof(data), 0);
            if(recv_len < 0)
            {
                ERR_LOG("recv");
                return;
            }
            else if(recv_len == 0)
            {
                printf("服务器离线 %d\n",__LINE__);
                close(sfd);
                exit(1);
            }
            else
            {
                //查询员工成功
                if(strcmp(databuf->cmd,CMD_DEMAND_ONE_OK) == 0)
                {
                    printf("查询员工成功！\n");
                    //显示某位员工
                    display_staff(databuf);
                }
                //查询员工失败
                else if(strcmp(databuf->cmd,CMD_DEMAND_ONE_FAIL) == 0 || strcmp(databuf->cmd,CMD_DEMAND_ALL_FAIL) == 0)
                {
                    printf("查询失败！\n");
                    HANG_LOG();
                }                
            }            
            break;    
        case STAFF_CHANGE:
            change_self(databuf,sfd);          
            break;

        case RECORD_DEMAND:
            databuf->staff.jobid = input_jobid;
            bzero(databuf->cmd,CMD_LEN);
            strcpy(databuf->cmd,CMD_DEMAND_ONE_STAFF_RECORD);

            if((send(sfd, databuf, sizeof(data), 0)) < 0)
            {
                ERR_LOG("send");
                return;
            }
            
            bzero(databuf->cmd, CMD_LEN);
            recv_len = recv(sfd, databuf, sizeof(data), 0);
            if(recv_len < 0)
            {
                ERR_LOG("recv");
                return;
            }
            else if(recv_len == 0)
            {
                printf("服务器离线 %d\n",__LINE__);
                close(sfd);
                exit(1);
            }
            else
            {
                //查询考勤记录成功
                if(strcmp(databuf->cmd,CMD_DEMAND_ONE_OK_RECORD) == 0)
                {
                    printf("查询考勤记录成功！\n");
                    //显示某位员工考勤记录
                    display_record(databuf);
                }
                //查询员工失败
                else if(strcmp(databuf->cmd,CMD_DEMAND_ONE_FAIL_RECORD) == 0)
                {
                    printf("查询考勤记录失败！\n");
                    HANG_LOG();
                }                
            }             
            break; 
        case STAFF_QUIT:
            quit(&databuf->staff);

            bzero(databuf->cmd,CMD_LEN);
            strcpy(databuf->cmd,CMD_QUIT); 
            if((send(sfd, databuf, sizeof(data), 0)) < 0)
            {
                ERR_LOG("send");
                return;
            }
            
            bzero(databuf->cmd, CMD_LEN);
            recv_len = recv(sfd, databuf, sizeof(data), 0);
            if(recv_len < 0)
            {
                ERR_LOG("recv");
                return;
            }
            else if(recv_len == 0)
            {
                printf("服务器离线 %d\n",__LINE__);
                close(sfd);
                exit(1);
            }
            else
            {
                //下班打卡成功
                if(strcmp(databuf->cmd,CMD_QUIT_OK) == 0)
                {
                    printf("下班打卡成功！\n");
                    HANG_LOG();
                }
                //下班打卡失败
                else if(strcmp(databuf->cmd,CMD_QUIT_FAIL) == 0)
                {
                    printf("下班打卡失败！\n");
                    HANG_LOG();
                }
                else
                {
                    printf("已经打过下班卡了！\n");
                    HANG_LOG();
                }
            } 
            close(sfd);
            exit(0);
            break; 
        default:
            INPUT_LOG();
        }
    }    
}

/**
 * @name:insert_staff 
 * @function: 
 * @param {*}
 * @return {*}
 */
void insert_staff(staff * staff)
{
    printf("————————————————————————————\n");
    printf("请输入工号> ");
    scanf("%d",&staff->jobid);
    getchar();
    
    bzero(staff->password,PASSWORD_LEN);
    printf("请输入密码> ");
    fgets(staff->password, PASSWORD_LEN, stdin);
    staff->password[strlen(staff->password)-1] = 0;

    bzero(staff->name,NAME_LEN);
    printf("请输入姓名> ");
    fgets(staff->name, NAME_LEN, stdin);
    staff->name[strlen(staff->name)-1] = 0;

    bzero(staff->sex, SEX_LEN);
    printf("请输入性别> ");
    fgets(staff->sex, SEX_LEN, stdin);
    staff->sex[strlen(staff->sex)-1] = 0;

    printf("请输入年龄> ");
    scanf("%d",&staff->age);
    getchar();

    bzero(staff->address,ADDRESS_LEN);
    printf("请输入住址> ");
    fgets(staff->address, ADDRESS_LEN, stdin);
    staff->address[strlen(staff->address)-1] = 0;

    printf("请输入薪资> ");
    scanf("%f",&staff->salary);
    getchar();

    bzero(staff->department,DEPARTMENT_LEN);
    printf("请输入部门> ");
    fgets(staff->department, DEPARTMENT_LEN, stdin);
    staff->department[strlen(staff->department)-1] = 0;

    bzero(staff->job,JOB_LEN);
    printf("请输入岗位> ");
    fgets(staff->job, JOB_LEN, stdin);
    staff->job[strlen(staff->job)-1] = 0;

    printf("请输入职级> ");
    scanf("%d",&staff->level);
    getchar();
    
    // printf("\n请输入权限> ");
    // scanf("%d",&staff->privilege);
    // getchar();
}

/**
 * @name: delete_staff
 * @function: 
 * @param {staff} *
 * @return {*}
 */
void delete_staff(staff * staff)
{
    printf("————————————————————————————\n");
    printf("请输入工号> ");
    scanf("%d",&staff->jobid);
    getchar();
}

/**
 * @name: change_staff
 * @function: 
 * @param {staff} *
 * @return {*}
 */
void change_staff(data * databuf, int sfd)
{
    int items;
    int finish = 0;
    printf("————————————————————————————\n");
    //先根据jobid获取要修改员工全部信息
    printf("请输入工号> ");
    scanf("%d",&databuf->staff.jobid);
    getchar();
    while(1)
    {
        bzero(databuf->cmd,CMD_LEN);
        strcpy(databuf->cmd,CMD_DEMAND_ONE_STAFF);
        if((send(sfd, databuf, sizeof(data), 0)) < 0)
        {
            ERR_LOG("send");
            return;
        }
        
        bzero(databuf->cmd, CMD_LEN);
        int recv_len = recv(sfd, databuf, sizeof(data), 0);
        if(recv_len < 0)
        {
            ERR_LOG("recv");
            return;
        }
        else if(recv_len == 0)
        {
            printf("服务器离线 %d\n",__LINE__);
            close(sfd);
            exit(1);
        }
        else
        {
            //查询员工成功
            if(strcmp(databuf->cmd,CMD_DEMAND_ONE_OK) == 0)
            {
                printf("该员工信息如下：\n");
                //显示某位员工
                // display_staff(databuf);
                system("clear");
                printf("————————————————————————————\n");
                printf("        员工信息表\n");
                printf("****************************\n");
                printf("%s",databuf->data);
                printf("____________________________\n");   
            }
            //查询员工失败
            else if(strcmp(databuf->cmd,CMD_DEMAND_ONE_FAIL) == 0)
            {
                printf("查询员工失败！系统中无员工，请添加！\n");
            }                
        }    
        
        printf("选项：\n");
        printf("    1.修改密码\n");
        printf("    2.修改年龄\n"); 
        printf("    3.修改住址\n");
        printf("    4.修改薪资\n");
        printf("    5.修改部门\n"); 
        printf("    6.修改岗位\n");
        printf("    7.修改职级\n");
        printf("    8.结束修改\n");
        printf("————————————————————————————\n");

        printf("请选择要修改的选项> ");
        scanf("%d",&items);
        getchar();
        switch (items)
        {
        case CHANGE_PASSWORD:
            bzero(databuf->staff.password,PASSWORD_LEN);
            printf("请输入密码> ");
            fgets(databuf->staff.password, PASSWORD_LEN, stdin);
            databuf->staff.password[strlen(databuf->staff.password)-1] = 0;  
            
            bzero(databuf->cmd,CMD_LEN);
            strcpy(databuf->cmd,CMD_CHANGE_PASSWD); 
            if((send(sfd, databuf, sizeof(data), 0)) < 0)
            {
                ERR_LOG("send");
                return;
            }            
            bzero(databuf->cmd, CMD_LEN);
            recv_len = recv(sfd, databuf, sizeof(data), 0);
            if(recv_len < 0)
            {
                ERR_LOG("recv");
                return;
            }
            else if(recv_len == 0)
            {
                printf("服务器离线 %d\n",__LINE__);
                close(sfd);
                exit(1);
            }
            else
            {
                //修改密码成功
                if(strcmp(databuf->cmd,CMD_CHANGE_PASSWD_OK) == 0)
                {
                    printf("修改密码成功！\n");
                    HANG_LOG();
                }
                //修改密码失败
                else if(strcmp(databuf->cmd,CMD_CHANGE_PASSWD_FAIL) == 0)
                {
                    printf("修改密码失败！\n");
                    HANG_LOG();
                }
            }   
            break;
        case CHANGE_AGE:
            printf("请输入年龄> ");
            scanf("%d",&databuf->staff.age);
            getchar();   

            bzero(databuf->cmd,CMD_LEN);
            strcpy(databuf->cmd,CMD_CHANGE_AGE); 
            if((send(sfd, databuf, sizeof(data), 0)) < 0)
            {
                ERR_LOG("send");
                return;
            }            
            bzero(databuf->cmd, CMD_LEN);
            recv_len = recv(sfd, databuf, sizeof(data), 0);
            if(recv_len < 0)
            {
                ERR_LOG("recv");
                return;
            }
            else if(recv_len == 0)
            {
                printf("服务器离线 %d\n",__LINE__);
                close(sfd);
                exit(1);
            }
            else
            {
                //修改年龄成功
                if(strcmp(databuf->cmd,CMD_CHANGE_AGE_OK) == 0)
                {
                    printf("修改年龄成功！\n");
                    HANG_LOG();
                }
                //修改年龄失败
                else if(strcmp(databuf->cmd,CMD_CHANGE_AGE_FAIL) == 0)
                {
                    printf("修改年龄失败！\n");
                    HANG_LOG();
                }
            }      
            break;
        case CHANGE_ADDRESS:
            bzero(databuf->staff.address,ADDRESS_LEN);
            printf("请输入住址> ");
            fgets(databuf->staff.address, ADDRESS_LEN, stdin);
            databuf->staff.address[strlen(databuf->staff.address)-1] = 0; 

             bzero(databuf->cmd,CMD_LEN);
            strcpy(databuf->cmd,CMD_CHANGE_ADDRES); 
            if((send(sfd, databuf, sizeof(data), 0)) < 0)
            {
                ERR_LOG("send");
                return;
            }            
            bzero(databuf->cmd, CMD_LEN);
            recv_len = recv(sfd, databuf, sizeof(data), 0);
            if(recv_len < 0)
            {
                ERR_LOG("recv");
                return;
            }
            else if(recv_len == 0)
            {
                printf("服务器离线 %d\n",__LINE__);
                close(sfd);
                exit(1);
            }
            else
            {
                //修改住址成功
                if(strcmp(databuf->cmd,CMD_CHANGE_ADDRES_OK) == 0)
                {
                    printf("修改住址成功！\n");
                    HANG_LOG();
                }
                //修改住址失败
                else if(strcmp(databuf->cmd,CMD_CHANGE_ADDRES_FAIL) == 0)
                {
                    printf("修改住址失败！\n");
                    HANG_LOG();
                }
            }                 
            break;
        case CHANGE_SALARY:
            printf("请输入薪资> ");
            scanf("%f",&databuf->staff.salary);
            getchar(); 

            bzero(databuf->cmd,CMD_LEN);
            strcpy(databuf->cmd,CMD_CHANGE_SALARY); 
            if((send(sfd, databuf, sizeof(data), 0)) < 0)
            {
                ERR_LOG("send");
                return;
            }            
            bzero(databuf->cmd, CMD_LEN);
            recv_len = recv(sfd, databuf, sizeof(data), 0);
            if(recv_len < 0)
            {
                ERR_LOG("recv");
                return;
            }
            else if(recv_len == 0)
            {
                printf("服务器离线 %d\n",__LINE__);
                close(sfd);
                exit(1);
            }
            else
            {
                //修改薪资成功
                if(strcmp(databuf->cmd,CMD_CHANGE_SALARY_OK) == 0)
                {
                    printf("修改薪资成功！\n");
                    HANG_LOG();
                }
                //修改薪资失败
                else if(strcmp(databuf->cmd,CMD_CHANGE_SALARY_FAIL) == 0)
                {
                    printf("修改薪资失败！\n");
                    HANG_LOG();
                }
            }       
            break;
        case CHANGE_DEPART:
            bzero(databuf->staff.department,DEPARTMENT_LEN);
            printf("请输入部门> ");
            fgets(databuf->staff.department, DEPARTMENT_LEN, stdin);
            databuf->staff.department[strlen(databuf->staff.department)-1] = 0; 

            bzero(databuf->cmd,CMD_LEN);
            strcpy(databuf->cmd,CMD_CHANGE_DEPART); 
            if((send(sfd, databuf, sizeof(data), 0)) < 0)
            {
                ERR_LOG("send");
                return;
            }            
            bzero(databuf->cmd, CMD_LEN);
            recv_len = recv(sfd, databuf, sizeof(data), 0);
            if(recv_len < 0)
            {
                ERR_LOG("recv");
                return;
            }
            else if(recv_len == 0)
            {
                printf("服务器离线 %d\n",__LINE__);
                close(sfd);
                exit(1);
            }
            else
            {
                //修改部门成功
                if(strcmp(databuf->cmd,CMD_CHANGE_DEPART_OK) == 0)
                {
                    printf("修改部门成功！\n");
                    HANG_LOG();
                }
                //修改部门失败
                else if(strcmp(databuf->cmd,CMD_CHANGE_DEPART_FAIL) == 0)
                {
                    printf("修改部门失败！\n");
                    HANG_LOG();
                }
            }       
            break;
        case CHANGE_JOB:
            bzero(databuf->staff.job,JOB_LEN);
            printf("请输入岗位> ");
            fgets(databuf->staff.job, JOB_LEN, stdin);
            databuf->staff.job[strlen(databuf->staff.job)-1] = 0;        

            bzero(databuf->cmd,CMD_LEN);
            strcpy(databuf->cmd,CMD_CHANGE_JOB); 
            if((send(sfd, databuf, sizeof(data), 0)) < 0)
            {
                ERR_LOG("send");
                return;
            }            
            bzero(databuf->cmd, CMD_LEN);
            recv_len = recv(sfd, databuf, sizeof(data), 0);
            if(recv_len < 0)
            {
                ERR_LOG("recv");
                return;
            }
            else if(recv_len == 0)
            {
                printf("服务器离线 %d\n",__LINE__);
                close(sfd);
                exit(1);
            }
            else
            {
                //修改岗位成功
                if(strcmp(databuf->cmd,CMD_CHANGE_JOB_OK) == 0)
                {
                    printf("修改岗位成功！\n");
                    HANG_LOG();
                }
                //修改岗位失败
                else if(strcmp(databuf->cmd,CMD_CHANGE_JOB_FAIL) == 0)
                {
                    printf("修改岗位失败！\n");
                    HANG_LOG();
                }
            }
            break;
        case CHANGE_LEVEL:
            printf("请输入职级> ");
            scanf("%d",&databuf->staff.level);
            getchar();       

            bzero(databuf->cmd,CMD_LEN);
            strcpy(databuf->cmd,CMD_CHANGE_LEVEL); 
            if((send(sfd, databuf, sizeof(data), 0)) < 0)
            {
                ERR_LOG("send");
                return;
            }            
            bzero(databuf->cmd, CMD_LEN);
            recv_len = recv(sfd, databuf, sizeof(data), 0);
            if(recv_len < 0)
            {
                ERR_LOG("recv");
                return;
            }
            else if(recv_len == 0)
            {
                printf("服务器离线 %d\n",__LINE__);
                close(sfd);
                exit(1);
            }
            else
            {
                //修改职级成功
                if(strcmp(databuf->cmd,CMD_CHANGE_LEVEL_OK) == 0)
                {
                    printf("修改职级成功！\n");
                    HANG_LOG();
                }
                //修改职级失败
                else if(strcmp(databuf->cmd,CMD_CHANGE_LEVEL_FAIL) == 0)
                {
                    printf("修改职级失败！\n");
                    HANG_LOG();
                }
            }
            break; 
        case CHANGE_FINISH:
            finish = 1;
            break;   
        default:
            INPUT_LOG();
        }
        if(finish)
            break;
    }
}

/**
 * @name: change_self
 * @function: 
 * @param {staff} *
 * @return {*}
 */
void change_self(data * databuf, int sfd)
{
    int items;
    int finish = 0;
    databuf->staff.jobid = input_jobid;
    int recv_len;
    while(1)
    {
        bzero(databuf->cmd,CMD_LEN);
        strcpy(databuf->cmd,CMD_DEMAND_ONE_STAFF);
        if((send(sfd, databuf, sizeof(data), 0)) < 0)
        {
            ERR_LOG("send");
            return;
        }
        
        bzero(databuf->cmd, CMD_LEN);
        int recv_len = recv(sfd, databuf, sizeof(data), 0);
        if(recv_len < 0)
        {
            ERR_LOG("recv");
            return;
        }
        else if(recv_len == 0)
        {
            printf("服务器离线 %d\n",__LINE__);
            close(sfd);
            exit(1);
        }
        else
        {
            //查询员工成功
            if(strcmp(databuf->cmd,CMD_DEMAND_ONE_OK) == 0)
            {
                printf("该员工信息如下：\n");
                //显示某位员工
                // display_staff(databuf);
                system("clear");
                printf("————————————————————————————\n");
                printf("        我的信息表\n");
                printf("****************************\n");
                printf("%s",databuf->data);
                printf("____________________________\n");   
            }
            //查询员工失败
            else if(strcmp(databuf->cmd,CMD_DEMAND_ONE_FAIL) == 0)
            {
                printf("查询员工失败！系统中无员工，请添加！\n");
            }                
        } 
        
        printf("选项：\n");
        printf("    1.修改密码\n");
        printf("    2.修改年龄\n"); 
        printf("    3.修改住址\n");
        printf("    4.结束修改\n");
        printf("————————————————————————————\n");

        printf("请选择要修改的选项> ");
        scanf("%d",&items);
        getchar();
        switch (items)
        {
        case CHANGE_PASSWORD:
            bzero(databuf->staff.password,PASSWORD_LEN);
            printf("请输入密码> ");
            fgets(databuf->staff.password, PASSWORD_LEN, stdin);
            databuf->staff.password[strlen(databuf->staff.password)-1] = 0;  
            
            bzero(databuf->cmd,CMD_LEN);
            strcpy(databuf->cmd,CMD_CHANGE_PASSWD); 
            if((send(sfd, databuf, sizeof(data), 0)) < 0)
            {
                ERR_LOG("send");
                return;
            }            
            bzero(databuf->cmd, CMD_LEN);
            recv_len = recv(sfd, databuf, sizeof(data), 0);
            if(recv_len < 0)
            {
                ERR_LOG("recv");
                return;
            }
            else if(recv_len == 0)
            {
                printf("服务器离线 %d\n",__LINE__);
                close(sfd);
                exit(1);
            }
            else
            {
                //修改密码成功
                if(strcmp(databuf->cmd,CMD_CHANGE_PASSWD_OK) == 0)
                {
                    printf("修改密码成功！\n");
                    HANG_LOG();
                }
                //修改密码失败
                else if(strcmp(databuf->cmd,CMD_CHANGE_PASSWD_FAIL) == 0)
                {
                    printf("修改密码失败！\n");
                    HANG_LOG();
                }
            }   
            break;
        case CHANGE_AGE:
            printf("请输入年龄> ");
            scanf("%d",&databuf->staff.age);
            getchar();   

            bzero(databuf->cmd,CMD_LEN);
            strcpy(databuf->cmd,CMD_CHANGE_AGE); 
            if((send(sfd, databuf, sizeof(data), 0)) < 0)
            {
                ERR_LOG("send");
                return;
            }            
            bzero(databuf->cmd, CMD_LEN);
            recv_len = recv(sfd, databuf, sizeof(data), 0);
            if(recv_len < 0)
            {
                ERR_LOG("recv");
                return;
            }
            else if(recv_len == 0)
            {
                printf("服务器离线 %d\n",__LINE__);
                close(sfd);
                exit(1);
            }
            else
            {
                //修改年龄成功
                if(strcmp(databuf->cmd,CMD_CHANGE_AGE_OK) == 0)
                {
                    printf("修改年龄成功！\n");
                    HANG_LOG();
                }
                //修改年龄失败
                else if(strcmp(databuf->cmd,CMD_CHANGE_AGE_FAIL) == 0)
                {
                    printf("修改年龄失败！\n");
                    HANG_LOG();
                }
            }      
            break;
        case CHANGE_ADDRESS:
            bzero(databuf->staff.address,ADDRESS_LEN);
            printf("请输入住址> ");
            fgets(databuf->staff.address, ADDRESS_LEN, stdin);
            databuf->staff.address[strlen(databuf->staff.address)-1] = 0; 

             bzero(databuf->cmd,CMD_LEN);
            strcpy(databuf->cmd,CMD_CHANGE_ADDRES); 
            if((send(sfd, databuf, sizeof(data), 0)) < 0)
            {
                ERR_LOG("send");
                return;
            }            
            bzero(databuf->cmd, CMD_LEN);
            recv_len = recv(sfd, databuf, sizeof(data), 0);
            if(recv_len < 0)
            {
                ERR_LOG("recv");
                return;
            }
            else if(recv_len == 0)
            {
                printf("服务器离线 %d\n",__LINE__);
                close(sfd);
                exit(1);
            }
            else
            {
                //修改住址成功
                if(strcmp(databuf->cmd,CMD_CHANGE_ADDRES_OK) == 0)
                {
                    printf("修改住址成功！\n");
                    HANG_LOG();
                }
                //修改住址失败
                else if(strcmp(databuf->cmd,CMD_CHANGE_ADDRES_FAIL) == 0)
                {
                    printf("修改住址失败！\n");
                    HANG_LOG();
                }
            }                 
            break;
        case SELFCH_FINISH:
            finish = 1;
            break;   
        default:
            INPUT_LOG();
        }
        if(finish)
            break;
    }
}

/**
 * @name: demand_staff
 * @function: 
 * @param {data} *
 * @return {*}
 */
void demand_staff(data * databuf)
{
    int items;
    int finish = 0;
    while(1)
    {
        printf("————————————————————————————\n");
        printf("1.查询某位员工信息\n");
        printf("2.查询全部员工信息"); 
        printf("\n请选择要查询的选项> ");
        scanf("%d",&items);
        getchar();
        switch (items)
        {
        case DEMAND_ONE:
            printf("请输入工号> ");
            scanf("%d",&databuf->staff.jobid);
            getchar();   
            strcpy(databuf->cmd,CMD_DEMAND_ONE_STAFF);  
            finish = 1;     
            break;
        case DEMAND_ALL:
            strcpy(databuf->cmd,CMD_DEMAND_ALL_STAFF);   
            finish = 1;    
            break;
        default:
            INPUT_LOG();
        }
        if(finish)
            break;
    }
}

/**
 * @name: display_staff
 * @function: 
 * @param {data} *
 * @return {*}
 */
void display_staff(data * databuf)
{
    system("clear");
    printf("————————————————————————————\n");
    printf("        员工信息表\n");
    printf("****************************\n");
    printf("%s",databuf->data);
    printf("____________________________\n"); 
    HANG_LOG();    
}

/**
 * @name: demand_record
 * @function: 
 * @param {data} *
 * @return {*}
 */
void demand_record(data * databuf)
{
    int items;
    int finish = 0;
    while(1)
    {
        printf("————————————————————————————\n");
        printf("1.查询某位员工考勤记录\n");
        printf("2.查询全部员工考勤记录"); 
        printf("\n请选择要查询的选项> ");
        scanf("%d",&items);
        getchar();
        switch (items)
        {
        case DEMAND_ONE:
            printf("请输入工号> ");
            scanf("%d",&databuf->staff.jobid);
            getchar();   
            strcpy(databuf->cmd,CMD_DEMAND_ONE_STAFF_RECORD);  
            finish = 1;     
            break;
        case DEMAND_ALL:
            strcpy(databuf->cmd,CMD_DEMAND_ALL_STAFF_RECORD);   
            finish = 1;    
            break;
        default:
            INPUT_LOG();
        }
        if(finish)
            break;
    }
}

/**
 * @name: display_record
 * @function: 
 * @param {data} *
 * @return {*}
 */
void display_record(data * databuf)
{
    system("clear");
    printf("————————————————————————————\n");
    printf("        考勤记录表\n");
    printf("****************************\n");
    printf("%s",databuf->data);
    printf("____________________________\n");     
    HANG_LOG();
}

/**
 * @name: quit
 * @function: 
 * @param {staff} *
 * @return {*}
 */
void quit(staff * staff)
{
    staff->jobid = input_jobid;
}

