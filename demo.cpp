/* GStreamer
 * Copyright (C) 2008 Wim Taymans <wim.taymans at gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
#include <gst/app/app.h>
#include "iostream"
#include "rtsp_meta_factory.h"

#define DEFAULT_RTSP_PORT "8554"

static char *port = (char *) DEFAULT_RTSP_PORT;


typedef struct
{
    GstElement *generator_pipe;
    GstElement *vid_appsink;
    GstElement *vid_appsrc;
} mediaGeneratorObject;


static void
ctx_free (mediaGeneratorObject * ctx)
{
    gst_element_set_state (ctx->generator_pipe, GST_STATE_NULL);

    gst_object_unref (ctx->generator_pipe);
    gst_object_unref (ctx->vid_appsrc);
    gst_object_unref (ctx->vid_appsink);

    g_free (ctx);
}


static void media_modify(GstElement * appsrc, guint unused, mediaGeneratorObject * ctx){

    GstSample* sample;
    GstFlowReturn ret; //The result of passing data to a pad. https://gstreamer.freedesktop.org/documentation/gstreamer/gstpad.html?gi-language=c#GstFlowReturn


    if(appsrc == ctx->vid_appsrc)
        sample = gst_app_sink_pull_sample (GST_APP_SINK (ctx->vid_appsink));


    if(sample){

        GstBuffer *buffer = gst_sample_get_buffer (sample);
        GstSegment *seg = gst_sample_get_segment (sample);
        GstClockTime pts, dts;

        /* Convert the PTS/DTS to running time so they start from 0 */
        pts = GST_BUFFER_PTS (buffer);
        if (GST_CLOCK_TIME_IS_VALID (pts))
            pts = gst_segment_to_running_time (seg, GST_FORMAT_TIME, pts);

        dts = GST_BUFFER_DTS (buffer);
        if (GST_CLOCK_TIME_IS_VALID (dts))
            dts = gst_segment_to_running_time (seg, GST_FORMAT_TIME, dts);

        if (buffer) {
            /* Make writable so we can adjust the timestamps */

            buffer = gst_buffer_copy (buffer);
            GST_BUFFER_PTS (buffer) = pts;
            GST_BUFFER_DTS (buffer) = dts;


            GstMeta * metaDate = gst_buffer_add_meta(buffer, BBOX_META_INFO, NULL);
            BBoxMeta *boxes = (BBoxMeta*) gst_buffer_get_meta(buffer, BBOX_META_API_TYPE);
            gint i;
            //boxes = g_array_new (FALSE, FALSE, sizeof (gint));
            for(i = 1;i<100;i++)
                g_array_append_val(boxes->boxes, i);


            BBoxMeta *meta = (BBoxMeta*) gst_buffer_get_meta(buffer, BBOX_META_API_TYPE);
            std::cout << "Meta data is : \n";
            for (i = 0; i < meta->boxes->len; i++)
                std::cout << g_array_index (meta->boxes, gint, i) << " ";



            //printf("PTS presentation timestamp: %" GST_STIME_FORMAT "\n", GST_STIME_ARGS(pts));
            //printf("DTS decoding timestamp: %" GST_STIME_FORMAT "\n", GST_STIME_ARGS(dts));
            //printf("Duration: %" GST_STIME_FORMAT "\n", GST_STIME_ARGS(buffer->duration));
            //g_print("Frame number: %" G_GUINT64_FORMAT "\n", buffer->offset);


            g_signal_emit_by_name (appsrc, "push-buffer", buffer, &ret);
        }


        gst_sample_unref (sample);

    }


}


