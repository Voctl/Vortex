#ifndef CONFIG_H
#define CONFIG_H

#include "vortex.h"

int  config_load(vortex_config *cfg);
int  config_save(const vortex_config *cfg);
void config_set_defaults(vortex_config *cfg);

#endif
