//
// gst-distort.cpp
//
// github:
//     https://github.com/yoggy/gst-distort
//
// license:
//     Copyright (c) 2017 yoggy <yoggy0@gmail.com>
//     Released under the MIT license
//     http://opensource.org/licenses/mit-license.php;
//
#include <gst/gst.h>
#include <gst/video/gstvideofilter.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "gst-distort.h"

#define VERSION "0.0.1"
GST_DEBUG_CATEGORY_STATIC(gst_distort_debug);
#define GST_CAT_DEFAULT gst_distort_debug

#ifdef __cplusplus
extern "C" {
#endif

enum
{
    LAST_SIGNAL
};

enum
{
    PROP_0,
};

#define CAPS_STR "video/x-raw, format=(string)\"BGR\""

static GstStaticPadTemplate sink_factory =
    GST_STATIC_PAD_TEMPLATE(
        "sink",
        GST_PAD_SINK,
        GST_PAD_ALWAYS,
        GST_STATIC_CAPS(CAPS_STR));

static GstStaticPadTemplate src_factory =
    GST_STATIC_PAD_TEMPLATE(
        "src",
        GST_PAD_SRC,
        GST_PAD_ALWAYS,
        GST_STATIC_CAPS(CAPS_STR));

#define parent_class gst_distort_parent_class
G_DEFINE_TYPE(GstDistort, gst_distort, GST_TYPE_VIDEO_FILTER);

static void gst_distort_finalize(GObject *obj);
static void gst_distort_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void gst_distort_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);

static gboolean gst_distort_sink_event(GstPad *pad, GstObject *parent, GstEvent *event);

static gboolean gst_distort_set_info(GstVideoFilter *trans, GstCaps *incaps, GstVideoInfo *in_info, GstCaps *outcaps, GstVideoInfo *out_info);
static GstFlowReturn gst_distort_transform_frame(GstVideoFilter *obj, GstVideoFrame *inframe, GstVideoFrame *outframe);

static void
gst_distort_class_init(GstDistortClass *klass)
{
    GObjectClass *gobject_class;
    GstVideoFilterClass *vfilter_class;

    gobject_class = (GObjectClass *)klass;
    GstElementClass *element_class = GST_ELEMENT_CLASS(klass);
    vfilter_class = (GstVideoFilterClass *)klass;

    GST_DEBUG_CATEGORY_INIT(gst_distort_debug, "gst_distort", 0, "gst_distort element");

    gobject_class->finalize = GST_DEBUG_FUNCPTR(gst_distort_finalize);

    gobject_class->set_property = gst_distort_set_property;
    gobject_class->get_property = gst_distort_get_property;

    vfilter_class->transform_frame = gst_distort_transform_frame;
    vfilter_class->set_info = gst_distort_set_info;

    gst_element_class_set_details_simple(element_class,
                                         "gst-distort",
                                         "FIXME:Generic",
                                         "FIXME:Generic",
                                         "AUTHOR_NAME AUTHOR_EMAIL");

    gst_element_class_add_pad_template(element_class,
                                       gst_static_pad_template_get(&src_factory));
    gst_element_class_add_pad_template(element_class,
                                       gst_static_pad_template_get(&sink_factory));
}

static void
gst_distort_init(GstDistort *filter)
{
    filter->sinkpad = gst_pad_new_from_static_template(&sink_factory, "sink");
    gst_pad_set_event_function(filter->sinkpad,
                               GST_DEBUG_FUNCPTR(gst_distort_sink_event));

    GST_PAD_SET_PROXY_CAPS(filter->sinkpad);
    gst_element_add_pad(GST_ELEMENT(filter), filter->sinkpad);

    filter->srcpad = gst_pad_new_from_static_template(&src_factory, "src");
    GST_PAD_SET_PROXY_CAPS(filter->srcpad);
    gst_element_add_pad(GST_ELEMENT(filter), filter->srcpad);

    filter->img_width = 0;
    filter->img_height = 0;
}

static void
gst_distort_finalize(GObject *obj)
{
    GstDistort *filter = GST_DISTORT(obj);

    // something to do...

    G_OBJECT_CLASS(parent_class)->finalize(obj);
}

