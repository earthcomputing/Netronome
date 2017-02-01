/*
 * Copyright (C) 2014-2016,  Netronome Systems, Inc.  All rights reserved.
 *
 * @file        flow_cache_test.h
 * @brief       Define hardcoded tests.
 */


#ifndef _FLOW_CACHE_TEST_H_
#define _FLOW_CACHE_TEST_H_


/*
 * Flow cache tests
 */
//#define FLOW_CACHE_TEST                         1       // Test one CAM bucket filled with CAM entries 1, 2, 3, 4
// #define FLOW_CACHE_TEST                         2       // Test one CAM entry (1:1) with linked entries
// #define FLOW_CACHE_TEST                         3       // Test CAM entry 2 with linked entries
// #define FLOW_CACHE_TEST                         4       // Test CAM entry 3 with linked entries
// #define FLOW_CACHE_TEST                         5       // Test CAM entry 4 with linked entries
//#define FLOW_CACHE_TEST                         6       // Test CAM entry 1,2,3,4 with linked entries
// #define FLOW_CACHE_TEST                         7       // Test with many linked entries in one CAM bucket

#ifdef FLOW_CACHE_TEST
#define FLOW_CACHE_TEST_BUCKET_MU                       0               // hard coded MU
#define FLOW_CACHE_TEST_BUCKET_ADDR                     0xe80           // hard coded CAM bucket address
#define FLOW_CACHE_TEST_CAM_ENTRY_1_HASH_VALUE_0        0xaabb          // hard coded hash value for CAM entry 1
#define FLOW_CACHE_TEST_CAM_ENTRY_2_HASH_VALUE_0        0xccdd          // hard coded hash value for CAM entry 2
#define FLOW_CACHE_TEST_CAM_ENTRY_3_HASH_VALUE_0        0xddee          // hard coded hash value for CAM entry 3
#define FLOW_CACHE_TEST_CAM_ENTRY_4_HASH_VALUE_0        0xeeff          // hard coded hash value for CAM entry 4
#endif
#endif
