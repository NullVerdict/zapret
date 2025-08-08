#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>

int unique_size_t(size_t *pu, int ct);
void qsort_size_t(size_t *array, size_t ct);

void rtrim(char *s);
void replace_char(char *s, char from, char to);
const char *strncasestr(const char *s, const char *find, size_t slen);

bool load_file(const char *filename, void *buffer, size_t *buffer_size);
bool load_file_nonempty(const char *filename, void *buffer, size_t *buffer_size);
bool save_file(const char *filename, const void *buffer, size_t buffer_size);
bool append_to_list_file(const char *filename, const char *s);

void expand_bits(void *target, const void *source, unsigned int source_bitlen, unsigned int target_bytelen);

bool strip_host_to_ip(char *host);

void ntop46(const struct sockaddr *sa, char *str, size_t len);
void ntop46_port(const struct sockaddr *sa, char *str, size_t len);
void print_sockaddr(const struct sockaddr *sa);