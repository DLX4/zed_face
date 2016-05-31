/*
drive the VDMA in Linux user mode

NOTE:
*/
#include<stdio.h>
#include<unistd.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/times.h>

#define VDMA_BASSADDR 0x43000000
#define HEAD_SIZE  0x36
#define DATA_SIZE  0xe1000
#define OUT_BASEADDR 0x1f800000
unsigned char * map_base;
unsigned char * out_base;
unsigned char * in_base;
int vdma_idle()
{
	int *temp = (unsigned int *)(map_base + 0x34);
	//printf("%x\n",*temp);
	return (*temp)&0x00001000;
}
int main()
{
  FILE *f;
  int n, fd;

  fd = open("/dev/mem", O_RDWR|O_SYNC);
  if (fd == -1)
  {
	    return (-1);
  }

  map_base = mmap(NULL, 0xff, PROT_READ|PROT_WRITE, MAP_SHARED, fd, VDMA_BASSADDR);
	out_base = mmap(NULL, 0x200000, PROT_READ|PROT_WRITE, MAP_SHARED, fd, OUT_BASEADDR);
  in_base = mmap(NULL, 0x200000, PROT_READ|PROT_WRITE, MAP_SHARED, fd, OUT_BASEADDR + 0x200000);

  if (map_base == 0)
 	{
      printf("NULL pointer!\n");
  }
  else
  {
      printf("Successfull!\n");
  }

	FILE *bmp;

	char head_buff[HEAD_SIZE + 1] = {0};
	char data_buff[DATA_SIZE + 1] = {0};
	//char data_out[DATA_SIZE + 1] = {0};
	int num_read = 0;
	if( (bmp = fopen("./test.bmp","r")) != NULL)
	{
  		num_read = fread(head_buff,1,HEAD_SIZE,bmp);
  		printf("%d byte read \n",num_read);
  		num_read = fread(in_base,1,DATA_SIZE,bmp);
  		printf("%d byte read \n",num_read);
  		printf("data_buff --%lx \n",data_buff);
	} else{
  		printf("open the bmp file error \n");
	}
  /*
  	Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x0, 0x4); //reset   MM2S VDMA Control Register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x0, 0x8); //gen-lock
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x5C,   0x04000000);   //MM2S Start Addresses

    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x54, 640*3);//MM2S HSIZE Register---buffer length
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x58, 0x01000780);//S2MM FRMDELAY_STRIDE Register 1920*3=5760 ¶ÔÆëÖ®ºóÎª8192=0x2000

    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x0, 0x03);//MM2S VDMA Control Register
  */
  int frame_count = 2;
	int *temp32 = 0;
	temp32 = (unsigned int *)(map_base + 0x0);
	*temp32 = 0x4;
	*temp32 = 0x8;
	temp32 = (unsigned int *)(map_base + 0x5c);
	*temp32 = 0x1f800000 + 0x200000;
	//dlx test
	temp32 = (unsigned int *)(map_base + 0x60);
	*temp32 = 0x1f800000 + 0x600000;

	temp32 = (unsigned int *)(map_base + 0x54);
	*temp32 = 640*3;
	temp32 = (unsigned int *)(map_base + 0x58);
	*temp32 = 0x01000780;
	temp32 = (unsigned int *)(map_base + 0x0);
	*temp32 = 0x03 ;
	/*
 	map_base[0x0] = 0x4;
 	map_base[0x0] = 0x8;
 	map_base[0x5c] = data_buff;
 	map_base[0x54] = 640*3;
 	map_base[0x58] = 0x01000780;
 	map_base[0x0] = 0x03;
 	*/
 /*
 Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xAC, 0x08000000);//S2MM Start Addresses
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xA4, 640*3);
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xA8, 0x01000780);//S2MM Frame Delay and Stride
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x30, 0x3);//S2MM VDMA Control Register



    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x50, 480);//MM2S_VSIZE    Æô¶¯Ž«Êä
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xA0, 480);//S2MM Vertical Size  start an S2MM transfer
 */
 	temp32 = (unsigned int *)(map_base + 0xac);
 	*temp32 = 0x1f800000;
 	//dlx test
 	temp32 = (unsigned int *)(map_base + 0xb0);
 	*temp32 = 0x1f800000+0x400000;


 	temp32 = (unsigned int *)(map_base + 0xa4);
 	*temp32 = 640*3;
 	temp32 = (unsigned int *)(map_base + 0xa8);
 	*temp32 = 0x01000780;
 	temp32 = (unsigned int *)(map_base + 0x30);
 	//*temp32 = 0x3 | frame_count << 16 | 0x4;
 	*temp32 = 0x3;

 	temp32 = (unsigned int *)(map_base + 0x50);
 	*temp32 = 480;
 	temp32 = (unsigned int *)(map_base + 0xa0);
 	*temp32 = 480;
 	vdma_idle();
 	/*
 	map_base[0xac] = data_out;
 	map_base[0xa4] = 640*3;
 	map_base[0xa8] = 0x01000780;
 	map_base[0x30] = 0x3;

 	map_base[0x50] = 480;
 	map_base[0xa0] = 480;
 	*/
 	//long i = 0;
 	//for(i = 0 ;i<1000000;i++){ temp32 = (unsigned int *)(map_base + 0x50);printf("-------(50) =%d \n",*temp32);}
 	//printf(" loop 1 \n");
 	//sleep(1);
 	//vdma_idle();
 	//for(i = 0 ;i<100000;i++){}
 	//printf(" loop 2 \n");
 	//sleep(1);
 	//printf("sizeof int %d \n",sizeof(int));

 	int count = 0;
 	//clock_t tBeginTime = times(NULL);

 	struct timeval tpstart,tpend;
 	double timeuse;
 	gettimeofday(&tpstart,NULL);

 	while(0 == vdma_idle())
 	{count++;}

 	//clock_t tEndTime = times(NULL);
 	//double fCostTime = (double)(tEndTime - tBeginTime)/sysconf(_SC_CLK_TCK);
 	//printf("cost time = %lf sec \n",fCostTime);
 	gettimeofday(&tpend,NULL);
 	timeuse = 1000000*(tpend.tv_sec - tpstart.tv_sec) + tpend.tv_usec - tpstart.tv_usec;
 	printf("use time :%f  us\n",timeuse);

 	printf("count----%d\n",count);
 	FILE *out_bmp = NULL;
 	if( (out_bmp = fopen("./out.bmp","w+")) != NULL)
 	{
 		int num_write = 0;
 		num_write = fwrite(head_buff,1,HEAD_SIZE,out_bmp);
 		//sleep(1);
 		printf("%x byte write \n",num_write);
 		num_write = fwrite(out_base,1,DATA_SIZE,out_bmp);
 		//sleep(1);
 		printf("%x byte write \n",num_write);

 	}else{
 		printf("open the out bmp fail \n");
 	}
 	//memset(out_base,255,0x200000);
  /*
    unsigned long addr;
    unsigned char content;

    int i = 0;
    for (;i < 0xff; ++i)
    {
        addr = (unsigned long)(map_base + i);
        content = map_base[i];
        printf("address: 0x%lx   content 0x%x\t\t", addr, (unsigned int)content);

        map_base[i] = (unsigned char)i;
        content = map_base[i];
        printf("updated address: 0x%lx   content 0x%x\n", addr, (unsigned int)content);
    }
  */
  temp32 = (unsigned int *)(map_base + 0x0);
	*temp32 = 0x4;
	temp32 = (unsigned int *)(map_base + 0x30);
	*temp32 = 0x4;

  close(fd);
	//memset(in_base,255,0x200000);
	fclose(bmp);
	fclose(out_bmp);
  munmap(map_base, 0xff);
	munmap(in_base, 0x200000);
  munmap(out_base, 0x200000);
  return (1);
}
