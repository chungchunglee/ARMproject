#ifndef _BUZZER_H_
#define _BUZZER_H_

// for GPH10
void Buzzer_Init(void);
void Buzzer_Beep(int tone, int duration);
/* Buzzer Beep with Timer4 */
void Beep(int tone, int duration);
void DelayForPlay2(unsigned short time);

#endif
