//
// Created by Abedr on 9/18/2020.
//

#include "rtsp_streamer.h"


rtsp_streamer::rtsp_streamer(unsigned int FWIDTH, unsigned int FHEIGHT, unsigned int BPP, unsigned int MAX_BOXES) {

    this->FHEIGHT = gint (FHEIGHT);
    this->FWIDTH = gint (FWIDTH);
    this->BPP = gint (BPP);
    this->MAX_BOXES = gint (MAX_BOXES);

    metaSize = MAX_BOXES * 4 * 4;
    FRAME_SIZE = FWIDTH * FHEIGHT * BPP;
    BUFFER_SIZE = FRAME_SIZE + metaSize;

    //TODO : init from outside
    init();
    rtsp_factory::init();

}

bool rtsp_streamer::init() {


    loop = g_main_loop_new (NULL, FALSE);
    server = gst_rtsp_server_new ();
    GstRTSPMountPoints *mounts = gst_rtsp_server_get_mount_points (server);
    factory = gst_rtsp_media_factory_new ();

    //TODO : add live and retrasmit packets to rtph264pay
    gst_rtsp_media_factory_set_launch (factory,
                                       "( appsrc name=mysrc ! videoconvert ! x264enc ! rtph264pay name=pay0 pt=96 )");

    g_object_set (server, "service", port, NULL);
    gst_rtsp_mount_points_add_factory (mounts, "/test",
                                       GST_RTSP_MEDIA_FACTORY (factory));

    g_signal_connect (factory, "media-configure", (GCallback) media_configure,
                      this);

    g_object_unref (mounts);

    if (gst_rtsp_server_attach (server, NULL) == 0)
        return false;

    return !(!loop || !server || !factory);

}

void
rtsp_streamer::media_configure (GstRTSPMediaFactory * factory, GstRTSPMedia * media,
                               gpointer data)
{
    auto* parent_streamer = (rtsp_streamer*)data;

    GstElement *element, *appsrc;

    /* get the element used for providing the streams of the media */
    element = gst_rtsp_media_get_element (media);

    /* get our appsrc, we named it 'mysrc' with the name property */
    appsrc = gst_bin_get_by_name_recurse_up (GST_BIN (element), "mysrc");

    /* this instructs appsrc that we will be dealing with timed buffer */
    gst_util_set_object_arg (G_OBJECT (appsrc), "format", "time");

    /* configure the caps of the video */
    g_object_set (G_OBJECT (appsrc), "caps",
                  gst_caps_new_simple ("video/x-raw",
                                       "format", G_TYPE_STRING, "RGB",
                                       "width", G_TYPE_INT, parent_streamer->FWIDTH,
                                       "height", G_TYPE_INT, parent_streamer->FHEIGHT,
                                       "framerate", GST_TYPE_FRACTION, 10, 1, NULL), NULL); //I420"
    //TODO : add variable frame rate

    /* make sure ther datais freed when the media is gone */
    g_object_set_data_full (G_OBJECT (media), "my-extra-data", NULL,
                            (GDestroyNotify) g_free);

    /* install the callback that will be called when a buffer is needed */
    g_signal_connect (appsrc, "need-data", (GCallback) get_data, parent_streamer);
    gst_object_unref (appsrc);
    gst_object_unref (element);
}

void
rtsp_streamer::get_data (GstElement * appsrc, guint unused, gpointer * ctx)
{

    auto* parent_streamer = (rtsp_streamer*)ctx;

    parent_streamer->buffer = gst_buffer_new_allocate (NULL, parent_streamer->BUFFER_SIZE, NULL);

    /* this makes the image black/white */
    gst_buffer_memset (parent_streamer->buffer, 0, 0xff, parent_streamer->FRAME_SIZE);


    GstMeta * metaDate = gst_buffer_add_meta(parent_streamer->buffer, BBOX_META_INFO, NULL);
    BBoxMeta *boxes = (BBoxMeta*) gst_buffer_get_meta(parent_streamer->buffer, BBOX_META_API_TYPE);
    gint i;
    //boxes = g_array_new (FALSE, FALSE, sizeof (gint));
    for(i = 1;i<100;i++)
        g_array_append_val(boxes->boxes, i);


    /* increment the timestamp every 1/2 second */
    GST_BUFFER_PTS (parent_streamer->buffer) = parent_streamer->timestamp;
    GST_BUFFER_DURATION (parent_streamer->buffer) = gst_util_uint64_scale_int (1, GST_SECOND, 2);
    parent_streamer->timestamp += GST_BUFFER_DURATION (parent_streamer->buffer);

    g_signal_emit_by_name (appsrc, "push-buffer", parent_streamer->buffer, &(parent_streamer->ret));
    gst_buffer_unref (parent_streamer->buffer);
}


void rtsp_streamer::decode_bbox_meta(GstBuffer* buffer, std::vector<int> boxes){

    BBoxMeta *meta = (BBoxMeta*) gst_buffer_get_meta(buffer, BBOX_META_API_TYPE);
    if(meta){
        for (uint i = 0; i < meta->boxes->len; i++)
            boxes.emplace_back(g_array_index (meta->boxes, gint, i));
    }
}

void rtsp_streamer::run(){

    g_print ("stream ready at rtsp://127.0.0.1:%s/test\n", port);
    g_main_loop_run (loop);

}