//
// Created by ilanit on 7/11/24.
//
#ifndef NEWSITEM_H
#define NEWSITEM_H

// create enum for the type of news
typedef enum {
    sports,
    weather,
    news
} newsType;

// a struct to hold a newsItem
typedef struct {
    int id;
    newsType type;
    int numOfType;
} newsItem;

#endif