static void media_configure(GstRTSPMediaFactory * factory, GstRTSPMedia * media,
                            gpointer user_data
){

    GstElement *element, *appsrc, *appsink;
    GstCaps *caps;
    mediaGeneratorObject *ctx;


    //g_new0(struct_type, n_structs) Allocates n_structs elements of type struct_type , initialized to 0's. The returned pointer is cast to a pointer to the given type. If n_structs is 0 it returns NULL.

    ctx = g_new0(mediaGeneratorObject, 1);
    ctx->generator_pipe =
            gst_parse_launch
                    ("videotestsrc is-live=true pattern=colors ! video/x-raw,width=1280,height=720 ! autovideoconvert ! x264enc speed-preset=superfast tune=zerolatency ! h264parse ! appsink name=vid max-buffers=1 drop=true",
                     NULL);


    //g_object_set_data() : Each object carries around a table of associations from strings to pointers. This function lets you set an association.
    //If the object already had an association with that name, the old association will be destroyed.
    //Like g_object_set_data() except it adds notification for when the association is destroyed, either by setting it to a different value or when the object is destroyed.
    g_object_set_data_full(G_OBJECT(media), "rtsp-extra-data", ctx, (GDestroyNotify) ctx_free);

    /* get the element (bin) used for providing the streams of the media */
    element = gst_rtsp_media_get_element(media);

    //set caps for the appsrc and appsink
    //TODO: inherit the caps from the sdk callback insteamd of createing new ones
    caps = gst_caps_new_simple("video/x-h264",
                               "stream-format", G_TYPE_STRING, "byte-stream",
                               "alignment", G_TYPE_STRING, "au",
                               "width", G_TYPE_INT, 1280, "height", G_TYPE_INT, 720,
                               "framerate", GST_TYPE_FRACTION, 15, 1, NULL);

    //Gets the element with the given name from this bin. If the element is not found, a recursion is performed on the parent bin.
    //gst_bin_get_by_name () does the recursion searching into child bins instead of parents
    ctx->vid_appsrc = appsrc = gst_bin_get_by_name_recurse_up(GST_BIN(element), "videosrc");
    ctx->vid_appsink = appsink =  gst_bin_get_by_name (GST_BIN (ctx->generator_pipe), "vid");
    gst_util_set_object_arg (G_OBJECT (appsrc), "format", "time");
    g_object_set (G_OBJECT (appsrc), "caps", caps, NULL);
    g_object_set (G_OBJECT (appsink), "caps", caps, NULL);


    /* this instructs appsrc that we will be dealing with timed buffer */
    gst_util_set_object_arg (G_OBJECT (appsrc), "format", "time");

    //add callback to the signal modifier
    g_signal_connect(G_OBJECT(appsrc), "need-data", (GCallback)media_modify, ctx);
    gst_caps_unref(caps);

    gst_element_set_state (ctx->generator_pipe, GST_STATE_PLAYING);
    gst_object_unref (element);

}


int
main (int argc, char *argv[])
{
    GMainLoop *loop;
    GstRTSPServer *server;
    GstRTSPMountPoints *mounts;
    GstRTSPMediaFactory *factory;
    GOptionContext *optctx;
    GError *error = NULL;
    gchar *str;


    gst_init (&argc, &argv);
    server = gst_rtsp_server_new ();
    g_object_set (server, "service", port, NULL);

    //Also these can be used to set the address and the port of the streaming server
    //gst_rtsp_server_set_address (server, "0.0.0.0");
    //gst_rtsp_server_set_service (server, "8554");


    loop = g_main_loop_new (NULL, FALSE);

    factory = gst_rtsp_media_factory_new ();
    //gst_rtsp_media_factory_set_launch (factory,"(appsrc name=videosrc ! videoconvert ! x264enc ! rtph264pay pt=96 name=pay0 )");
    gst_rtsp_media_factory_set_launch (factory,"( appsrc name=videosrc ! h264parse ! rtph264pay name=pay0 pt=96 )");
    /*
     * x264enc encodes raw video to H.264 video.
     * h264parse parses a H.264 encoded stream. Parsing means it looks at the stream and signal downstream the
     * format of the stream. Furthermore h264parse can convert the H.264 stream from one H.264 format to another,
     * but no encoding takes place.
     */


    g_signal_connect(factory, "media-configure", (GCallback)media_configure, NULL);

    mounts = gst_rtsp_server_get_mount_points (server);

    /* attach the video test signal to the "/test" URL */
    gst_rtsp_mount_points_add_factory (mounts, "/test", factory);
    g_object_unref (mounts);

    gst_rtsp_server_attach (server, NULL);


    g_main_loop_run (loop);




    return 0;
}
