/* /home/damien/dev/cairo/OpTime.c */
int OpGetTimeInSecs_execute(Op *self, OpContext *ctx);
Op *OpGetTimeInSecs_new(void);
int compute_getgmttime(OpVariable *res, OpVariable *v, String *err_msg);
int check_args_GetGMTTime(OpVariable *v);
Op *OpGetGMTTime_new(void);
int compute_getlocaltime(OpVariable *res, OpVariable *v, String *err_msg);
int check_args_GetLocalTime(OpVariable *v);
Op *OpGetLocalTime_new(void);
