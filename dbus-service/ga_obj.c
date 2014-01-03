#include <stdio.h>
#include <stdlib.h>
#include "graph_auth.h"
/**
 * * SECTION: graph-auth
 * * @short_description: A graph login authenticator
 * *
 * * The #GraphAuth is a class to authenticate a graph login.
 * */
G_DEFINE_TYPE (GraphAuth, graph_auth, G_TYPE_OBJECT);

gboolean graph_auth_authenticate(GraphAuth* self, gint* result, GError* error);

#include "dbus_binding_server.h"


#include <security/pam_appl.h>
int
pam_token_pass (int num_msg, const struct pam_message **msg,
                    struct pam_response **resp, void *appdata_ptr){

    for(num_msg--;num_msg >= 0;num_msg--){
        struct pam_response* r;
        r = resp[num_msg];
        if(r != NULL){
            r->resp = "testtesttest";
        }
    }

    return PAM_SUCCESS;
}

static void
graph_auth_class_init(GraphAuthClass* ga_class){
    int error = 0;

    struct pam_conv* pam_c = g_malloc(sizeof(struct pam_conv));
    
    pam_c->conv = &pam_token_pass;
    pam_c->appdata_ptr = ga_class;

    error = pam_start("graph", "reyad", pam_c, &(ga_class->pam_handle));
    if(error != PAM_SUCCESS){
        g_printerr("Error starting pam transaction\n");
    }

    //Install interscoption data
    dbus_g_object_type_install_info(GRAPH_AUTH_TYPE,
                                    &dbus_glib_graph_auth_object_info);

}

static void
graph_auth_init(GraphAuth* self){

}

/**
* graph_auth_new:
* *
* * Allocates a new #GraphAuth.
* *
* * Return value: a new #GraphAuth.
* */
GraphAuth*
graph_auth_new(){
    GraphAuth* Auth;

    Auth = g_object_new(GRAPH_AUTH_TYPE, NULL);

    return Auth;
}


static void
graph_auth_finalize(GObject *object){
   G_OBJECT_CLASS(graph_auth_parent_class)->finalize(object);
}

gboolean 
graph_auth_authenticate(GraphAuth* self, gint* result, GError* error){
    GraphAuthClass* ga_class;

    int e = 0;
    char* user = g_malloc(255);

    //struct pam_conv pam_conv;
    char* service = g_malloc(255);

    pam_handle_t* pam_handle = NULL;


    *result = 0;

    ga_class = GRAPH_AUTH_CLASS(G_OBJECT_GET_CLASS(self));
    if(ga_class == NULL){
        
        g_printerr("Error getting graph auth class.\n" );
        return FALSE;
    }

    pam_handle = ga_class->pam_handle;

    if(pam_handle == NULL){
        
        g_printerr("Error getting PAM.!!!!!!!!\n" );
        return FALSE;
    }


    e = pam_get_item(pam_handle, PAM_SERVICE, (void*)&service);
    if(e != PAM_SUCCESS){
        g_printerr("Error getting service.\n ERROR  NUM %d\n", e);
        return TRUE;
    } else {
        g_print("got service: %s \n", service);
    }

    e = pam_get_item(pam_handle, PAM_USER, (void*)&user);
    if(e != PAM_SUCCESS){
        g_printerr("Error getting user.\n ERROR  NUM %d\n", e);
        return TRUE;
    } else {
        g_print("got user: %s \n", user);
    }

    e = pam_authenticate(pam_handle, 0);
    if(e != PAM_SUCCESS){
        g_printerr("Error authenticating.\n ERROR  NUM %d\n PAM Error %s\n", e, pam_strerror(pam_handle, e));
        return TRUE;
    }

    g_print("SUCCES AUTHENTICATING");
    
    *result = 1;
    
    return TRUE;
}
