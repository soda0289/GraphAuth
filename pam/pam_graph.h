
#ifndef PAM_GRAPH_H
#define PAM_GRAPH_H

typedef struct user_t_ user_t;
typedef struct plist_t_ plist_t;

struct user_t_{
	const char* name;
	char* password;
	user_t* next;
};

struct plist_t_{
	user_t* head;
};


#endif
