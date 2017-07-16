//
// gst-distort.h
//
// github:
//     https://github.com/yoggy/gst-distort
//
// license:
//     Copyright (c) 2017 yoggy <yoggy0@gmail.com>
//     Released under the MIT license
//     http://opensource.org/licenses/mit-license.php;
//
#ifndef __GST_DISTORT_H__
#define __GST_DISTORT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <gst/gst.h>
#include <gst/video/gstvideofilter.h>
#include <opencv2/core/core.hpp>

G_BEGIN_DECLS

#define GST_TYPE_DISTORT \
  (gst_distort_get_type())
#define GST_DISTORT(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_DISTORT,GstDistort))
#define GST_DISTORT_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_DISTORT,GstDistortClass))
#define GST_IS_DISTORT(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_DISTORT))
#define GST_IS_DISTORT_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_DISTORT))
#define GST_DISTORT_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS((obj),GST_TYPE_DISTORT,GstDistortClass))

typedef struct _GstDistort      GstDistort;
typedef struct _GstDistortClass GstDistortClass;

struct _GstDistort
{
  GstVideoFilter element;
  GstPad *sinkpad, *srcpad;
  gint img_width;
  gint img_height;
};

struct _GstDistortClass
{
  GstVideoFilterClass parent_class;
};

GType gst_distort_get_type (void);
gboolean gst_distort_plugin_init (GstPlugin * plugin);

G_END_DECLS

#ifdef __cplusplus
}
#endif

#endif

