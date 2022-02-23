//
// Created by Abedr on 9/18/2020.
//

#ifndef RTSP_ENCODER_DECODER_RTSP_STREAMER_H
#define RTSP_ENCODER_DECODER_RTSP_STREAMER_H

#include "gst/gst.h"
#include "gst/rtsp-server/rtsp-server.h"
#include "gst/rtsp/rtsp.h"
#include <iostream>
#include <vector>
#include "rtsp_factory.h"


class rtsp_streamer : public rtsp_factory {

public:

    GstClockTime timestamp = 0;
    gint FWIDTH = 1280;
    gint FHEIGHT = 720;
    gint BPP = 3;
    gint MAX_BOXES = 100;

    //TODO : Adjust accordingly
    gint metaSize = MAX_BOXES * 4 * 4;

    GstBuffer *buffer{};
    guint FRAME_SIZE = FWIDTH * FHEIGHT * BPP;
    guint BUFFER_SIZE = FRAME_SIZE + metaSize;
    GstFlowReturn ret;

    rtsp_streamer() = default;

    ~rtsp_streamer() = default;

    rtsp_streamer(unsigned int FWIDTH, unsigned int FHEIGHT, unsigned int BPP, unsigned int MAX_BOXES);

    bool init();

    void static media_configure(GstRTSPMediaFactory *factory, GstRTSPMedia *media,
                         gpointer data);

    static void get_data(GstElement *appsrc, guint unused, gpointer *ctx);

    void decode_bbox_meta(GstBuffer *buffer, std::vector<int> boxes);

    void run();
};

#endif //RTSP_ENCODER_DECODER_RTSP_STREAMER_H
