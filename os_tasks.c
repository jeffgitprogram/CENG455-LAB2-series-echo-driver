/* ###################################################################
**     Filename    : os_tasks.c
**     Project     : Serial_Echo
**     Processor   : MK64FN1M0VLL12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-02-03, 15:12, # CodeGen: 1
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         user_task - void user_task(os_task_param_t task_init_data);
**
** ###################################################################*/
/*!
** @file os_tasks.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup os_tasks_module os_tasks module documentation
**  @{
*/         
/* MODULE os_tasks */

#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"
#include "handler_queue.h"
#include "string.h"
#include "access_function.h"

#ifdef __cplusplus
extern "C" {
#endif 

//Globe Value
_pool_id array_pool;
_pool_id pointer_pool;
_pool_id int_pool;

int broadcast_count;

_queue_id handler_w_qid;
int getline_flag;

int strlength;
/* User includes (#include below this line is not maintained by Processor Expert) */

/*
** ===================================================================
**     Callback    : user_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void user_task(os_task_param_t task_init_data)
{
	printf("user task Created! \n \r");

	/* Write your local variable definition here */
	_task_id task_id;
	_task_id task_id2;
	task_id = _task_create(0, STARTTASK_TASK, (uint32_t)(1));
	task_id2 = _task_create(0, STARTTASK_TASK, (uint32_t)(2));
	_task_id task_id3 = _task_create(0, STARTTASK_TASK, (uint32_t)(3));
	/*char buf[13];
	sprintf(buf, "\n\rType Here");
	UART_DRV_SendDataBlocking(myUART_IDX, buf, sizeof(buf), 1000);*/

	#ifdef PEX_USE_RTOS
	while (1) {
	#endif
    /* Write your code here ... */


    
    OSA_TimeDelay(10);                 /* Example code (for task release) */
   
    
    #ifdef PEX_USE_RTOS
	}
	#endif
}

