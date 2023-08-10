#include <string.h>

#include "redismodule.h"

static void timer_callback(RedisModuleCtx *module_context, void *data);

int RedisModule_OnLoad(RedisModuleCtx *module_context) {
  int result;

  result = RedisModule_Init(
      module_context,
      "test-timer",
      1,
      REDISMODULE_APIVER_1);
  if (result) {
    return REDISMODULE_ERR;
  }

  // Initial timer to expire quickly and trigger next event calculation
  RedisModule_CreateTimer(
      module_context,
      1000,
      timer_callback,
      NULL);

  // Timer which will overflow when millisecond value is scaled to microseconds 
  // in 32 bits
  // 40 * 60 * 1000 * 1000 > 0x7fffffff
  RedisModule_CreateTimer(
      module_context,
      40 * 60 * 1000,
      timer_callback,
      NULL);

  RedisModule_Log(module_context, "warning", "Initialized test timer module");

  return REDISMODULE_OK;
}

static void timer_callback(RedisModuleCtx *module_context, void *data) {
  RedisModule_Log(module_context, "warning", "Timer callbak");
}

