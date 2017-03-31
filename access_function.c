/*
 * access_function.c

 *
 *  Created on: Feb 18, 2017
 *      Author: lijiefei
 */
#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"
#include "handler_queue.h"
#include "access_function.h"
#include <mqx.h>
#include <message.h>
#include <mutex.h>



// int PUTLINE_FLAG;

 //char sendmsg[MAXLINELENGTH];
 _queue_id userqidsaver[TASKMAX] = {0};
 int readprivilege[TASKMAX]={0};
// char textmsg[MAXLINELENGTH] = { '\0'};


//Initialize globe flag
int call_openw_flag = 0;
int find_write_privilege_flag = 0;
int call_putline_flag = 0;
int call_openr_flag = 0;
int find_read_privilege_flag = 0;
int getline_ready_flag = 0;
int task_close_flag = 0;


/*
 * This function returns the task index number with read privilege
 * If the task has no read permission, then return index 0
 */


/*
 * This function return the task id with the write privilege
 *
 */
int find_write_privilege(int index){

	INT_MESSAGE_PTR check_write_privilege_msg_ptr;
	_queue_id temp_qid;
	temp_qid = _msgq_open((_queue_number)FIND_WRITE_P_QUEUE+index-1 , 0);
	check_write_privilege_msg_ptr = (INT_MESSAGE_PTR)_msg_alloc(int_pool);
	//printf("temp_qid %d created, qid is : %d \n", index,temp_qid);
	if(check_write_privilege_msg_ptr==NULL){
				printf("\nCould not allocate a message\n");
				_mqx_exit(0);
			}
	check_write_privilege_msg_ptr->HEADER.SOURCE_QID = temp_qid;
	check_write_privilege_msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0,CHECK_QUEUE);
	check_write_privilege_msg_ptr->HEADER.SIZE= sizeof(MESSAGE_HEADER_STRUCT)+sizeof(check_write_privilege_msg_ptr->DATA)+1;
	check_write_privilege_msg_ptr->DATA = 0;
	//set flag
	_msgq_send(check_write_privilege_msg_ptr);
	find_write_privilege_flag ++;

	//wait for the return message
	check_write_privilege_msg_ptr = _msgq_receive(temp_qid,0);
	//printf("back message received, message is : %d \n", check_write_privilege_msg_ptr->DATA);
	int write_privilege = check_write_privilege_msg_ptr->DATA;

	_msg_free(check_write_privilege_msg_ptr);
	_msgq_close(temp_qid);
	return write_privilege;


}

int find_read_privilege(int stream_no,int index){


	INT_MESSAGE_PTR check_read_privilege_msg_ptr;
	_queue_id temp_qid;
	temp_qid = _msgq_open((_queue_number)FIND_READ_P_QUEUE+index-1 , 0);
	//printf("temp_qid in find read privilege created, qid is : %d \n\n", temp_qid);

	check_read_privilege_msg_ptr = (INT_MESSAGE_PTR)_msg_alloc(int_pool);
	if(check_read_privilege_msg_ptr==NULL){
			printf("\nCould not allocate a message\n");
			_mqx_exit(0);
		}
	check_read_privilege_msg_ptr->HEADER.SOURCE_QID = temp_qid;
	check_read_privilege_msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0,CHECK_READ_QUEUE);
	check_read_privilege_msg_ptr->HEADER.SIZE= sizeof(MESSAGE_HEADER_STRUCT)+sizeof(check_read_privilege_msg_ptr->DATA)+1;
	check_read_privilege_msg_ptr->DATA = stream_no;
	//set flag


		//Sends the message to the message queue, send ownership of this pointer to receiver
	    _msgq_send(check_read_privilege_msg_ptr);
	    find_read_privilege_flag ++;


	//wait for the return message
	check_read_privilege_msg_ptr = _msgq_receive(temp_qid,0);
	//printf("The getline queue ID is : %d \n", check_read_privilege_msg_ptr->DATA);
	int getline_qid = check_read_privilege_msg_ptr->DATA;

	_msg_free(check_read_privilege_msg_ptr);
	_msgq_close(temp_qid);
	return getline_qid;


}


