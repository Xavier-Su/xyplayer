
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>

#include "lrc.h"
#include "linklist.h"

LinkList *lrc_node;
#define LRC_YES 1
#define LRC_NO 0


map_t *get_lrc_map(char *filename)
{

    regex_t re_text;
    char line[MAX_LINE_LENGTH] = "";
    regmatch_t matches[2];

    int ret_text;

    // 编译正则表达式

    ret_text = regcomp(&re_text, "\\[[0-9]{2}:[0-9]{2}\\.[0-9]{3}\\] *(.*)", REG_EXTENDED);
    if (ret_text != 0)
    {
        fprintf(stderr, "Failed to compile regex\n");
        return NULL;
    }


    map_t *map = map_create();

    char time_min[10];
    char time_sec[10];
    char lyric[MAX_LINE_LENGTH];
    int i, j, k;

    // 打开歌词文件

    char lrc_path[128] = "";
    char *ext;
    ext = strchr(filename, '.'); // 获取文件后缀的点号
    if (ext == NULL)
    {
        printf("File name format error!\n");
    }
    strncpy(lrc_path, filename, ext - filename); // 复制点号之前的字符串
    lrc_path[ext - filename] = '\0';             // 添加字符串结束符
    strcat(lrc_path, ".lrc");                    // 添加新的文件后缀
    printf("%s\n", lrc_path);                    // 打印新的文件名
    FILE *fp = fopen(lrc_path, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Could not open file\n");
        return NULL;
        // exit(EXIT_FAILURE);
    }

    // 逐行读取并提取时间和歌词
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL)
    {
        // 查找时间
        for (i = 0; i < strlen(line); i++)
        {
            if (line[i] == '[')
            {
                k = 0;
                for (j = i + 1; j < strlen(line) && line[j] != ':'; j++)
                {
                    time_min[k++] = line[j];
                }
                time_min[k] = '\0';

                break;
            }
        }
        for (i = 0; i < strlen(line); i++)
        {
            if (line[i] == ':')
            {
                k = 0;
                for (j = i + 1; j < strlen(line) && line[j] != '.'; j++)
                {
                    time_sec[k++] = line[j];
                }
                time_sec[k] = '\0';

                break;
            }
        }

        // 查找歌词
        for (; i < strlen(line); i++)
        {
            if (line[i] == ']')
            {
                k = 0;
                for (j = i + 1; j < strlen(line) && line[j] != '\n'; j++)
                {
                    lyric[k++] = line[j];
                }
                lyric[k] = '\0';
                break;
            }
        }

        // 打印时间和歌词
        int num_min = atoi(time_min);
        int num_sec = atoi(time_sec);
        int time_now = num_min * 60 + num_sec;

        int i = 0;
        while (isspace(lyric[i]))
        { // 检查开头的空格
            i++;
        }
        char *trimmed_str = lyric + i;

        char str_time[6];
        char str_text[128];
        sprintf(str_time, "%d", time_now);
        sprintf(str_text, "%s", trimmed_str);
        // printf("text==%s\n", str_text);
        map_put(map, str_time, str_text);
    }

    // 关闭文件
    fclose(fp);
    regfree(&re_text);

    return map;
}

