#include <Op.h>
#include <OpProgram.h>
#include <stdio.h>
#include <stdlib.h>
#include <String.h>
#include <string.h>

#define D1 42.0
#define D2 13.0
#define S1 "foo"



int get_double_in_string(void)
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
    Op *gv = OpGetValue_new();

    OpGetValue_copy_variable((OpGetValue*)gv, &v1);

    Op_fix_operandes(gv, &c);

    OpContext_set_running_state(&c, gv, Run, "");

    if(Op_execute_get_double(gv, gv, &c, &d) != -1)//Pas ultra propre sender, mais c'est juste pour le nom.
    {
    	ret = -1;
    	fprintf(stderr, "get double width STRING should give error\n");
    }

    Op_free(gv);
    OpContext_terminate(&c);
    OpProgram_terminate(&p);
    OpVariable_terminate(&v1);
    return ret;
}

int get_double_in_array_of_strings(void)
{
    int ret = 0;
    double d;
    OpVariable v1;
    OpProgram p;
    OpContext c;
    String err;
    OpVariable_init(&v1);
    OpVariable_append_string(&v1, S1);

    OpProgram_init(&p);
    OpContext_init(&c);
    OpProgram_set_context(&p, &c);
    Op *gv = OpGetValue_new();

    OpGetValue_copy_variable((OpGetValue*)gv, &v1);

    Op_fix_operandes(gv, &c);

    OpContext_set_running_state(&c, gv, Run, "");

    if(Op_execute_get_double(gv, gv, &c, &d) != -1)//Pas ultra propre sender, mais c'est juste pour le nom.
    {
    	ret = -1;
    	fprintf(stderr, "get double width STRINGS should give error\n");
    }

    Op_free(gv);
    OpContext_terminate(&c);
    OpProgram_terminate(&p);
    OpVariable_terminate(&v1);
    return ret;
}

int get_double_in_array_of_strings_empty(void)
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
    Op *gv = OpGetValue_new();

    OpGetValue_copy_variable((OpGetValue*)gv, &v1);

    Op_fix_operandes(gv, &c);

    OpContext_set_running_state(&c, gv, Run, "");

    if(Op_execute_get_double(gv, gv, &c, &d) != -1)//Pas ultra propre sender, mais c'est juste pour le nom.
    {
    	ret = -1;
    	fprintf(stderr, "get double width STRINGS empty should give error\n");
    }

    Op_free(gv);
    OpContext_terminate(&c);
    OpProgram_terminate(&p);
    OpVariable_terminate(&v1);
    return ret;
}

int get_double_in_array_empty(void)
{
    int ret = 0;
    double d;
    OpVariable v1;
    OpProgram p;
    OpContext c;
    String err;
    OpVariable_init(&v1);
    OpVariable_set_type(&v1, DOUBLES);

    OpProgram_init(&p);
    OpContext_init(&c);
    OpProgram_set_context(&p, &c);
    Op *gv = OpGetValue_new();

    OpGetValue_copy_variable((OpGetValue*)gv, &v1);

    Op_fix_operandes(gv, &c);

    OpContext_set_running_state(&c, gv, Run, "");

    if(Op_execute_get_double(gv, gv, &c, &d) != -1)//Pas ultra propre sender, mais c'est juste pour le nom.
    {
    	ret = -1;
    	fprintf(stderr, "get double width DOUBLES empty should give error\n");
    }

    Op_free(gv);
    OpContext_terminate(&c);
    OpProgram_terminate(&p);
    OpVariable_terminate(&v1);
    return ret;
}

int get_double_in_array_not_empty(void)
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
    Op *gv = OpGetValue_new();

    OpVariable_append_double(&v1, D1);
    OpVariable_append_double(&v1, D2);

    OpGetValue_copy_variable((OpGetValue*)gv, &v1);

    Op_fix_operandes(gv, &c);

    OpContext_set_running_state(&c, gv, Run, "");

    if(Op_execute_get_double(gv, gv, &c, &d) != 0)//Pas ultra propre sender, mais c'est juste pour le nom.
    {
    	ret = -1;
    	fprintf(stderr, "get double width DOUBLES not empty should work\n");
    }
    if(double_eq(d, D1) == 0)
    {
    	ret = -1;
    	fprintf(stderr, "get double width DOUBLE should give correct result\n");
    }

    Op_free(gv);
    OpContext_terminate(&c);
    OpProgram_terminate(&p);
    OpVariable_terminate(&v1);
    return ret;
}

int get_double_in_double(void)
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
    Op *gv = OpGetValue_new();

    OpVariable_set_double(&v1, D1);

    OpGetValue_copy_variable((OpGetValue*)gv, &v1);

    Op_fix_operandes(gv, &c);

    OpContext_set_running_state(&c, gv, Run, "");

    if(Op_execute_get_double(gv, gv, &c, &d) != 0)//Pas ultra propre sender, mais c'est juste pour le nom.
    {
    	ret = -1;
    	fprintf(stderr, "get double width DOUBLE should work\n");
    }
    if(double_eq(d, D1) == 0)
    {
    	ret = -1;
    	fprintf(stderr, "get double width DOUBLE should give correct result\n");
    }

    Op_free(gv);
    OpContext_terminate(&c);
    OpProgram_terminate(&p);
    OpVariable_terminate(&v1);
    return ret;
}

int main(int argc, char **argv)
{
	int ret;
	ret = get_double_in_double();
	if(ret == 0)ret = get_double_in_array_not_empty();
	if(ret == 0)ret = get_double_in_array_empty();
	if(ret == 0)ret = get_double_in_string();
	if(ret == 0)ret = get_double_in_array_of_strings();
	if(ret == 0)ret = get_double_in_array_of_strings_empty();

    if(ret == 0)
        exit(EXIT_SUCCESS);
    else
        exit(EXIT_FAILURE);

}