bool OpenR(uint16_t stream_no,int index){

	_task_id tid = _task_get_id();
	_queue_id openr_qid;
	INT_MESSAGE_PTR openr_msg_ptr;
	//create openw_qid message queue id
	openr_qid = _msgq_open((_queue_number)USER_OPENR_QUEUE_BASE+index-1 , 0);

	 //allocates a message from a message pool, return to a pointer
	openr_msg_ptr = (INT_MESSAGE_PTR)_msg_alloc(int_pool);
	//check if a message is pointed
	if(openr_msg_ptr==NULL){
		printf("\nCould not allocate a message\n");
		_mqx_exit(0);
	}

	 //Write header and data into the message
	openr_msg_ptr->HEADER.SOURCE_QID = openr_qid;
	openr_msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0,OPENR_QUEUE);
	openr_msg_ptr->HEADER.SIZE= sizeof(MESSAGE_HEADER_STRUCT)+sizeof(openr_msg_ptr->DATA)+sizeof(openr_msg_ptr->DATA2)+1;
	openr_msg_ptr->DATA = tid;
	openr_msg_ptr->DATA2 = stream_no;

	//Sends the message to the message queue, send ownership of this pointer to receiver
    _msgq_send(openr_msg_ptr);
    call_openr_flag ++;

    openr_msg_ptr = _msgq_receive(openr_qid,0);
    //printf("OpenR getting result:%d\n",openr_msg_ptr->DATA);
    	 if(openr_msg_ptr->DATA == 1){
    		 //free the message and queue id
    		 _msg_free(openr_msg_ptr);
    		 _msgq_close(openr_qid);
    		 return TRUE;
    	 }
    	 else{
    		 //free the message and close queue id
    		 _msg_free(openr_msg_ptr);
    		 _msgq_close(openr_qid);
    		 return FALSE;
    	 }




}

bool _getline(char *string,int index){

	_task_id tid = _task_get_id();
	_queue_id user_getline_qid;
	user_getline_qid = find_read_privilege(tid,index);
	//printf("getline target is is %d \n",usrid);
if(user_getline_qid!=0)
	{
	getline_ready_flag ++;
	HANDLER_MESSAGE_PTR getline_msg_ptr = (HANDLER_MESSAGE_PTR)_msg_alloc(array_pool);//establish a msg ptr to receive text from kinder
	getline_msg_ptr = _msgq_receive(user_getline_qid, 0); //waiting for incoming msg from handler

	sprintf(string,getline_msg_ptr->DATA);
	//char teststr[MAXLINELENGTH];
	//sprintf(teststr,getline_msg_ptr->DATA);
	//printf("Task id: %d is getting line:%s \n",index,teststr);
	_msg_free(getline_msg_ptr);
	//printf("Task id: %d is getting line: %s \n", index,string_ptr);
	return TRUE;
	} else return FALSE;


}

 _queue_id OpenW(int index){
	 //set flag

	 //get the task id
	 _task_id task_id = _task_get_id();

	 //create a message pointer and queue id
	 INT_MESSAGE_PTR openw_msg_ptr;
	 _queue_id openw_qid;
	 //create openw_qid message queue id
	 openw_qid = _msgq_open((_queue_number)USER_QUEUE_W_BASE+index-1, 0);

	 //allocates a message from a message pool, return to a pointer
	 openw_msg_ptr = (INT_MESSAGE_PTR)_msg_alloc(int_pool);
	 //check if a message is pointed
	 	if(openw_msg_ptr==NULL){
	 		printf("\nCould not allocate a message\n");
	 		_mqx_exit(0);
	 		}


	 //Write header and data into the message
	 openw_msg_ptr->HEADER.SOURCE_QID = openw_qid;
	 openw_msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0,OPENW_QUEUE);
	 openw_msg_ptr->HEADER.SIZE= sizeof(MESSAGE_HEADER_STRUCT)+sizeof(openw_msg_ptr->DATA)+1;
	 openw_msg_ptr->DATA = task_id;


	 //Sends the message to the message queue, send ownership of this pointer to receiver
	 bool result =_msgq_send(openw_msg_ptr);
	 delay(2500);
	 call_openw_flag++;
	 if (result != TRUE) {
	          printf("\nCould not send a message 1\n");
	          _task_block();
	       }

	 //wait for the return message
	 openw_msg_ptr = _msgq_receive(openw_qid,0);

	 if(openw_msg_ptr->DATA == 1){
		 //free the message and queue id
		 _msg_free(openw_msg_ptr);
		 _msgq_close(openw_qid);
		 return _msgq_get_id(0,PUTLINE_QUEUE_BASE+index-1);
	 }
	 else{
		 //free the message and close queue id
		 _msg_free(openw_msg_ptr);
		 _msgq_close(openw_qid);
		 return 0;
	 }
}

