/* ###################################################################
**     Filename    : Events.c
**     Project     : Serial_Echo
**     Processor   : MK64FN1M0VLL12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-02-03, 14:26, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file Events.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"
#include "handler_queue.h"

#ifdef __cplusplus
extern "C" {
#endif 

//global value
int ISR_FLAG;


/* User includes (#include below this line is not maintained by Processor Expert) */

/*
** ===================================================================
**     Callback    : myUART_RxCallback
**     Description : This callback occurs when data are received.
**     Parameters  :
**       instance - The UART instance number.
**       uartState - A pointer to the UART driver state structure
**       memory.
**     Returns : Nothing
** ===================================================================
*/
void myUART_RxCallback(uint32_t instance, void * uartState)
{
	//set a flag, indicates that interrupt has occurred
	ISR_FLAG = 1;

	//Create a message pointer to allocate a private message pool
	HANDLER_MESSAGE_PTR isr_msg_ptr;

	//Create a valuable to acquire the queue id of ISR
	_queue_id isr_qid;

	//Open isr message queue, store queue id to isr_qid
	//this function will also create a message component
	isr_qid = _msgq_open((_queue_number)ISR_QUEUE , 0);

	//allocates a message from a message pool, return to a pointer
	isr_msg_ptr = (HANDLER_MESSAGE_PTR)_msg_alloc(array_pool);
	//check if a message is pointed
	if(isr_msg_ptr==NULL){
		printf("\nCould not allocate a message\n");
		_mqx_exit(0);
		}

	//Write header and data into the message
	isr_msg_ptr->HEADER.SOURCE_QID = isr_qid;
	isr_msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0,HANDLER_QUEUE);
	isr_msg_ptr->HEADER.SIZE= sizeof(MESSAGE_HEADER_STRUCT)+strlen((char*)isr_msg_ptr->DATA)+1;
	isr_msg_ptr->DATA[0] = myRxBuff[0];

	//Sends the message to the message queue, send ownership of this pointer to receiver
	_msgq_send(isr_msg_ptr);

	return;
}

/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
