/*
 * (C) Copyright 2015 Kurento (http://kurento.org/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "kmsrtpsession.h"

#define GST_DEFAULT_NAME "kmsrtpsession"
#define GST_CAT_DEFAULT kms_rtp_session_debug
GST_DEBUG_CATEGORY_STATIC (GST_CAT_DEFAULT);

#define kms_rtp_session_parent_class parent_class
G_DEFINE_TYPE (KmsRtpSession, kms_rtp_session, KMS_TYPE_BASE_RTP_SESSION);

KmsRtpSession *
kms_rtp_session_new (KmsBaseSdpEndpoint * ep, guint id,
    KmsIRtpSessionManager * manager, gboolean use_ipv6)
{
  GObject *obj;

  KmsRtpSession *self;

  obj = g_object_new (KMS_TYPE_RTP_SESSION, NULL);
  self = KMS_RTP_SESSION (obj);
  KMS_RTP_SESSION_CLASS (G_OBJECT_GET_CLASS (self))->post_constructor
      (self, ep, id, manager, use_ipv6);

  return self;
}

/* Connection management begin */

KmsRtpBaseConnection *
kms_rtp_session_get_connection (KmsRtpSession * self,
    KmsSdpMediaHandler * handler)
{
  KmsBaseRtpSession *base_rtp_sess = KMS_BASE_RTP_SESSION (self);

  KmsIRtpConnection *conn;

  conn = kms_base_rtp_session_get_connection (base_rtp_sess, handler);
  if (conn == NULL) {
    return NULL;
  }

  return KMS_RTP_BASE_CONNECTION (conn);
}

static KmsIRtpConnection *
kms_rtp_session_create_connection (KmsBaseRtpSession * base_rtp_sess,
    const GstSDPMedia * media, const gchar * name, guint16 min_port,
    guint16 max_port)
{
  const gchar *media_str;

  KmsRtpConnection *conn = NULL;

  media_str = gst_sdp_media_get_media (media);

  if (g_strcmp0 (media_str, "audio") == 0)      //ru-bu check audio
  {

    conn = kms_rtp_connection_new (min_port, max_port,
        KMS_RTP_SESSION (base_rtp_sess)->use_ipv6,
        KMS_SDP_SESSION (base_rtp_sess)->rtp_socket_reuse_audio,
        KMS_SDP_SESSION (base_rtp_sess)->rtcp_socket_reuse_audio);

  } else if (g_strcmp0 (media_str, "video") == 0) {
    conn = kms_rtp_connection_new (min_port, max_port,
        KMS_RTP_SESSION (base_rtp_sess)->use_ipv6,
        KMS_SDP_SESSION (base_rtp_sess)->rtp_socket_reuse_video,
        KMS_SDP_SESSION (base_rtp_sess)->rtcp_socket_reuse_video);
  } else {
    conn = kms_rtp_connection_new (min_port, max_port,
        KMS_RTP_SESSION (base_rtp_sess)->use_ipv6, NULL, NULL);
  }
  return KMS_I_RTP_CONNECTION (conn);
}

/* Connection management end */

static void
kms_rtp_session_post_constructor (KmsRtpSession * self,
    KmsBaseSdpEndpoint * ep, guint id, KmsIRtpSessionManager * manager,
    gboolean use_ipv6)
{
  KmsBaseRtpSession *base_rtp_session = KMS_BASE_RTP_SESSION (self);

  self->use_ipv6 = use_ipv6;
  KMS_BASE_RTP_SESSION_CLASS
      (kms_rtp_session_parent_class)->post_constructor (base_rtp_session, ep,
      id, manager);
}

static void
kms_rtp_session_init (KmsRtpSession * self)
{
  /* nothing to do */
}

static void
kms_rtp_session_class_init (KmsRtpSessionClass * klass)
{
  GstElementClass *gstelement_class = GST_ELEMENT_CLASS (klass);

  KmsBaseRtpSessionClass *base_rtp_session_class;

  GST_DEBUG_CATEGORY_INIT (GST_CAT_DEFAULT, GST_DEFAULT_NAME, 0,
      GST_DEFAULT_NAME);

  klass->post_constructor = kms_rtp_session_post_constructor;

  base_rtp_session_class = KMS_BASE_RTP_SESSION_CLASS (klass);
  /* Connection management */
  base_rtp_session_class->create_connection = kms_rtp_session_create_connection;

  gst_element_class_set_details_simple (gstelement_class,
      "RtpSession",
      "Generic",
      "Base bin to manage elements related with a RTP session.",
      "Miguel París Díaz <mparisdiaz@gmail.com>");
}
