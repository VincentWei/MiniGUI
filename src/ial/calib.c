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
