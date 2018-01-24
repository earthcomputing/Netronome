/*
 * Copyright (C) 2018,  Earth Computing Inc.  All rights reserved.
 *
 *
 * @file          entl_sm2_test_main.cpp
 * @brief         ENTL State Machine Unit Test main
 *
 * Author:        Atsushi Kasuya
 *
 */

#include <iostream>
#include <time.h>
#include <string>
#include <unistd.h>
#include "entl_sm2_tester.h"
#include "cyc_random.h"
#include "gtest/gtest.h"


static entl_state_machine_t *mcn0, *mcn1 ;
//static cyc_random_t *rd0, *rd1, *rbit ;

class EntlUnitTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        entl_state_init_all() ;
        mcn0 = get_entl_state_machine(0) ;
        mcn1 = get_entl_state_machine(1) ;
        mcn0->name = "DUT0" ;
        mcn1->name = "DUT1"
        //rd0 = alloc_cyc_random( 5 ) ;
        //rd1 = alloc_cyc_random( 5 ) ;
        //rbit = alloc_cyc_random( 6 ) ;
    }
    
    virtual void TearDown() {
        printf( "EntlUnitTest %s done\n", testName.c_str() ) ;
    }
    
    std::string testName ;
};

// Testing Hello to Wait transition
TEST_F( EntlUnitTest, EntlHello2WaitUnitTest ) {
    int result ;

    result = hello_2_wait( mcn0 ) ;

    EXPECT_EQ( result, 1 ) ;

}

// Testing Hello to Wait transition
TEST_F( EntlUnitTest, EntlHello2HelloUnitTest ) {
    int result ;

    result = hello_2_hello( &mcn ) ;

    EXPECT_EQ( result, 1 ) ;

}

// Testing Hello to Wait transition
TEST_F( EntlUnitTest, EntlHello2EntlUnitTest ) {
    int result ;

    result = hello_2_entl( &mcn ) ;

    EXPECT_EQ( result, 1 ) ;

}

// Testing Hello to Wait transition
TEST_F( EntlUnitTest, EntlHello2Wait2EntlUnitTest ) {
    int result ;

    result = hello_2_wait_2_entl( &mcn ) ;

    EXPECT_EQ( result, 1 ) ;

}


// Testing Hello to Wait transition
TEST_F( EntlUnitTest, EntlHello2EntlLoopUnitTest ) {
    int result ;

    result = hello_2_entl_loop( &mcn ) ;

    EXPECT_EQ( result, 1 ) ;

}

// Testing Hello to Wait to ENTL loop transition
TEST_F( EntlUnitTest, EntlHello2Wait2EntlLoopUnitTest ) {
    int result ;

    result = hello_2_wait_2_entl_loop( &mcn ) ;

    EXPECT_EQ( result, 1 ) ;

}


// Testing Hello to Wait to ENTL loop transition
TEST_F( EntlUnitTest, EntlAitSendUnitTest ) {
    int result ;

    result = entl_ait_send( &mcn ) ;

    EXPECT_EQ( result, 1 ) ;

}

// Testing Hello to Wait to ENTL loop transition
TEST_F( EntlUnitTest, EntlAitRejectedUnitTest ) {
    int result ;
    
    result = entl_ait_rejected( &mcn ) ;

    EXPECT_EQ( result, 1 ) ;

}

// Testing Hello to Wait to ENTL loop transition
TEST_F( EntlUnitTest, EntlAitReceiveUnitTest ) {
    int result ;
    
    result = entl_ait_receive( &mcn ) ;

    EXPECT_EQ( result, 1 ) ;

}

// Testing Hello to Wait to ENTL loop transition
TEST_F( EntlUnitTest, EntlAitRejectUnitTest ) {
    int result ;
    
    result = entl_ait_reject( &mcn ) ;

    EXPECT_EQ( result, 1 ) ;

}

// Testing Hello to Wait to ENTL loop transition
TEST_F( EntlUnitTest, EntlAloSendUnitTest ) {
    int result ;
    
    result = entl_alo_send( &mcn ) ;

    EXPECT_EQ( result, 1 ) ;

}

// Testing Hello to Wait to ENTL loop transition
TEST_F( EntlUnitTest, EntlAloReceiveUnitTest ) {
    int result ;
    
    result = entl_alo_receive( &mcn ) ;

    EXPECT_EQ( result, 1 ) ;

}


// Testing Hello to Wait transition
TEST_F( EntlUnitTest, EntlHello2HelloSameValueUnitTest ) {
    int result ;

    result = hello_2_hello_same_value( &mcn ) ;

    EXPECT_EQ( result, 1 ) ;

}

TEST_F( EntlUnitTest, EntlHelloErrorHelloUnitTest ) {
    int result ;

    result = hello_error_hello( &mcn ) ;

    EXPECT_EQ( result, 1 ) ;

}

TEST_F( EntlUnitTest, EntlHelloEntlErrorUnitTest ) {
    int result ;

    result = hello_2_entl_error( &mcn ) ;

    EXPECT_EQ( result, 1 ) ;

}

TEST_F( EntlUnitTest, EntlHelloWaitEntlErrorUnitTest ) {
    int result ;

    result = hello_2_wait_2_entl_error( &mcn ) ;

    EXPECT_EQ( result, 1 ) ;

}

TEST_F( EntlUnitTest, EntlAitSendErrorUnitTest ) {
    int result ;

    result = entl_ait_send_error( &mcn ) ;

    EXPECT_EQ( result, 1 ) ;

}

TEST_F( EntlUnitTest, EntlAitSendComErrorUnitTest ) {
    int result ;

    result = entl_ait_send_comm_error( &mcn ) ;

    EXPECT_EQ( result, 1 ) ;

}

GTEST_API_ int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);


    return RUN_ALL_TESTS();
}
