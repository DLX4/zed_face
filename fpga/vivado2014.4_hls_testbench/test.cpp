#include "top.h"
#include "opencv/cv.h"
#include "opencv/cxcore.h"
#include "opencv/highgui.h"
#include "hls_opencv.h"
int main (int argc, char** argv) {
    IplImage* src = cvLoadImage(INPUT_IMAGE);
    IplImage* dst = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
    AXI_STREAM_IN src_axi;
    AXI_STREAM_OUT dst_axi;
    IplImage2AXIvideo(src, src_axi); //将图像转为视频流结构
    hls_sobel(src_axi, dst_axi, src->height, src->width);
    AXIvideo2IplImage(dst_axi, dst);
    cvSaveImage(OUTPUT_IMAGE, dst);
    cvReleaseImage(&src);
    cvReleaseImage(&dst);
}
