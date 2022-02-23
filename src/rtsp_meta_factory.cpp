//
// Created by Abedr on 9/11/2020.
//

#include "rtsp_meta_factory.h"


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

    //for (gint i = 0; i < emeta->boxes->len; i++)
    //    g_free(g_array_index (emeta->boxes, gint, i));

    ////Free ptr error crash
    //g_free(emeta->boxes);

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
