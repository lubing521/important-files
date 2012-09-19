
#pragma once


#ifdef NETSEND_EXPORTS
#define NETSEND_API __declspec(dllexport)
#else
#define NETSEND_API __declspec(dllimport)
#endif

void NETSEND_API __stdcall Send_I(const char *buf, size_t len, size_t task_id, size_t freq, size_t width, size_t height, const char *file_name);


void NETSEND_API __stdcall Send_J(const char *buf, size_t len, size_t task_id, size_t freq, size_t width, size_t height, const char *file_name);