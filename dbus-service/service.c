#include <dbus/dbus-glib.h>
#include <stdlib.h>
#include <stdio.h>
#include "graph_auth.h"
#include "ga_dbus.h"
#include <girepository.h>

int
main (int argc, char* argv[]){
    GraphAuth* Auth;
    GMainLoop* mainloop = NULL;
    DBusGProxy* busProxy = NULL;
    DBusGConnection* bus = NULL;

    GError* error = NULL;
    
    guint result = 0;
    GOptionContext *ctx;

    ctx = g_option_context_new (NULL);
    g_option_context_add_group (ctx, g_irepository_get_option_group ());  

    if (!g_option_context_parse (ctx, &argc, &argv, &error)) {
        g_print ("greeter: %s\n", error->message);
        return 1;
    }

    mainloop = g_main_loop_new(NULL, FALSE);
    if (mainloop == NULL) {
        /* Print error and terminate. */
        printf("Couldn't create GMainLoop");
        return -2;
    }


    bus = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
    if (error != NULL) {
        /* Print error and terminate. */
        printf("Couldn't connect to session bus\n %s", error->message);
        return -1;
    }

    busProxy = dbus_g_proxy_new_for_name(bus,
                                         DBUS_SERVICE_DBUS,
                                         DBUS_PATH_DBUS,
                                         DBUS_INTERFACE_DBUS);
    if (busProxy == NULL) {
        printf("Failed to get a proxy for D-Bus \n Unknown(dbus_g_proxy_new_for_name)");
    }
    /* Attempt to register the well-known name.
     * The RPC call requires two parameters:
     * - arg0: (D-Bus STRING): name to request
     * - arg1: (D-Bus UINT32): flags for registration.
     * (please see "org.freedesktop.DBus.RequestName" in
     * http://dbus.freedesktop.org/doc/dbus-specification.html)
     *       Will return one uint32 giving the result of the RPC call.
     *We're interested in 1 (we're now the primary owner of the name)
     *or 4 (we were already the owner of the name, however in this
     *application it wouldn't make much sense).
     */
    if (!dbus_g_proxy_call(busProxy,
                             /* Method name to call. */
                             "RequestName",
                             /* Where to store the GError. */
                             &error,
                             /* Parameter type of argument 0. Note that
                              * since we're dealing with GLib/D-Bus
                              * wrappers, you will need to find a suitable
                              * GType instead of using the "native" D-Bus
                              * type codes. */
                             G_TYPE_STRING,
                             /* Data of argument 0. In our case, this is
                              * the well-known name for our server
                              * example ("org.maemo.Platdev_ex"). */
                             VALUE_SERVICE_NAME,
                             /* Parameter type of argument 1. */
                             G_TYPE_UINT,
                             0,
                             G_TYPE_INVALID,
                             G_TYPE_UINT,
                             &result,
                             G_TYPE_INVALID)) {
        printf("ERROR: D-Bus.RequestName RPC failed %s", error->message);
    }

    if (result != 1) 
        printf("Failed to get the primary well-known name.RequestName result != 1");

    Auth = graph_auth_new();
    if (Auth == NULL) {
        printf("Failed to create one Value instance.");
    }
    dbus_g_connection_register_g_object(bus,
                                        VALUE_SERVICE_OBJECT_PATH,

                                         G_OBJECT(Auth));
    if(argc < 2)
        g_main_loop_run(mainloop); 

    g_object_unref(Auth);


    return 0;
}
