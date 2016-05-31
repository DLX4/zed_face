 #include <stdio.h>
 #include <setjmp.h>
 #include <string.h>
 #include <stdlib.h>
 #include <jpeglib.h>
 
 FILE *bmpfile;//输入文件
 int image_width,image_height,image_size,bits_per_pixel,headersize,depth;
 unsigned char *src_data;//位图数据
 
 int get_2b(unsigned char*a,int offset)
 {
     return a[offset+1]<<8|a[offset];
 }
 int get_4b(unsigned char*a,int offset)
 {
     return (a[offset+3]<<24)|(a[offset+2]<<16)|(a[offset+1]<<8)|a[offset];
 }
 //读取位图文件头和位图数据
 void read_bmp_header(char *bmpfilename)
 {
     unsigned char bmpfileheader[14];//文件头
     unsigned char bmpinfoheader[40];//信息头
     bmpfile=fopen(bmpfilename,"r");//
     if(bmpfile<0)
         printf("open bmp file error!\n");
     printf("open bmp file success!\n");
     //读取bmp文件头
     fread(bmpfileheader,14,1,bmpfile);
     int type=get_2b(bmpfileheader,0);
     printf("type=0x%x\n",type);
     int filesize=get_4b(bmpfileheader,2);
     printf("filesize=%d bytes\n",filesize);
     headersize=get_4b(bmpfileheader,10);
     printf("headersize=%d bytes\n",headersize);
     if(headersize>54)
         printf("colormap size=%d bytes\n",headersize-54);
     //读取bmp信息头
     fseek(bmpfile,14,SEEK_SET);
     fread(bmpinfoheader,40,1,bmpfile);
     image_width=get_4b(bmpinfoheader,4);
     /*图像的宽度必须被4整除，否则jpg图像会变形！！！
     这个很多代码里都没有说明,我在压缩图片时发现有些图片能够正确压缩，有些图片压缩后变形
     经过仔细研究发现是宽度有问题*/
     while (image_width%4!=0)
         image_width++;
     printf("weight=%d\n",image_width);
     image_height=get_4b(bmpinfoheader,8);
     printf("height=%d\n",image_height);
     bits_per_pixel=get_2b(bmpinfoheader,14);
     printf("bits_per_pixel=%d\n",bits_per_pixel);
     depth=bits_per_pixel/8;
     image_size=image_width*image_height*depth;
     src_data=(unsigned char *)malloc(image_size);
     fseek(bmpfile,headersize,SEEK_SET);
     fread(src_data,sizeof(unsigned char)*image_size,1,bmpfile);
     fclose(bmpfile);
 }

 //参考了libjpeg库里的example.c
 void write_jpeg_file (char * outfilename, unsigned char * buffer,int quality)
 {
     struct jpeg_compress_struct cinfo;
     struct jpeg_error_mgr jerr;
     FILE * outfile;
     unsigned char *dst_data;
     int i,j;
     //char *point;
     JSAMPROW row_pointer[1];
     //js amparray buffer;
     int row_stride;
     cinfo.err = jpeg_std_error(&jerr);
     jpeg_create_compress(&cinfo);
     //打开输出jpg文件
     if ((outfile = fopen(outfilename, "w+")) == NULL) {
     fprintf(stderr, "can't open %s\n", outfilename);
     exit(1);
     }
 
     jpeg_stdio_dest(&cinfo, outfile);
     cinfo.image_width = image_width;     /* image width and height, in pixels */
     cinfo.image_height = image_height;
     cinfo.input_components = depth;        /* # of color components per pixel */
     cinfo.in_color_space = (depth==3) ? JCS_RGB : JCS_GRAYSCALE;     /* colorspace of input image */
 
     jpeg_set_defaults(&cinfo);
     jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-jpeg values */);
     dst_data=(unsigned char *)malloc(image_size*sizeof(unsigned char));
     //调整rgb顺序bgr->rgb
     for(i=0;i<image_height;i++){
         for(j=0;j<image_width;j++)
         {
             if(depth==1)//灰度图
                 *(dst_data+i*image_width+j)=*(src_data+i*image_width+j);
             else //真彩图
                 {
                     *(dst_data+i*image_width*depth+j*3+0)=*(src_data+i*image_width*depth+j*3+2);
                     *(dst_data+i*image_width*depth+j*3+1)=*(src_data+i*image_width*depth+j*3+1);
                     *(dst_data+i*image_width*depth+j*3+2)=*(src_data+i*image_width*depth+j*3+0);
                 }
         }
     }
     jpeg_start_compress(&cinfo, TRUE);
 
     row_stride = image_width * cinfo.input_components;    /* js amples per row in image_buffer */
     
     while (cinfo.next_scanline < cinfo.image_height) {
          row_pointer[0] = & dst_data[(cinfo.image_height - cinfo.next_scanline - 1) * row_stride];//cinfo.next_scanline * row_stride
          (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
 
     }
 
     jpeg_finish_compress(&cinfo);
     fclose(outfile);
     jpeg_destroy_compress(&cinfo);
     free(src_data);
     free(dst_data);
 }
