//
// Created by Abedr on 9/18/2020.
//

#ifndef RTSP_ENCODER_DECODER_RTSP_FACTORY_H
#define RTSP_ENCODER_DECODER_RTSP_FACTORY_H

#include "gst/gst.h"
#include "gst/rtsp-server/rtsp-server.h"
#include "gst/rtsp/rtsp.h"
#include <iostream>
#include <vector>
#include "rtsp_meta_factory.h"

#define DEFAULT_RTSP_PORT "8554"

static char *port = (char *) DEFAULT_RTSP_PORT;


class rtsp_factory {

private:
    static uint idMap;
    uint id;

public:

    GMainLoop *loop;
    GstRTSPServer *server;
    GstRTSPMediaFactory *factory;

    rtsp_factory(){
        this->id = idMap;
        idMap+=1;
        //TODO : change port on different factories
    };
    ~rtsp_factory()=default;

    virtual bool init();

};

#endif //RTSP_ENCODER_DECODER_RTSP_FACTORY_H
