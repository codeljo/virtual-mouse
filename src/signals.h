#pragma once
#ifndef SIGNALS_H
#define SIGNALS_H

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#ifdef __cplusplus
extern "C" {
#endif

void signal_handler_init();
void signal_handler(int signum, siginfo_t *info, void *ptr);

#ifdef __cplusplus
}
#endif

#endif  /* SIGNALS_H */
