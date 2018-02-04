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
/*
** init_ts.c
**
** Create date: 2000/06/13
*/

#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>

#define MAXX  9
#define MAXY  7 
#define NOISX 900
#define NOISY 1900
#define CON  40

struct point {
	int x;
	int y;
};

struct point array[MAXX][MAXY];
int  get_up_point(int *p, int *q, struct point real_point);
float caculate_x(int i,int j,struct point real_point);
float caculate_y(int i,int j,struct point real_point);
int init_ts(void);
int change_to_logic(struct point *temp);

/*
 * Initialize the data of the reference point.
 * the data of point is in the file data.conf
 */
int init_ts()
{
	int fd,size;

	if((fd=open("data.conf",O_RDONLY)) == -1)
	{
		perror("open");
		exit(1);
	}

	if((size=read(fd, array,MAXX*MAXY*sizeof(struct point))) == -1)
	{
		perror("read error");
		return -1;
	}
	close(fd);
	return 0;
}

/*
 * this function change the value of the device /dev/ts,
 * to the logical point address.
 */
int MapCoords(struct point *temp)
{
	int i,j;
	float x,y;

	i=j=0;
/*
	temp1.x=NOISX;
	temp1.y=NOISY;
	ioctl(fd, TSTHRESHOLD, &temp1);

	if((size=read(fd, &temp,sizeof(struct point))) == -1)
	{
		perror("read error");
		exit(1);
	}
	close(fd);
*/
	get_up_point(&i,&j,*temp);
	x = caculate_x(i,j,*temp);
	y = caculate_y(i,j,*temp);
	temp->x = CON*x;
	temp->y = CON*y;
	return 1;
#ifdef DEBUG
	printf("%f,%f\n",CON*x,CON*y);
#endif
}

/*
 * this function is used to determine the left up point 
 * argument real_point is the point which was read from 
 * device.
 */
int  get_up_point(int *p, int *q, struct point real_point)
{
	int average_x[MAXX];
	int average_y[MAXY];
	int i ,j,total;

	for(i=0;i<MAXX;i++)
	{
		for(j=0,total=0;j<MAXY;j++)
		{
			total+=array[i][j].x;
		}
		average_x[i]=total/MAXY;
	}
	
	for(i=0;i<MAXY;i++)
	{
		for(j=0,total=0;j<MAXX;j++)
		{
			total+=array[j][i].y;
		}
		average_y[i]=total/MAXX;
	}
	
	for(i = 0; i < MAXX; i ++)
	{
		if(real_point.x > average_x[i])
			*p = i - 1;
		break;
	}

	for(i = 0; i < MAXY; i ++)
	{
		if(real_point.y > average_y[i])
			*q = i - 1;
		break;
	}
	return 1;
}

/*
 * caculate the true x position.
 */
float caculate_x(int i,int j,struct point real_point)
{
	float t = (float)(real_point.x - array[i][j].x)/(array[i+1][j].x - array[i][j].x);
	return i + t;
}

/*
 * caculate the true y position.
 */
float caculate_y(int i,int j,struct point real_point)
{
	float t = (float)(real_point.y - array[i][j].y)/(array[i][j+1].y - array[i][j].y);
	return j + t;
}
