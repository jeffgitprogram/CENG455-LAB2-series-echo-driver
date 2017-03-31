/*
 * access_function.h
 *
 *  Created on: Feb 18, 2017
 *      Author: lijiefei
 */

#ifndef SOURCES_ACCESS_FUNCTION_H_
#define SOURCES_ACCESS_FUNCTION_H_

#include "handler_queue.h"
#include "os_tasks.h"


extern int call_openw_flag;
extern int call_openr_flag;
extern int call_putline_flag;
extern int find_write_privilege_flag;
extern int find_read_privilege_flag;
extern int getline_ready_flag;
extern int task_close_flag;


void task_configuration(unsigned int  task_init_data);

bool OpenR(uint16_t stream_no,int index);

//int find_write_privilege();

//int find_read_privilege();

bool _getline(char *string,int index);

bool find_id(uint16_t stream_no);

int find_index(int stream_no);

_queue_id OpenW(int index);

bool _putline(_queue_id qid, char *string,int index);

bool Close(int index);
void delay(int val);
#endif /* SOURCES_ACCESS_FUNCTION_H_ */
