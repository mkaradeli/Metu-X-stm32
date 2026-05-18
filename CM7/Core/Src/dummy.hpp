/*
 * dummy.hpp
 *
 *  Created on: May 13, 2026
 *      Author: karadeli
 */

#ifndef SRC_DUMMY_HPP_
#define SRC_DUMMY_HPP_


#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void dummy_init(void);
uint32_t dummy_tick(void);

#ifdef __cplusplus
}
#endif


#endif /* SRC_DUMMY_HPP_ */
