
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

#define HASH_SALT "rew8235483"
#define GRAPH_AUTH_TOKEN_FILE "/etc/graph_passwd"

//Binary flags for modes
enum file_modes{
    READ_TOKEN  = 1 << 0,
    WRITE_TOKEN = 1 << 1,
    WRITE_USER  = 1 << 2
};


#endif
