#pragma once

#ifdef __cplusplus
extern "C" {
#endif

size_t GetTotalNewlibHeapSize();

size_t GetAvailableNewlibHeapSize();

size_t GetUsedNewlibHeapSize();

#ifdef __cplusplus
}
#endif
