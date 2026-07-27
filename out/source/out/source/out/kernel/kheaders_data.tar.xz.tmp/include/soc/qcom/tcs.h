/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __SOC_QCOM_TCS_H__
#define __SOC_QCOM_TCS_H__

#define MAX_RPMH_PAYLOAD	16


enum rpmh_state {
	RPMH_SLEEP_STATE,
	RPMH_WAKE_ONLY_STATE,
	RPMH_ACTIVE_ONLY_STATE,
};


struct tcs_cmd {
	u32 addr;
	u32 data;
	u32 wait;
};


struct tcs_request {
	enum rpmh_state state;
	u32 wait_for_compl;
	u32 num_cmds;
	struct tcs_cmd *cmds;
};

#endif 
