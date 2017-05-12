#ifndef SYS_TIME_H
#define SYS_TIME_H

void InitTimeSystem();
void UpdateTimeSystem();
float GetTime();

#ifdef MOCK_TIME
void SetTime(float);
#endif

#endif
