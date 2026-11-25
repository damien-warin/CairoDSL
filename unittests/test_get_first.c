#include <Op.h>
#include <OpProgram.h>
#include <stdio.h>
#include <stdlib.h>
#include <String.h>
#include <string.h>
#include <math.h>

#define D1 42.0
#define D2 13.0
#define S1 "foo"
#define S2 "bar"


int get_first_in_NONE(void)
{
    int ret = 0;
    double d;
    OpVariable v1;
    OpProgram p;
    OpContext c;
    String err;
    OpVariable_init(&v1);

    OpProgram_init(&p);
    OpContext_init(&c);
    OpProgram_set_context(&p, &c);
    Op *gv = OpGetValue_new(), *gf = OpGetFirst_new();

    OpGetValue_copy_variable((OpGetValue*)gv, &v1);

    Op1_set_operande((Op1*)gf, gv);

    Op_fix_operandes(gf, &c);

    OpContext_set_running_state(&c, gv, Run, "");

    if(Op_execute(gf,&c) != -1)
    {
    	ret = -1;
    	fprintf(stderr, "get first with STRING should not work\n");
    }

    Op_free(gf);
    OpContext_terminate(&c);
    OpProgram_terminate(&p);
    OpVariable_terminate(&v1);
    return ret;
}

int get_first_in_string(void)
{
    int ret = 0;
    double d;
    OpVariable v1;
    OpProgram p;
    OpContext c;
    String err;
    OpVariable_init(&v1);
    OpVariable_set_string(&v1, S1);

    OpProgram_init(&p);
    OpContext_init(&c);
    OpProgram_set_context(&p, &c);
    Op *gv = OpGetValue_new(), *gf = OpGetFirst_new();

    OpGetValue_copy_variable((OpGetValue*)gv, &v1);

    Op1_set_operande((Op1*)gf, gv);

    Op_fix_operandes(gf, &c);

    OpContext_set_running_state(&c, gv, Run, "");

    if(Op_execute(gf,&c) != 0)
    {
    	ret = -1;
    	fprintf(stderr, "get first with STRING should work\n");
    }
    if(OpVariable_get_type(OpContext_get_current_value(&c)) != STRING)
    {
    	ret = -1;
    	fprintf(stderr, "get first with STRING should give STRING\n");
    }
    if(OpVariable_get_number_elements(OpContext_get_current_value(&c)) != 2)
    {
    	ret = -1;
    	fprintf(stderr, "get first with STRING should give single char STRING\n");
    }
    if(OpVariable_get_string(OpContext_get_current_value(&c))[0] != S1[0])
    {
    	ret = -1;
    	fprintf(stderr, "get first with STRING should give single char STRING %c and not %c\n", S1[0], OpVariable_get_string(OpContext_get_current_value(&c))[0]);
    }

    Op_free(gf);
    OpContext_terminate(&c);
    OpProgram_terminate(&p);
    OpVariable_terminate(&v1);
    return ret;
}

int get_first_in_empty_string(void)
{
    int ret = 0;
    double d;
    OpVariable v1;
    OpProgram p;
    OpContext c;
    String err;
    OpVariable_init(&v1);
    OpVariable_set_string(&v1, "");

    OpProgram_init(&p);
    OpContext_init(&c);
    OpProgram_set_context(&p, &c);
    Op *gv = OpGetValue_new(), *gf = OpGetFirst_new();

    OpGetValue_copy_variable((OpGetValue*)gv, &v1);

    Op1_set_operande((Op1*)gf, gv);

    Op_fix_operandes(gf, &c);

    OpContext_set_running_state(&c, gv, Run, "");

    if(Op_execute(gf,&c) != 0)
    {
    	ret = -1;
    	fprintf(stderr, "get first with STRING should work\n");
    }
    if(OpVariable_get_type(OpContext_get_current_value(&c)) != STRING)
    {
    	ret = -1;
    	fprintf(stderr, "get first with STRING should give STRING\n");
    }
    if(strcmp(OpVariable_get_string(OpContext_get_current_value(&c)), "") != 0)
    {
    	ret = -1;
    	fprintf(stderr, "get first with STRING should give empty STRING\n");
    }

    Op_free(gf);
    OpContext_terminate(&c);
    OpProgram_terminate(&p);
    OpVariable_terminate(&v1);
    return ret;
}

int get_first_in_array_of_strings(void)
{
    int ret = 0;
    double d;
    OpVariable v1;
    OpProgram p;
    OpContext c;
    String err;
    OpVariable_init(&v1);
    OpVariable_append_string(&v1, S1);
    OpVariable_append_string(&v1, S2);

    OpProgram_init(&p);
    OpContext_init(&c);
    OpProgram_set_context(&p, &c);
    Op *gv = OpGetValue_new(), *gf = OpGetFirst_new();

    OpGetValue_copy_variable((OpGetValue*)gv, &v1);

    Op1_set_operande((Op1*)gf, gv);

    Op_fix_operandes(gf, &c);

    OpContext_set_running_state(&c, gv, Run, "");

    if(Op_execute(gf,&c) != 0)
    {
    	ret = -1;
    	fprintf(stderr, "get first with STRINGS should work\n");
    }
    if(OpVariable_get_type(OpContext_get_current_value(&c)) != STRING)
    {
    	ret = -1;
    	fprintf(stderr, "get first with STRINGS should give STRING\n");
    }
    if(OpVariable_get_number_elements(OpContext_get_current_value(&c)) != sizeof(S1))
    {
    	ret = -1;
    	fprintf(stderr, "get first with STRINGS should give single char STRING\n");
    }
    if(strcmp(OpVariable_get_string(OpContext_get_current_value(&c)), S1) != 0)
    {
    	ret = -1;
    	fprintf(stderr, "get first with STRINGS should give STRING %s and not %s\n", S1, OpVariable_get_string(OpContext_get_current_value(&c)));
    }

    Op_free(gf);
    OpContext_terminate(&c);
    OpProgram_terminate(&p);
    OpVariable_terminate(&v1);
    return ret;
}

