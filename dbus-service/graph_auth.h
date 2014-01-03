#ifndef __GRAPH_AUTH_H__
#define __GRAPH_AUTH_H__

#include <glib-object.h>
#include <security/pam_appl.h>

#define GRAPH_AUTH_TYPE (graph_auth_get_type())
#define GRAPH_AUTH(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GRAPH_AUTH_TYPE, GraphAuth))
#define GRAPH_AUTH_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GRAPH_AUTH_TYPE, GraphAuthClass))

typedef struct _GraphAuth GraphAuth;
typedef struct _GraphAuthClass GraphAuthClass;

struct _GraphAuth {
    GObject parent_instance;
};

struct _GraphAuthClass {
    GObjectClass parent_class;
    pam_handle_t* pam_handle;
};

GType graph_auth_get_type (void);

GraphAuth* graph_auth_new();

int graph_auth_authenticate(GraphAuth* self, gint* result, GError* error);
#endif

