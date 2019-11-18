#ifndef _KEY_H_
#define _KEY_H_


//Function Declaration 
void Key_Port_Init(void);
void Key_IRQ_Port_Init(void);
int Key_Get_Pressed(void);
int Key_Wait_Get_Pressed(void);
void Key_Wait_Get_Released(void);



#endif