int get_first_in_array_of_strings_empty(void)
{
    int ret = 0;
    double d;
    OpVariable v1;
    OpProgram p;
    OpContext c;
    String err;
    OpVariable_init(&v1);
    OpVariable_set_type(&v1, STRINGS);

    OpProgram_init(&p);
    OpContext_init(&c);
    OpProgram_set_context(&p, &c);
    Op *gv = OpGetValue_new(), *gf = OpGetFirst_new();

    OpGetValue_copy_variable((OpGetValue*)gv, &v1);

    Op1_set_operande((Op1*)gf, gv);

    Op_fix_operandes(gf, &c);

    OpContext_set_running_state(&c, gv, Run, "");

    if(Op_execute(gf,&c) != -1)
    {
    	ret = -1;
    	fprintf(stderr, "get first with STRINGS empty should not work\n");
    }

    Op_free(gf);
    OpContext_terminate(&c);
    OpProgram_terminate(&p);
    OpVariable_terminate(&v1);
    return ret;
}

int get_first_in_array_of_doubles_empty(void)
{
    int ret = 0;
    OpVariable v1;
    OpProgram p;
    OpContext c;
    String err;
    OpVariable_init(&v1);
    OpVariable_set_type(&v1, DOUBLES);

    OpProgram_init(&p);
    OpContext_init(&c);
    OpProgram_set_context(&p, &c);
    Op *gv = OpGetValue_new(), *gf = OpGetFirst_new();

    OpGetValue_copy_variable((OpGetValue*)gv, &v1);

    Op1_set_operande((Op1*)gf, gv);

    Op_fix_operandes(gf, &c);

    OpContext_set_running_state(&c, gv, Run, "");

    if(Op_execute(gf,&c) != -1)
    {
    	ret = -1;
    	fprintf(stderr, "get first with DOUBLES empty should not work\n");
    }

    Op_free(gf);
    OpContext_terminate(&c);
    OpProgram_terminate(&p);
    OpVariable_terminate(&v1);
    return ret;
}

int get_first_in_array_of_doubles_not_empty(void)
{
    int ret = 0;
    OpVariable v1;
    OpProgram p;
    OpContext c;
    String err;
    OpVariable_init(&v1);
    OpVariable_append_double(&v1, D1);
    OpVariable_append_double(&v1, D2);

    OpProgram_init(&p);
    OpContext_init(&c);
    OpProgram_set_context(&p, &c);
    Op *gv = OpGetValue_new(), *gf = OpGetFirst_new();

    OpGetValue_copy_variable((OpGetValue*)gv, &v1);

    Op1_set_operande((Op1*)gf, gv);

    Op_fix_operandes(gf, &c);

    OpContext_set_running_state(&c, gv, Run, "");

    if(Op_execute(gf,&c) != 0)
    {
    	ret = -1;
    	fprintf(stderr, "get first with DOUBLES should work\n");
    }
    if(OpVariable_get_type(OpContext_get_current_value(&c)) != DOUBLE)
    {
    	ret = -1;
    	fprintf(stderr, "get first with DOUBLES should give DOUBLE\n");
    }
    if(double_eq(OpVariable_get_double(OpContext_get_current_value(&c)), D1) != 1)
    {
    	ret = -1;
    	fprintf(stderr, "get first with DOUBLES should give DOUBLE %f and not %f\n", D1, OpVariable_get_double(OpContext_get_current_value(&c)));
    }

    Op_free(gf);
    OpContext_terminate(&c);
    OpProgram_terminate(&p);
    OpVariable_terminate(&v1);
    return ret;
}

int get_first_in_double(void)
{
    int ret = 0;
    OpVariable v1;
    OpProgram p;
    OpContext c;
    String err;
    OpVariable_init(&v1);
    OpVariable_set_double(&v1, D1);

    OpProgram_init(&p);
    OpContext_init(&c);
    OpProgram_set_context(&p, &c);
    Op *gv = OpGetValue_new(), *gf = OpGetFirst_new();

    OpGetValue_copy_variable((OpGetValue*)gv, &v1);

    Op1_set_operande((Op1*)gf, gv);

    Op_fix_operandes(gf, &c);

    OpContext_set_running_state(&c, gv, Run, "");

    if(Op_execute(gf,&c) != -1)
    {
    	ret = -1;
    	fprintf(stderr, "get first with DOUBLE should not work\n");
    }

    Op_free(gf);
    OpContext_terminate(&c);
    OpProgram_terminate(&p);
    OpVariable_terminate(&v1);
    return ret;
}

int main(int argc, char **argv)
{
	int ret;
	ret = get_first_in_double();
	if(ret == 0)ret = get_first_in_array_of_doubles_not_empty();
	if(ret == 0)ret = get_first_in_array_of_doubles_empty();
	if(ret == 0)ret = get_first_in_string();
	if(ret == 0)ret = get_first_in_empty_string();
	if(ret == 0)ret = get_first_in_array_of_strings();
	if(ret == 0)ret = get_first_in_array_of_strings_empty();
	if(ret == 0)ret = get_first_in_NONE();

    if(ret == 0)
        exit(EXIT_SUCCESS);
    else
        exit(EXIT_FAILURE);

}
