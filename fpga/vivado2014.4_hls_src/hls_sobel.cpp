#include "top.h"
void hls_sobel(AXI_STREAM_IN& input, AXI_STREAM_OUT& output, int rows, int cols) {
#pragma HLS RESOURCE variable=input core=AXI4Stream metadata="-bus_bundle INPUT_STREAM"
#pragma HLS RESOURCE variable=output core=AXI4Stream metadata="-bus_bundle OUTPUT_STREAM"
#pragma HLS INTERFACE ap_none port=cols
#pragma HLS INTERFACE ap_none port=rows
//AP_CONTROL_BUS_AXI(CONTROL_BUS);
//set_directive_interface -mode ap_ctrl_none hls_sobel
#pragma HLS interface ap_ctrl_none port=return

RGB_IMAGE img_0(rows, cols);
RGB_IMAGE img_1(rows, cols);
#pragma HLS DATAFLOW // must use data flow to stream the data
hls::AXIvideo2Mat(input, img_0); //read video stream by frames
hls::Sobel<1,0,1>(img_0, img_1);//use Hls Sobel
hls::Mat2AXIvideo(img_1, output); //write the frames to video stream

}
