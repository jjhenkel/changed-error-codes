#include "common.h"

enum ucode_type {
	UCODE_NONE = 0,
	UCODE_INIT,
	UCODE_RT
};

#define MAX_EVENT_LOG_SIZE (512)
#define DEFAULT_DUMP_EVENT_LOG_ENTRIES (20)

struct iwl_priv { 
	int ucode_type;
	struct {
		int log_event_table_ptr;
	} card_alive_init;
	struct {
		int log_event_table_ptr;
	} card_alive;
	struct {
		struct {
			struct {
				int (*is_valid_rtc_data_addr)(int);
			} * lib;
		} * ops;
	} * cfg;
};
