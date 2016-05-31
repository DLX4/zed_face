 #include <stdio.h>
 #include <setjmp.h>
 #include <string.h>
 #include <stdlib.h>
 #include "jpeglib.h"
#include <stdint.h>
 
  //将value低8位写入array[offset],高8位写入array[offset+1]
 #define put_2b(array,offset,value)    (array[offset] = (char) ((value) & 0xff),  array[offset+1] = (char) (((value) >> 8) & 0xff))
 #define put_4b(array,offset,value)   (array[offset] = (char) ((value) & 0xff),  array[offset+1] = (char) (((value) >> 8) & 0xff),  array[offset+2] = (char) (((value) >> 16) & 0xff),  array[offset+3] = (char) (((value) >> 24) & 0xff))
  //写文件头
 void write_bmp_header(j_decompress_ptr cinfo, FILE *output_file)
  {
          char bmpfileheader[14];
          char bmpinfoheader[40];
          long headersize, bfsize;
          int bits_per_pixel, cmap_entries;
  
         struct colormap{  //颜色表结构
         UINT8 blue;
         UINT8 green;
         UINT8 red;
         UINT8 reserved;
         };
         int step;
         int i=0;
         /* compute colormap size and total file size */
          if (cinfo->out_color_space == JCS_RGB) {
                  if (cinfo->quantize_colors) {
                          /* colormapped rgb */
                          bits_per_pixel = 8;
                          cmap_entries = 256;
                  } else {
                          /* unquantized, full color rgb */
                          bits_per_pixel = 24;
                          cmap_entries = 0;
                  }
          } else {
                  /* grayscale output.  we need to fake a 256-entry colormap. */
                  bits_per_pixel = 8;
                  cmap_entries = 256;
          }
  
         step = cinfo->output_width * cinfo->output_components;
  
         while ((step & 3) != 0) step++;
  
         /* file size */
          headersize = 14 + 40 + cmap_entries * 4; /* header and colormap */
  
         bfsize = headersize + (long) step * (long) cinfo->output_height;
  
         /* set unused fields of header to 0 */
          memset(bmpfileheader, 0, sizeof(bmpfileheader));
          memset(bmpinfoheader, 0 ,sizeof(bmpinfoheader));
  
         /* fill the file header */
          bmpfileheader[0] = 0x42;/* first 2 bytes are ascii 'b', 'm' */
          bmpfileheader[1] = 0x4d;
          put_4b(bmpfileheader, 2, bfsize); /* bfsize */
          /* we leave bfreserved1 & bfreserved2 = 0 */
          put_4b(bmpfileheader, 10, headersize); /* bfoffbits */
  
         /* fill the info header (microsoft calls this a bitmapinfoheader) */
          put_2b(bmpinfoheader, 0, 40);   /* bisize */
          put_4b(bmpinfoheader, 4, cinfo->output_width); /* biwidth */
          put_4b(bmpinfoheader, 8, cinfo->output_height); /* biheight */
          put_2b(bmpinfoheader, 12, 1);   /* biplanes - must be 1 */
          put_2b(bmpinfoheader, 14, bits_per_pixel); /* bibitcount */
          /* we leave bicompression = 0, for none */
          /* we leave bisizeimage = 0; this is correct for uncompressed data */
          if (cinfo->density_unit == 2) { /* if have density in dots/cm, then */
                  put_4b(bmpinfoheader, 24, (INT32) (cinfo->X_density*100)); /* xpels/m */
                  put_4b(bmpinfoheader, 28, (INT32) (cinfo->Y_density*100)); /* xpels/m */
          }
          put_2b(bmpinfoheader, 32, cmap_entries); /* biclrused */
          /* we leave biclrimportant = 0 */
         //写入文件头
         if (fwrite(bmpfileheader, 1, 14, output_file) != (size_t) 14) {
                  printf("write bmpfileheader error\n");
          }
          //写入信息头
          if (fwrite(bmpinfoheader, 1, 40, output_file) != (size_t) 40) {
                  printf("write bmpinfoheader error\n");
          }
         //写入颜色表,灰度图
         if (cmap_entries ==256) {
             struct colormap rgb[256];
             for(i=0;i<256;i++)
                 {    
                     rgb[i].blue=i;
                     rgb[i].green=i;
                     rgb[i].red=i;
                     rgb[i].reserved=0;
                 }
              //写入颜色表
             if (fwrite(rgb, 1, sizeof(struct colormap)*256, output_file) <0) {
                  printf("write color error\n");
          }
          }
  }
  //写入位图数据
 void write_pixel_data(j_decompress_ptr cinfo, unsigned char *output_buffer, FILE *output_file)
  {
          int rows, cols;
          int row_width;
          int step;
          unsigned char *tmp = NULL;
  
         unsigned char *pdata;
  
         row_width = cinfo->output_width * cinfo->output_components;
          step = row_width;
          while ((step & 3) != 0) step++;
  
         pdata = (unsigned char *)malloc(step);
          memset(pdata, 0, step);
         //每次写入一行，并且调整rgb顺序。windows位图存储顺序是bgr，而libjpeg库是rgb
         tmp = output_buffer + row_width * (cinfo->output_height - 1);
          if(cinfo->output_components==3){ //24bit真彩图
          for (rows = 0; rows < cinfo->output_height; rows++) {
                  for (cols = 0; cols < row_width; cols += 3) {
                          pdata[cols + 2] = tmp[cols + 0];
                          pdata[cols + 1] = tmp[cols + 1];
                          pdata[cols + 0] = tmp[cols + 2];
                  }
                  tmp -= row_width;
                  fwrite(pdata, 1, step, output_file);
          }
         }
         else //cinfo->output_components==1，8bit灰度图
         {
             for (rows = 0; rows < cinfo->output_height; rows++) {
                  for (cols = 0; cols < row_width; cols ++) {
                          pdata[cols] = tmp[cols];
                  }
                  tmp -= row_width;
                  fwrite(pdata, 1, step, output_file);
          }
         }
  
         free(pdata);
  }
  //参考了libjpeg库里的example.c
 int read_jpeg_file(FILE *input_file, FILE *output_file)
  {
          struct jpeg_decompress_struct cinfo;
          struct jpeg_error_mgr jerr;
          //FILE *input_file;
          //FILE *output_file;
          JSAMPARRAY buffer;
          int row_width;
  
         unsigned char *output_buffer;
          unsigned char *tmp = NULL;
  
         cinfo.err = jpeg_std_error(&jerr);
  /*
         if ((input_file = fopen(input_filename, "rb")) == NULL) {
                  fprintf(stderr, "can't open %s\n", input_filename);
                  return -1;
          }
  
         if ((output_file = fopen(output_filename, "wb")) == NULL) {
  
                 fprintf(stderr, "can't open %s\n", output_filename);
                  return -1;
          }
  */
         jpeg_create_decompress(&cinfo);
  
         /* specify data source for decompression */
          jpeg_stdio_src(&cinfo, input_file);
  
         /* read file header, set default decompression parameters */
          (void) jpeg_read_header(&cinfo, TRUE);
  
         /* start decompressor */
          (void) jpeg_start_decompress(&cinfo);
  
         row_width = cinfo.output_width * cinfo.output_components;
  
         buffer = (*cinfo.mem->alloc_sarray)
                  ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_width, 1);
         //写bmp文件头
         write_bmp_header(&cinfo, output_file);
         //分配bmp缓冲区
         output_buffer = (unsigned char *)malloc(row_width * cinfo.output_height);
          memset(output_buffer, 0, row_width * cinfo.output_height);
          tmp = output_buffer;
  
         /* process data */
          while (cinfo.output_scanline < cinfo.output_height) {
                  (void) jpeg_read_scanlines(&cinfo, buffer, 1);
  
                 memcpy(tmp, *buffer, row_width);
                  tmp += row_width;
          }
         //写bmp数据
         write_pixel_data(&cinfo, output_buffer, output_file);
  
         free(output_buffer);
  
         (void) jpeg_finish_decompress(&cinfo);
  
         jpeg_destroy_decompress(&cinfo);
  
         /* close files, if we opened them */
          //fclose(input_file);
          //fclose(output_file);
         return 0;
  }


  /*
 int main(int argc, char *argv[])
  {
          if (argc < 3) {
                  //read_jpeg_file("tt.jpg", "tt.bmp");
                  printf("use:appname inputfile outputfile!\n");
          } else {
                  read_jpeg_file(argv[1], argv[2]);
          }
          return 0;
  }
  */
