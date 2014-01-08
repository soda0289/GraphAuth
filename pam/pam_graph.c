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

int get_user_pass(const char* username, const char** password_ptr){
    const char* password = NULL;
    const char* luser = NULL;
    const char* lpassword = NULL;
    FILE *fp = NULL;
    char line[512];

    luser = malloc(255);
    lpassword = malloc(255);


    fp = fopen("/etc/graph_passwd", "r");
    if(fp == NULL){
        printf("GRAPH AUTH: Error cannot open password file\n");
        return PAM_AUTH_ERR;
    }

    while(fgets(line, 512, fp) != NULL){
        sscanf(line, "%[^:]:%[^:\n]", luser, lpassword);
        if(strcmp(luser, username) == 0){
            password = strndup(lpassword,255);
            fclose(fp);
            break;
        }
    }

    if(password != NULL){
        *password_ptr = password;
        return PAM_SUCCESS; 
    }else{
        return PAM_AUTH_ERR;
     }
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

    status =  pam_get_authtok(pamh, PAM_AUTHTOK, (const void**) &auth_token, "Password: ");
    if(status != PAM_SUCCESS){
        int ret;
        struct pam_conv* conv;
        const struct pam_message* pam_msgs = NULL;
        struct pam_response* pam_resps = malloc(1 * sizeof(struct pam_response));
        struct pam_message pam_msg = {
            .msg_style = PAM_PROMPT_ECHO_OFF,
            .msg = "auth_token"
        };

        pam_msgs = &pam_msg;

        ret = pam_get_item(pamh, PAM_CONV, (void*)&conv);
        if(ret != PAM_SUCCESS){
            return ret;
        }

        //Call PAM conversation function in application
        if(conv->conv == NULL){
            return -33;
        }

        ret = conv->conv(1, &pam_msgs, &pam_resps, conv->appdata_ptr);
        if(ret != PAM_SUCCESS){
            return ret;
        }

        printf("GRAPH AUTH: Got conversation response %s\n", pam_resps[0].resp);

        if(pam_resps[0].resp != NULL){
            auth_token = strndup(pam_resps[0].resp, strlen(pam_resps[0].resp));
        }
    }

    printf("GRAPH AUTH: Auth Token: %s\n", auth_token);

    if(auth_token != NULL){
        crypt_auth_token = crypt(auth_token, "rew8235483"); 
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

PAM_EXTERN int pam_sm_chauthtok(pam_handle_t* pamh, int flags, int argc, const char** argv){
    return PAM_AUTHTOK_ERR;
}
