//
// Created by Abedr on 9/15/2020.
//

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


GType
bbox_meta_api_get_type (void)
{
    static volatile GType type;
    static const gchar *tags[] = { "boxes", NULL };

    if (g_once_init_enter (&type)) {
        GType _type = gst_meta_api_type_register ("BBoxMetaAPI", tags);
        g_once_init_leave (&type, _type);
    }
    return type;
}


static gboolean
bbox_meta_init (GstMeta * meta, gpointer params, GstBuffer * buffer)
{
    BBoxMeta *emeta = (BBoxMeta *) meta;

    emeta->boxes = g_array_new(FALSE, FALSE, sizeof(gint));

    return TRUE;
}


static gboolean
bbox_meta_transform (GstBuffer * transbuf, GstMeta * meta,
                           GstBuffer * buffer, GQuark type, gpointer data)
{
    BBoxMeta *emeta = (BBoxMeta *) meta;

    /* we always copy no matter what transform */
    gst_buffer_add_bbox_meta(transbuf, emeta->boxes);

    return TRUE;
}


static void
bbox_meta_free (GstMeta * meta, GstBuffer * buffer)
{
    BBoxMeta *emeta = (BBoxMeta *) meta;

    //TODO : traverse throw the array and free each element separately (Maybe)
    /*
     * for (gint i = 0; i < emeta->boxes->len; i++)
     *      g_free(g_array_index (emeta->boxes, gint, i));
     */
    g_array_free(emeta->boxes, TRUE);
}

const GstMetaInfo *
bbox_meta_get_info (void)
{
    static const GstMetaInfo *meta_info = NULL;

    if (g_once_init_enter (&meta_info)) {
        const GstMetaInfo *mi = gst_meta_register (BBOX_META_API_TYPE,
                                                   "BBoxMeta",
                                                   sizeof (BBoxMeta),
                                                   bbox_meta_init,
                                                   bbox_meta_free,
                                                   bbox_meta_transform);
        g_once_init_leave (&meta_info, mi);
    }
    return meta_info;
}

BBoxMeta *
gst_buffer_add_bbox_meta (GstBuffer *buffer, GArray* boxes)
{
    BBoxMeta *meta;

    g_return_val_if_fail (GST_IS_BUFFER (buffer), NULL);

    meta = (BBoxMeta *) gst_buffer_add_meta (buffer, BBOX_META_INFO, NULL);

    meta->boxes = boxes;

    return meta;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int main(int argc, char*argv[]){


    //Initialize GStreamer
    gst_init(&argc, &argv);

    //Create new buffer
    GstBuffer* buffer;
    GstMemory *memory;
    gint size, width, height, bpp, metaSize;
    width = height = 200;
    bpp = 8;
    metaSize = 100 * 4 * 4; //Maximum 100 people metas can be stored, box = 4 numbers, 4 bytes per number

    //Please note that i need to increase the size to include the additional meta data size as well
    size = width * height * bpp + metaSize;
    buffer = gst_buffer_new ();
    memory = gst_allocator_alloc (NULL, size, NULL);
    //Insert the memory at buffer at index -1 which means at the beggninig
    gst_buffer_insert_memory (buffer, -1, memory);

    std::cout << "Created the buffer successfully\n";

    //Adding the meta data to the buffer
    GstMeta * metaDate = gst_buffer_add_meta(buffer, BBOX_META_INFO, NULL);
    std::cout << "Added meta to the buffer successfully\n";


    BBoxMeta *boxes = (BBoxMeta*) gst_buffer_get_meta(buffer, BBOX_META_API_TYPE);
    gint i;
    //boxes = g_array_new (FALSE, FALSE, sizeof (gint));
    for(i = 1;i<5;i++)
        g_array_append_val(boxes->boxes, i);

    std::cout << "Meta data set Successfully\n";

    std::cout << "Sleeping for 5 seconds\n";
    sleep(2);


    //Getting the meta data and printing it
    BBoxMeta *meta = (BBoxMeta*) gst_buffer_get_meta(buffer, BBOX_META_API_TYPE);
    std::cout << "Meta data is : \n";
    for (i = 0; i < meta->boxes->len; i++)
       std::cout << g_array_index (meta->boxes, gint, i) << " ";


    //std::cout << "Size of gint is : " << sizeof(gint) << "\n";

    return 0;
}