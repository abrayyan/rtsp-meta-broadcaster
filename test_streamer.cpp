//
// Created by root on 18/09/2020.
//

#include "rtsp_streamer.h"

int
main (int argc, char *argv[])
{

    gst_init(&argc, &argv);

    auto* streamer = new rtsp_streamer();
    streamer->init();
    streamer->run();

    return 0;
}