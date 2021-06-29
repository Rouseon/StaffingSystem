/*
 * @Descripttion: 员工管理系统客户端
 * @version: 1.0
 * @Author: Lou Sheng
 * @Date: 2021-06-25 14:49:47
 * @LastEditors: Lou Sheng
 * @LastEditTime: 2021-06-29 19:54:51
 */

#include "client.h"

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
	//建立TCP模型
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd < 0)
	{
		ERR_LOG("socket");//test
		return -1;
	}
    //端口快速重用
    int reuse = 1;
    if(setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int)) < 0)
    {
        ERR_LOG("setsockopt");
        return -1;
    }
	//根据服务器的IP和端口信息连接到服务器
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = inet_addr(argv[1]);
	saddr.sin_port = htons(*((unsigned short *)argv[2]));
	if(connect(sfd, (struct sockaddr *)&saddr, sizeof(saddr)))
	{
		ERR_LOG("connect");//test
		printf("连接服务器失败，请重试！\n");
		return -1;
	}

	while(1)
	{
		data databuf;
USER_SELECT:
		//进入选择界面
		switch(user_select_UI())
		{
			case I_AM_ADMIN:
				goto ADMIN_LOGIN;
				break;
			case I_AM_STAFF:
				goto STAFF_LOGIN;
				break;
			default:
				INPUT_LOG();
				goto USER_SELECT;
		}

ADMIN_LOGIN:
		//管理员登录界面
		if(login_UI(&databuf,sfd,I_AM_ADMIN) == 0)
			goto ADMIN_WORK;
		else
			goto ADMIN_LOGIN;
			
STAFF_LOGIN:
		//员工登录界面
		if(login_UI(&databuf,sfd,I_AM_STAFF) == 0)
			goto STAFF_WORK;
		else
			goto STAFF_LOGIN;
		
ADMIN_WORK:
		//管理员操作界面
		admin_work_UI(&databuf,sfd);
		
STAFF_WORK:
		//员工操作界面
		staff_work_UI(&databuf,sfd);
	}
	close(sfd);
	return 0;
}
