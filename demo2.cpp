//
// Created by root on 17/09/2020.
//

#include "gst/gst.h"
#include "gst/rtsp-server/rtsp-server.h"
#include "gst/rtsp/rtsp.h"
#include <iostream>
#include <vector>
#include "rtsp_meta_factory.h"



GstClockTime timestamp = 0;
gint FWIDTH = 1280;
gint FHEIGHT = 720;
gint BPP = 3;
gint MAX_BOXES = 100;
gint metaSize = MAX_BOXES * 4 * 4;

GstBuffer *buffer;
guint FRAME_SIZE = FWIDTH * FHEIGHT * BPP;
guint BUFFER_SIZE = FRAME_SIZE + metaSize;
GstFlowReturn ret;


void init_stream(){


}

static void
get_boxes (GstElement * appsrc, guint unused, gpointer * ctx)
{

    buffer = gst_buffer_new_allocate (NULL, BUFFER_SIZE, NULL);

    /* this makes the image black/white */
    gst_buffer_memset (buffer, 0, 0xff, FRAME_SIZE);


    GstMeta * metaDate = gst_buffer_add_meta(buffer, BBOX_META_INFO, NULL);
    BBoxMeta *boxes = (BBoxMeta*) gst_buffer_get_meta(buffer, BBOX_META_API_TYPE);
    gint i;
    //boxes = g_array_new (FALSE, FALSE, sizeof (gint));
    for(i = 1;i<100;i++)
        g_array_append_val(boxes->boxes, i);


//    BBoxMeta *meta = (BBoxMeta*) gst_buffer_get_meta(buffer, BBOX_META_API_TYPE);
//    std::cout << "Meta data is : \n";
//    for (i = 0; i < meta->boxes->len; i++)
//        std::cout << g_array_index (meta->boxes, gint, i) << " ";


    /* increment the timestamp every 1/2 second */
    GST_BUFFER_PTS (buffer) = timestamp;
    GST_BUFFER_DURATION (buffer) = gst_util_uint64_scale_int (1, GST_SECOND, 2);
    timestamp += GST_BUFFER_DURATION (buffer);

    g_signal_emit_by_name (appsrc, "push-buffer", buffer, &ret);
    gst_buffer_unref (buffer);
}




static void
media_configure (GstRTSPMediaFactory * factory, GstRTSPMedia * media,
                 gpointer user_data)
{
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
                                       "width", G_TYPE_INT, FWIDTH,
                                       "height", G_TYPE_INT, FHEIGHT,
                                       "framerate", GST_TYPE_FRACTION, 10, 1, NULL), NULL); //I420"

    /* make sure ther datais freed when the media is gone */
    g_object_set_data_full (G_OBJECT (media), "my-extra-data", NULL,
                            (GDestroyNotify) g_free);

    /* install the callback that will be called when a buffer is needed */
    g_signal_connect (appsrc, "need-data", (GCallback) get_boxes, NULL);
    gst_object_unref (appsrc);
    gst_object_unref (element);
}


int main(int argc, char *argv[]){


    GMainLoop *loop;
    GstRTSPServer *server;
    GstRTSPMountPoints *mounts;
    GstRTSPMediaFactory *factory;


    gst_init (&argc, &argv);
    loop = g_main_loop_new (NULL, FALSE);
    server = gst_rtsp_server_new ();
    mounts = gst_rtsp_server_get_mount_points (server);
    factory = gst_rtsp_media_factory_new ();

    gst_rtsp_media_factory_set_launch (factory,
                                       "( appsrc name=mysrc ! videoconvert ! x264enc ! rtph264pay name=pay0 pt=96 )"); //video/x-raw,width=1280,height=720,format=RGB      ! video/x-h264, profile=high-4:4:4


    g_signal_connect (factory, "media-configure", (GCallback) media_configure,
                      NULL);



    gst_rtsp_mount_points_add_factory (mounts, "/test", factory);

    /* don't need the ref to the mounts anymore */
    g_object_unref (mounts);

    /* attach the server to the default maincontext */
    gst_rtsp_server_attach (server, NULL);


    g_main_loop_run (loop);


    return 0;
}
