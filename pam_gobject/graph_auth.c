#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "graph_auth.h"
/**
 * * SECTION: graph-auth
 * * @short_description: A graph login authenticator
 * *
 * * The #GraphAuthPam is a class to authenticate a graph login.
 * */
G_DEFINE_TYPE_WITH_PRIVATE (GraphAuthPam, graph_auth_pam, G_TYPE_OBJECT)

gint graph_auth_pam_authenticate(GraphAuthPam* self, gint* err);



#include <security/pam_appl.h>

const char* PAM_SERVICE_NAME = "graph";

int
pam_token_pass (int num_msg, const struct pam_message **msg,
                    struct pam_response **resp, void *appdata_ptr){
    
    int i;
    GraphAuthPam* self = (GraphAuthPam*) appdata_ptr;
    GraphAuthPamClass* klass = GRAPH_AUTH_PAM_CLASS (G_OBJECT_GET_CLASS(self));
    
    struct pam_response* rs = *resp = calloc(num_msg, sizeof(struct pam_response));

    for(i = 0;i < num_msg;i++){
        struct pam_response* r = &(rs[i]); 
        struct pam_message* m = msg[i];

        if(m != NULL){
            g_signal_emit (self, klass->new_msg_sigid,0, m->msg, &(r->resp));
            if(r->resp == NULL){
                //Failed to get response
                r->resp = NULL;
            }
        }
    }
    
    return PAM_SUCCESS;
}

static void
graph_auth_pam_class_init(GraphAuthPamClass* klass){

    klass->new_msg_sigid = g_signal_new("new-message",
                                         GRAPH_AUTH_PAM_TYPE,
                                         G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                                         0,
                                         NULL,
                                         NULL,
                                         NULL,
                                         G_TYPE_STRING,
                                         1,
                                         G_TYPE_STRING);


#ifdef DBUS_SERVICE
    //Install interscoption data
    dbus_g_object_type_install_info(graph_auth_pam_TYPE,
                                    &dbus_glib_graph_auth_pam_object_info);
#endif

}

static void
graph_auth_pam_init(GraphAuthPam* self){
    int error = 0;

    uid_t uid = getuid();
    char username[0xFF];

    GraphAuthPamPrivate* gapp = G_TYPE_INSTANCE_GET_PRIVATE(self, GRAPH_AUTH_PAM_TYPE, GraphAuthPamPrivate);
    struct pam_conv* pam_c = g_malloc(sizeof(struct pam_conv));
   
    cuserid(username);

    pam_c->conv = &pam_token_pass;
    pam_c->appdata_ptr = self;

    g_print("UID: %d\n", uid);

    error = pam_start(PAM_SERVICE_NAME, username, pam_c, &(gapp->pam_handle));
    if(error != PAM_SUCCESS){
        g_printerr("Error starting pam transaction\n");
    }
}

/**
* graph_auth_pam_new:
* *
* * Allocates a new #GraphAuthPam.
* *
* * Return value: a new #GraphAuthPam.
* */
GraphAuthPam*
graph_auth_pam_new(){
    GraphAuthPam* Auth;

    Auth = g_object_new(GRAPH_AUTH_PAM_TYPE, NULL);

    return Auth;
}


static void
graph_auth_pam_finalize(GObject *object){
   G_OBJECT_CLASS(graph_auth_pam_parent_class)->finalize(object);
}

/**
 * graph_auth_pam_authenticate:
 * @self: graph auth instance
 * @err: (inout): status code
 * returns: the pam error code from authenticating 
 **/
gint 
graph_auth_pam_authenticate(GraphAuthPam* self, gint* err){
    GraphAuthPamPrivate* gapp = graph_auth_pam_get_instance_private(self);

    int e = 0;
    char* user = g_malloc(255);

    char* service = g_malloc(255);

    pam_handle_t* pam_handle = NULL;


    *err = 0;

    pam_handle = gapp->pam_handle;
    if(pam_handle == NULL){
        g_printerr("Error getting PAM!\n" );
        return FALSE;
    }

    e = pam_get_item(pam_handle, PAM_SERVICE, (void*)&service);
    if(e != PAM_SUCCESS){
        g_printerr("Error getting service.\n ERROR  NUM %d\n", e);
        return FALSE;
    }

    g_print("got service: %s \n", service);

    e = pam_get_item(pam_handle, PAM_USER, (void*)&user);
    if(e != PAM_SUCCESS){
        g_printerr("Error getting user.\n ERROR  NUM %d\n", e);
        return FALSE;
    }

    g_print("got user: %s \n", user);
    

    e = pam_authenticate(pam_handle, 0);
    if(e != PAM_SUCCESS){
        g_printerr("Error authenticating.\n ERROR  NUM %d\n PAM Error %s\n", e, pam_strerror(pam_handle, e));
        return FALSE;
    }

    g_print("SUCCES AUTHENTICATING");
    
    *err = 1;
    
    return TRUE;
}

/**
 * graph_auth_pam_chauthtok:
 * @self: graph auth instance
 * @err: (inout): status code
 * returns: the pam error code from changing the auth token
 **/
gint 
graph_auth_pam_chauthtok(GraphAuthPam* self, gint* err){
    GraphAuthPamPrivate* gapp = graph_auth_pam_get_instance_private(self);

    int e;
    pam_handle_t* pam_handle = NULL;


    *err = 0;

    pam_handle = gapp->pam_handle;
    if(pam_handle == NULL){
        g_printerr("Error getting PAM!\n" );
        return FALSE;
    }

    e = pam_chauthtok(pam_handle, NULL);
    if(e != PAM_SUCCESS){
        g_printerr("Error changing authentication token.\n ERROR  NUM %d\n PAM Error %s\n", e, pam_strerror(pam_handle, e));
        return FALSE;
    }
    return 0;
}
