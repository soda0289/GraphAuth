#ifndef __GRAPH_AUTH_PAM_H__
#define __GRAPH_AUTH_PAM_H__

#include <glib-object.h>
#include <security/pam_appl.h>

#define GRAPH_AUTH_PAM_TYPE (graph_auth_pam_get_type())
#define GRAPH_AUTH_PAM(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GRAPH_AUTH_PAM_TYPE, GraphAuthPam))
#define GRAPH_AUTH_PAM_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GRAPH_AUTH_PAM_TYPE, GraphAuthPamClass))

typedef struct _GraphAuthPam GraphAuthPam;
typedef struct _GraphAuthPamClass GraphAuthPamClass;
typedef struct _GraphAuthPamPrivate GraphAuthPamPrivate;

struct _GraphAuthPamPrivate{
    pam_handle_t* pam_handle;
};


struct _GraphAuthPam {
    GObject parent_instance;

    gchar* auth_token;
    gchar* old_auth_token;
};

struct _GraphAuthPamClass {
    GObjectClass parent_class;
    guint new_msg_sigid;
};

GType graph_auth_pam_get_type (void);

GraphAuthPam* graph_auth_pam_new();

gint graph_auth_pam_authenticate(GraphAuthPam* self, gint* err);

#endif

