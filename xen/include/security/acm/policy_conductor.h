#include "acm.h"

#define ACM_MODELNAME_MAXLEN	32
#define ACM_MODEL_MAX		8


struct acm_policy_header{
	int pc_magic_num;
	int acdm_magic_num;
	int policy_type;	/* either system-defined or user-defined */
	int bin_size;
	int version;
};

typedef struct _ac_model_info{
	char model_name[ACM_MODELNAME_MAXLEN];
	struct ac_model_ops *acdm_ops;
}ac_model_info;

struct acdm_ops{
	uint16_t (* get_decision)(aci_domain *subj, aci_object *obj, uint32_t req_type, aci_context *context);
	int (* set_policy)(void *bin, int size);
	int (* relocate_policy)(void *bin);
	void (* control_cpu_usage)(struct domain *d);
	uint16_t (* control_battery_usage)(struct domain *d, int usage);
};

struct ac_model{
	char model_name[ACM_MODELNAME_MAXLEN];
	int32_t storage_index;
	uint32_t magic_num;
	struct acdm_ops *ops;
	void *policy_bin;
	unsigned int bin_size;
	int32_t version;
};

#ifdef CONFIG_VMM_SECURITY_ACM
int init_acm(void);
int register_decision_maker(char *model_name, uint32_t magic_num, struct acdm_ops *ops);
struct ac_model *get_ac_models_ptr(void);

int init_te(void);
int init_chinesewall(void);

#else
inline int init_acm(void){ return 1; }
inline int register_decision_maker(char *model_name, uint32_t magic_num, struct acdm_ops *ops){ return 0; }
#endif

