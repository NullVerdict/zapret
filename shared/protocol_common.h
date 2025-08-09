#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Common protocol functions shared between nfq and tpws

// DPI redirects are global redirects to another domain
bool HttpReplyLooksLikeDPIRedirect(const uint8_t *data, size_t len, const char *host);
