#ifndef STMOS_CRT_PANIC_H
#define STMOS_CRT_PANIC_H

#ifdef __cplusplus
extern "C" {
#endif 

__attribute__((noreturn))
void panic(const char *msg);

#ifdef __cplusplus
}
#endif 

#endif
