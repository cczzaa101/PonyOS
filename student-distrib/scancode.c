/* credit to https://github.com/graffiti-hypervisor/graffiti-hypervisor/ for code below */
#include "scancode.h"

#define SCANCODE_CURSORUP	   72   
#define SCANCODE_CURSORLEFT	   75   
#define SCANCODE_CURSORRIGHT	   77
#define SCANCODE_CURSORDOWN	   80

#define SCANCODE_CURSORUPLEFT	   71   
#define SCANCODE_CURSORUPRIGHT	   73
#define SCANCODE_CURSORDOWNLEFT	   79
#define SCANCODE_CURSORDOWNRIGHT   81

#define SCANCODE_CURSORBLOCKUP	  103   
#define SCANCODE_CURSORBLOCKLEFT  105
#define SCANCODE_CURSORBLOCKRIGHT 106
#define SCANCODE_CURSORBLOCKDOWN  108

#define SCANCODE_KEYPAD0	82
#define SCANCODE_KEYPAD1	79
#define SCANCODE_KEYPAD2	80
#define SCANCODE_KEYPAD3	81
#define SCANCODE_KEYPAD4	75
#define SCANCODE_KEYPAD5	76
#define SCANCODE_KEYPAD6	77
#define SCANCODE_KEYPAD7	71
#define SCANCODE_KEYPAD8	72
#define SCANCODE_KEYPAD9	73
#define SCANCODE_KEYPADENTER	96
#define SCANCODE_KEYPADPLUS	78
#define SCANCODE_KEYPADMINUS	74

#define SCANCODE_Q		16
#define SCANCODE_W		17
#define SCANCODE_E		18
#define SCANCODE_R		19
#define SCANCODE_T		20
#define SCANCODE_Y		21
#define SCANCODE_U		22
#define SCANCODE_I		23
#define SCANCODE_O		24
#define SCANCODE_P		25
#define SCANCODE_A		30
#define SCANCODE_S		31
#define SCANCODE_D		32
#define SCANCODE_F		33
#define SCANCODE_G		34
#define SCANCODE_H		35
#define SCANCODE_J		36
#define SCANCODE_K		37
#define SCANCODE_L		38
#define SCANCODE_Z		44
#define SCANCODE_X		45
#define SCANCODE_C		46
#define SCANCODE_V		47
#define SCANCODE_B		48
#define SCANCODE_N		49
#define SCANCODE_M		50

#define SCANCODE_FSLASH         53
#define SCANCODE_BSLASH         43

#define SCANCODE_ESCAPE		1
#define SCANCODE_ENTER		28
#define SCANCODE_RIGHTCONTROL	97
#define SCANCODE_CONTROL	97
#define SCANCODE_RIGHTALT	100
#define SCANCODE_LEFTCONTROL	29
#define SCANCODE_LEFTCONTROL_RELEASE	(29+128)
#define SCANCODE_LEFTALT        56

#define SCANCODE_SPACE		57
#define SCANCODE_LEFTSHIFT	42
#define SCANCODE_RIGHTSHIFT	54
#define SCANCODE_LEFTSHIFT	42
#define SCANCODE_LEFTSHIFT_RELEASE	(42+128)
#define SCANCODE_RIGHTSHIFT_RELEASE	(54+128)
#define SCANCODE_TAB		15
#define SCANCODE_CAPSLOCK	58
#define SCANCODE_BACKSPACE	14

#define SCANCODE_F1             59
#define SCANCODE_F2             60
#define SCANCODE_F3             61
#define SCANCODE_F4             62
#define SCANCODE_F5             63
#define SCANCODE_F6             64
#define SCANCODE_F7             65
#define SCANCODE_F8             66
#define SCANCODE_F9             67
#define SCANCODE_F10            68
#define SCANCODE_F11            87
#define SCANCODE_F12            88

#define SCANCODE_1		 2
#define SCANCODE_2               3
#define SCANCODE_3               4
#define SCANCODE_4               5
#define SCANCODE_5               6
#define SCANCODE_6               7
#define SCANCODE_7               8
#define SCANCODE_8               9
#define SCANCODE_9               10
#define SCANCODE_0               11

#define SCANCODE_DASH		 12
#define SCANCODE_EQUAL		 13

#define SCANCODE_PAGEUP		 104
#define SCANCODE_PAGEDOWN        109
#define SCANCODE_HOME            102
#define SCANCODE_END             107
#define SCANCODE_INSERT          110
#define SCANCODE_DELETE          111

