/**
 * @file logger.h
 * @author Gabriele Serra
 * @date 19 Nov 2018
 * @brief Redirect log string toward terminal, syslog or nothing 
 *
 * This file contains the definition of LOG macro. LOG macro is
 * a wrapper for print operation, and it is thought for LOG print.
 * Changing the define, the print operation is re-directed towards
 * another output.
 */

#ifndef LOGGER_H
#define LOGGER_H

/**
 * @brief Represent the current choice
 * 
 * Represent the current choice. Change
 * this define to redirect output toward
 * another channel 
 */
#define LOG_DEBUG

/**
 * @brief Define the different choices
 * 
 * Define the three different choices,
 * terminal, syslogs or nothing. Please
 * note that stdio.h or syslog.h must be
 * included. Note also that in case of syslog,
 * it must be open before.
 * 
 * @param str string to be printed
 * @param args variable number of args
 */
#ifdef LOG_DEBUG
    #define LOG(str, args...) printf(str, ##args)
#elif LOG_SYS
    #define LOG(str, args...) syslog(LOG_DAEMON, str, args)
#else
    #define LOG(str, args...) 
#endif

#endif