/*
** ===================================================================
**     Callback    : handler_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void handler_task(os_task_param_t task_init_data)
{

	printf("HandlerTask Created! \n \r");

	/*write_privilege*/
	int write_privilege_grant_id = 0;
	/*Read_privilege*/
	int read_privilege_grant_queueid[TASKMAX] = {0};
	int read_privilege_grant_taskid[TASKMAX] = {0};
	int read_privilege_request_taskid = 0;
	int read_privilege_request_queueid = 0;
	/*Define handler message pointer here*/
	HANDLER_MESSAGE_PTR isr_msg_ptr;//Used for receiving data from ISR
	HANDLER_MESSAGE_PTR getline_str_ptr;//Used for sending data to usertask

	INT_MESSAGE_PTR openw_msg_ptr;//Used for openw
	//INT_MESSAGE_PTR check_write_privilege_msg_ptr;//
	INT_MESSAGE_PTR openr_msg_ptr;//Used for openr
	//INT_MESSAGE_PTR check_read_privilege_msg_ptr;//

	/*Define my handler queue id here*/
	_queue_id handler_r_qid = _msgq_open(HANDLER_QUEUE, 0);//Open the handler queue for reading from ISR
	_queue_id handler_w_qid = _msgq_open(HANDLER_W_QUEUE,0);//Open the handler queue for communicate with access function
	_queue_id handler_openw_qid = _msgq_open(OPENW_QUEUE,0);//Open the handler queue for communicate with openw function
	_queue_id handler_openr_qid = _msgq_open(OPENR_QUEUE,0);//Open the handler queue for communicate with openr function
	_queue_id check_read_privilege_msg_qid = _msgq_open((_queue_number)CHECK_READ_QUEUE , 0);
	_queue_id check_write_privilege_msg_qid = _msgq_open((_queue_number)CHECK_QUEUE , 0);
	_queue_id close_cmd_msg_qid = _msgq_open((_queue_number)CLOSE_QUEUE , 0);

	/*Define my message pools here*/
	//Open the array pool,Unlimited growth by 1
	array_pool = _msgpool_create(sizeof(HANDLER_MESSAGE),NUM_USERS,1,0);
	//Open the int_pool, unlimited growth by 1
	int_pool = _msgpool_create(sizeof(INT_MESSAGE),NUM_USERS,1,0);
	//printf("Int_pool created pool id is: %d \n",int_pool);

	/* Initialize mutex attributes
	if (_mutatr_init(&mutexattr) != MQX_OK) {
		printf("Initializing mutex attributes failed.\n");
		_mqx_exit(0);
	}
	mutex_Var = _mutex_init(&read_mutex, &mutexattr);
	if(mutex_Var != MQX_EOK){
		_mqx_exit(0);
	}else
		printf("Mutex initialization succeeded.\n");*/



  char charbuf[MAXLINELENGTH]={"/0"};
  int count = 0;
  char curr[1];
  char disp[MAXLINELENGTH]={"/0"};
  getline_flag = 0;
  int saveindex = 0;

  	#ifdef PEX_USE_RTOS
  	while (1) {
    #endif
  		/*
  		 * This section handle a private structure "Read_privilege_grant_id"
  		 *  "Read_privilege_grant_id" save the task id which has the read privilege
  		 */
  		 if(call_openr_flag >0){

  			    			int match_result = 0;
  			    			openr_msg_ptr = _msgq_receive(handler_openr_qid,0);
  			    			read_privilege_request_taskid = openr_msg_ptr->DATA;
  			    			read_privilege_request_queueid = openr_msg_ptr->DATA2;
  			    			//printf("the read privilege ID is now:%d and %d\n",read_privilege_request_taskid,read_privilege_request_queueid);
  			    			for (int n= 0; n<TASKMAX;n++ ){
  			    				if(read_privilege_grant_taskid[n] == read_privilege_request_taskid){
  			    					openr_msg_ptr->HEADER.TARGET_QID = openr_msg_ptr->HEADER.SOURCE_QID;
  			    					openr_msg_ptr->HEADER.SOURCE_QID = _msgq_get_id(0,OPENR_QUEUE);
  			    					openr_msg_ptr->HEADER.SIZE= sizeof(MESSAGE_HEADER_STRUCT)+sizeof(openw_msg_ptr->DATA)+1;
  			    					openr_msg_ptr->DATA = 0;
  			    					_msgq_send(openr_msg_ptr);

  			    					match_result = 1;
  			    					break;
  			    				}
  			    			}
  			    			if(match_result == 0)
  			    			{
  			    					openr_msg_ptr->HEADER.TARGET_QID = openr_msg_ptr->HEADER.SOURCE_QID;
  			    			  		openr_msg_ptr->HEADER.SOURCE_QID = _msgq_get_id(0,OPENR_QUEUE);
  			    			  		openr_msg_ptr->HEADER.SIZE= sizeof(MESSAGE_HEADER_STRUCT)+sizeof(openw_msg_ptr->DATA)+1;
  			    			  		openr_msg_ptr->DATA = 1;
  			    			  		_msgq_send(openr_msg_ptr);
  			    			  		read_privilege_grant_taskid[saveindex] = read_privilege_request_taskid ;
  			    			  		read_privilege_grant_queueid[saveindex] = read_privilege_request_queueid;
  			    			  		//printf("the read privilege array is now:%s%s",read_privilege_grant_taskid,read_privilege_grant_queueid);
  			    			  		saveindex++;

  			    			}
  			    			call_openr_flag--;
  			    	}






  			    		/*
  			    		 * This section handle a private value "write_privilege_grant_id"
  			    		 *  "write_privilege_grant_id" save the task id which has the write privilege
  			    		 */
  			    		if(call_openw_flag > 0){


  			    		bool result;
  			    		//wait for receiving message send from openW()
  			    		openw_msg_ptr = _msgq_receive(handler_openw_qid,0);
  			    		//printf("Current write privilege owner:%d\n",write_privilege_grant_id);
  			    		OSA_TimeDelay(20);
  			    		delay(2500);
  			    		if(write_privilege_grant_id == 0){
  			    			write_privilege_grant_id = openw_msg_ptr->DATA;
  			    			//return write permit message
  			    			openw_msg_ptr->HEADER.TARGET_QID = openw_msg_ptr->HEADER.SOURCE_QID;
  			    			openw_msg_ptr->HEADER.SOURCE_QID = _msgq_get_id(0,OPENW_QUEUE);
  			    			openw_msg_ptr->HEADER.SIZE= sizeof(MESSAGE_HEADER_STRUCT)+sizeof(openw_msg_ptr->DATA)+1;
  			    			openw_msg_ptr->DATA = 1;
  			    			result = _msgq_send(openw_msg_ptr);
  			    			call_openw_flag --;
  			    			if (result != TRUE) {
  			    			         printf("\nCould not send a message\n");
  			    			         _task_block();
  			    			      }
  			    		}else{
  			    			//return fail message
  			    			openw_msg_ptr->HEADER.TARGET_QID = openw_msg_ptr->HEADER.SOURCE_QID;
  			    			openw_msg_ptr->HEADER.SOURCE_QID = _msgq_get_id(0,OPENW_QUEUE);
  			    			openw_msg_ptr->HEADER.SIZE= sizeof(MESSAGE_HEADER_STRUCT)+sizeof(openw_msg_ptr->DATA)+1;
  			    			openw_msg_ptr->DATA = 0;
  			    			result = _msgq_send(openw_msg_ptr);

  			    			call_openw_flag --;
  			    			if (result != TRUE) {
  			    			  		 printf("\nCould not send a message\n");
  			    			  		  _task_block();
  			    			  	      }
  			    		}

  			    		}

  			    		/*
  			    		 *
  			    		 */
  			    		if(find_read_privilege_flag >0){


  			    		  			//create a message pointer and queue id
  			    		  			INT_MESSAGE_PTR check_read_privilege_msg_ptr;


  			    		  			//create openw_qid message queue id

  			    		  			//printf("check_read_privilege_msg_qid created, qid is : %d \n", check_read_privilege_msg_qid);
  			    		  			int find_read_match =0;
  			    		  		   check_read_privilege_msg_ptr = _msgq_receive(check_read_privilege_msg_qid,0);
  			    		  		   int getline_request_taskid = check_read_privilege_msg_ptr->DATA;
  			    		  		  // printf("check_read_privilege_taskID is : %d \n", getline_request_taskid);
  			    		  		   for (int m= 0; m<TASKMAX;m++ ){
  			    		  		  		if(read_privilege_grant_taskid[m] == getline_request_taskid){
  			    		  		  		check_read_privilege_msg_ptr->HEADER.TARGET_QID = check_read_privilege_msg_ptr->HEADER.SOURCE_QID;
  			    		  		  		check_read_privilege_msg_ptr->HEADER.SOURCE_QID = _msgq_get_id(0,CHECK_READ_QUEUE);
  			    		  		  		check_read_privilege_msg_ptr->HEADER.SIZE= sizeof(MESSAGE_HEADER_STRUCT)+sizeof(check_read_privilege_msg_ptr->DATA)+1;
  			    		  		  		check_read_privilege_msg_ptr->DATA = read_privilege_grant_queueid[m];
  			    		  		  		_msgq_send(check_read_privilege_msg_ptr);
  			    		  		  	    find_read_match = 1;
  			    		  		  				break;
  			    		  		  			}
  			    		  		  	}
  			    		  		   if(find_read_match ==0){
  			    		  		check_read_privilege_msg_ptr->HEADER.TARGET_QID = check_read_privilege_msg_ptr->HEADER.SOURCE_QID;
  			    		  		check_read_privilege_msg_ptr->HEADER.SOURCE_QID = _msgq_get_id(0,CHECK_READ_QUEUE);
  			    		  		check_read_privilege_msg_ptr->HEADER.SIZE= sizeof(MESSAGE_HEADER_STRUCT)+sizeof(check_read_privilege_msg_ptr->DATA)+1;
  			    		  		check_read_privilege_msg_ptr->DATA = 0;
  			    		  		_msgq_send(check_read_privilege_msg_ptr);
  			    		  		   }
  			    		  		find_read_privilege_flag--;

  			    		}
  			    		/*
  			    		 *
  			    		 */
  			    		if(find_write_privilege_flag >0){


  			    			//create a message pointer and queue id
  			    			INT_MESSAGE_PTR check_write_privilege_msg_ptr;


  			    			//create openw_qid message queue id

  			    			//printf("check_write_privilege_msg_qid created, qid is : %d \n", check_write_privilege_msg_qid);

  			    			//allocates a message from a int pool, return to a pointer
  			    			check_write_privilege_msg_ptr = _msgq_receive(check_write_privilege_msg_qid,0);
  			    			//printf(("check_write_privilege_msg_ptr created, id is : %d \n", check_write_privilege_msg_qid));
  			    			//check if a message is pointed
  			    			if(check_write_privilege_msg_ptr==NULL){
  			    				printf("\nCould not allocate a message\n");
  			    				_mqx_exit(0);
  			    			}

  			    			 //Write header and data into the message

  			    			check_write_privilege_msg_ptr->HEADER.TARGET_QID = check_write_privilege_msg_ptr->HEADER.SOURCE_QID;
  			    			check_write_privilege_msg_ptr->HEADER.SOURCE_QID = _msgq_get_id(0,CHECK_QUEUE);
  			    			check_write_privilege_msg_ptr->HEADER.SIZE= sizeof(MESSAGE_HEADER_STRUCT)+sizeof(check_write_privilege_msg_ptr->DATA)+1;
  			    			check_write_privilege_msg_ptr->DATA = write_privilege_grant_id;
  			    			//printf("write_privilege_grant_id found! id is: %d \n",check_write_privilege_msg_ptr->DATA);
  			    			int find_write_result = 0;
  			    			find_write_result=_msgq_send(check_write_privilege_msg_ptr);
  			    			find_write_privilege_flag --;
  			    			if (find_write_result != TRUE) {
  			    			  			    			         printf("\nCould not send a write check message\n");
  			    			  			    			         _task_block();
  			    			  			    			      }
  			    			 //free queue id
  			    			 //_msgq_close(check_write_privilege_msg_qid);


  			    		}
  			    		if(call_putline_flag > 0)
  			    	 {

  			    	  	HANDLER_MESSAGE_PTR msgpl_ptr;//Used for putline from usertask
  			    	  	msgpl_ptr = _msgq_receive(handler_w_qid, 0);
  			    	  	sprintf(disp , msgpl_ptr->DATA);
  			    	  	//printf("handler getting data is %s and display is %s",msgpl_ptr->DATA,disp);
  			    	  	UART_DRV_SendDataBlocking(myUART_IDX, disp, strlen(disp),1000);
  			    	  	UART_DRV_SendDataBlocking(myUART_IDX, &"\r", sizeof(char),1000);
  			    	  	_msg_free(msgpl_ptr);
  			    	  	call_putline_flag --;
  			    	  	memset(disp, 0, sizeof disp);
  			    	  	}

  			    		if(task_close_flag > 0){
  			    			INT_MESSAGE_PTR close_cmd_msg_ptr;
  			    			close_cmd_msg_ptr = _msgq_receive(close_cmd_msg_qid , 0);
  			    			if(close_cmd_msg_ptr == NULL){
  			    				printf("\nCould not allocate a message\n");
  			    				_mqx_exit(0);
  			    			}
  			    			int close_tid = close_cmd_msg_ptr->DATA;
  			    			int close_read_result = 0;
  			    			int revoke_cont = 0;
  			    			 for (int c= 0; c<TASKMAX;c++ ){
  			    				 if(read_privilege_grant_taskid[c] == close_tid){
  			    					read_privilege_grant_taskid[c]=0;
  			    					revoke_cont++;

  			    			  	     close_read_result = 1;
  			    			  		 break;
  			    			  	 }
  			    			  }
  			    			 if(close_read_result == 1)
  			    				 {
  			    				 if(write_privilege_grant_id == close_tid){

  			    				        write_privilege_grant_id = 0;
  			    				      revoke_cont++;
  			    			  		}
  			    			      close_cmd_msg_ptr->HEADER.TARGET_QID = close_cmd_msg_ptr->HEADER.SOURCE_QID;
  			    			      close_cmd_msg_ptr->HEADER.SOURCE_QID = _msgq_get_id(0,CLOSE_QUEUE);
  			    			      close_cmd_msg_ptr->HEADER.SIZE= sizeof(MESSAGE_HEADER_STRUCT)+sizeof(close_cmd_msg_ptr->DATA)+1;
  			    			      close_cmd_msg_ptr->DATA = 1;
  			    			      _msgq_send(close_cmd_msg_ptr);
  			    			       _msg_free(close_cmd_msg_ptr);
  			    				 }
  			    			 else if(write_privilege_grant_id != close_tid){
  			    				close_cmd_msg_ptr->HEADER.TARGET_QID = close_cmd_msg_ptr->HEADER.SOURCE_QID;
  			    				close_cmd_msg_ptr->HEADER.SOURCE_QID = _msgq_get_id(0,CLOSE_QUEUE);
  			    				close_cmd_msg_ptr->HEADER.SIZE= sizeof(MESSAGE_HEADER_STRUCT)+sizeof(close_cmd_msg_ptr->DATA)+1;
  			    				close_cmd_msg_ptr->DATA = 0;
  			    				_msgq_send(close_cmd_msg_ptr);
  			    				_msg_free(close_cmd_msg_ptr);
  			    			 }

  			    			task_close_flag --;
  			    			printf("Current task revoke %d privilege\n\n",revoke_cont);
  			    		}





/**************write ISR handler below this line*****************************************************************************************/

  			    		while(ISR_FLAG == 1){
  			    			ISR_FLAG = 0;

  			    			//pointer point to the handler queue
  			    			isr_msg_ptr = _msgq_receive(handler_r_qid, 0);
  			    			//acquire message data, store to an char array curr[0]
  			    			curr[0]= isr_msg_ptr->DATA[0];
  			    			//check if any active user open the device for reading
  			    			int read_user = 0;

  			    			for(int r= 0; r<TASKMAX;r++){
  			    				if(read_privilege_grant_taskid[r] !=0){
  			    					read_user = 1;
  			    					break;
  			    				}

  			    			}

  			    			if(read_user == 1)
  			    			{
  			    				/*
  			    			 * If the data is printable, put into character buff, increment count
  			    			 */
  			    			if(curr[0] >= 0x20 && curr[0] <= 0x7F )
  			    			{

  			    				charbuf[count] = isr_msg_ptr->DATA[0];
  			    				count++;
  			    			  	if(count > 49){ // maximum line characters are 50, if larger than 50, automatic change line
  			    			  	curr[0]= '\n';
  			    			  	UART_DRV_SendDataBlocking(myUART_IDX, curr, sizeof(curr), 1000);
  			    			    UART_DRV_SendDataBlocking(myUART_IDX, &"\r", sizeof(char), 1000);

  			    			  	count = 0;
  			    			  	}



  			    			  	UART_DRV_SendDataBlocking(myUART_IDX, curr, sizeof(curr), 1000); // display on the screen
  			    			  	}
  			    			/*
  			    			 * If the data is not printable
  			    			 * enter	:	change line, clear charbuf, set count to 0
  			    			 * backspace:	move cursor to the left, print a space, move to the left again
  			    			 * control+w:	delete a word
  			    			 * control+u:	delete whole line
  			    			 */

  			    			 else
  			    				  {
  			    			  		  				  if(curr[0] == 0x0D){				//enter
  			    			  		  					  curr[0]= '\n';
  			    			  		  					  UART_DRV_SendDataBlocking(myUART_IDX, curr, sizeof(curr), 1000);
  			    			  		  					  UART_DRV_SendDataBlocking(myUART_IDX, &"\r", sizeof(char), 1000);

  			    			  		  					  //creat a charbuf message ptr
  			    			  		  				if(getline_ready_flag >0)
  			    			  		  				{
  			    			  		  						for (int l=0;l<TASKMAX;l++){
  			    			  		  							if(read_privilege_grant_queueid[l]!=0)
  			    			  		  							{
  			    			  		  								getline_str_ptr = (HANDLER_MESSAGE_PTR)_msg_alloc(array_pool);
  			    			  		  								getline_str_ptr->HEADER.SOURCE_QID = handler_r_qid;
  			    			  		  								getline_str_ptr->HEADER.TARGET_QID = read_privilege_grant_queueid[l];
  			    			  		  								getline_str_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT)+strlen((char*)getline_str_ptr->DATA)+1;
  			    			  		  								sprintf(getline_str_ptr->DATA,charbuf);
  			    			  		  								int getline_send_result = _msgq_send(getline_str_ptr);
  			    			  		  								if(getline_send_result!=TRUE){
  			    			  		  									printf("\nCould not send a getline text\n");

  			    			  		  								}
  			    			  		  							}
  			    			  		  						getline_ready_flag--;
  			    			  		  				    }
  			    			  		 	}

  			    			  	  						  count = 0;
  			    			  	  						  memset(charbuf, 0, sizeof charbuf);


  			    			  	  					  //add broadcast to user queue here
  			    			  	  				  }

  			    			  	  				  // backspace
  			    			  	  				  if( curr[0] == 0x08){
  			    			  	  					  curr[0]= '\b';
  			    			  	  					  UART_DRV_SendDataBlocking(myUART_IDX, curr, sizeof(curr), 1000);
  			    			  	  					  curr[0] = ' ';
  			    			  	  					  UART_DRV_SendDataBlocking(myUART_IDX, curr, sizeof(curr), 1000);
  			    			  	  					  curr[0] = '\b';
  			    			  	  					  UART_DRV_SendDataBlocking(myUART_IDX, curr, sizeof(curr), 1000);
  			    			  	  					  count--;
  			    			  	  				  }

  			    			    				  //control+w
  			    			    				  if( curr[0] == 0x17){
  			    			    					  int mark = 0;
  			    			    				 		  for(int i=count-1;i>=0;i--){
  			    			    				 			  if(charbuf[i] == ' '){
  			    			    				 				  mark = i;
  			    			    				 				  break;
  			    			    				 			  }
  			    			    				 		  }

  			    			    				 		  for( int j=(count-mark);j>0;j--){
  			    			    				 			 curr[0] = '\b';
  			    			    				 	 		  UART_DRV_SendDataBlocking(myUART_IDX, curr, sizeof(curr), 1000);
  			    			    				 	 		curr[0] = ' ';
  			    			    				 	 		  UART_DRV_SendDataBlocking(myUART_IDX, curr, sizeof(curr), 1000);
  			    			    				 	 		curr[0] = '\b';
  			    			    				 	 		  UART_DRV_SendDataBlocking(myUART_IDX, curr, sizeof(curr), 1000);
  			    			    				 	 		  count--;
  			    			    				 		  }

  			    			    				 	  }

  			    			    				  //control+u Delete line
  			    			    				  if(curr[0] == 0x15){
  			    			    					  for(int f=count;f>0;f--){
  			    			    						  curr[0] = '\b';
  			    			    						  UART_DRV_SendDataBlocking(myUART_IDX, curr, sizeof(curr), 1000);
  			    			    						  curr[0] = ' ';
  			    			    						  UART_DRV_SendDataBlocking(myUART_IDX, curr, sizeof(curr), 1000);
  			    			    						  curr[0] = '\b';
  			    			    						  UART_DRV_SendDataBlocking(myUART_IDX, curr, sizeof(curr), 1000);
  			    			    					  }
  			    			    					  count = 0;

  			    			  					}
  			    			  }
  			    		}
  			    			else printf("No user opened device for reading\n\n");
  			    			  		//free ISR message
  			    			  		_msg_free(isr_msg_ptr);
  			    		 }







    OSA_TimeDelay(10);                 /*for task release) */
   
    #ifdef PEX_USE_RTOS
  	}

	#endif
}
//while (UART_DRV_GetTransmitStatus(myUART_IDX,0)==Tx_busy);
/*
** ===================================================================
**     Callback    : StartTask_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void StartTask_task(os_task_param_t task_init_data)
{

	_task_id tid = _task_get_id();
     int index = task_init_data;

	printf("\n""UserTask Created ID: %d, task index: %d\n", tid, index);



    _queue_id user_putline_queue = _msgq_open((_queue_number)PUTLINE_QUEUE_BASE+index-1 , 0);

    //printf("user_putline_queue created!!!!!! task%d id is %d \n", index,user_putline_queue);

    _queue_id user_r_qid = _msgq_open((_queue_number)(USER_QUEUE_R_BASE+index-1), 0);
    //printf("user task q id : %d \n", user_r_qid);


    char sendmsg[MAXLINELENGTH] ={'\0'};//Message used for putline
    sprintf(sendmsg,"This is task%d putline message\n",index);

    //printf("Current task is having putline message %s\n\n",sendmsg);

    int tmpqid1;
    int tmpqid2;
   // _queue_id writetarget;
    uint32_t readpermission;
    char textmsg[MAXLINELENGTH] = { '\0'};
    /*Task Configuration part starts here*/
    if(index == 1){
    	//Task one
    		/*Call OpenR*/

    		readpermission = OpenR(user_r_qid,index); //Pass the userqueue id to the openR as streamno
    		if(readpermission == FALSE)
    		{
    			printf("Read Permission not granted or has been granted before, Task %d\n\n", index);
    		}
    		else printf("Read Permission granted to Task %d\n\n", index);

    	 	readpermission = OpenR(user_r_qid,index); //Pass the userqueue id to the openR as streamno
    		    		if(readpermission == FALSE)
    		    		{
    		    			printf("Read Permission not granted or has been granted before, Task %d\n\n", index);
    		    		}
    		    		else printf("Read Permission granted to Task %d\n\n", index);
    		/*Call _getline*/
    		bool getlineresult = _getline(textmsg,index);
    		if(getlineresult == 1)
    		{
    			printf("Task%d get the following text: %s\n\n", index,textmsg);
    		}
    		else printf("Task%d getline failed\n\n",index);



    		/*call OpenW*/
    	         tmpqid1 = OpenW(index);
    	      		if(tmpqid1 == 0 ){//MSGQ_NULL_QUEUE_ID
    	      			printf("Write permission not granted to the task %d\n\n",index);
    	      		}
    	      		else{
    	      			printf("Write permission granted to the task %d\n\n",index);
    	      			OSA_TimeDelay(20);
    	      			//printf("openw return PUTLINE_QUEUE_BASE+index-1 queue_id! id is : %d \n",tmpqid1);
    	      			//OSA_TimeDelay(20);
    	      		}


    		//int i = find_write_privilege();
    		//printf("The Task id %d has write privilege \n",i);

    		/*Call _putline*/

    			bool putlineresult = _putline(tmpqid1, sendmsg,index);
    			if(putlineresult == TRUE)
    			{
    				printf("Task%d putline suceeded.\n\n",index);
    			}
    			else printf("Task%d putline failed.\n\n",index);



    		/*Call _close()*/
    		bool closestatus = Close(index);
    		if(closestatus == TRUE){
    			printf("Task%d is closed successfuly\n\n", index);
    		}
    		else printf("Task%d don't have any privilege\n\n",index);

    }
    if(index == 2)
    {
    	   readpermission = OpenR(user_r_qid,index); //Pass the userqueue id to the openR as streamno
    	    if(readpermission == FALSE)
    	   {
    	    	printf("Read Permission not granted or has been granted before, Task %d\n\n", index);
    	   }
    	    else printf("Read Permission granted to Task %d\n\n", index);

    	    bool getlineresult = _getline(textmsg,index);
    	    if(getlineresult == 1)
    	    {
    	    		printf("Task%d get the following text:%s\n\n",index,textmsg);
    	    }
    	    	else printf("Task%d getline failed\n\n",index);

    	   tmpqid1 = OpenW(index);
    	   if(tmpqid1 == 0 ){//MSGQ_NULL_QUEUE_ID
    	    	   printf("Write permission not granted to the task %d\n\n",index);
    	    }
    	   else{
    	    	    printf("Write permission granted to the task %d\n\n",index);
    	    	     OSA_TimeDelay(20);
    	    	      			//printf("openw return PUTLINE_QUEUE_BASE+index-1 queue_id! id is : %d \n",tmpqid1);
    	    	      			//OSA_TimeDelay(20);
    	    	 }

    	   bool putlineresult = _putline(tmpqid1, sendmsg,index);
    	    if(putlineresult == TRUE)
    	   {
    	    	    printf("Task%d putline suceeded.\n\n",index);
    	    	 }
    	   else printf("Task%d putline failed.\n\n",index);

    	    bool closestatus = Close(index);
    	        	 if(closestatus == TRUE){
    	        	     printf("Task%d is closed successfuly\n\n", index);
    	        	    }
    	        	  else printf("Task%d don't have any privilege\n\n",index);

    }
    if(index == 3){
    	bool closestatus = Close(index);
    	 if(closestatus == TRUE){
    	     printf("Task%d is closed successfuly\n\n", index);
    	    }
    	  else printf("Task%d don't have any privilege\n\n",index);
    }



