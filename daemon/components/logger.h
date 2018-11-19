/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   logger.h
 * Author: root
 *
 * Created on November 19, 2018, 12:56 AM
 */

#ifndef LOGGER_H
#define LOGGER_H

#define LOG_DEBUG

#ifdef LOG_DEBUG
    #define LOG(str, args...) printf(str, ##args)
#elif LOG_SYS
    #define LOG(str, args...) syslog(LOG_DAEMON, str, args)
#else
    #define LOG(str, args...) 
#endif

#endif /* LOGGER_H */

