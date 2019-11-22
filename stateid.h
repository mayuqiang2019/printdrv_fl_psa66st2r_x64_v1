#pragma once

#define STATE_TOP 0
	#define STATE_UNINIT        1
	#define STATE_INIT          2
	#define STATE_DISCONNECTED  3
	#define STATE_READY         4
		#define STATE_SUSPENDED       10
		#define STATE_GAT_REPORT      11
		#define STATE_CRC             12
		#define STATE_IDLE            13
		#define STATE_DEFINE_GRAPHIC  14
			#define STATE_DELETE_GRAPHIC  15
			#define STATE_ADD_GRAPHIC     16
		#define STATE_DEFINE_REGION   17
			#define STATE_DELETE_REGION 18
			#define STATE_ADD_REGION    19
		#define STATE_DEFINE_TEMPL    20
			#define STATE_DELETE_TEMPL				21
			#define STATE_ADD_TEMPL					22
			#define STATE_FLASH_TRANSFER			23
			#define STATE_COMPLETE_FLASH_TRANSFER	24
	  #define STATE_PRINTING        25
