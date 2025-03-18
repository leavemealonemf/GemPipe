#define  _XOPEN_SOURCE
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#define _DEFAULT_SOURCE
#include<dirent.h>
#include<cjson/cJSON.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define BUFFER_SIZ 1024

char buffer[BUFFER_SIZ];
char main_gem[] = "gem_config_2.json";
char cfg_dir[] = "./gems";

int get_gemdir_len() {
    DIR *dir = opendir(cfg_dir);
    struct dirent *d;
    int len = 0;

    while (d = readdir(dir))
    {
        if (d->d_type == 8) {
            len += 1;
        }
    }

    closedir(dir);
    return len;    
}

char** read_config_dir(int gems_siz) {
    DIR *dir = opendir(cfg_dir);
    struct dirent *d;
    
    char **gems;

    if (dir) {
        gems = malloc(gems_siz * sizeof(char*));
        int offset = 0;

        while (d = readdir(dir))
        {
            if (d->d_type == 8) {
                gems[offset] = malloc(strlen(d->d_name));
                strcpy(gems[offset], d->d_name);
                offset += 1;
            }
        }

        closedir(dir);    
    }

    return gems;
}

char** read_gem(int *siz, char *r_addr, char *r_pass, char *gem) {
    char full_path[100];
    sprintf(full_path, "gems/%s", gem);

    FILE *fp = fopen(full_path, "r");

    fread(buffer, sizeof(*buffer), ARRAY_SIZE(buffer), fp);
    fclose(fp);

    cJSON *json = cJSON_Parse(buffer);
    cJSON *addr = cJSON_GetObjectItem(json, "addr");
    cJSON *pass = cJSON_GetObjectItem(json, "pass");
    cJSON *pipeline = cJSON_GetObjectItem(json, "pipeline");
    int pipeline_siz;
    
    if ((cJSON_IsString(addr)) && (addr->valuestring != NULL)) {
        sprintf(r_addr, "%s", addr->valuestring);
    }

    if ((cJSON_IsString(pass)) && (pass->valuestring != NULL)) {
        sprintf(r_pass, "%s", pass->valuestring);
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

int main(int argc, char **argv)
{    

    if (argc < 2) {
        printf("Too few arguments.\nPrint -h or --help to get help.\n");
        return EXIT_FAILURE;
    }

    if ((strcmp(argv[1], "--help") == 0) || (strcmp(argv[1], "-h")) == 0) {
        printf("GEM HELP:\nFirst of all you need to create a config in \"gem\" folder.\nThen you can run program. Provide a config number to execute program with it.\nFor example: pipe 1\n\nMore information here: https://github.com/leavemealonemf/GemPipe\n");
        exit(EXIT_SUCCESS);
    }


    int gems_len = get_gemdir_len();
    char **gems = read_config_dir(gems_len);
    
    int gem_num = atoi(argv[1]);

    if (gem_num == 0) {
        printf("Can't find provided number.\n");
        return EXIT_FAILURE;
    }

    char temp_gem_preview[100];
    char current_gem[100];
    unsigned int is_find = 0;

    sprintf(temp_gem_preview, "gem_config_%d.json", gem_num);

    for (int i = 0; i < gems_len; i++)
    {   
        if ((strcmp(temp_gem_preview, gems[i])) == 0) {
            sprintf(current_gem, "%s", gems[i]);
            is_find = 1;
            break;
        }
        free(gems[i]);
    }
    
    free(gems);

    if (is_find == 0) {
        printf("failed to find a gem with num: %d\n", gem_num);
        return EXIT_FAILURE;
    }

    int pipe_siz;
    char addr[50];
    char pass[50];

    char **cmds = read_gem(&pipe_siz, addr, pass, current_gem);

    char ssh_root[150];
    sprintf(ssh_root, "sshpass -p %s ssh root@%s > log.txt", pass, addr);

    FILE *ssh_rem = popen(ssh_root, "w");
    if (!ssh_rem) {
        perror("popen ssh failed"); 
        exit(EXIT_FAILURE); 
    };

    for (int i = 0; i < pipe_siz; i++)
    {   
        printf("RUN CMD: %s\n", cmds[i]);
        fprintf(ssh_rem, cmds[i]);
        free(cmds[i]);
    }
    
    free(cmds);

    pclose(ssh_rem);

    printf("gem ok.\n");
    return EXIT_SUCCESS;
}