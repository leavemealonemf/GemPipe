#define  _XOPEN_SOURCE
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<cjson/cJSON.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define BUFFER_SIZ 1024

char buffer[BUFFER_SIZ];
char main_gem[] = "gem_config.json";

char** read_gem(int *siz, char *r_addr, char *r_pass) {
    FILE *fp = fopen(main_gem, "r");

    fread(buffer, sizeof(*buffer), ARRAY_SIZE(buffer), fp);
    fclose(fp);

    cJSON *json = cJSON_Parse(buffer);
    cJSON *addr = cJSON_GetObjectItem(json, "addr");
    cJSON *pass = cJSON_GetObjectItem(json, "pass");
    cJSON *pipeline = cJSON_GetObjectItem(json, "pipeline");
    int pipeline_siz;
    

    if ((cJSON_IsString(addr)) && (addr->valuestring != NULL)) {
        sprintf(r_addr, "%s", addr->valuestring);
        printf("%s\n", addr->valuestring);
    }

    if ((cJSON_IsString(pass)) && (pass->valuestring != NULL)) {
        sprintf(r_pass, "%s", pass->valuestring);
        printf("%s\n", pass->valuestring);
    }

    char **cmds;

    if ((cJSON_IsArray(pipeline))) {
        pipeline_siz = cJSON_GetArraySize(pipeline);
        *siz = pipeline_siz;
        cmds = malloc(pipeline_siz * sizeof(char*));
        for (int i = 0; i < pipeline_siz; i++)
        {
            cJSON *item = cJSON_GetArrayItem(pipeline, i);
            if ((cJSON_IsString(item)) && (item->valuestring != NULL)) {
                cmds[i] = malloc(strlen(item->valuestring));
                strcpy(cmds[i], item->valuestring);
            }
        }
    }

    cJSON_Delete(json);

    return cmds;
}

int main(int argc, char *argv[])
{    

    int pipe_siz;
    char addr[50];
    char pass[50]; 
    char **cmds = read_gem(&pipe_siz, addr, pass);

    char ssh_root[150];
    sprintf(ssh_root, "sshpass -p %s ssh root@%s > log.txt", pass, addr);

    FILE *ssh_rem = popen(ssh_root, "w");
    if (!ssh_rem) {
        perror("popen ssh failed"); 
        exit(EXIT_FAILURE); 
    };

    for (int i = 0; i < pipe_siz; i++)
    {   
        fprintf(ssh_rem, cmds[i]);        
    }
    
    pclose(ssh_rem);

    printf("gem ok.\n");
    return EXIT_SUCCESS;
}