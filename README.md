This repository implements an example Redis Module to demonstrate a bug related Redis timers to using [`RedisModule_CreateTimer`](https://redis.io/docs/reference/modules/modules-api-ref/#redismodule_createtimer).

Because of an overflow bug [in this line](https://github.com/redis/redis/blob/7c179f9bf4390512196b3a2b2ad6d0f4cb625c8a/src/ae.c#L336), when a timer event returns with a `retval` greater than 2147483 ms, which is about 35 minutes, then the computation for `te->when` will be distorted, potentially resulting in `te->when` being set in the past. As a result, calls to [`usUntilEarliestTimer` will return 0](https://github.com/redis/redis/blob/7c179f9bf4390512196b3a2b2ad6d0f4cb625c8a/src/ae.c#L276) for as long as `retval` returns the overflowing value. While `usUntilEarliestTimer` continues to return `0`, `aeApiPoll` will have a no timeout and so Redis will use significantly more CPU.

### Recreation Steps

0. These steps assume that the Redis Server is installed locally and that the `redis-server` command is available on the system path.

1. Clone the repository and compile the module with the following commands
```
git clone https://github.com/nirrattner/RedisModuleTimerExample.git
cd RedisModuleTimerExample
make
```

2. Start the Redis Server without the module to get a baseline for CPU usage with the following command
```
redis-server
```

3. In a second terminal or using another tool, measure the baseline CPU Usage for the Redis server process
```
top
```

3. In the original terminal, restart the Redis Server loading the module with the following command
```
redis-server --loadmodule module.so
```

4. In the second terminal or with with the other tool, observe that the Redis server process is using much more CPU

