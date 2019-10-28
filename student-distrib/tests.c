#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "keyboard.h"
#include "terminal.h"
#include "filesys.h"
#include "rtc.h"

#define PASS 1
#define FAIL 0
#define BUF_SIZE 3
#define TEST_FD 2


/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

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

/* Paging Test NULL
 *
 * Tries to dereference null
 *
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: init paging
 * Files: page.c/h pageasm.S
 */
int page_test_null(){
    TEST_HEADER;

    int result = PASS;
    uint32_t* temp = NULL;
    *temp = 3;

    return result;
}

/* Paging Test video mem
 *
 * Tries to write to video memory
 *
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: init paging
 * Files: page.c/h pageasm.S
 */
int page_test_video(){
    TEST_HEADER;

    int result = PASS;
    putc('*'); //putc writes to video memory

    return result;
}

/* Paging Test dereference kernel address outside
 *
 * Tries to dereference an address outside the kernel
 *
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: init paging
 * Files: page.c/h pageasm.S
 */
int page_test_deref_out(){
    TEST_HEADER;

    int result = PASS;
    uint32_t* temp = (uint32_t* )0xFFFFFFF;
    *temp = 3;

    return result;
}

/* Paging Test dereference inside kernel
 *
 * Tries to dereference something from inside the kernel
 *
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: init paging
 * Files: page.c/h pageasm.S
 */
int page_test_deref_in(){
    TEST_HEADER;

    int result = PASS;
    uint32_t* temp = (uint32_t*)0x400005;
    temp = NULL;

    return result;
}
// add more tests here

/* Checkpoint 2 tests */
// read_file_test
// print the stuff inside of a file, read by name
void read_file_test(){
    int32_t fd = 2;
    int32_t nbytes = 10000;
    uint8_t buf[10000];
    int i;
    clear_kb();
    i = file_open((uint8_t*)"syserr");
    i = file_read(fd, buf, nbytes);

    unsigned j = 0;
    for(; j < 10000; j++){
     	if (buf[j])
     		putc(buf[j]);
    }
}

/*list_files
 * lists the files in the current directory
 *
 */
void list_files(){
    int i = 0;
    uint8_t str[32];
    int retval;
    // dentry_t dentry;
    printf("Printing files in current directory...\n");
    for (; i < 16; i++){
        retval = dir_read((uint8_t*)&str);
        printf("file %d: %s\n",i, str);
        rtc_read();
    }
    return;
}

void test_rtc_write(){
    uint32_t freq = 2;
    int i = 0;
    int count = 0;
    while (count < 10){
        // clear_kb();
        rtc_write(freq);
        // printf("RATE: %d HZ", freq);

        while (i < freq*freq){
            rtc_read();
            if(i % 2 == 0){
                int32_t fd = 0;
                int32_t nbytes = 1024;
                uint8_t buf[1024];
                int i;
                clear_kb();
                i = file_open((uint8_t*)"frame0.txt");
                i = file_read(fd, buf, nbytes);

                unsigned j = 0;
                for(; j < 1024; j++){
                    putc(buf[j]);
                }
            }
            else{
                int32_t fd = 0;
                int32_t nbytes = 1024;
                uint8_t buf[1024];
                int i;
                clear_kb();
                i = file_open((uint8_t*)"frame1.txt");
                i = file_read(fd, buf, nbytes);

                unsigned j = 0;
                for(; j < 1024; j++){
                    putc(buf[j]);
                }
            }
            i++;
        }
        i = 0;
        count++;
        // freq <<= 1;
    }
}

void test_rtc_write2(){
	int freq = 2;
	int wait = 0;
	while (freq <= 1024){
		clear_kb();
		rtc_write(freq);
		printf("%d HZ R", freq);
		while (wait < freq*6){
			putc('E');
			rtc_read();
			wait++;
		}
		wait = 0;
		freq *= 2;

	}

}

int terminal_write_works(){
	TEST_HEADER;

	int result = PASS;
	int temp;
	char test_buf[3];
	test_buf[0] = '3';
	test_buf[1] = '9';
	test_buf[2] = '1';

	// write 3 chars to the buffer
	temp = terminal_write(TEST_FD, test_buf, BUF_SIZE);

	if (temp != BUF_SIZE){
		result = FAIL;
	}
	else{
		result = PASS;
	}

	return result;
}

int terminal_read_works(){
	TEST_HEADER;

	int result = PASS;
	int temp;
	char test_buf[BUF_SIZE];
	test_buf[0] = '3';
	test_buf[1] = '9';
	test_buf[2] = '1';

	// write 3 chars to the buffer
	memmove(buf_kb, &test_buf, BUF_SIZE);
	temp = terminal_read(TEST_FD, buf_kb, BUF_SIZE);

	if (temp != BUF_SIZE){
		result = FAIL;
	}
	else{
		result = PASS;
	}

	memset(&buf_kb, 0, BUFFER_SIZE);	

	return result;
}









/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
    // ----------------------------CHECKPOINT 1 TESTS--------------------------------
	//TEST_OUTPUT("idt_test", idt_test());
	// launch your tests here
	//TEST_OUTPUT("page_test_null", page_test_null());
	//TEST_OUTPUT("page_test_video", page_test_video());
	//TEST_OUTPUT("page_test_deref_in", page_test_deref_in());
	//TEST_OUTPUT("page_test_deref_out", page_test_deref_out());

	// -----------------------------CHECKPOINT 2 TESTS-------------------------------
    clear_kb();
    list_files();
    int i;
    for(i = 0; i < 10; i++){
        rtc_read();
    }
    test_rtc_write();
    for(i = 0; i < 10; i++){
        rtc_read();
    }
    read_file_test();
    for(i = 0; i < 10; i++){
        rtc_read();
    }
	test_rtc_write2();
	clear_kb();
	rtc_close();
	//TEST_OUTPUT("terminal_write_works", terminal_write_works());
	//TEST_OUTPUT("terminal_read_works", terminal_read_works());

}
