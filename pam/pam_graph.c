/**  pam_graph.c Copyright 2013 Reyad Attiyat
**
**   Licensed under the Apache License, Version 2.0 (the "License");
**   you may not use this file except in compliance with the License.
**   You may obtain a copy of the License at
**
**       http://www.apache.org/licenses/LICENSE-2.0
**
**   Unless required by applicable law or agreed to in writing, software
**   distributed under the License is distributed on an "AS IS" BASIS,
**   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**   See the License for the specific language governing permissions and
**   limitations under the License.
**/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <crypt.h>

#define PAM_SM_AUTH
#define PAM_SM_PASSWORD

#include <security/pam_modules.h>

#include "pam_graph.h"

typedef enum file_modes file_modes_t;

int add_user_list(plist_t* plist, const char* user, const char* pass){
	user_t* new_user = malloc(sizeof(user_t));
	user_t* curr_user;
	user_t* prev_user;
	
	new_user->name = user;
	new_user->password = (char*) pass;
	new_user->next = NULL;

	if(plist->head == NULL){
		plist->head = new_user;
		return 0;
	}

	for(curr_user = plist->head,
		prev_user = NULL; 

		curr_user != NULL;
		
		prev_user = curr_user,
		curr_user = curr_user->next)
	{
        if(strcmp(new_user->name, curr_user->name) < 0){
			if(prev_user != NULL){
				prev_user->next = new_user;
			}else{
				plist->head = new_user;
			}

			new_user->next = curr_user;
			return 0;
		}
	}

	//Append to end
	prev_user->next = new_user;

	return 0;
}

int write_user_list(plist_t* plist, FILE* fp){
	user_t* curr_user = NULL;
	
	fp = freopen(NULL, "w", fp);
	if(fp == NULL){
		return PAM_PERM_DENIED;
	}

	for(curr_user = plist->head; curr_user != NULL; curr_user = curr_user->next){
		fprintf(fp, "%s:%s\n", curr_user->name, curr_user->password);
	}


	return 0;
}

int access_token_file(const char* username, const char** password_ptr, file_modes_t mode){
	plist_t plist = {.head = NULL};

    int found = 0;
	int status = PAM_SUCCESS;

    //Password that we should write when in write mode
    const char* password = *password_ptr;


    FILE* fp = NULL;
    char line[512];
	
    //Check if we got username
    if(username == NULL || strlen(username) == 0){
        return PAM_AUTH_ERR;
    }

    fp = fopen(GRAPH_AUTH_TOKEN_FILE, "r");
    if(fp == NULL){
        printf("GRAPH AUTH: Error cannot open password file\n");
        return PAM_AUTH_ERR;
    }

    while(fgets(line, 512, fp) != NULL){
		int scan_status = 0;
		
		//Line username and password buffers
		const char* luser = malloc(255);
		char* lpassword = malloc(255);

        scan_status = sscanf(line, "%[^:]:%[^:\n]", luser, lpassword);
		if(scan_status == EOF){
			continue;
		}

        //Skip incomplete lines
        if(luser == NULL){
            continue;
        }

        if(strcmp(luser, username) == 0){
            /* Read mode we read the password thats on the line
             * and set password_ptr
             * Write mode we overwrite the current password with
             * the new one
             * Both of these modes can be set at the same time
             */
            if(mode & READ_TOKEN){
                *password_ptr = strndup(lpassword, 255);
            }

            if(mode & WRITE_TOKEN){
				//Change the password
				free(lpassword);
				lpassword = (char*) password;
            }
			found++;
        }

        if(mode & WRITE_TOKEN | WRITE_USER){
			add_user_list(&plist, luser, lpassword);
		}
    }
    
    //Check if user should be added
    //This will happen on write mode for token and user
	if(mode & (WRITE_TOKEN | WRITE_USER)){
		if(!found){
			add_user_list(&plist, username, password);
		}

		status = write_user_list(&plist, fp);
		if(status != PAM_SUCCESS){
			printf("Error could not write changes\n");
		}
	}else if(mode == READ_TOKEN){
		if(!found){
			status = PAM_USER_UNKNOWN;
		}
	}

    fclose(fp);

    //Check if we actually got a password when in read mode
    if(*password_ptr == NULL && (mode & READ_TOKEN)){
        status = PAM_AUTH_ERR;
    }

    return status; 
}

