/* credit to https://github.com/graffiti-hypervisor/graffiti-hypervisor/ for code below */
#include "scancode.h"


//255 scancodes in total
char scancodes_map[mapSize];
//char special_convert_map[mapSize];

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
  special_convert_map['3'] = '#';
  special_convert_map['4'] = '$';
  special_convert_map['5'] = '%';
  special_convert_map['6'] = '^';
  special_convert_map['7'] = '&';
  special_convert_map['8'] = '*';
  special_convert_map['9'] = '(';
  special_convert_map['0'] = ')';
  special_convert_map['-'] = '_';
  special_convert_map['='] = '+';
  special_convert_map['['] = '{';
  special_convert_map[']'] = '}';
  special_convert_map['\\'] = '|';
  special_convert_map[';'] = ':';
  special_convert_map['\''] = '\"';
  special_convert_map[','] = '<';
  special_convert_map['.'] = '>';
  special_convert_map['/'] = '?';
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
  scancodes_map[SCANCODE_DASH]  = '-';
  //scancodes_map[SCANCODE_BACKSPACE] = '\b';
  scancodes_map[SCANCODE_EQUAL]  = '=';
  scancodes_map[SCANCODE_YH]  = '\'';
  scancodes_map[SCANCODE_FH]  = ';';
  scancodes_map[SCANCODE_LSB]  = '[';
  scancodes_map[SCANCODE_RSB]  = ']';
  //scancodes_map[SCANCODE_CURSORUP] = 0x3; //some special characters for cursorup
  //scancodes_map[SCANCODE_CURSORDOWN] = 0x4; //some special characters for cursordown
}