#define SCANCODE_COMMA           51
#define SCANCODE_POINT           52
#define mapSize 255
//255 scancodes in total
char scancodes_map[mapSize];
char special_convert_map[mapSize];

/*
Initialize scancodes map
Input: None
Output: None
Effect: Initialize the scancodes map above
*/
void init_scancodes_map(void)
{
  int i;  
  for(i='a'; i<='z'; i++)
      special_convert_map[i] = i + 'A' - 'a';
  
  for(i='A'; i<='Z'; i++)
      special_convert_map[i] = i + 'a' - 'A';
  
  special_convert_map['1'] = '!';
  special_convert_map['2'] = '@';
  for(i=0; i<mapSize; i++)
      scancodes_map[i] = 0;

  scancodes_map[SCANCODE_KEYPAD0] = '0';
  scancodes_map[SCANCODE_KEYPAD1] = '1';
  scancodes_map[SCANCODE_KEYPAD2] = '2';
  scancodes_map[SCANCODE_KEYPAD3] = '3';
  scancodes_map[SCANCODE_KEYPAD4] = '4';
  scancodes_map[SCANCODE_KEYPAD5] = '5';
  scancodes_map[SCANCODE_KEYPAD6] = '6';
  scancodes_map[SCANCODE_KEYPAD7] = '7';
  scancodes_map[SCANCODE_KEYPAD8] = '8';
  scancodes_map[SCANCODE_KEYPAD9] = '9';
  scancodes_map[SCANCODE_KEYPADPLUS]  = '+';
  scancodes_map[SCANCODE_KEYPADMINUS] = '-';

  scancodes_map[SCANCODE_0] = '0';
  scancodes_map[SCANCODE_1] = '1';
  scancodes_map[SCANCODE_2] = '2';
  scancodes_map[SCANCODE_3] = '3';
  scancodes_map[SCANCODE_4] = '4';
  scancodes_map[SCANCODE_5] = '5';
  scancodes_map[SCANCODE_6] = '6';
  scancodes_map[SCANCODE_7] = '7';
  scancodes_map[SCANCODE_8] = '8';
  scancodes_map[SCANCODE_9] = '9';

  scancodes_map[SCANCODE_Q] = 'q';
  scancodes_map[SCANCODE_W] = 'w';
  scancodes_map[SCANCODE_E] = 'e';
  scancodes_map[SCANCODE_R] = 'r';
  scancodes_map[SCANCODE_T] = 't';
  scancodes_map[SCANCODE_Y] = 'y';
  scancodes_map[SCANCODE_U] = 'u';
  scancodes_map[SCANCODE_I] = 'i';
  scancodes_map[SCANCODE_O] = 'o';
  scancodes_map[SCANCODE_P] = 'p';
  scancodes_map[SCANCODE_A] = 'a';
  scancodes_map[SCANCODE_S] = 's';
  scancodes_map[SCANCODE_D] = 'd';
  scancodes_map[SCANCODE_F] = 'f';
  scancodes_map[SCANCODE_G] = 'g';
  scancodes_map[SCANCODE_H] = 'h';
  scancodes_map[SCANCODE_J] = 'j';
  scancodes_map[SCANCODE_K] = 'k';
  scancodes_map[SCANCODE_L] = 'l';
  scancodes_map[SCANCODE_Z] = 'z';
  scancodes_map[SCANCODE_X] = 'x';
  scancodes_map[SCANCODE_C] = 'c';
  scancodes_map[SCANCODE_V] = 'v';
  scancodes_map[SCANCODE_B] = 'b';
  scancodes_map[SCANCODE_N] = 'n';
  scancodes_map[SCANCODE_M] = 'm';

  scancodes_map[SCANCODE_SPACE]  = ' ';
  scancodes_map[SCANCODE_TAB]    = '\t';
  scancodes_map[SCANCODE_ENTER]  = '\n';
  scancodes_map[SCANCODE_COMMA]  = ',';
  scancodes_map[SCANCODE_POINT]  = '.';
  scancodes_map[SCANCODE_FSLASH] = '\\';
  scancodes_map[SCANCODE_BSLASH] = '/';
  scancodes_map[SCANCODE_BACKSPACE] = '\b';
  scancodes_map[SCANCODE_EQUAL]  = '=';

  scancodes_map[SCANCODE_CURSORUP] = 0x3; //some special characters for cursorup
  scancodes_map[SCANCODE_CURSORDOWN] = 0x4; //some special characters for cursordown
}
