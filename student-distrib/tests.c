#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "scancode.h"
#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");
static int cnt = 0;
static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

int exception_test_de(){
    TEST_HEADER;
    int i = 5, p = 2-2;
    i = i/p;
    return 1;
}

void interruption_test(char c, unsigned char arg)
{
    return;
    if(c=='k') //keyboard test
    {
        if(scancodes_map[arg]!=0)
        {
            clear();
            printf("                      Kbtest: pressed %c   \n", scancodes_map[(int)arg] );
        }
    }
    if(c=='r') //rtc test
    {
        printf("rtcCount: %d\n", ++cnt );
    }
    return;
}

int null_test()
{
    TEST_HEADER;
    int *p = (int*)0;
    *p = *p +1;
    return 1;
}

int out_of_bound_test()
{
    TEST_HEADER;
    int *p = (int*)0x5000020; //arbitrary location 50MB+20bytes
    printf("content at %d is %d",p,*p);
    return 1;
}

int page_test()
{
    TEST_HEADER;
    int *p = (int*)0x500020; //arbitrary location 5MB+20bytes
    printf("content at %d is %d\n",p,*p);
    return 1;
}
// add more tests here

/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
    clear();
	//TEST_OUTPUT("idt_test", idt_test());
    //TEST_OUTPUT("exception_test_de", exception_test_de());
    //TEST_OUTPUT("null_test", null_test());
    //TEST_OUTPUT("out_of_bound_test", out_of_bound_test());
    //TEST_OUTPUT("page_test", page_test());
	// launch your tests here
}
