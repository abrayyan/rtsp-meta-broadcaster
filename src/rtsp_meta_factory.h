//
// Created by Abedr on 9/11/2020.
//

#ifndef RTSP_ENCODER_DECODER_RTSP_META_FACTORY_H
#define RTSP_ENCODER_DECODER_RTSP_META_FACTORY_H

#include "gst/gst.h"
#include "gst/rtsp-server/rtsp-server.h"
#include "gst/rtsp/rtsp.h"
#include <iostream>
#include <vector>



typedef struct _BBoxMeta BBoxMeta;

struct _BBoxMeta {
    GstMeta meta;
    GArray *boxes;
};

GType bbox_meta_api_get_type (void);
#define BBOX_META_API_TYPE (bbox_meta_api_get_type())

#define gst_buffer_get_bbox_meta(b) \
  ((BBoxMeta*)gst_buffer_get_meta((b),BBOX_META_API_TYPE))

const GstMetaInfo *bbox_meta_get_info (void);
#define BBOX_META_INFO (bbox_meta_get_info())


BBoxMeta * gst_buffer_add_bbox_meta (GstBuffer* buffer, GArray* boxes);

static gboolean
bbox_meta_init (GstMeta * meta, gpointer params, GstBuffer * buffer);


static gboolean
bbox_meta_transform (GstBuffer * transbuf, GstMeta * meta,
                     GstBuffer * buffer, GQuark type, gpointer data);


static void
bbox_meta_free (GstMeta * meta, GstBuffer * buffer);

const GstMetaInfo *
bbox_meta_get_info (void);




#endif //RTSP_ENCODER_DECODER_RTSP_META_FACTORY_H
