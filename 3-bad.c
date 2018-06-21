#include "3-fake.h"

int
iwl3945_dump_nic_event_log(struct iwl_priv *priv, bool full_log,
			   char **buf, bool display)
{
    u32             base;	/* SRAM byte address of event log header */
    u32             capacity;	/* event log capacity in # entries */
    u32             mode;	/* 0 - no timestamp, 1 - timestamp
				 * recorded */
    u32             num_wraps;	/* # times uCode wrapped to top of log */
    u32             next_entry;	/* index of next entry to be written by
				 * uCode */
    u32             size;	/* # entries that we'll print */
    int             pos = 0;
    size_t          bufsz = 0;

    base = le32_to_cpu(priv->card_alive.log_event_table_ptr);
    if (!iwl3945_hw_valid_rtc_data_addr(base)) {
	IWL_ERR(priv, "Invalid event log pointer 0x%08X\n", base);
	return pos;
    }

    /*
     * event log header 
     */
    capacity = iwl_read_targ_mem(priv, base);
    mode = iwl_read_targ_mem(priv, base + (1 * sizeof(u32)));
    num_wraps = iwl_read_targ_mem(priv, base + (2 * sizeof(u32)));
    next_entry = iwl_read_targ_mem(priv, base + (3 * sizeof(u32)));

    if (capacity > IWL3945_MAX_EVENT_LOG_SIZE) {
	IWL_ERR(priv, "Log capacity %d is bogus, limit to %d entries\n",
		capacity, IWL3945_MAX_EVENT_LOG_SIZE);
	capacity = IWL3945_MAX_EVENT_LOG_SIZE;
    }

    if (next_entry > IWL3945_MAX_EVENT_LOG_SIZE) {
	IWL_ERR(priv, "Log write index %d is bogus, limit to %d\n",
		next_entry, IWL3945_MAX_EVENT_LOG_SIZE);
	next_entry = IWL3945_MAX_EVENT_LOG_SIZE;
    }

    size = num_wraps ? capacity : next_entry;

    /*
     * bail out if nothing in log 
     */
    if (size == 0) {
	IWL_ERR(priv, "Start IWL Event Log Dump: nothing in log\n");
	return pos;
    }
#ifdef CONFIG_IWLWIFI_DEBUG
    if (!(iwl_get_debug_level(priv) & IWL_DL_FW_ERRORS) && !full_log)
	size = (size > DEFAULT_IWL3945_DUMP_EVENT_LOG_ENTRIES)
	    ? DEFAULT_IWL3945_DUMP_EVENT_LOG_ENTRIES : size;
#else
    size = (size > DEFAULT_IWL3945_DUMP_EVENT_LOG_ENTRIES)
	? DEFAULT_IWL3945_DUMP_EVENT_LOG_ENTRIES : size;
#endif

    IWL_ERR(priv, "Start IWL Event Log Dump: display last %d count\n",
	    size);

#ifdef CONFIG_IWLWIFI_DEBUG
    if (display) {
	if (full_log)
	    bufsz = capacity * 48;
	else
	    bufsz = size * 48;
	*buf = kmalloc(bufsz, GFP_KERNEL);
	if (!*buf)
	    return pos;
    }
    if ((iwl_get_debug_level(priv) & IWL_DL_FW_ERRORS) || full_log) {
	/*
	 * if uCode has wrapped back to top of log, start at the oldest
	 * entry, i.e the next one that uCode would fill. 
	 */
	if (num_wraps)
	    pos = iwl3945_print_event_log(priv, next_entry,
					  capacity - next_entry, mode,
					  pos, buf, bufsz);

	/*
	 * (then/else) start at top of log 
	 */
	pos = iwl3945_print_event_log(priv, 0, next_entry, mode,
				      pos, buf, bufsz);
    } else
	pos = iwl3945_print_last_event_logs(priv, capacity, num_wraps,
					    next_entry, size, mode,
					    pos, buf, bufsz);
#else
    pos = iwl3945_print_last_event_logs(priv, capacity, num_wraps,
					next_entry, size, mode,
					pos, buf, bufsz);
#endif
    return pos;
}
