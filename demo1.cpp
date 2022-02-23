//
// Created by root on 16/09/2020.
//

#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
#include <gst/app/app.h>
#include "iostream"


static gboolean
timeout (GstRTSPServer * server)
{
    GstRTSPSessionPool *pool;

    pool = gst_rtsp_server_get_session_pool (server);
    gst_rtsp_session_pool_cleanup (pool);
    g_object_unref (pool);

    return TRUE;
}


void stream_file(){

    GMainLoop* loop;
    GstRTSPServer* server;
    GstRTSPMediaFactoryURI* factory;
    GstRTSPMountPoints* mounts;

    loop = g_main_loop_new(NULL, false);
    server = gst_rtsp_server_new();

    factory = gst_rtsp_media_factory_uri_new();
    mounts = gst_rtsp_server_get_mount_points(server);

    gchar u[] = "file:///home/abed/Downloads/wynn-employee-entry.mp4";
    //= {"rtsp://admin:Abcd1234@62.6.190.206:30091"};
    //gchar *fileName = gst_filename_to_uri(u, NULL);

    gchar* uri = g_strdup(u);
    gst_rtsp_media_factory_uri_set_uri (factory, uri);
    g_free(uri);

    gst_rtsp_mount_points_add_factory(mounts, "/test", GST_RTSP_MEDIA_FACTORY(factory));
    g_object_unref(mounts);

    /* attach the server to the default maincontext */
    if (gst_rtsp_server_attach (server, NULL) == 0)
        goto failed;


    /* do session cleanup every 2 seconds */
    g_timeout_add_seconds (2, (GSourceFunc) timeout, server);

    g_main_loop_run (loop);

    failed:
    {
        g_print ("failed to attach the server\n");
        return;
    }


}

static gboolean print_field (GQuark field, const GValue * value, gpointer pfx) {
    gchar *str = gst_value_serialize (value);

    g_print ("%s  %15s: %s\n", (gchar *) pfx, g_quark_to_string (field), str);
    g_free (str);
    return TRUE;
}

static void print_caps (const GstCaps * caps, const gchar * pfx) {
    guint i;

    g_return_if_fail (caps != NULL);

    if (gst_caps_is_any (caps)) {
        g_print ("%sANY\n", pfx);
        return;
    }
    if (gst_caps_is_empty (caps)) {
        g_print ("%sEMPTY\n", pfx);
        return;
    }

    for (i = 0; i < gst_caps_get_size (caps); i++) {
        GstStructure *structure = gst_caps_get_structure (caps, i);

        g_print ("%s%s\n", pfx, gst_structure_get_name (structure));
        gst_structure_foreach (structure, print_field, (gpointer) pfx);
    }
}

static void get_pad(GstElement* object,
                    GstPad* pad,
                    gpointer user_data){

    GstCaps *caps = NULL;

    caps = gst_pad_get_current_caps (pad);
    if (!caps)
        caps = gst_pad_query_caps (pad, NULL);

    //TODO: initialize frame caps with the right ones
    print_caps (caps, "      ");
    gst_caps_unref (caps);

}


void static media_construct (GstRTSPMediaFactory * factory,
                                        GstRTSPMedia * media,
                                        gpointer user_data){

    GstElement* element = gst_rtsp_media_get_element(media);
    GstElement* rtspsrc =  gst_bin_get_by_name_recurse_up(GST_BIN(element), "rtspsrc");
    g_signal_connect(rtspsrc, "pad-added", (GCallback)get_pad, NULL);

    gst_object_unref (element);
    gst_object_unref (rtspsrc);
}


static void stream_rtsp(){


    GMainLoop* loop;
    GstRTSPServer* server;
    GstRTSPMediaFactory* factory;
    GstRTSPMountPoints* mounts;

    loop = g_main_loop_new(NULL, false);
    server = gst_rtsp_server_new();

    factory = gst_rtsp_media_factory_new();
    mounts = gst_rtsp_server_get_mount_points(server);


    //rtsp://admin:Abcd1234@62.6.190.206:30091
    //rtsp://admin:Abcd1234@62.6.190.206:30051/Streaming/channels/102

//    gst_rtsp_media_factory_set_launch(factory, "( rtspsrc location=rtsp://admin:Abcd1234@62.6.190.206:30051/Streaming/channels/102 !"
//                                               " rtph264depay !"
//                                               " decodebin !"
//                                               " autovideoconvert !"
//                                               " x265enc !"
//                                               " rtph265pay name=pay0 pt=96 )");

    //Driver issue cant use x264enc or (nvv4l2h265enc or nvv4l2decoder which are the nvidia hardware accelerated enc/dec)
    //rtspsrc default latency is 2000
    //TODO: enable buffering as well
    //udp-buffer-size=6220800
    //    gst_rtsp_media_factory_set_launch(factory, "( rtspsrc location=rtsp://admin:Abcd1234@62.6.190.206:30081/Streaming/channels/101 buffer-mode=synced ntp-sync=true drop-on-latency=true buffer-mode=4 do-retransmission=false name=rtspsrc !"
    gst_rtsp_media_factory_set_launch(factory, "( rtspsrc location=rtsp://admin:Abcd1234@62.6.190.206:30051/Streaming/channels/102 buffer-mode=synced ntp-sync=true drop-on-latency=true buffer-mode=4 do-retransmission=false name=rtspsrc !"
                                               " rtph264depay !"
                                               " decodebin !"
                                               " autovideoconvert !"
                                               " x265enc !"
                                               " rtph265pay name=pay0 )");

    g_signal_connect(factory, "media-constructed", (GCallback)media_construct, NULL);


    //+++++++++++++++++++++++++++++++++++++++++
    /*
     * gst_rtsp_media_factory_construct() used to reconstruct specific streaming url
     * http://gstreamer-devel.966125.n4.nabble.com/GstAppSrc-and-GstRTSPServer-td4656822.html
     *
     */
    //-----------------------------------------

//    gst_rtsp_server_set_address (server, "127.0.0.1");
//    gst_rtsp_server_set_service (server, "8554");
//    GstRTSPUrl* url = (GstRTSPUrl*) malloc(sizeof(GstRTSPUrl));
//    gst_rtsp_url_parse("rtsp://127.0.0.1:8554/test", &url);
//    GstRTSPMedia* media = gst_rtsp_media_factory_construct(factory, url);

    gst_rtsp_mount_points_add_factory(mounts, "/test", GST_RTSP_MEDIA_FACTORY(factory));
    g_object_unref(mounts);

    /* attach the server to the default maincontext */
    if (gst_rtsp_server_attach (server, NULL) == 0)
        goto failed;


    /* do session cleanup every 2 seconds */
    g_timeout_add_seconds (2, (GSourceFunc) timeout, server);

    g_main_loop_run (loop);

    failed:
    {
        g_print ("failed to attach the server\n");
        return;
    }


}

int main(int argc, char* argv[]){

    gst_init(&argc, &argv);

    //stream_rtsp();
    stream_file();


    return 0;
}