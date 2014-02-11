#include <gjs/gjs.h>

int
main (int argc, char** argv){
	int status = 0;
	GError *error = NULL;
	const char *search_path[] = { "resource:///ca/reyad/GraphAuth", NULL };
	
	GjsContext* gjs_context = g_object_new (GJS_TYPE_CONTEXT,
											"search-path", search_path,
											NULL);
	
	if (!gjs_context_eval (gjs_context,
						   "const Main = imports.passwd;",
						   -1,
						   "<main>",
						   &status,
						   &error)) 
	{
		g_message ("Execution of Graph Auth's passwd.js threw exception: %s", error->message);
		g_error_free (error);
		return status;
	}

	return 0;
}