//buffer = pglobal->in[pcontext->id].buf
//fread(src_data,sizeof(unsigned char)*image_size,1,bmpfile);
//remember free(src_data);
//vd->width = image_width  int image_width,image_height,image_size,bits_per_pixel,headersize,depth;
int compress_bmp_to_jpeg(unsigned char *buffer,int quality)
{
//copy from jpeg_utils
	struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];
    //unsigned char *line_buffer, *yuyv;
	unsigned char *dst_data;
    int i,j;
    static int written;
int row_stride;
    //line_buffer = calloc(vd->width * 3, 1);
    //yuyv = vd->framebuffer;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    /* jpeg_stdio_dest (&cinfo, file); */
    dest_buffer(&cinfo, buffer, image_size, &written);

    //cinfo.image_width = vd->width;
    //cinfo.image_height = vd->height;
    //cinfo.input_components = 3;
    //cinfo.in_color_space = JCS_RGB;
		cinfo.image_width = image_width;     /* image width and height, in pixels */
     cinfo.image_height = image_height;
     cinfo.input_components = depth;        /* # of color components per pixel */
     cinfo.in_color_space = (depth==3) ? JCS_RGB : JCS_GRAYSCALE;     /* colorspace of input image */

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);

printf("test ======\n");
printf("8888888888888888888888888888888888888888\n");
	dst_data=(unsigned char *)malloc(image_size*sizeof(unsigned char));
 //调整rgb顺序bgr->rgb
     for(i=0;i<image_height;i++){
         for(j=0;j<image_width;j++)
         {
             if(depth==1)//灰度图
                 *(dst_data+i*image_width+j)=*(src_data+i*image_width+j);
             else //真彩图
                 {
                     *(dst_data+i*image_width*depth+j*3+0)=*(src_data+i*image_width*depth+j*3+2);
                     *(dst_data+i*image_width*depth+j*3+1)=*(src_data+i*image_width*depth+j*3+1);
                     *(dst_data+i*image_width*depth+j*3+2)=*(src_data+i*image_width*depth+j*3+0);
                 }
         }
     }

jpeg_start_compress(&cinfo, TRUE);
printf("test ======\n");
row_stride = image_width * cinfo.input_components; 

while (cinfo.next_scanline < cinfo.image_height) {
          row_pointer[0] = & dst_data[(cinfo.image_height - cinfo.next_scanline - 1) * row_stride];//cinfo.next_scanline * row_stride
          (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
 
     }
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    //free(line_buffer);
		free(src_data);
     free(dst_data);

    return (written);
}
 int bmp2jpg_main()
 {
     
         //read_bmp_header("/temp.bmp");
        //compress_bmp_to_jpeg();

     return 0;
 }

