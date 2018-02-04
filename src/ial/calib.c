/*
 *   This file is part of MiniGUI, a mature cross-platform windowing 
 *   and Graphics User Interface (GUI) support system for embedded systems
 *   and smart IoT devices.
 * 
 *   Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
 *   Copyright (C) 1998~2002, WEI Yongming
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *   Or,
 * 
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 * 
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 * 
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/en/about/licensing-policy/>.
 */
#if 0
#include<stdio.h>
#include<unistd.h>
#include</usr/src/arm/include/asm/ioctls.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>

#define MAXX  9
#define MAXY  7
#define NOISX 900
#define NOISY 1900
#define DEVICE "/dev/ts" 

struct point {
	int x;
	int y;
};

main()
{
	struct point array[MAXX][MAXY];
	int fd,size,i,j;
	struct point temp,temp1;
	fd_set rset;
	char nouse[10];

	i=j=0;
	if((fd=open(DEVICE,O_RDONLY)) == -1)
	{
		perror("open1");
		exit(1);
	}

	temp1.x=NOISX;
	temp1.y=NOISY;
	ioctl(fd, TSTHRESHOLD, &temp1);


	for( i = 0 ; i < MAXX; i++ )
	{
		for(j = 0 ; j < MAXY; j ++)
		{
			while(1)
			{
				FD_ZERO(&rset);
				FD_SET(fd,&rset);
				FD_SET(STDIN_FILENO,&rset);
				select(fd+1,&rset,NULL,NULL,0);

				if(FD_ISSET(fd,&rset))
				{
					if((size = read(fd,&temp,sizeof(struct point))) == -1)
					{
						perror("read");
						exit(1);
					}
					if(temp.x != 0 && temp.y!= 0)
						temp1=temp;
#ifdef DEBUG
fprintf(stderr,"%d,%d",temp.x,temp.y);
#endif
				}
				if(FD_ISSET(STDIN_FILENO, &rset))
				{
					gets(nouse);
					break;
				}
			}

			array[i][j] = temp1;
#ifdef DEBUG
	fprintf(stderr,"%d,%d\n",temp1.x,temp1.y);
#endif
		}
	}

/*	while(1)
	{
		if((size = read(fd,&temp,sizeof(struct point))) == -1)
		{
			perror("read");
			exit(1);
		}

		array[i][j] = temp;

		if(j == MAXY-1)
		{
			j=0;
			i++;
			if(i == MAXX)
				break;
			continue;
		}
		j++;
	}
*/	close(fd);

	if((fd = open("data.conf",O_CREAT|O_WRONLY,0666)) == -1)
	{
		perror("open");
		exit(1);
	}

	if((size=write(fd, &array,MAXX*MAXY*sizeof(struct point))) == -1)
	{
		perror("write error");
		exit(1);
	}
	close(fd);
}
#endif
