### Implement user defined syscall in XV6

1. Add a new syscall number in `syscall.h`
2. Add a new syscall lib function in `usys.S`
3. Add a kernel syscall function in the `syscall.c`
4. Write a kernel syscall function
5. Define new functions added to proc.c in `defs.h`
6. Add the syscall function prototype in `user.h`
---
#### 2023-09-27
```c
#define SYS_getnice 24
#define SYS_setnice 25
```
- `syscall.h` 파일에 `getnice`, `setnice` 함수의 시스템콜 번호 선언

```c
SYSCALL(getnice)
SYSCALL(setnice)
```
- `usys.S` 파일에 시스템 콜 래퍼 함수 선언

```c
...
extern int sys_getnice(void);
extern int sys_setnice(void);
...
```
```c
static int(*syscalls[])(void) = {
    ...
    [SYS_getnice] sys_getnice,  
    [SYS_setnice] sys_setnice,
};
```
- `syscall.c` 파일에 kernel 시스템콜 선언

