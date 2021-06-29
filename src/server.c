/*
 * @Descripttion: 员工管理系统服务器
 * @version: 1.0
 * @Author: Lou Sheng
 * @Date: 2021-06-25 14:49:41
 * @LastEditors: Lou Sheng
 * @LastEditTime: 2021-06-29 19:49:41
 */

#include "server.h"

int main(int argc, const char *argv[])
{
	if(argc < 3)
	{
		printf("请输入服务器IP号和端口号\n");
		return 0;
	}
	//注册SIGINT信号 
	sighandler sig;
	sig = signal(SIGINT,handler_quit);
	if(sig == SIG_ERR)
	{
		ERR_LOG("signal");
		return -1;
	}
    //创建(打开)一个数据库
    sqlite3* db = NULL;
	create_staffInfo(&db);
    //创建(打开)2个表
	create_staff_record_table(db);

	//建立套接字
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd < 0)
	{
		ERR_LOG("socket");
		return -1;
	}
    //端口快速重用
    int reuse = 1;
    if(setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int)) < 0)
    {
        ERR_LOG("setsockopt");
        return -1;
    }
	//绑定服务器IP和端口信息
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = inet_addr(argv[1]);
	saddr.sin_port = htons((*(unsigned short *)argv[2]));
	if(bind(sfd, (struct sockaddr *)&saddr,sizeof(saddr)) < 0)
	{
		ERR_LOG("bind");
		return -1;
	}
	//监听套接字
	if(listen(sfd, 10))
	{
		ERR_LOG("listen");
		return -1;
	}
	//printf("开始监听\n");//test

	//接收保存客户端的IP和端口信息
	struct sockaddr_in caddr;
	socklen_t caddr_len = sizeof(caddr);

	//创建1个子进程用于通信
	pid_t pid = fork();
	//父进程用于管理服务器
	if(pid > 0)
	{
		staff adminer;
		while(1)
		{
ADMIN_SELECT:
			system("clear");
			switch(server_admin_UI())
			{
				case INSERT_ADMIN:
					goto ADMIN_INSERT;
					break;
				case DELETE_ADMIN:
					goto ADMIN_DELETE;
					break;
				case SHOW_ADMIN:
					goto ADMIN_SHOW;
					break;
				default:
					INPUT_LOG();
					goto ADMIN_SELECT;
			}
		
ADMIN_INSERT:
			//添加管理员信息界面
			system("clear");
			server_admin_insert_UI(&adminer);
			if(insert_admin(db,&adminer) == 0)
			{
				printf("添加管理员成功！\n");
				HANG_LOG();
			}
			goto ADMIN_SELECT;
			
ADMIN_DELETE:
			//删除管理员信息界面

			server_admin_delete_UI(db,&adminer);		
			if(delete_admin(db,&adminer) == 0)
			{
				printf("删除管理员成功！\n");
				HANG_LOG();
			}
			goto ADMIN_SELECT;

ADMIN_SHOW:
			//显示所有管理员信息
			system("clear");
			select_admin_all(db);
			HANG_LOG();
		}
	}
	//子进程用于与客户端建立通信
	else if(pid == 0)
	{
		while (1)
		{
			data databuf;
			//建立连接
			// printf("准备连接\n");
			int cfd = accept(sfd , (struct sockaddr *)&caddr, &caddr_len);
			// printf("cfd = %d\n",cfd);
			if(cfd < 0)
			{
				ERR_LOG("accept");
				return -1;
			}
			// printf("连接成功\n");//test

			//传递线程参数：cfd
			pthread_args arg;
			arg.cfd = cfd;
			arg.databuf = databuf;
			arg.db = &db;
			pthread_t tid;
			if(pthread_create(&tid, NULL, client_handler, (void*)&arg) != 0)
			{
				ERR_LOG("pthread_creat");
				return -1;
			}
		}
	}
	else 
	{
		ERR_LOG("fork");
		return -1;
	}


	sqlite3_close(db);
	//printf("数据库已关闭\n");
	close(sfd);
	return 0;
}