bool _putline(_queue_id qid, char *string,int index){

	_task_id tid = _task_get_id();
	bool result;
	int current_writing_task = find_write_privilege(index);
	if(tid != current_writing_task){
		return FALSE;
	}
	else{
			HANDLER_MESSAGE_PTR msgpl_ptr;
			//int length = strlen(string);
			//printf("length is %d\n",length);
			msgpl_ptr = (HANDLER_MESSAGE_PTR)_msg_alloc(array_pool);
			msgpl_ptr->HEADER.SOURCE_QID = qid;
			msgpl_ptr->HEADER.TARGET_QID = _msgq_get_id(0, HANDLER_W_QUEUE);
			msgpl_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT)+strlen((char*)msgpl_ptr->DATA)+1;
			sprintf(msgpl_ptr->DATA,string);
			result = _msgq_send(msgpl_ptr);
			call_putline_flag ++;
			if(!result){
				return FALSE;
			}
		}




	return TRUE;



}

bool Close(int index){
	_task_id tid = _task_get_id();
	bool result;
	INT_MESSAGE_PTR close_cmd_ptr;
     _queue_id close_cmd_qid;
		 //create openw_qid message queue id
     close_cmd_qid = _msgq_open((_queue_number)CLOSE_CMD__BASE+index-1, 0);

		 //allocates a message from a message pool, return to a pointer
     close_cmd_ptr = (INT_MESSAGE_PTR)_msg_alloc(int_pool);
		 //check if a message is pointed
		 	if(close_cmd_ptr==NULL){
		 		printf("\nCould not allocate a message\n");
		 		_mqx_exit(0);
		 		}


		 //Write header and data into the message
		 close_cmd_ptr->HEADER.SOURCE_QID = close_cmd_qid;
		 close_cmd_ptr->HEADER.TARGET_QID = _msgq_get_id(0,CLOSE_QUEUE);
		 close_cmd_ptr->HEADER.SIZE= sizeof(MESSAGE_HEADER_STRUCT)+sizeof(close_cmd_ptr->DATA)+1;
		 close_cmd_ptr->DATA = tid;
		 result =_msgq_send(close_cmd_ptr);
		 task_close_flag++;
		 if (result != TRUE) {
		 	  printf("\nCould not send a message 1\n");
		 	  _task_block();
		 	  }

		 close_cmd_ptr = _msgq_receive(close_cmd_qid,0);

		 	 if(close_cmd_ptr->DATA == 1){
		 		 //free the message and queue id
		 		 _msg_free(close_cmd_ptr);
		 		 _msgq_close(close_cmd_qid);
		 		 return TRUE;
		 	 }
		 	 else{
		 		 //free the message and close queue id
		 		 _msg_free(close_cmd_ptr);
		 		 _msgq_close(close_cmd_qid);
		 		 return FALSE;
		 	 }
}

void delay(int val){
	while(val)
	{
		val--;
	}
}