#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */
	/*  HANDLER_MESSAGE_PTR taskreceive_ptr;//Pointer for receive the getline string from the handler
	  	taskreceive_ptr = _msgq_receive(userqidsaver[1], 0);
	  	char string_ptr[1];
	  	string_ptr[0] = taskreceive_ptr->DATA[0];
	  			//UART_DRV_SendDataBlocking(myUART_IDX, string_ptr, sizeof(string_ptr), 1000);
	  	printf("Task id: 1 is getting line: %s \n",string_ptr);*/

    OSA_TimeDelay(10);                 /* Example code (for task release) */
   
    
    
    
#ifdef PEX_USE_RTOS   
 }
#endif    
}

/*
** ===================================================================
**     Callback    : UserTask1_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void UserTask1_task(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */
	_task_id tid = _task_get_id();
	     int index = task_init_data;

	printf("\n""UserTask Created ID: %d, task index: %d\n", tid, index);



	    _queue_id user_putline_queue = _msgq_open((_queue_number)PUTLINE_QUEUE_BASE+index-1 , 0);


	    //printf("user_putline_queue created!!!!!! task%d id is %d \n", index,user_putline_queue);

	    _queue_id user_r_qid = _msgq_open((_queue_number)(USER_QUEUE_R_BASE+index-1), 0);

	    //printf("user task q id : %d \n", user_r_qid);


	    char sendmsg[MAXLINELENGTH] ={'\0'};//Message used for putline
	    sprintf(sendmsg,"This is task%d putline message\n",index);

	    //printf("Current task is having putline message %s\n\n",sendmsg);

	    int tmpqid1;
	    int tmpqid2;
	   // _queue_id writetarget;
	    uint32_t readpermission;
	    char textmsg[MAXLINELENGTH] = { '\0'};

	    					/*readpermission = OpenR(user_r_qid); //Pass the userqueue id to the openR as streamno
	    		    		if(readpermission == FALSE)
	    		    		{
	    		    			printf("Read Permission not granted or has been granted before, Task %d\n", task_init_data);
	    		    		}
	    		    		else printf("Read Permission granted to Task %d\n", task_init_data);*/


	    		    		/*Call _getline*/
	    		    		/*bool getlineresult2 = _getline(textmsg);
	    		    		if(getlineresult2 == 1)
	    		    		{
	    		    			printf("Task%d get the following text EXT\n", task_init_data);
	    		    		}
	    		    		else printf("Task%d getline failed",task_init_data);*/
	    tmpqid1 = OpenW(index);
	   	  if(tmpqid1 == 0 ){//MSGQ_NULL_QUEUE_ID
	   	   printf("Write permission not granted to the task %d\n",index);
	   	     OSA_TimeDelay(20);
	   	  }
	   	  	 else{
	   	  	    	printf("Write permission granted to the task %d\n",index);
	   	  	    	printf("openw return PUTLINE_QUEUE_BASE+index-1 queue_id! id is : %d \n",tmpqid1);
	   	  	   OSA_TimeDelay(20);
	   	  	    		 }
	    		    		   // printf("The Task id %d has write privilege \n",i);

#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */


    
    OSA_TimeDelay(10);                 /* Example code (for task release) */
   
    
    
    
#ifdef PEX_USE_RTOS   
 }
#endif    
}

/* END os_tasks */

#ifdef __cplusplus
} /* extern "C" */
#endif



/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
