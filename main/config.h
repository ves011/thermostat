/*
 * config.h
 *
 *  Created on: Jan 4, 2026
 *      Author: viorel_serbu
 */

#ifndef MAIN_CONFIG_H_
#define MAIN_CONFIG_H_


void get_configuration();
void set_configuration(int targett, int histt, int freezet, int onoffc, int maintt);
void register_config();
int do_conf(int argc, char **argv);


#endif /* MAIN_CONFIG_H_ */
