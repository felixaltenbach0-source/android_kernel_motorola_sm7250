/* SPDX-License-Identifier: GPL-2.0-only */



#ifndef _SERVICE_LOCATOR_H
#define _SERVICE_LOCATOR_H

#define QMI_SERVREG_LOC_NAME_LENGTH_V01 64
#define QMI_SERVREG_LOC_LIST_LENGTH_V01 32


struct servreg_loc_entry_v01 {
	char name[QMI_SERVREG_LOC_NAME_LENGTH_V01 + 1];
	uint32_t instance_id;
	uint8_t service_data_valid;
	uint32_t service_data;
};


struct pd_qmi_client_data {
	char client_name[QMI_SERVREG_LOC_NAME_LENGTH_V01 + 1];
	char service_name[QMI_SERVREG_LOC_NAME_LENGTH_V01 + 1];
	int total_domains;
	int db_rev_count;
	struct servreg_loc_entry_v01 *domain_list;
};

enum service_locator_state {
	LOCATOR_DOWN = 0x0F,
	LOCATOR_UP = 0x1F,
};

#if defined(CONFIG_MSM_SERVICE_LOCATOR)

int get_service_location(char *client_name, char *service_name,
		struct notifier_block *locator_nb);


int find_subsys(const char *pd_path, char *subsys);

#else

static inline int get_service_location(char *client_name,
		char *service_name, struct notifier_block *locator_nb)
{
	return -ENODEV;
}

static inline int find_subsys(const char *pd_path, const char *subsys)
{
	return 0;
}

#endif 

#endif