static void
gst_distort_set_property(GObject *obj, guint prop_id,
                                 const GValue *value, GParamSpec *pspec)
{
    GstDistort *filter = GST_DISTORT(obj);

    switch (prop_id)
    {
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
        break;
    }
}

static void
gst_distort_get_property(GObject *obj, guint prop_id,
                                 GValue *value, GParamSpec *pspec)
{
    GstDistort *filter = GST_DISTORT(obj);

    switch (prop_id)
    {
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
        break;
    }
}

static gboolean
gst_distort_sink_event(GstPad *pad, GstObject *parent, GstEvent *event)
{
    GstDistort *filter;
    gboolean ret;

    filter = GST_DISTORT(parent);

    GST_LOG_OBJECT(filter, "Received %s event: %" GST_PTR_FORMAT,
                   GST_EVENT_TYPE_NAME(event), event);

    switch (GST_EVENT_TYPE(event))
    {
    case GST_EVENT_CAPS:
    {
        GstCaps *caps;

        gst_event_parse_caps(event, &caps);
        /* do something with the caps */

        /* and forward */
        ret = gst_pad_event_default(pad, parent, event);
        break;
    }
    default:
        ret = gst_pad_event_default(pad, parent, event);
        break;
    }
    return ret;
}

static gboolean
gst_distort_set_info(GstVideoFilter *obj, GstCaps *incaps,
                            GstVideoInfo *in_info, GstCaps *outcaps, GstVideoInfo *out_info)
{
    GstDistort *filter;
    GstDistortClass *fclass;
    GstFlowReturn ret;

    filter = GST_DISTORT(obj);
    fclass = GST_DISTORT_GET_CLASS(filter);

    // see also... https://github.com/GStreamer/gst-plugins-bad/blob/master/gst-libs/gst/opencv/gstopencvutils.cpp#L99
    filter->img_width = GST_VIDEO_INFO_WIDTH(in_info);
    filter->img_height = GST_VIDEO_INFO_HEIGHT(in_info);

    // https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer-libs/html/GstBaseTransform.html#gst-base-transform-set-in-place
    // TRUE : transform_frame_ipを使う場合 (バッファをコピーしない場合)
    // FALSE : transform_frameを使う場合 (バッファをコピーする場合)

    gst_base_transform_set_in_place(GST_BASE_TRANSFORM(filter), FALSE);

    return TRUE;
}

static GstFlowReturn
gst_distort_transform_frame(GstVideoFilter *obj,
                                    GstVideoFrame *inframe, GstVideoFrame *outframe)
{
    GstDistort *filter;
    GstDistortClass *fclass;
    GstFlowReturn ret;

    filter = GST_DISTORT(obj);
    fclass = GST_DISTORT_GET_CLASS(filter);

    cv::Mat img_src(filter->img_height, filter->img_width, CV_8UC3, inframe->data[0]);
    cv::Mat img_dst(filter->img_height, filter->img_width, CV_8UC3, outframe->data[0]);

    float w = img_src.cols;
    float h = img_src.rows;

    const cv::Point2f p_src[4]={
                cv::Point2f(0, 0),
                cv::Point2f(w, 0),
                cv::Point2f(w, h),
                cv::Point2f(0, h)};

    float offset_x = w / 8;

    const cv::Point2f p_dst[4]={
                cv::Point2f(0, 0),
                cv::Point2f(w, 0),
                cv::Point2f(w - offset_x, h),
                cv::Point2f(offset_x, h)};

    const cv::Mat mat_h = cv::getPerspectiveTransform(p_src, p_dst);

    cv::warpPerspective(img_src, img_dst, mat_h, img_src.size());

    return GST_FLOW_OK;
}

static gboolean
plugin_init(GstPlugin *plugin)
{
    GST_DEBUG_CATEGORY_INIT(gst_distort_debug, "gst-distort", 0, "distort plugin");

    return gst_element_register(plugin, "gst-distort", GST_RANK_NONE,
                                GST_TYPE_DISTORT);
}

#ifndef PACKAGE
#define PACKAGE "package_name"
#endif

GST_PLUGIN_DEFINE(
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    gst-distort,
    "description",
    plugin_init,
    VERSION,
    "MIT/X11",
    "gst-gidtort",
    "http://github.com/yoggy/gst-distort")

#ifdef __cplusplus
}
#endif