char* hash_plain_password(const char* in){
        return crypt(in, HASH_SALT); 
}

int add_user(const char* username, const char* password){
	int status = 0;

	password = hash_plain_password(password);
    status = access_token_file(username, &password, WRITE_TOKEN | WRITE_USER);

	return status;
}

int set_user_pass(const char* username, const char* password){
    int status = 0;

	password = hash_plain_password(password);
    status = access_token_file(username, &password, WRITE_TOKEN | WRITE_USER);

    return status;
}

int get_user_pass(const char* username, const char** password_ptr){
    int status = 0;

    status = access_token_file(username, password_ptr, READ_TOKEN);

    return status;
}

PAM_EXTERN int pam_sm_authenticate(pam_handle_t* pamh, int flags, int argc, const char** argv){

    int status = 0;
    const char* username = NULL;
    const char* auth_token = NULL;
    char *crypt_auth_token = NULL;
    const char* password = NULL;

    status =  pam_get_user(pamh, &username, NULL);
    if(status != PAM_SUCCESS){
        return status;
    }

    printf("GRAPH AUTH: USERNAME: %s\n", username);

    status = get_user_pass(username, &password);
    if(status != PAM_SUCCESS){
        return status;
    }
    
	printf("GRAPH AUTH: USER Pasword: %s\n", password);

	status =  pam_get_authtok(pamh, PAM_AUTHTOK, (const void**) &auth_token, "GraphAuthToken: ");
	if(status != PAM_SUCCESS){
		return PAM_AUTH_ERR;
	}


	printf("GRAPH AUTH: Auth Token: %s\n", auth_token);
	
	if(auth_token != NULL){
		crypt_auth_token = hash_plain_password(auth_token);
	}

	printf("GRAPH AUTH: Crypt Auth Token: %s\n", crypt_auth_token);

	if(crypt_auth_token == NULL || strncmp(password, crypt_auth_token, 255) != 0){
		printf("GRAPH AUTH: FAILED\n");
		return PAM_AUTH_ERR;
	}

	printf("GRAPH AUTH: SUCCESS\n");


    return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_setcred(pam_handle_t* pamh, int flags, int argc, const char** argv){
        return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt (pam_handle_t* pamh, int flags, int argc, const char** argv){
    return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_chauthtok(pam_handle_t* pamh, int flags, int argc, const char** argv){

    int status = PAM_SUCCESS;
    const char* username = NULL;
    const char* auth_token = NULL;
    const char* old_token = NULL;
    const char* new_token = NULL;
    char *crypt_auth_token = NULL;
    const char* password = NULL;

    status =  pam_get_user(pamh, &username, NULL);
    if(status != PAM_SUCCESS){
        return status;
    }

    printf("GRAPH AUTH: USERNAME: %s\n", username);

	if(flags & PAM_UPDATE_AUTHTOK){

		status = pam_get_item (pamh, PAM_OLDAUTHTOK, (const void**) &old_token);
		if(status != PAM_SUCCESS){
			return status;
		}

		if(old_token == NULL){
		
		}else{
			 printf("GRAPH AUTH: OLD AUTH TOKEN: %s\n", old_token);
		}

		status = pam_get_authtok_noverify(pamh, &new_token, "New patteren:"); 
		if(status != PAM_SUCCESS){
			return status;
		}
		
		status = pam_get_authtok_verify(pamh, &new_token, "retype new patteren:"); 
		if(status != PAM_SUCCESS){
			return status;
		}
		
		status = set_user_pass(username, new_token);
	}

    return status;
}
