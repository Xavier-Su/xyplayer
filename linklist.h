#ifndef __LINKLIST_H_
#define __LINKLIST_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



typedef struct
{
    /* data */

    int time;
    char lrc_text[128];
} LRC;

typedef struct
{
    /* data */
    // 当数据域中有参数为数组时需要使用strcpy豳数进行赋值
    // 当数据域中的参数为name时可以直接给字符串常量,但是修改值时需要时候strcpy函数

    int id;
    char name[32];
    int score;
} MSG;

typedef struct node
{
    // 数据域
    // MSG data;
    LRC data;

    struct node *next;

} LinkList;
static int lrc_now_row=0;

/*
 * 功能：创建一个新的链表
 * 参数：无
 * 返回值：新链表的头指针
 * 作者：蒋伟伦
 */
LinkList *LinkListCreate();

/*
 * 功能：释放链表的内存空间
 * 参数：链表的头指针
 * 返回值：无
 * 作者：蒋伟伦
 */
void LinkListFree(LinkList *head);

/*
 * 功能：在链表头插入一条数据
 * 参数：链表的头指针，待插入的数据
 * 返回值：无
 * 作者：蒋伟伦
 */
void LinkListInsertByHead(LinkList *head, LRC msg);

/*
 * 功能：在链表尾插入一条数据
 * 参数：链表的头指针，待插入的数据
 * 返回值：无
 * 作者：蒋伟伦
 */
void LinkListInsertByTail(LinkList *head, LRC msg);

/*
 * 功能：删除链表头的一条数据
 * 参数：链表的头指针
 * 返回值：无
 * 作者：蒋伟伦
 */
void LinkListDeleteByHead(LinkList *head);

/*
 * 功能：删除链表尾的一条数据
 * 参数：链表的头指针
 * 返回值：无
 * 作者：蒋伟伦
 */
void LinkListDeleteByTail(LinkList *head);

/*
 * 功能：显示链表中的所有数据
 * 参数：链表的头指针
 * 返回值：无
 * 作者：蒋伟伦
 */
void ShowList(LinkList *head);

/*
 * 功能：根据时间查找链表中的歌词文本
 * 参数：链表的头指针，时间
 * 返回值：该时间对应的歌词文本
 * 作者：蒋伟伦
 */
char* FindTextByTime(LinkList *head, int time);

/*
 * 功能：返回链表中当前播放位置的下一条歌词文本
 * 参数：链表的头指针
 * 返回值：下一条歌词文本
 * 作者：蒋伟伦
 */
char* NextDataByNode(LinkList *head);

/*
 * 功能：查找链表中指定步长后的歌词文本
 * 参数：链表的头指针，步长
 * 返回值：指定步长后的歌词文本
 * 作者：蒋伟伦
 */
char *SeekStepText(LinkList *head, int step);

/*
 * 功能：获取链表中当前播放的歌词所在行号
 * 参数：无
 * 返回值：当前播放的歌词所在行号
 * 作者：蒋伟伦
 */
int GetLrcNowRow();


#endif


