#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linklist.h"

// 空头节点单链表
LinkList *LinkListCreate()
{
    // 去内存堆区空间开辟节点
    LinkList *head = (LinkList *)malloc(sizeof(LinkList));
    // 指针域指向下一个节点的地址
    head->next = NULL;
    // 返回头节点地址
    return head;
}

void LinkListFree(LinkList *head)
{
    // 第一步整个链表判空
    if (head->next == NULL)
    {
        free(head);
        printf("头节点释放。\n");
    }
    // 第二步，用指针保存下一个节点地址然后释放
    else
    {
        // p is next point
        LinkList *p = head->next;
        while (p != NULL)
        {
            //
            p = head->next;
            // 释放当前的值
            free(head);
            // 让下节点变为头节点
            head = p;
        }
        printf("链表释空\n");
    }
}
// 头插法
void LinkListInsertByHead(LinkList *head, LRC msg)
{
    // 创节点
    LinkList *temp = (LinkList *)malloc(sizeof(LinkList));
    // 数据域赋值
    temp->data = msg;

    // 头节点的指针域赋值给当前节点
    temp->next = head->next;
    // temp地址给头节点
    head->next = temp;
    // printf("头插法成功,time=%d,lrc=%s\n",msg.time,msg.lrc_text);
}

// 尾插法
void LinkListInsertByTail(LinkList *head, LRC msg)
{

    LinkList *temp = (LinkList *)malloc(sizeof(LinkList));
    // 给新尾节点数据域赋值
    temp->data = msg;
    // 将指针域置为NULL
    temp->next = NULL;

    // 遍历链表，找到最后一个节点
    LinkList *p = head;
    while (p->next != NULL)
    {
        p = p->next;
    }
    // 将新尾节点插入到链表尾部
    p->next = temp;
    // printf("尾插法放入数据成功。\n");
}

// 头删除法
void LinkListDeleteByHead(LinkList *head)
{
    if (head->next != NULL)
    {
        LinkList *p = head->next;
        head->next = p->next;
        free(p);
        p = NULL;
    }
}
// 尾删除法
void LinkListDeleteByTail(LinkList *head)
{
    // 第一步整个链表判空
    if (head->next == NULL)
    {
        printf("链表为空，无法删除。\n");
    }
    // 第二步，判断是否只有一个节点
    else if (head->next->next == NULL)
    {
        free(head->next);
        head->next = NULL;
        printf("尾删法删除成功。\n");
    }
    // 第三步，找到最后一个节点并删除
    else
    {
        LinkList *p = head->next;
        while (p->next->next != NULL)
        {
            p = p->next;
        }
        free(p->next);
        p->next = NULL;
        printf("尾删法删除成功。\n");
    }
}

void ShowList(LinkList *head)
{
    LinkList *p = head->next;
    while (p != NULL)
    {                // 遍历到链表结尾
        p = p->next; // 将指针指向下一个节点
        // 对当前节点进行操作，如输出节点值
        printf("time: %d, lrc_text: %s\n", p->data.time, p->data.lrc_text);
    }
}

char *FindTextByTime(LinkList *head, int time)
{
    if (head == NULL)
    {
        char *no = "no_lrc";

        return no;
    }
    lrc_now_row=-1;
    LinkList *p = head->next;
    while (p != NULL)
    {
        lrc_now_row++;
        if (p->data.time == time)
        {
            return p->data.lrc_text;
        }
        
        p = p->next;
    }

    return NULL;
}

char *NextDataByNode(LinkList *head)
{
    if (head == NULL)
    {
        char *no = "no_lrc";

        return no;
    }
    LinkList *p = head->next;

    if (p->data.lrc_text != NULL)
    {
        return p->data.lrc_text;
    }


    return NULL;
}

char *SeekStepText(LinkList *head, int step)
{
    {
        if (head == NULL)
        {
            char *no = "no_lrc";

            return no;
        }
        LinkList *p = head->next;
        int i=0;
        for (i = 0; i < step; i++)
        {
            p = p->next;
        }

        if ((p!=NULL)&&(p->data.lrc_text != NULL))
        {
            return p->data.lrc_text;
        }

        return NULL;
    }
}

int GetLrcNowRow()
{
    return lrc_now_row;
}
