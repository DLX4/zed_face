#ifndef _TOP_H_
#define _TOP_H_
#include "hls_video.h" //这里调用可以综合的视频库

// maximum image size
#define MAX_WIDTH 1920
#define MAX_HEIGHT 1080
// I/O Image Settings
#define INPUT_IMAGE "test_1080p.jpg"
#define OUTPUT_IMAGE "result_1080p.jpg"
#define OUTPUT_IMAGE_GOLDEN "result_1080p_golden.jpg"
// typedef video library core structures
typedef hls::stream<ap_axiu<24,1,1,1> > AXI_STREAM_IN;
typedef hls::stream<ap_axiu<24,1,1,1> > AXI_STREAM_OUT;
typedef hls::Mat<MAX_HEIGHT, MAX_WIDTH, HLS_8UC3> RGB_IMAGE;
typedef hls::Mat<MAX_HEIGHT, MAX_WIDTH, HLS_8UC1> GRAY_IMAGE;
// top level function for HW synthesis
void hls_sobel(AXI_STREAM_IN& src_axi, AXI_STREAM_OUT& dst_axi, int rows, int cols);

#endif
