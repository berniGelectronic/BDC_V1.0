#ifndef DATA_H
	#define DATA_H

	typedef struct{
		float limitCurrent;
		float motorCurrent;
	}values_t;

	typedef enum{
		CLOSED=0,
		OPENING,
		ERROR_OVERCURRENT_OPENING,
		OPENED,
		CLOSING,
		ERROR_OVERCURRENT_CLOSING,
		ERROR_OBSTACLE,
		RESET_DOOR
	}states_t;

	typedef enum{
		DEFAULT=0,
		OBSTACLE,
		OPEN,
		CLOSE,
		OPENED_LIMIT,
		CLOSED_LIMIT
	}buttons_t;
	
	#endif
