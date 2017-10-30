#include "tests.h"
#include "fileSystem.h"
#include "x86_desc.h"
#include "lib.h"
#include "scancode.h"
#define INT_SIZE 4
#define PASS 1
#define FAIL 0
#define KB_IDT 0x21
#define RTC_IDT 0x28
#define MAX_NAME_SIZE 32
#define MAX_CONTENT_SIZE 128
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
	for (i = 0; i < NUM_VEC; ++i){ //test all exceptions
        
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}
/*
    if ((idt[KB_IDT].offset_15_00 == NULL) && 
			(idt[KB_IDT].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
        
    if ((idt[RTC_IDT].offset_15_00 == NULL) && 
			(idt[RTC_IDT].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}    
*/        
	return result;
}

/* de exception test
 * 
 * test divide by zero exception
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: bluescreen
 * Coverage: Load IDT, IDT definition
 * Files: idt.c, idt.h
 */
int exception_test_de(){
    TEST_HEADER;
    int i = 5, p = 2-2; // calculate 5/0
    i = i/p;
    return 1;
}

/* more exception test
 * 
 * test any exception by changeing the interrupt id below
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: bluescreen
 * Coverage: Load IDT, IDT definition
 * Files: idt.c, idt.h
 */
 
int exception_test(){
    TEST_HEADER;
    asm volatile("int $2");
    return 1;
}

/* interruption test
 * 
 * test interruption from keyboard and rtc. comment return at line91 to enable
 * Inputs: c to discrete kb/rtc, arg for printing kb's characters
 * Outputs: None
 * Side Effects: print test message
 * Coverage: interrupts of keyboard and rtc
 * Files: keyboard.c, rtc.c, i8259.c
 */
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

/* null pointer test
 * 
 * test dereferencing null points
 * Inputs: None
 * Outputs: 1
 * Side Effects: blue screen
 * Coverage: paging setup, exception handling
 * Files: paging.c, idt.c
 */
int null_test()
{
    TEST_HEADER;
    int *p = (int*)0;
    *p = *p +1;
    return 1;
}

/* invalid pointer test
 * 
 * test dereferencing pointer out of kernel
 * Inputs: None
 * Outputs: 1
 * Side Effects: blue screen
 * Coverage: paging setup, exception handling
 * Files: paging.c, idt.c
 */
int out_of_bound_test()
{
    TEST_HEADER;
    int *p = (int*)0x5000020; //arbitrary location 50MB+20bytes
    printf("content at %d is %d",p,*p);
    return 1;
}

/* valid pointer test
 * 
 * test dereferencing pointer within kernel
 * Inputs: None
 * Outputs: 1
 * Side Effects: print content at 0x500020
 * Coverage: paging setup
 * Files: paging.c
 */
int page_test()
{
    TEST_HEADER;
    int *p = (int*)0x500020; //arbitrary location 5MB+20bytes
    printf("content at %d is %d\n",p,*p);
    return 1;
}
// add more tests here

/* Checkpoint 2 tests */

int read_input_test(){
	TEST_HEADER;
	char str[150];
	strcpy(str,kb_read());
	if (strlen(str) > MAX_CONTENT_SIZE){
		return FAIL;
	}
	puts(str);
	return PASS;
}

// The length of strshort is within the range of the buffer while the strlong is not
int write_strings_test(){
	TEST_HEADER;
	char strshort[]="Sample Test";  
	char strlong[]="1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567\n if u c the char its wrong";
	t_write(strshort);
	t_write(strlong);
	return 1;
}

int print_list_of_files_test(){
	TEST_HEADER;
	char buf[MAX_NAME_SIZE];
	while(dir_read(buf)==0){
		printf("%s\n",buf);
	}
	
	return 1;
}



int print_contents_test(){
	TEST_HEADER;
	char buf[MAX_CONTENT_SIZE]; 
	char fname[] = "cat";
	filesys_read_by_name(fname,buf,MAX_CONTENT_SIZE);
	printf("%s",buf);
	return 1;
}

int rtc_test(){
	TEST_HEADER;
	int i;
	int *freq; 
	
	clear();
	*freq =1;
	rtc_write(freq, INT_SIZE);
	for (i=0;i<500;i++){
		if (rtc_read() == 0){
			putc('1');
		}		
	}
	
	clear();
	*freq =16;
	rtc_write(freq, INT_SIZE);
	for (i=0;i<500;i++){
		if (rtc_read() == 0){
			putc('1');
		}		
	}
	
	clear();
	*freq = 128;
	rtc_write(freq, INT_SIZE);
	for (i=0;i<500;i++){
		if (rtc_read() == 0){
			putc('1');
		}		
	}
	
	clear();
	*freq = 512;
	rtc_write(freq, INT_SIZE);
	for (i=0;i<500;i++){
		if (rtc_read() == 0){
			putc('1');
		}		
	}
	
	clear();
	*freq = 1024;
	rtc_write(freq, INT_SIZE);
	for (i=0;i<500;i++){
		if (rtc_read() == 0){
			putc('1');
		}		
	}
	
	clear();
	*freq = 2048;
	rtc_write(freq, INT_SIZE);
	for (i=0;i<500;i++){
		if (rtc_read() == 0){
			putc('1');
		}		
	}
	
	clear();
	*freq = 666;
	rtc_write(freq, INT_SIZE);
	for (i=0;i<500;i++){
		if (rtc_read() == 0){
			putc('1');
		}		
	}
	
}


/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
    clear();
	//Checkpoint 1
	//TEST_OUTPUT("idt_test", idt_test());
    //TEST_OUTPUT("exception_test_de", exception_test_de());
    //TEST_OUTPUT("null_test", null_test());
    //TEST_OUTPUT("out_of_bound_test", out_of_bound_test());
    //TEST_OUTPUT("page_test", page_test());
    //TEST_OUTPUT("exception_test", exception_test());

	//Checkpoint 2
    //TEST_OUTPUT("read_input_test", read_input_test());
    //TEST_OUTPUT("write_strings_test", write_strings_test());
    //TEST_OUTPUT("print_list_of_files_test", print_list_of_files_test());
    //TEST_OUTPUT("print_contents_tests", print_contents_test());
    //TEST_OUTPUT("rtc_test", rtc_test());
    
	// launch your tests here
}