int get_lrc_linklist(char *filename)
{

    regex_t re_text;
    char line[MAX_LINE_LENGTH] = "";
    regmatch_t matches[2];

    int ret_text;

    // 编译正则表达式

    ret_text = regcomp(&re_text, "\\[[0-9]{2}:[0-9]{2}\\.[0-9]{3}\\] *(.*)", REG_EXTENDED);
    if (ret_text != 0)
    {
        fprintf(stderr, "Failed to compile regex\n");
        // return NULL;
    }

    LRC lrc = {0, ""};
    lrc_node = LinkListCreate();

    char time_min[10] = "";
    char time_sec[10] = "";
    char lyric[MAX_LINE_LENGTH] = "";
    int i, j, k;

    char lrc_path[128] = "";
    char *ext;
    ext = strchr(filename, '.'); // 获取文件后缀的点号
    if (ext == NULL)
    {
        printf("File name format error!\n");
        return LRC_NO;
    }
    strncpy(lrc_path, filename, ext - filename); // 复制点号之前的字符串
    lrc_path[ext - filename] = '\0';             // 添加字符串结束符
    strcat(lrc_path, ".lrc");                    // 添加新的文件后缀


    FILE *fp = fopen(lrc_path, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Could not open file\n");
        printf("没有歌词\n");
        return LRC_NO;
     
    }

    // 逐行读取并提取时间和歌词

    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL)
    {
        // 查找时间
        for (i = 0; i < strlen(line); i++)
        {
            if (line[i] == '[')
            {
                k = 0;
                for (j = i + 1; j < strlen(line) && line[j] != ':'; j++)
                {
                    time_min[k++] = line[j];
                }
                time_min[k] = '\0';

                break;
            }
        }
        for (i = 0; i < strlen(line); i++)
        {
            if (line[i] == ':')
            {
                k = 0;
                for (j = i + 1; j < strlen(line) && line[j] != '.'; j++)
                {
                    time_sec[k++] = line[j];
                }
                time_sec[k] = '\0';

                break;
            }
        }

        // 查找歌词
        for (i = 0; i < strlen(line); i++)
        {
            if (line[i] == ']')
            {
                k = 0;
                for (j = i + 1; j < strlen(line) && line[j] != '\n'; j++)
                {
                    lyric[k++] = line[j];
                }
                lyric[k] = '\0';
                break;
            }
        }

        // 打印时间和歌词
        int num_min = atoi(time_min);
        int num_sec = atoi(time_sec);
        int time_now = num_min * 60 + num_sec;

        int i = 0;
        while (isspace(lyric[i]))
        { // 检查开头的空格
            i++;
        }
        char *trimmed_str = lyric + i;


        if ((time_now >= 0) && (trimmed_str != NULL))
        {
            lrc.time = time_now;
            strcpy(lrc.lrc_text, trimmed_str);

            LinkListInsertByTail(lrc_node, lrc);
        }
    }

 
    //  关闭文件

    fclose(fp);
    regfree(&re_text);
    return LRC_YES;
}

void GetAndDealLrc(char *file_path)
{

    char file_name[128];
    char *ext;
    ext = strchr(file_path, '.'); // 获取文件后缀的点号
    if (ext == NULL)
    {
        printf("File name format error!\n");
    }
    strncpy(file_name, file_path, ext - file_path); // 复制点号之前的字符串
    file_name[ext - file_path] = '\0';              // 添加字符串结束符
    strcat(file_name, ".lrc");                      // 添加新的文件后缀
    printf("%s\n", file_name);                      // 打印新的文件名

    // 第一步：打开文件

    FILE *fp = fopen(file_name, "r");
    if (fp == NULL)
    {
        perror("fopen");
    }

    // 创建一个空链表
    lrc_node = LinkListCreate();
    // 歌词处理
    char buf[128] = "";
    char *str_time = NULL;
    char *str_lrc = NULL;
    LRC lrc_msg;
    while (fgets(buf, sizeof(buf), fp) != NULL)
    {

        // 将每一行的内容进行拆分
        // 时间
        str_time = buf;
        if (*str_time == '[')
        {
            int m = 0, s = 0;
            // 将括号中的时间(分钟和秒钟)解析出来
            sscanf(str_time, "[%d:%d.64]", &m, &s);
            // 转化单位
            int time = m * 60 + s;
            lrc_msg.time = time;
            // printf("time :%d\n",time);
        }

        // 歌词
        str_lrc = buf;
        if (*str_lrc == '[')
        {
            if (strncmp(str_lrc, "[0", strlen("[0")) != 0)
            {
                // 非歌词内容
                strcpy(lrc_msg.lrc_text, str_lrc);
            }
            // 歌词内容
            else
            {
                // 将指针往后移十个单位则是歌词的首地址
                str_lrc = str_lrc + 10;
                strcpy(lrc_msg.lrc_text, str_lrc);
            }
        }
        // 使用尾插法将对应的数据插入链表中
        LinkListInsertByTail(lrc_node, lrc_msg);
    }

    fclose(fp);
}
