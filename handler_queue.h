#ifndef __server_h__
#define __server_h__
/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 1989-2008 ARC International
*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale MQX RTOS License
* distributed with this Material.
* See the MQX_RTOS_LICENSE file distributed for more details.
*
* Brief License Summary:
* This software is provided in source form for you to use free of charge,
* but it is not open source software. You are allowed to use this software
* but you cannot redistribute it or derivative works of it in source form.
* The software may be used only in connection with a product containing
* a Freescale microprocessor, microcontroller, or digital signal processor.
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains definitions for this application
*
*
*END************************************************************************/

#include <mqx.h>
#include <message.h>
/*Global Parameter*/
//#define SUCCESS 1
//#define FAILURE 0
#define TASKMAX 5
#define MAXLINELENGTH 50

/* Number of clients */
#define NUM_USERS           (10)
#define FP_SYSTEM_QUEUE_BASE   2

/* Task IDs */
#define HANDLER_TASK       5
#define ISR_TASK       6
#define USER_TASK      7

/* Queue */
#define HANDLER_QUEUE		8 	// In handler, receive message from ISR, only one
#define ISR_QUEUE			9	// In ISR, for sending message to handler, only one
#define HANDLER_W_QUEUE 	10	// In handler, to receive messagen from queue PUTLINE_QUEUE_BASE only one
#define OPENW_QUEUE 		11	//in handler, to exchange message with queue USER_QUEUE_W_BASE only one
#define CHECK_QUEUE 		12	//in handler, to exchange message with queue FIND_WRITE_P_QUEUE only one
#define OPENR_QUEUE         13// In handler, used for receiving command message from openR()
#define CHECK_READ_QUEUE 	14//In handler task, used for receiving command massage for getline()
#define CLOSE_QUEUE         15// In hander task, used for receiving command message for revoke privilege
#define USER_OPENR_QUEUE_BASE 16// In user task, OPENR() used it to send command message
#define FIND_READ_P_QUEUE	20//In user task, used for receiving command message from handler
#define PUTLINE_QUEUE_BASE	24	// ****incremental*** create in openW(), then pass to _putline(), to exchange message with handler
#define USER_QUEUE_R_BASE 	28	// User task queue *****incremental******
#define USER_QUEUE_W_BASE 	32	// ****incremental***in openW() queue, to exchange message with queue OPENW_QUEUE
#define FIND_WRITE_P_QUEUE 	36	// in find_write_privilege(), to exchange message with queue CHECK_QUEUE
#define CLOSE_CMD__BASE 	40  //in user task, used for sending cmd message to handler to revoke the R/W privilege
/* This structure contains a data field and a message header structure */
typedef struct handler_message
{
   MESSAGE_HEADER_STRUCT   HEADER;
   unsigned char DATA[MAXLINELENGTH];
} HANDLER_MESSAGE, * HANDLER_MESSAGE_PTR;//For sending char array type data

typedef struct int_message
{
	MESSAGE_HEADER_STRUCT   HEADER;
	unsigned int   DATA;
	unsigned int	DATA2;
}INT_MESSAGE, *INT_MESSAGE_PTR;//for sending int  pointer data

/*typedef struct pointer_message
{
	MESSAGE_HEADER_STRUCT   HEADER;
	unsigned char*          DATA;
}POINTER_MESSAGE, *POINTER_MESSAGE_PTR;//for sending char* type pointer data*/

/* Function prototypes */

extern _pool_id array_pool;
//extern _pool_id pointer_pool;
extern _pool_id int_pool;

//extern _queue_id handler_r_qid;//This Queue is for reading data from ISR
//extern _queue_id handler_w_qid;//This Queue is for writing data to getline();
extern char textmsg[MAXLINELENGTH];
extern char sendmsg[MAXLINELENGTH];


#endif
/* EOF */
