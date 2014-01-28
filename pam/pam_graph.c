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

#define HASH_SALT "rew8235483"
#define GRAPH_AUTH_TOKEN_FILE "/etc/graph_passwd"

//Binary flags for modes
enum file_modes{
    READ_TOKEN  = 1 << 0,
    WRITE_TOKEN = 1 << 1,
    WRITE_USER  = 1 << 2
};

typedef enum file_modes file_modes_t;

int access_token_file(const char* username, const char** password_ptr, file_modes_t mode){
    int found = 0;

    //Password that we should write when in write mode
    const char* password = *password_ptr;

    //Line username and password buffers
    const char* luser = malloc(255);
    const char* lpassword = malloc(255);

    FILE* fp = NULL;
    char line[512];
	//File access string.
	//r  - for read only
	//a+ - for append and read
	char access_string[3];
	
	//Starting byte
	long int start = 0;
 
    //Check if we got username
    if(username == NULL || strlen(username) == 0){
        return PAM_AUTH_ERR;
    }

	//If mode is only read access string is "r"
	if(mode == READ_TOKEN){
		strncpy(access_string, "r", 1);
	}else{
		strncpy(access_string, "a+", 2);
	}

    fp = fopen(GRAPH_AUTH_TOKEN_FILE, access_string);
    if(fp == NULL){
        printf("GRAPH AUTH: Error cannot open password file\n");
        return PAM_AUTH_ERR;
    }

    while(fgets(line, 512, fp) != NULL){
        sscanf(line, "%[^:]:%[^:\n]", luser, lpassword);
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
                *password_ptr = password = strndup(lpassword,255);
            }
            if(mode & WRITE_TOKEN){
                fseek(fp, strlen(luser) + 1, start);
                fputs(password, fp);
            }
            found++;
            break;
        }
		//Start of next line is current position ( ftell(fp) ) plus 
		//one byte.
		start = ftell(fp) + 1;
    }
    
    //Check if user should be added
    //This will happen on write mode for token and user
    if(!found && (mode & (WRITE_TOKEN & WRITE_USER))){
       fprintf(fp, "%s:%s", username, password); 
    }

    fclose(fp);

    //Check if we actually got a password when in read mode
    if(password == NULL && (mode & READ_TOKEN)){
        return PAM_AUTH_ERR;
    }

    return PAM_SUCCESS; 
}

char* hash_plain_password(const char* in){
        return crypt(in, HASH_SALT); 
}

int set_user_pass(const char* username, const char* password){
    int status = 0;
	password = hash_plain_password(password);
    status = access_token_file(username, &password, WRITE_TOKEN);

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

    int status = 0;
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

    status = pam_get_item (pamh, PAM_OLDAUTHTOK, (const void**) &old_token);
    if(status != PAM_SUCCESS){
        return status;
    }

    printf("GRAPH AUTH: OLD AUTH TOKEN: %s\n", old_token);
    
    status = pam_get_authtok_noverify(pamh, &new_token, "New patteren:"); 
    if(status != PAM_SUCCESS){
        return status;
    }
    
    status = pam_get_authtok_verify(pamh, &new_token, "Reenter new patteren:"); 
    if(status != PAM_SUCCESS){
        return status;
    }
    
    status = set_user_pass(username, new_token);
    return status;
}
