#include "czl_paser.h"
#include "czl_opt.h"
#include "czl_lib.h"

#ifdef CZL_LIB_TCP
#include "czl_tcp.h" //TCP库
#endif //CZL_LIB_TCP

///////////////////////////////////////////////////////////////
enum czl_keyword_index_enum
{
    CZL_IF_INDEX = 1, //必须从1开始
    CZL_ELIF_INDEX,
    CZL_ELSE_INDEX,
    CZL_FOR_INDEX,
    CZL_WHILE_INDEX,
    CZL_DO_INDEX,
    CZL_SWITCH_INDEX,
    CZL_CASE_INDEX,
    CZL_DEFAULT_INDEX,
    CZL_TIMER_INDEX,
    CZL_TASK_INDEX,
    CZL_BREAK_INDEX,
    CZL_CONTINUE_INDEX,
    CZL_GOTO_INDEX,
    CZL_RETURN_INDEX,
    CZL_YEILD_INDEX,
    CZL_TRY_INDEX,
    CZL_LOAD_INDEX,
    CZL_AS_INDEX,
    CZL_LIB_INDEX,
    CZL_FUN_INDEX,
    CZL_VAR_INDEX,
    CZL_INT_INDEX,
    CZL_FLOAT_INDEX,
    CZL_NUM_INDEX,
    CZL_STR_INDEX,
    CZL_FILE_INDEX,
    CZL_SOURCE_INDEX,
    CZL_ARR_INDEX,
    CZL_TABLE_INDEX,
    CZL_STACK_INDEX,
    CZL_QUEUE_INDEX,
    CZL_STATIC_INDEX,
    CZL_CONST_INDEX,
    CZL_CLASS_INDEX,
    CZL_FINAL_INDEX,
    CZL_MY_INDEX,
    CZL_FUNC_INDEX,
    CZL_PUBLIC_INDEX,
    CZL_PROTECTED_INDEX,
    CZL_PRIVATE_INDEX,
    CZL_NEW_INDEX,
    CZL_NIL_INDEX,
    CZL_NULL_INDEX,
    CZL_TRUE_INDEX,
    CZL_FALSE_INDEX,
    CZL_LBRACKET_INDEX,
    CZL_RBRACKET_INDEX,
    //以下仅起到标记作用
    CZL_INS_INDEX,
    CZL_GOTO_FLAG,
};

//系统关键字列表
const czl_keyword czl_keyword_table[] =
{
    {"if",          CZL_IF_INDEX},
    {"elif",        CZL_ELIF_INDEX},
    {"else",        CZL_ELSE_INDEX},
    {"for",         CZL_FOR_INDEX},
    {"while",       CZL_WHILE_INDEX},
    {"do",          CZL_DO_INDEX},
    {"switch",      CZL_SWITCH_INDEX},
    {"case",        CZL_CASE_INDEX},
    {"default",     CZL_DEFAULT_INDEX},
    {"timer",       CZL_TIMER_INDEX},
    {"task",        CZL_TASK_INDEX},
    {"break",       CZL_BREAK_INDEX},
    {"continue",    CZL_CONTINUE_INDEX},
    {"goto",        CZL_GOTO_INDEX},
    {"return",      CZL_RETURN_INDEX},
    {"yeild",       CZL_YEILD_INDEX},
    {"try",         CZL_TRY_INDEX},
    {"load",        CZL_LOAD_INDEX},
    {"as",          CZL_AS_INDEX},
    {"lib",         CZL_LIB_INDEX},
    {"fun",         CZL_FUN_INDEX},
    {"var",         CZL_VAR_INDEX},
    {"int",         CZL_INT_INDEX},
    {"float",       CZL_FLOAT_INDEX},
    {"num",         CZL_NUM_INDEX},
    {"str",         CZL_STR_INDEX},
    {"file",        CZL_FILE_INDEX},
    {"src",         CZL_SOURCE_INDEX},
    {"arr",         CZL_ARR_INDEX},
    {"map",         CZL_TABLE_INDEX},
    {"stack",       CZL_STACK_INDEX},
    {"queue",       CZL_QUEUE_INDEX},
    {"static",      CZL_STATIC_INDEX},
    {"const",       CZL_CONST_INDEX},
    {"class",       CZL_CLASS_INDEX},
    {"final",       CZL_FINAL_INDEX},
    {"my",          CZL_MY_INDEX},
    {"func",        CZL_FUNC_INDEX},
    {"public",      CZL_PUBLIC_INDEX},
    {"protected",   CZL_PROTECTED_INDEX},
    {"private",     CZL_PRIVATE_INDEX},
    {"new",         CZL_NEW_INDEX},
    {"nil",         CZL_NIL_INDEX},
    {"null",        CZL_NULL_INDEX},
    {"true",        CZL_TRUE_INDEX},
    {"false",       CZL_FALSE_INDEX},
    {"{",           CZL_LBRACKET_INDEX},
    {"}",           CZL_RBRACKET_INDEX},
};
const unsigned long czl_keyword_table_num =
        sizeof(czl_keyword_table) / sizeof(czl_keyword);
///////////////////////////////////////////////////////////////
//是否是转义关键字
#define CZL_IS_ESCAPE_KW(index) \
(CZL_INT_INDEX == index || CZL_FLOAT_INDEX == index || \
 CZL_NUM_INDEX == index || CZL_STR_INDEX == index || CZL_TIMER_INDEX == index)

//是否是系统常量关键字
#define CZL_IS_CONST_KW(index) \
(CZL_NIL_INDEX == index || CZL_NULL_INDEX == index || \
 CZL_TRUE_INDEX == index || CZL_FALSE_INDEX == index)
///////////////////////////////////////////////////////////////
czl_var czl_nil =   {NULL, CZL_NIL, CZL_CONST_VAR, 0, CZL_NIL, {0}};
czl_var czl_null =  {NULL, CZL_OBJ_REF, CZL_REF_ELE, 0, CZL_NULL, {0}};
//
czl_var czl_true =  {NULL, CZL_INT, CZL_CONST_VAR, 0, CZL_INT, {1}};
czl_var czl_false = {NULL, CZL_INT, CZL_CONST_VAR, 0, CZL_INT, {0}};
//
czl_var czl_type_int =      {NULL, CZL_INT, CZL_CONST_VAR, 0, CZL_INT, {0}};
czl_var czl_type_float =    {NULL, CZL_INT, CZL_CONST_VAR, 0, CZL_INT, {1}};
czl_var czl_type_string =   {NULL, CZL_INT, CZL_CONST_VAR, 0, CZL_INT, {2}};
czl_var czl_type_array =    {NULL, CZL_INT, CZL_CONST_VAR, 0, CZL_INT, {3}};
czl_var czl_type_table =    {NULL, CZL_INT, CZL_CONST_VAR, 0, CZL_INT, {4}};
czl_var czl_type_ins =      {NULL, CZL_INT, CZL_CONST_VAR, 0, CZL_INT, {5}};
czl_var czl_type_stack =    {NULL, CZL_INT, CZL_CONST_VAR, 0, CZL_INT, {6}};
czl_var czl_type_queue =    {NULL, CZL_INT, CZL_CONST_VAR, 0, CZL_INT, {7}};
czl_var czl_type_file =     {NULL, CZL_INT, CZL_CONST_VAR, 0, CZL_INT, {8}};
czl_var czl_type_source =   {NULL, CZL_INT, CZL_CONST_VAR, 0, CZL_INT, {9}};
czl_var czl_type_fun =      {NULL, CZL_INT, CZL_CONST_VAR, 0, CZL_INT, {10}};
///////////////////////////////////////////////////////////////
char* czl_exp_analysis(czl_gp*, char*, czl_exp_handle*);
char* czl_expression_analysis(czl_gp*, char*);
char czl_shell_analysis(czl_gp*, char*);
char czl_condition_block_check(czl_gp*, int, char**, char*);
char* czl_class_def(czl_gp*, char*, char, char, char, czl_class*);
char* czl_fun_def(czl_gp*, char*, char);
///////////////////////////////////////////////////////////////
char* czl_annotate_check(czl_gp *gp, char *code)
{
    if ('/' == *code++)
    {
        while (*code != '\0')
        {
            if ('\n' == *code)
                return code;
            ++code;
        }
    }
    else
    {
        char flag = 1;
        while (*code != '\0')
        {
            if ('\n' == *code)
                gp->error_line++;
            else if ('\"' == *code && *(code-1) != '\\')
                flag = !flag;
            else if (flag && '/' == *code && '*' == *(code+1))
            {
                code = czl_annotate_check(gp, code+1);
                continue;
            }
            ++code;
            if (flag && '*' == *code && '/' == *(code+1))
                return code+2;
        }
    }

    return code;
}

//忽略不可见字符(czl_ignore_sign)和注释
char* czl_ignore_sign_filt(czl_gp *gp, char *code)
{
    while (*code != '\0')
    {
        if ('\n' == *code)
            gp->error_line++;
        if (' ' == *code || '\t' == *code || '\n' == *code || '\r' == *code)
            ++code;
        else if ('/' == *code && ('/' == *(code+1) || '*' == *(code+1)))
            code = czl_annotate_check(gp, code+1);
        else
            break;
    }

    return code;
}

int czl_is_keyword(czl_gp *gp, char *name)
{
    czl_keyword *key = (czl_keyword*)czl_sys_hash_find(CZL_STRING, CZL_NIL,
                                                       name, &gp->keyword_hash);
    if (!key)
        return 0;

    return key->index;
}

char* czl_name_match(czl_gp *gp, char *code, char *name)
{
    int len = 0;
    char *value;

    code = czl_ignore_sign_filt(gp, code);

    value = code;

    if (!((*code >= 'a' && *code <= 'z') ||
          (*code >= 'A' && *code <= 'Z') ||
          ('_' == *code)))
        return NULL;

    while (*code != '\0' && len < CZL_NAME_MAX_SIZE)
    {
        len++;
        code++;
        if (!((*code >= '0' && *code <= '9') ||
              (*code >= 'a' && *code <= 'z') ||
              (*code >= 'A' && *code <= 'Z') ||
              ('_' == *code)))
            break;
    }

    if (CZL_NAME_MAX_SIZE == len)
        return NULL;

    strncpy(name, value, len);
    name[len] = '\0';

    return code;
}

czl_goto_flag* czl_goto_node_find(const char *name, czl_goto_flag *p)
{
    while (p)
    {
        if (strcmp(name, p->name) == 0)
            return p;
        p = p->next;
    }

    return NULL;
}

char* czl_goto_flag_insert(czl_gp *gp, char *code)
{
    char name[CZL_NAME_MAX_SIZE];
	czl_goto_flag *node;

    if (!(code=czl_name_match(gp, code, name)))
        return NULL;

    if (czl_goto_node_find(name, gp->cur_fun->goto_flags))
    {
        sprintf(gp->log_buf, "redeclared goto_flag %s, ", name);
        return NULL;
    }
    if (!(node=czl_goto_node_create(gp, name)))
        return NULL;
    czl_goto_node_insert(&gp->cur_fun->goto_flags, node);

    return czl_ignore_sign_filt(gp, code) + 1;
}

//系统关键字查找
char* czl_keyword_find(czl_gp *gp, char *code, int *index, char flag)
{
    char name[CZL_NAME_MAX_SIZE];
    unsigned long error_line_backup;
    char *tmp = code = czl_ignore_sign_filt(gp, code);

    if (flag)
    {
        if ('{' == *code)
        {
            *index = CZL_LBRACKET_INDEX;
            return code+1;
        }
        else if ('}' == *code)
        {
            *index = CZL_RBRACKET_INDEX;
            return code+1;
        }
    }

    if (!(code=czl_name_match(gp, code, name)))
    {
        *index = 0;
        return tmp;
    }

    if ((*index=czl_is_keyword(gp, name)))
    {
        if (1 == flag)
        {
            if (CZL_IS_ESCAPE_KW(*index))
            {
                error_line_backup = gp->error_line;
                code = czl_ignore_sign_filt(gp, code);
                if ('(' == *code)
                {
                    gp->error_line = error_line_backup;
                    *index = 0;
                    return tmp;
                }
            }
            else if (CZL_IS_CONST_KW(*index))
            {
                *index = 0;
                return tmp;
            }
        }
        return code;
    }

    error_line_backup = gp->error_line;
    code = czl_ignore_sign_filt(gp, code);
    if ('.' == *code)
    {
        if (!(code=czl_name_match(gp, code+1, name)))
        {
            gp->error_line = error_line_backup;
            return tmp;
        }
        code = czl_ignore_sign_filt(gp, code);
        flag = 0;
    }

    if ((*code >= 'a' && *code <= 'z') || (*code >= 'A' && *code <= 'Z') || ('_' == *code))
        *index = CZL_INS_INDEX;
    else if (1 == flag && ':' == *code && *(code+1) != ':')
        *index = CZL_GOTO_FLAG;

    gp->error_line = error_line_backup;
    return tmp;
}

char czl_loop_find(czl_gp *gp)
{
    int i;

    for (i = gp->tmp->block_count-1; i >= 0; i--)
        if (CZL_FOR_INDEX == gp->tmp->block_stack[i].index ||
            CZL_WHILE_INDEX == gp->tmp->block_stack[i].index ||
            CZL_DO_INDEX == gp->tmp->block_stack[i].index ||
            CZL_TIMER_INDEX == gp->tmp->block_stack[i].index)
            return 1;

    return 0;
}

char czl_switch_pop(czl_gp *gp)
{
    int i;

    for (i = gp->tmp->block_count-1; i >= 0; i--)
        if (CZL_LBRACKET_INDEX == gp->tmp->block_stack[i].index)
        {
            if (i > 0 && CZL_SWITCH_INDEX == gp->tmp->block_stack[i-1].index)
            {
                gp->tmp->block_count = i-1;
                return 1;
            }
            return 0;
        }

    return 0;
}

char czl_switch_case_find(czl_gp *gp)
{
    int i;

    for (i = gp->tmp->block_count-1; i >= 0; i--)
    {
        switch (gp->tmp->block_stack[i].index)
        {
        case CZL_SWITCH_INDEX: case CZL_CASE_INDEX:
            return 1;
        case CZL_LBRACKET_INDEX:
            break;
        default:
            return 0;
        }
    }

    return 0;
}

char czl_switch_find(czl_gp *gp)
{
    int i;

    for (i = gp->tmp->block_count-1; i >= 0; i--)
        if (CZL_SWITCH_INDEX == gp->tmp->block_stack[i].index)
            return 1;

    return 0;
}

char czl_timer_find(czl_gp *gp)
{
    int i;

    for (i = gp->tmp->code_blocks_count-1; i >= 0; i--)
        if (CZL_LOOP_BLOCK == gp->tmp->code_blocks[i].type &&
            CZL_TIMER_LOOP == gp->tmp->code_blocks[i].block.loop->type)
        {
            ++gp->tmp->code_blocks[i].block.loop->task_cnt;
            return 1;
        }

    return 0;
}

char czl_block_context_check(czl_gp *gp, int index)
{
    int last_index = (0 == gp->tmp->block_count ? -1 :
                      gp->tmp->block_stack[gp->tmp->block_count-1].index);

    switch (index)
    {
    case CZL_ELIF_INDEX: case CZL_ELSE_INDEX:
        if (last_index < 0 || (last_index != CZL_IF_INDEX &&
                               last_index != CZL_ELIF_INDEX))
        {
            sprintf(gp->log_buf, "missed if for elif/else, ");
            return 0;
        }
        break;
    case CZL_CASE_INDEX: case CZL_DEFAULT_INDEX:
        if (!czl_switch_case_find(gp))
        {
            sprintf(gp->log_buf, "missed switch for case/default, ");
            return 0;
        }
        break;
    case CZL_TASK_INDEX:
        if (!czl_timer_find(gp))
        {
            sprintf(gp->log_buf, "missed timer for task, ");
            return 0;
        }
        break;
    case CZL_BREAK_INDEX:
        if (!czl_loop_find(gp) && !czl_switch_find(gp))
        {
            sprintf(gp->log_buf, "missed loop/switch for break, ");
            return 0;
        }
        break;
    case CZL_CONTINUE_INDEX:
        if (!czl_loop_find(gp))
        {
            sprintf(gp->log_buf, "missed loop for continue, ");
            return 0;
        }
        break;
    case CZL_RBRACKET_INDEX:
        if (last_index == CZL_LBRACKET_INDEX)
        {
            gp->tmp->block_count--;
            //检查是否是空switch语句
            if (gp->tmp->block_count > 0 &&
                CZL_SWITCH_INDEX == gp->tmp->block_stack[gp->tmp->block_count-1].index)
                gp->tmp->block_count--;
        }
        else
        {
            //检查switch语句是否正确结束
            if (!czl_switch_pop(gp))
            {
                sprintf(gp->log_buf, "missed '{' for '}', ");
                return 0;
            }
            //退出switch/case/default语句块
            gp->tmp->code_blocks_count--;
        }
        break;
    default:
        break;
    }

    return 1;
}

char czl_code_blocks_count(czl_gp *gp, int index)
{
    if (gp->tmp->block_count >= CZL_MAX_CODE_NEST_LAYER)
    {
        sprintf(gp->log_buf, "nest layer of block should not be much %d, ", CZL_MAX_CODE_NEST_LAYER);
        return 0;
    }

    gp->tmp->block_stack[gp->tmp->block_count].index = index;

    if (CZL_LBRACKET_INDEX == index)
        gp->tmp->block_stack[gp->tmp->block_count].flag = CZL_SURE;
    else
        gp->tmp->block_stack[gp->tmp->block_count].flag = CZL_NOT_SURE;

    gp->tmp->block_count++;

    return 1;
}

void czl_if_elif_else_pop(czl_gp *gp)
{
    while (gp->tmp->block_stack[--gp->tmp->block_count].index != CZL_IF_INDEX)
        ;
}

char* czl_end_sign_check(czl_gp *gp, char *code)
{
    code = czl_ignore_sign_filt(gp, code);

    if (';' == *code)
        return code+1;

    if ((*code >= 'a' && *code <= 'z') ||
        (*code >= 'A' && *code <= 'Z') || (*code >= '0' && *code <= '9'))
        return code;

    if ('.' == *code && (*(code+1) >= '0' && *(code+1) <= '9'))
        return code;

    switch (*code)
    {
    case '_': case '(': case '{': case '}': case '\"': case '\'':
    case '!': case '~': case '+': case '-': case '#': case '$': case '\0':
        return code;
    default:
        return NULL;
    }
}

char* czl_number_match(czl_gp *gp, char *code, czl_exp_node **node)
{
    //number
    czl_var res;

    if (!(code=czl_get_number_from_str(code, &res)))
    {
        sprintf(gp->log_buf, "number structure error, ");
        return NULL;
    }

    *node = czl_opr_node_create(gp, res.type, &res.val, gp->error_line);

    return code;
}

char czl_is_esc_sign(const char ch)
{
    switch (ch)
    {
    case '\'' :
        return '\'';
    case '\"' :
        return '\"';
    case '\\' :
        return '\\';
    case '0' :
        return '\0';
    case 'a' :
        return '\a';
    case 'b' :
        return '\b';
    case 'f' :
        return '\f';
    case 'n' :
        return '\n';
    case 'r' :
        return '\r';
    case 't' :
        return '\t';
    case 'v' :
        return '\v';
    default :
        return -1;
    }
}

char* czl_char_match(czl_gp *gp, char *code, czl_exp_node **node)
{
    //'char'
    char ch;
    czl_value val;

    code++;

    if ('\\' == *code)
    {
        ch = czl_is_esc_sign(*(code+1));
        if (ch < 0)
        {
            sprintf(gp->log_buf, "escape character should be within [\' \" \\ 0 a b f n r t v], ");
            return code;
        }
        code++;
    }
    else
    {
        ch = *code;
        if ('\'' == ch)
        {
            sprintf(gp->log_buf, "character should not be empty, ");
            return code;
        }
    }

    if (*(code+1) != '\'')
    {
        sprintf(gp->log_buf, "missed '\'' for end defined a character, ");
        return code;
    }

    val.inum = ch;

    *node = czl_opr_node_create(gp, CZL_INT, &val, gp->error_line);

    return code+2;
}

int czl_get_str_len(czl_gp *gp, char *code)
{
    int len = 0;

    while (*code != '\n' && *code != '\"')
    {
        if ('\\' == *code)
        {
            if (czl_is_esc_sign(*(code+1)) < 0)
                return -1;
            len++;
            code += 2;
        }
        else
        {
            len++;
            code++;
        }
    }

    if ('\n' == *code)
    {
        sprintf(gp->log_buf, "missed '\"' for end defined string, ");
        return -1;
    }

    return len;
}

char* czl_get_str(char *code, char *val)
{
    while (*code != '\"')
    {
        if ('\\' == *code)
        {
            *val++ = czl_is_esc_sign(*(code+1));
            code += 2;
        }
        else
            *val++ = *code++;
    }

    return code+1;
}

char* czl_string_filt(czl_gp *gp, char *code, char **val)
{
    long len = czl_get_str_len(gp, ++code);
    if (len < 0)
        return NULL;

    if (!(*val=(char*)CZL_TMP_MALLOC(gp, len+1)))
        return NULL;

    (*val)[len] = '\0';

    return czl_get_str(code, *val);
}

char* czl_str_match(czl_gp *gp, char *code, czl_var **ret)
{
    //"string"
	czl_value val;
	czl_var *node;

    long size = czl_get_str_len(gp, ++code);
    if (size < 0)
        return NULL;

    if (!czl_str_create(gp, &val.str, size+1, size, NULL))
        return NULL;
    code = czl_get_str(code, CZL_STR(val.str.Obj)->str);
    if (!(node=czl_const_create(gp, CZL_STRING, &val)))
        return NULL;
    *ret = node;

    return code;
}

char* czl_string_match(czl_gp *gp, char *code, czl_exp_node **node)
{
    //"string"
	czl_value val;
    long size = czl_get_str_len(gp, ++code);
    if (size < 0)
        return NULL;

    if (!czl_str_create(gp, &val.str, size+1, size, NULL))
        return NULL;
    code = czl_get_str(code, CZL_STR(val.str.Obj)->str);

    if (!(*node=czl_opr_node_create(gp, CZL_STRING, &val, gp->error_line)))
        return NULL;

    return code;
}

int czl_get_numstr_len(czl_gp *gp, char *code)
{
    int len = 0;
    int cnt;

    code = czl_numstr_ignore_sign_filt(code);

    do {
        if (*(code++) != '0')
            goto CZL_SYNTAX_ERROR;
        cnt = 0;
        switch (*(code++))
        {
        case 'x': case 'X':
            if ('\'' == *code)
                goto CZL_SYNTAX_ERROR;
            while (((*code >= '0' && *code <= '9') ||
                    (*code >= 'a' && *code <= 'f') ||
                    (*code >= 'A' && *code <= 'F')) &&
                   ((*(code+1) >= '0' && *(code+1) <= '9') ||
                    (*(code+1) >= 'a' && *(code+1) <= 'f') ||
                    (*(code+1) >= 'A' && *(code+1) <= 'F')))
            {
                cnt++;
                code += 2;
            }
            if (0 == cnt)
                goto CZL_SYNTAX_ERROR;
            len += cnt;
            break;
        case 'b': case 'B':
            if ('\'' == *code)
                goto CZL_SYNTAX_ERROR;
            while ('0' == *code || '1' == *code)
            {
                cnt++;
                code++;
            }
            if (cnt%8 != 0)
                goto CZL_SYNTAX_ERROR;
            len += cnt/8;
            break;
        default:
            goto CZL_SYNTAX_ERROR;
        }

        code = czl_numstr_ignore_sign_filt(code);
    } while (*code != '\'');

    if ('\n' == *code)
    {
        sprintf(gp->log_buf, "missed '\'' for end defined numstr, ");
        return -1;
    }

    return len;

CZL_SYNTAX_ERROR:
    sprintf(gp->log_buf, "numstr error, ");
    return -1;
}

char* czl_get_numstr(char *code, char *val)
{
    int cnt = 0;
    unsigned char h, l;
    const unsigned char bTable[] = {128, 64, 32, 16, 8, 4, 2, 1};

    code = czl_numstr_ignore_sign_filt(code);

    do {
        code++;
        switch (*(code++))
        {
        case 'x': case 'X':
            while (!(' ' == *code || '\t' == *code ||
                     '\n' == *code || '\r' == *code || '\'' == *code))
            {
                if (*code >= '0' && *code <= '9')
                    h = *code - '0';
                else if (*code >= 'a' && *code <= 'z')
                    h = *code - 'a' + 10;
                else
                    h = *code - 'A' + 10;
                code++;
                if (*code >= '0' && *code <= '9')
                    l = *code - '0';
                else if (*code >= 'a' && *code <= 'z')
                    l = *code - 'a' + 10;
                else
                    l = *code - 'A' + 10;
                code++;
                *(val++) = h*16 + l;
            }
            break;
        case 'b': case 'B':
            cnt = 0;
            while ('0' == *code || '1' == *code)
            {
                *val += ('0' == *code ? 0 : bTable[cnt]);
                if (++cnt == 8)
                {
                    val++;
                    cnt = 0;
                }
                code++;
            }
            break;
        default:
            break;
        }

        code = czl_numstr_ignore_sign_filt(code);
    } while (*code != '\'');

    return code+1;
}

char* czl_numstr_match(czl_gp *gp, char *code, czl_exp_node **node)
{
    //'0x0a 0b11110000'
    czl_value val;
    long size = czl_get_numstr_len(gp, ++code);
    if (size < 0)
        return NULL;

    if (!czl_str_create(gp, &val.str, size+1, size, NULL))
        return NULL;
    code = czl_get_numstr(code, CZL_STR(val.str.Obj)->str);

    if (!(*node=czl_opr_node_create(gp, CZL_STRING, &val, gp->error_line)))
        return NULL;

    return code;
}

char czl_is_constant(czl_gp *gp, char **code, czl_exp_node **node)
{
    if ('\'' == **code)
    {
        if ('\'' == *(*code+2) || ('\\' == *(*code+1) && '\'' == *(*code+3)))
            *code = czl_char_match(gp, *code, node);
        else
            *code = czl_numstr_match(gp, *code, node);
    }
    else if ('\"' == **code)
        *code = czl_string_match(gp, *code, node);
    else if ((**code >= '0' && **code <= '9') ||
             ('.' == **code && (*(*code+1) >= '0' && *(*code+1) <= '9')))
        *code = czl_number_match(gp, *code, node);
    else
        return 0;

    return 1;
}

char* czl_tabkv_list_match
(
    czl_gp *gp,
    char *code,
    czl_table_list **list
)
{
    czl_exp_ele *key, *val;
    czl_table_node *para;
    czl_table_node *paras_tail;

    if (!((*list)=czl_table_list_create(gp)))
        return NULL;

    code = czl_ignore_sign_filt(gp, code);
    if ('}' == *code)
        return code+1;

    for (;;)
    {
        ++(*list)->paras_count;
        key = val = NULL;
        //key match
        if (!(code=czl_expression_analysis(gp, code)))
            goto CZL_SYNTAX_ERROR;
        if (!gp->tmp->exp_head)
        {
            sprintf(gp->log_buf, "para should not be empty, ");
            goto CZL_SYNTAX_ERROR;
        }
        key = gp->tmp->exp_head;
        gp->tmp->exp_head = NULL;
        // ':' check
        code = czl_ignore_sign_filt(gp, code);
        if (*code != ':')
        {
            sprintf(gp->log_buf, "missed ':' for key-value of table, ");
            goto CZL_SYNTAX_ERROR;
        }
        //val match
        if (!(code=czl_expression_analysis(gp, code+1)))
            goto CZL_SYNTAX_ERROR;
        if (!gp->tmp->exp_head)
        {
            sprintf(gp->log_buf, "para should not be empty, ");
            goto CZL_SYNTAX_ERROR;
        }
        val = gp->tmp->exp_head;
        gp->tmp->exp_head = NULL;
        //insert key_val
        if (!(para=czl_table_node_create(gp, key, val)))
            goto CZL_SYNTAX_ERROR;
        czl_table_node_insert(&(*list)->paras, &paras_tail, para);
        if (',' == *code)
        {
            code = czl_ignore_sign_filt(gp, code+1);
            if ('}' == *code)
            {
                code++;
                break;
            }
        }
        else if ('}' == *code)
        {
            code++;
            break;
        }
        else
        {
            key = val = NULL;
            sprintf(gp->log_buf, "missed '}' for end a table, ");
            goto CZL_SYNTAX_ERROR;
        }
    }

    return code;

CZL_SYNTAX_ERROR:
    czl_exp_stack_delete(gp, key);
    czl_exp_stack_delete(gp, val);
    czl_table_list_delete(gp, *list);
    return NULL;
}

char* czl_paras_match
(
    czl_gp *gp,
    char *code,
    czl_para_list *paras_head,
    unsigned long *paras_count,
    char exp_null_flag,
    char alloc_flag,
    char end_sign
)
{
    czl_para *para;
    czl_para_list paras_tail;
    unsigned long cnt = 0;

    *paras_head = NULL;

    //过滤参数列表
    for (;;)
    {
        cnt++;
        if (!(code=czl_expression_analysis(gp, code)))
            return NULL;
        if (gp->tmp->exp_head || 1 == exp_null_flag)
        {
            if (!(para=czl_para_node_create(gp, alloc_flag)))
                return NULL;
            czl_para_node_insert(paras_head, &paras_tail, para);
        }
        else if (0 == exp_null_flag)
        {
            sprintf(gp->log_buf, "para should not be empty, ");
            return NULL;
        }
        if (',' == *code)
        {
            ++code;
            if (']' == end_sign)
            {
                code = czl_ignore_sign_filt(gp, code);
                if (']' == *code)
                {
                    ++code;
                    break;
                }
            }
        }
        else if (end_sign == *code++)
            break;
        else
        {
            sprintf(gp->log_buf, "missed '%c' for end a paras-list, ", end_sign);
            return NULL;
        }
    }

    if (paras_count)
        *paras_count = cnt;

    return code;
}

char* czl_exp_fun_build
(
    czl_gp *gp,
    char *code,
    czl_exp_fun **ret_fun,
    czl_fun *fun,
    char fun_type
)
{
    czl_exp_fun *exp_fun = czl_exp_fun_create(gp, fun, fun_type);
    if (!exp_fun)
        return NULL;

    code = czl_ignore_sign_filt(gp, code);
    if (')' == *code)
        code++;
    else
    {
        unsigned long paras_count;
        if (!(code=czl_paras_match(gp, code, &exp_fun->paras, &paras_count, 1, 1, ')')))
            return NULL;
        exp_fun->paras_count = paras_count;
    }

    if (CZL_STATIC_FUN == exp_fun->quality &&
        fun->state != CZL_IN_STATEMENT &&
        !czl_fun_paras_check(gp, exp_fun, fun))
    {
        return NULL;
    }

    *ret_fun = exp_fun;
    return code;
}

czl_fun* czl_undef_static_fun_create(czl_gp *gp, char *name)
{
    char type = (CZL_IN_GLOBAL == gp->tmp->analysis_field ||
                 CZL_IN_GLOBAL_FUN == gp->tmp->analysis_field ?
                 CZL_USR_GLOBAL_FUN : CZL_USR_CLASS_FUN);

    czl_fun *fun = czl_fun_node_create(gp, name, CZL_IN_STATEMENT, type, CZL_NIL, NULL);
    if (!fun || !czl_fun_insert(gp, fun))
        return NULL;

    return fun;
}

char* czl_syslib_fun_match
(
    czl_gp *gp,
    char *code,
    char *libName,
    czl_exp_node **node
)
{
    char funName[CZL_NAME_MAX_SIZE];
    czl_fun *fun;
    czl_sysfun *sys_fun;
    czl_exp_fun *exp_fun;
    czl_sys_hash *hash = czl_sys_fun_hash_create(gp, libName);
    unsigned long line;

    if (!hash)
        return NULL;
    if (!(code=czl_name_match(gp, code, funName)))
    {
        sprintf(gp->log_buf, "missed function after library %s, ", libName);
        return NULL;
    }
    line = gp->error_line;
    if (!(sys_fun=czl_sys_fun_find(funName, hash)))
    {
        sprintf(gp->log_buf, "undefined function %s in library %s, ", funName, libName);
        return NULL;
    }
    if (!(fun=czl_sys_fun_create(gp, sys_fun)))
        return NULL;
    code = czl_ignore_sign_filt(gp, code);
    if (*code != '(')
    {
        sprintf(gp->log_buf, "missed '(' for calling function %s, ", funName);
        return NULL;
    }
    if (!(code=czl_exp_fun_build(gp, code+1, &exp_fun, fun, CZL_STATIC_FUN)) ||
        !(*node=czl_opr_node_create(gp, CZL_FUNCTION, exp_fun, line)))
        return NULL;

    return code;
}

char* czl_lib_obj_match
(
    czl_gp *gp,
    char *code,
    char *libName,
    czl_exp_node **node
)
{
    char name[CZL_NAME_MAX_SIZE];
    unsigned long line;
    czl_usrlib *lib = (czl_usrlib*)czl_sys_hash_find(CZL_STRING, CZL_NIL,
                                                     libName, &gp->tmp->usrlibs_hash);

    if (!lib)
        return czl_syslib_fun_match(gp, code, libName, node);
    if (!(code=czl_name_match(gp, code, name)))
    {
        sprintf(gp->log_buf, "missed object after library %s, ", libName);
        return NULL;
    }
    line = gp->error_line;
    code = czl_ignore_sign_filt(gp, code);
    if ('(' == *code)
    {
        char fun_type = CZL_STATIC_FUN;
        czl_exp_fun *exp_fun;
        czl_fun *fun = (czl_fun*)czl_sys_hash_find(CZL_STRING, CZL_NIL,
                                                   name, &lib->funs_hash);
        if (!fun)
        {
            if (!(fun=(czl_fun*)czl_sys_hash_find(CZL_STRING, CZL_NIL,
                                                  name, &lib->vars_hash)))
            {
                sprintf(gp->log_buf, "undefined function %s in library %s, ", name, libName);
                return NULL;
            }
            fun_type = CZL_LG_VAR_DYNAMIC_FUN;
        }
        if (!(code=czl_exp_fun_build(gp, code+1, &exp_fun, fun, fun_type)) ||
            !(*node=czl_opr_node_create(gp, CZL_FUNCTION, exp_fun, line)))
            return NULL;
    }
    else
    {
        czl_var *var = (czl_var*)czl_sys_hash_find(CZL_STRING, CZL_NIL, name, &lib->vars_hash);
        if (!var)
        {
            if (CZL_IN_CONSTANT == gp->tmp->variable_field)
                sprintf(gp->log_buf, "undefined constant %s in library %s, ", name, libName);
            else
                sprintf(gp->log_buf, "undefined variable %s in library %s, ", name, libName);
            return NULL;
        }
        if (CZL_IN_CONSTANT == gp->tmp->variable_field && var->quality != CZL_CONST_VAR)
        {
            sprintf(gp->log_buf, "undefined constant %s in library %s, ", name, libName);
            return NULL;
        }
        if (!(*node=czl_opr_node_create(gp, (CZL_CONST_VAR == var->quality ?
                                         CZL_ENUM : CZL_LG_VAR), var, line)))
            return NULL;
    }

    return code;
}

char* czl_fun_match
(
    czl_gp *gp,
    char *code,
    char *name,
    czl_exp_node **node,
    char my_flag,
    unsigned long line
)
{
	char fun_type = CZL_STATIC_FUN;
    czl_fun *fun;
	czl_sysfun *sys_fun;
	czl_exp_fun *exp_fun;

    //静态用户函数查询
    if (!(fun=czl_fun_find_in_exp(gp, name, my_flag)))
    {
        //静态系统函数查询
        if (!gp->tmp->osfun_hash &&
            !(gp->tmp->osfun_hash=czl_sys_fun_hash_create(gp, CZL_LIB_OS_NAME)))
            return NULL;
        if ((sys_fun=czl_sys_fun_find(name, gp->tmp->osfun_hash)))
        {
            if (!(fun=czl_sys_fun_create(gp, sys_fun)))
                return NULL;
        }
        else
        {
            fun = (czl_fun*)czl_var_find_in_exp(gp, name, my_flag);
            if (fun) //变量调用方式动态函数查询
            {
                if (gp->tmp->ins_var_index >= 0)
                    fun_type = CZL_INS_VAR_DYNAMIC_FUN;
                else
                    fun_type = CZL_LG_VAR_DYNAMIC_FUN;
            }
            else //未定义的静态函数调用
            {
                if (!(fun=czl_undef_static_fun_create(gp, name)))
                    return NULL;
            }
        }
    }

    if (!(code=czl_exp_fun_build(gp, code, &exp_fun, fun, fun_type)) ||
        !(*node=czl_opr_node_create(gp, CZL_FUNCTION, exp_fun, line)))
        return NULL;

    return code;
}

czl_var* czl_dynamic_var_create(czl_gp *gp, char *name)
{
    if (czl_is_keyword(gp, name))
    {
        sprintf(gp->log_buf, "%s is a keyword, ", name);
        return NULL;
    }
    if (!czl_store_device_check(gp))
        return NULL;

    return czl_var_create_by_field(gp, name, CZL_DYNAMIC_VAR, CZL_NIL);
}

char* czl_array_list_build(czl_gp *gp, char *code, czl_array_list **list)
{
    if (!(*list=czl_array_list_create(gp)))
        return NULL;

    code = czl_ignore_sign_filt(gp, code);
    if (']' == *code)
        return code+1;

    if (!(code=czl_paras_match(gp, code, &(*list)->paras,
                               &(*list)->paras_count, 0, 1, ']')))
    {
        czl_array_list_delete(gp, *list);
        return NULL;
    }

    return code;
}

char* czl_is_exp_right(czl_gp *gp, char *code)
{
    return czl_expression_analysis(gp, code);
}

char* czl_get_arr_len(czl_gp *gp, char *code, czl_exp_ele **len)
{
    code = czl_ignore_sign_filt(gp, code);
    if (*code != '[')
    {
        *len = NULL;
        return code;
    }

    if (!(code=czl_is_exp_right(gp, code+1)))
        return NULL;
    if (!gp->tmp->exp_head)
    {
        sprintf(gp->log_buf, "[] length should not be empty, ");
        return NULL;
    }
    if (*(code++) != ']')
    {
        sprintf(gp->log_buf, "missed ']' for end declared array length, ");
        return NULL;
    }

    if (CZL_EIO(gp->tmp->exp_head) && CZL_ENUM == gp->tmp->exp_head->kind)
    {
        czl_var *var = gp->tmp->exp_head->res;
        if ((CZL_INT == var->type && var->val.inum < 0) ||
            (CZL_FLOAT == var->type && var->val.fnum < 0))
        {
            sprintf(gp->log_buf, "[] length should not be less 0, ");
            return NULL;
        }
        else if (var->type != CZL_INT && var->type != CZL_FLOAT)
        {
            sprintf(gp->log_buf, "[] length should be a number, ");
            return NULL;
        }
    }

    *len = gp->tmp->exp_head;
    gp->tmp->exp_head = NULL;

    return code;
}

char* czl_str_def(czl_gp *gp, char *code, czl_new_sentence **newObj)
{
    if (!(*newObj=czl_new_sentence_create(gp, CZL_STRING)))
        return NULL;

    if (!(code=czl_get_arr_len(gp, code, &(*newObj)->new_obj.arr.len)))
        goto CZL_SYNTAX_ERROR;

    code = czl_ignore_sign_filt(gp, code);
    if ('\"' == *code)
    {
        czl_var *str;
        if (!(code=czl_str_match(gp, code, &str)))
            goto CZL_SYNTAX_ERROR;
        (*newObj)->new_obj.arr.init_list = (czl_array_list*)str;
    }

    return code;

CZL_SYNTAX_ERROR:
    czl_new_sentence_delete(gp, *newObj);
    return NULL;
}

char* czl_arr_def(czl_gp *gp, char *code, czl_new_sentence **newObj, char type)
{
    if (!(*newObj=czl_new_sentence_create(gp, type)))
        return NULL;

    if (!(code=czl_get_arr_len(gp, code, &(*newObj)->new_obj.arr.len)))
        goto CZL_SYNTAX_ERROR;

    code = czl_ignore_sign_filt(gp, code);
    if ('[' == *code && !(code=czl_array_list_build
                          (gp, code+1, &(*newObj)->new_obj.arr.init_list)))
        goto CZL_SYNTAX_ERROR;

    return code;

CZL_SYNTAX_ERROR:
    czl_new_sentence_delete(gp, *newObj);
    return NULL;
}

char* czl_tab_def(czl_gp *gp, char *code, czl_new_sentence **newObj)
{
    if (!(*newObj=czl_new_sentence_create(gp, CZL_TABLE)))
        return NULL;

    code = czl_ignore_sign_filt(gp, code);
    if ('{' == *code)
    {
        if (!(code=czl_tabkv_list_match(gp, code+1, &(*newObj)->new_obj.tab)))
        {
            czl_new_sentence_delete(gp, *newObj);
            return NULL;
        }
    }

    return code;
}

char* czl_class_name_match(czl_gp *gp, char *code, char *name, char *flag)
{
    char *tmp;
    unsigned long line;

    if (!(code=czl_name_match(gp, code, name)))
        goto CZL_SYNTAX_ERROR;

    tmp = code;
    line = gp->error_line;
    code = czl_ignore_sign_filt(gp, code);

    if ('.' == *code)
    {
        if (strcmp("g", name) == 0)
        {
            if (!(code=czl_name_match(gp, code+1, name)))
                goto CZL_SYNTAX_ERROR;
        }
        else
        {
            unsigned long len = strlen(name);
            name[len] = '.';
            if (!(code=czl_name_match(gp, code+1, name+len+1)))
                goto CZL_SYNTAX_ERROR;
        }
        *flag = 1;
        return code;
    }
    else
    {
        *flag = 0;
        gp->error_line = line;
        return tmp;
    }

CZL_SYNTAX_ERROR:
    sprintf(gp->log_buf, "missed class name, ");
    return NULL;
}

char* czl_ins_def(czl_gp *gp, char *code, czl_new_sentence **newObj)
{
    char flag;
    char name[CZL_NAME_MAX_SIZE*2];
    czl_class *pclass;

    if (!(code=czl_class_name_match(gp, code, name, &flag)))
        return NULL;
    if (!(pclass=czl_class_find(gp, name, flag)))
    {
        sprintf(gp->log_buf, "undefined class %s, ", name);
        return NULL;
    }
    if (CZL_IN_CONSTANT == gp->tmp->variable_field && CZL_NOT_SURE == pclass->flag)
    {
        sprintf(gp->log_buf, "unsure class %s, ", name);
        return NULL;
    }
    if (!(*newObj=czl_new_sentence_create(gp, pclass->ot_num)))
        return NULL;
    (*newObj)->new_obj.ins.pclass = pclass;
    if (!(code=czl_get_arr_len(gp, code, &(*newObj)->new_obj.ins.len)))
        goto CZL_SYNTAX_ERROR;
    code = czl_ignore_sign_filt(gp, code);
    if ('(' == *code)
    {
        czl_fun *fun;
        if (CZL_IN_CONSTANT == gp->tmp->variable_field)
        {
            sprintf(gp->log_buf, "init static/const instance should not be with structure function, ");
            goto CZL_SYNTAX_ERROR;
        }
        fun = czl_fun_node_find((*newObj)->new_obj.ins.pclass->name,
                                &(*newObj)->new_obj.ins.pclass->funs_hash);
        if (!fun)
        {
            sprintf(gp->log_buf, "undefined structure function in class %s, ", name);
            goto CZL_SYNTAX_ERROR;
        }
        if (!(code=czl_exp_fun_build(gp, code+1,
                                     &(*newObj)->new_obj.ins.init_fun,
                                     fun, CZL_STATIC_FUN)))
            goto CZL_SYNTAX_ERROR;
    }
    return code;

CZL_SYNTAX_ERROR:
    czl_new_sentence_delete(gp, *newObj);
    return NULL;
}

char* czl_new_sentence_def(czl_gp *gp, char *code, czl_new_sentence **newObj)
{
    int index;

    code = czl_keyword_find(gp, code, &index, 0);
    switch (index)
    {
    case CZL_TABLE_INDEX:
        return czl_tab_def(gp, code, newObj);
    case CZL_STR_INDEX:
        return czl_str_def(gp, code, newObj);
    case CZL_ARR_INDEX:
        return czl_arr_def(gp, code, newObj, CZL_ARRAY);
    case CZL_STACK_INDEX:
        return czl_arr_def(gp, code, newObj, CZL_STACK);
    case CZL_QUEUE_INDEX:
        return czl_arr_def(gp, code, newObj, CZL_QUEUE);
    default:
        return czl_ins_def(gp, code, newObj);
    }
}

char* czl_var_match
(
    czl_gp *gp,
    char *code,
    char *name,
    czl_exp_node **node,
    char my_flag,
    unsigned long line
)
{
    char type;
    czl_var *var;

    if ((var=czl_var_find_in_exp(gp, name, my_flag)))
    {
        if (gp->tmp->ins_var_index >= 0)
            type = CZL_INS_VAR;
        else
            type = (CZL_CONST_VAR == var->quality ? CZL_ENUM : CZL_LG_VAR);
    }
    else if ('=' == *code && *(code+1) != '=')
    {
        if (!(var=czl_dynamic_var_create(gp, name)))
            return NULL;
        type = CZL_LG_VAR;
    }
    else if (my_flag)
    {
        sprintf(gp->log_buf, "undefined variable %s, ", name);
        return NULL;
    }
    else if ('.' == *code)
    {
        //系统变量、函数调用
        return czl_lib_obj_match(gp, code+1, name, node);
    }
    else if (strcmp("new", name) == 0)
    {
        czl_new_sentence *newObj;
        if (!(code=czl_new_sentence_def(gp, code, &newObj)))
            return NULL;
        type = CZL_NEW;
        var = (czl_var*)newObj;
    }
    else
    {
        type = CZL_ENUM;
        if (strcmp("true", name) == 0) var = &czl_true;
        else if (strcmp("false", name) == 0) var = &czl_false;
        else if (strcmp("nil", name) == 0) var = &czl_nil;
        else if (strcmp("null", name) == 0) var = &czl_null;
        else if (strcmp("TP_INT", name) == 0) var = &czl_type_int;
        else if (strcmp("TP_FLOAT", name) == 0) var = &czl_type_float;
        else if (strcmp("TP_STR", name) == 0) var = &czl_type_string;
        else if (strcmp("TP_ARR", name) == 0) var = &czl_type_array;
        else if (strcmp("TP_MAP", name) == 0) var = &czl_type_table;
        else if (strcmp("TP_OBJ", name) == 0) var = &czl_type_ins;
        else if (strcmp("TP_STACK", name) == 0) var = &czl_type_stack;
        else if (strcmp("TP_QUEUE", name) == 0) var = &czl_type_queue;
        else if (strcmp("TP_FILE", name) == 0) var = &czl_type_file;
        else if (strcmp("TP_SRC", name) == 0) var = &czl_type_source;
        else if (strcmp("TP_FUN", name) == 0) var = &czl_type_fun;
        else
        {
            if (CZL_IN_CONSTANT == gp->tmp->variable_field)
                sprintf(gp->log_buf, "undefined constant %s, ", name);
            else
                sprintf(gp->log_buf, "undefined variable %s, ", name);
            return NULL;
        }
    }

    if (!(*node=czl_opr_node_create(gp, type, var, line)))
        return NULL;

    return code;
}

char* czl_array_index_match
(
    czl_gp *gp,
    char *code,
    czl_exp_ele **index
)
{
    if (!(code=czl_is_exp_right(gp, code)))
        return NULL;
    if (!gp->tmp->exp_head)
    {
        sprintf(gp->log_buf, "array index [] should not be empty, ");
        return NULL;
    }
    if (*(code++) != ']')
    {
        sprintf(gp->log_buf, "missed ']' for end an array index, ");
        return NULL;
    }

    *index = gp->tmp->exp_head;
    gp->tmp->exp_head = NULL;

    return code;
}

char* czl_object_member_match(czl_gp *gp, char *code, czl_exp_node *node)
{
    unsigned long cnt = 0;
    czl_exp_fun *exp_fun;
    czl_obj_member *member;
    czl_member_index *index = NULL, *index_tail = NULL;
    czl_var *obj = ((czl_var*)node->op.obj);
    czl_class *pclass = (obj->ot > CZL_NIL ? ((czl_loc_var*)obj)->pclass : NULL);

    if (!(member=czl_obj_member_create(gp, node)))
        return NULL;

    gp->cur_var = obj;

    for (;;)
    {
        if (obj &&
            (('.' == *code && obj->ot < CZL_NIL && obj->ot != CZL_SOURCE) ||
             ('[' == *code && obj->ot != CZL_NIL &&
              obj->ot != CZL_STRING && obj->ot != CZL_ARRAY &&
              obj->ot != CZL_TABLE && obj->ot != CZL_SOURCE)))
        {
            sprintf(gp->log_buf, "index not match of %s, ", obj->name);
            return NULL;
        }
        if ('.' == *code)
        {
            char name[CZL_NAME_MAX_SIZE];
            if (!(code=czl_name_match(gp, code+1, name)))
            {
                sprintf(gp->log_buf, "missed name after '.', ");
                return NULL;
            }
            code = czl_ignore_sign_filt(gp, code);
            if (*code != '(')
                exp_fun = NULL;
            else if (!(code=czl_exp_fun_build(gp, code+1, &exp_fun, NULL,
                                              CZL_LG_VAR_DYNAMIC_FUN)))
                return NULL;
            gp->cur_var = NULL;
            if (!(index=czl_ins_index_create(gp, name, &pclass, exp_fun, member)) &&
                !gp->cur_var)
                return NULL;
        }
        else if ('[' == *code)
        {
            czl_var *cur_var = gp->cur_var;
            czl_exp_ele *exp;
            if (!(code=czl_array_index_match(gp, code+1, &exp)))
                return NULL;
            code = czl_ignore_sign_filt(gp, code);
            if (*code != '(')
                exp_fun = NULL;
            else if (!(code=czl_exp_fun_build(gp, code+1, &exp_fun, NULL,
                                              CZL_LG_VAR_DYNAMIC_FUN)))
            {
                czl_exp_stack_delete(gp, exp);
                return NULL;
            }
            gp->cur_var = cur_var;
            if (!(index=czl_arr_index_create(gp, exp, exp_fun)))
            {
                czl_exp_stack_delete(gp, exp);
                return NULL;
            }
            pclass = NULL;
            gp->cur_var = NULL;
        }
        else
            break;
        if (index)
            czl_member_index_node_insert(&member->index, &index_tail, index);
        if (exp_fun) //不允许 a.fun().b 这种情况，函数在.和[]索引必须是终点
            break;
        if (++cnt > CZL_MAX_MEMBER_INDEX_LAYER)
        {
            sprintf(gp->log_buf, "object index number should be less %d, ", CZL_MAX_MEMBER_INDEX_LAYER);
            return 0;
        }
        code = czl_ignore_sign_filt(gp, code);
        obj = gp->cur_var;
    }

    if (!member->index)
        czl_obj_member_reset(gp, node);
    else if (index->index.ins.exp_fun)
    {
        member->kind = CZL_FUN_INX;
        if (CZL_STATIC_FUNCION == index->flag)
        {
            czl_fun *fun = (czl_fun*)index->index.ins.hash;
            member->ot = (fun->state != CZL_IN_STATEMENT ? fun->ret.ot : CZL_NIL);
        }
        else
            member->ot = CZL_NIL;
    }
    else
    {
        member->kind = CZL_VAR_INX;
        if (CZL_STATIC_INX == index->flag)
            member->ot = gp->cur_var->ot;
        else
            member->ot = CZL_NIL;
    }

    return code;
}

char* czl_class_member_match
(
    czl_gp *gp,
    char *code,
    czl_exp_node **node,
    char *name,
    unsigned long line
)
{
	char type;
	czl_class_var *var;
    czl_class *pclass = czl_class_find(gp, name, 0);
    if (!pclass)
    {
        sprintf(gp->log_buf, "undefined class %s, ", name);
        return NULL;
    }
    if (!(code=czl_name_match(gp, code, name)))
    {
        sprintf(gp->log_buf, "missed variable after :: , ");
        return NULL;
    }
    if (!(var=czl_class_var_find(name, pclass)))
    {
        sprintf(gp->log_buf, "undefined variable %s in class %s, ", name, pclass->name);
        return NULL;
    }
    if (var->quality != CZL_CONST_VAR &&
        CZL_IN_CONSTANT == gp->tmp->variable_field)
    {
        sprintf(gp->log_buf, "variable %s is not a constant in class %s, ", name, pclass->name);
        return NULL;
    }
    type = (CZL_CONST_VAR == var->quality ? CZL_ENUM : CZL_LG_VAR);
    if (!(*node=czl_opr_node_create(gp, type, var, line)))
        return NULL;
    code = czl_ignore_sign_filt(gp, code);
    if (('.' == *code || '[' == *code) && !(code=czl_object_member_match(gp, code, *node)))
    {
        gp->error_line = line;
        *node = NULL;
        return NULL;
    }

    return code;
}

char czl_is_var_or_fun(czl_gp *gp, char **code, czl_exp_node **node, char *flag)
{
    char name[CZL_NAME_MAX_SIZE];
    char *s = *code;
    char my_flag = 0;
    unsigned long line = gp->error_line;

    if (!((*s >= 'a' && *s <= 'z') || (*s >= 'A' && *s <= 'Z') || ('_' == *s)))
        return 0;

    if (!(s=czl_name_match(gp, s, name)))
        return 1;
    s = czl_ignore_sign_filt(gp, s);

    if (flag)
    {
        if (0 == strcmp("add", name))
            *flag = 1;
        else if (0 == strcmp("dec", name))
            *flag = 2;
        if (*flag)
        {
            *code = s;
            return 0;
        }
    }

    if ('.' == *s)
    {
        if (strcmp("my", name) == 0)
        {
            if (gp->tmp->analysis_field != CZL_IN_CLASS &&
                gp->tmp->analysis_field != CZL_IN_CLASS_FUN)
            {
                sprintf(gp->log_buf, "my should use in class-scope, ");
                goto CZL_SYNTAX_ERROR;
            }
            if (!(s=czl_name_match(gp, s+1, name)))
            {
                sprintf(gp->log_buf, "missed variable or function after my, ");
                goto CZL_SYNTAX_ERROR;
            }
            my_flag = 1;
        }
    }
    else if (':' == *s && ':' == *(s+1))
    {
        *code = czl_class_member_match(gp, s+2, node, name, line);
        return 1;
    }

    s = czl_ignore_sign_filt(gp, s);
    if ('(' == *s)
    {
        if (CZL_IN_CONSTANT == gp->tmp->variable_field)
        {
            sprintf(gp->log_buf, "should not use function in constant init, ");
            goto CZL_SYNTAX_ERROR;
        }
        *code = czl_fun_match(gp, s+1, name, node, my_flag, line);
        return 1;
    }

    if (!(s=czl_var_match(gp, s, name, node, my_flag, line)))
        goto CZL_SYNTAX_ERROR;

    if ((*node)->type != CZL_FUNCTION)
    {
        s = czl_ignore_sign_filt(gp, s);
        if (('.' == *s || '[' == *s) && !(s=czl_object_member_match(gp, s, *node)))
            *node = NULL;
    }
    else if (CZL_IN_CONSTANT == gp->tmp->variable_field)
    {
        sprintf(gp->log_buf, "should not use function in constant init, ");
        *node = NULL;
    }

    *code = s;
    return 1;

CZL_SYNTAX_ERROR:
    gp->error_line = line;
    return 1;
}

char* czl_array_list_match(czl_gp *gp, char *code, czl_exp_node **node)
{
    czl_array_list *list;

    if (!(code=czl_array_list_build(gp, code+1, &list)))
        return NULL;

    if (!(*node=czl_opr_node_create(gp, CZL_ARRAY_LIST, list, gp->error_line)))
    {
        czl_array_list_delete(gp, list);
        return NULL;
    }

    return code;
}

char* czl_table_list_match(czl_gp *gp, char *code, czl_exp_node **node)
{
    czl_table_list *list;

    if (!(code=czl_tabkv_list_match(gp, code+1, &list)))
        return NULL;

    if (!(*node=czl_opr_node_create(gp, CZL_TABLE_LIST, list, gp->error_line)))
    {
        czl_table_list_delete(gp, list);
        return NULL;
    }

    return code;
}

char czl_is_array_or_table(czl_gp *gp, char **code, czl_exp_node **node)
{
    char *s = *code;

    if (*s != '[' && *s != '{')
        return 0;

    if ('[' == *s)
        *code = czl_array_list_match(gp, s, node);
    else // '{'
        *code = czl_table_list_match(gp, s, node);

    return 1;
}

char* czl_fun_addr_match(czl_gp *gp, char *code, czl_exp_node **node)
{
    char name[CZL_NAME_MAX_SIZE];
    char funName[CZL_NAME_MAX_SIZE];
	czl_fun *fun;
    czl_sysfun *sys_fun;
    czl_sys_hash *hash;
    czl_usrlib *lib;
    unsigned long line;

    *node = NULL;

    if (!(code=czl_name_match(gp, code, name)))
    {
        sprintf(gp->log_buf, "missed function name after @, ");
        return NULL;
    }

    line = gp->error_line;
    code = czl_ignore_sign_filt(gp, code);
    if ('.' == *code)
    {
        if (!(code=czl_name_match(gp, code+1, funName)))
        {
            sprintf(gp->log_buf, "missed function name after library %s, ", name);
            return NULL;
        }
        if ((lib=(czl_usrlib*)czl_sys_hash_find(CZL_STRING, CZL_NIL,
                                                name, &gp->tmp->usrlibs_hash)))
        {
            if (!(fun=(czl_fun*)czl_sys_hash_find(CZL_STRING, CZL_NIL,
                                                  funName, &lib->funs_hash)))
            {
                sprintf(gp->log_buf, "undefined function %s in library %s, ", funName, name);
                return NULL;
            }
        }
        else
        {
            if (!(hash=czl_sys_fun_hash_create(gp, name)))
                return NULL;
            if (!(sys_fun=czl_sys_fun_find(funName, hash)))
            {
                sprintf(gp->log_buf, "undefined function %s in library %s, ", funName, name);
                return NULL;
            }
            if (!(fun=czl_sys_fun_create(gp, sys_fun)))
                return NULL;
        }
    }
    else if (!(fun=czl_fun_find(gp, name, CZL_GLOBAL_FIND)))
    {
        if (!gp->tmp->osfun_hash &&
            !(gp->tmp->osfun_hash=czl_sys_fun_hash_create(gp, CZL_LIB_OS_NAME)))
            return NULL;
        if ((sys_fun=czl_sys_fun_find(name, gp->tmp->osfun_hash)))
        {
            if (!(fun=czl_sys_fun_create(gp, sys_fun)))
                return NULL;
        }
        else if (!(fun=czl_undef_static_fun_create(gp, name)) ||
                 !czl_nsef_create(gp, fun, 0, line))
            return NULL;
    }

    if (!(*node=czl_opr_node_create(gp, CZL_FUN_REF, fun, gp->error_line)))
        return NULL;

    return code;
}

char czl_is_unary_opt(czl_gp *gp, char **code, czl_exp_node **node, czl_exp_handle *h)
{
    char *s = *code;
    unsigned long i, j, len;

    if (czl_exp_is_integrity(gp, h, 0))
    {
        char sign = *(s-1);
        if (' ' == sign || '\t' == sign || '\n' == sign || '\r' == sign ||
            (*s != '+' && !('-' == *s && '-' == *(s+1))))
            return 0;
    }

    // '-' 优先考虑作为单目运算符
    for (i = 0; i < czl_unary_opt_table_num; i++)
    {
        len = strlen(czl_unary_opt_table[i].name);
        for (j = 0; j < len; j++)
        {
            if (s[j] != czl_unary_opt_table[i].name[j])
                break;
        }
        if (j == len) //匹配上
        {
            //- ! 与 -= != 冲突检测
            if ((CZL_NUMBER_NOT == czl_unary_opt_table[i].macro ||
                 CZL_LOGIC_NOT == czl_unary_opt_table[i].macro) && '=' == s[j])
                return 0;
            //- 与 -> 冲突检测
            if (CZL_NUMBER_NOT == czl_unary_opt_table[i].macro && '>' == s[j])
                return 0;
            //& 与 &= && 冲突检测
            if (CZL_REF_VAR == czl_unary_opt_table[i].macro &&
                ('=' == s[j] || '&' == s[j]))
                return 0;
            *code = s+len;
            *node = czl_unary_opt_node_create
                    (gp, czl_unary_opt_table[i].macro,
                     czl_unary_opt_table[i].position);
            return 1;
        }
    }

    return 0;
}

char czl_is_binary_opt(czl_gp *gp, char **code, czl_exp_node **node)
{
    char *s = *code;
    unsigned long i, j, len;

    if (('+' == *s && '+' == *(s+1) && *(s+2) != '+') ||
        ('-' == *s && '-' == *(s+1) && *(s+2) != '-'))
        return 0;

    for (i = 0; i < czl_binary_opt_table_num; i++)
    {
        len = strlen(czl_binary_opt_table[i].name);
        for (j = 0; j < len; j++)
        {
            if (s[j] != czl_binary_opt_table[i].name[j])
                break;
        }
        if (j == len) //匹配上
        {
            if (CZL_COL == czl_binary_opt_table[i].macro && 0 == gp->tmp->colon_flag)
                return 0;
            *code = s+len;
            *node = czl_binary_opt_node_create
                    (gp, czl_binary_opt_table[i].macro,
                     czl_binary_opt_table[i].priority,
                     czl_binary_opt_table[i].associativity);
            return 1;
        }
    }

    return 0;
}

char* czl_child_exp_analysis_start
(
    czl_gp *gp,
    char *code,
    czl_exp_handle *h
)
{
    h->bc++;
    if (!h->root)
    {
        if (!(code=czl_exp_analysis(gp, code+1, h)))
            return NULL;
        if (h->root)
            h->root->bracket = CZL_BRACKET_YES;
        h->cur = h->root;
    }
    else
    {
        czl_exp_node *node;
        czl_exp_node *root = h->root;
        czl_exp_node *cur = h->cur;
        unsigned long mtoc = h->mtoc;
        h->root = h->cur = NULL;
        h->mtoc = 0;
        code = czl_exp_analysis(gp, code+1, h);
        if (!h->root)
            return NULL;
        node = h->root;
        node->bracket = CZL_BRACKET_YES;
        h->root = root;
        h->cur = cur;
        h->mtoc = mtoc;
        if (!code || !czl_exp_node_insert(gp, node, h))
            return NULL;
    }

    return code;
}

char* czl_child_exp_analysis_end
(
    czl_gp *gp,
    char *code,
    czl_exp_handle *h
)
{
    h->bc--;

    //表达式树完整新检查
    if (!czl_exp_integrity_check(gp, h))
        return NULL;

    return code+1;
}

char* czl_exp_analysis(czl_gp *gp, char *code, czl_exp_handle *h)
{
    czl_exp_node *node;

    for (node = NULL; *code != '\0'; node = NULL)
    {
        code = czl_ignore_sign_filt(gp, code);
        //括号
        if ('(' == *code)
        {
            if (czl_exp_is_integrity(gp, h, 0))
                return czl_exp_integrity_check(gp, h) ? code : NULL;
            if (!(code=czl_child_exp_analysis_start(gp, code, h)))
                return NULL;
            if (2 == gp->ry_flag)
                return code;
            continue;
        }
        else if (')' == *code)
        {
            if (0 == h->bc)
                return czl_exp_integrity_check(gp, h) ? code : NULL;
            return czl_child_exp_analysis_end(gp, code, h);
        }
        else if (!((*code >= 'a' && *code <= 'z') ||
                   (*code >= 'A' && *code <= 'Z') ||
                   (*code >= '0' && *code <= '9') ||
                   '.' == *code || '\'' == *code || '\"' == *code ||
                   '[' == *code || '{' == *code ||
                   '_' == *code || '@' == *code ||
                   ',' == *code || ';' == *code || '}' == *code))
        {
            //单目运算符，注意：必须在双目运算符前判断
            if (czl_is_unary_opt(gp, &code, &node, h)) {}
            //双目运算符
            else if (czl_is_binary_opt(gp, &code, &node)) {}
            else goto CZL_CHECK;
        }
        else
        {
        CZL_CHECK:
            //检查表达式是否已经完整
            if (czl_exp_is_integrity(gp, h, *code))
                return czl_exp_integrity_check(gp, h) ? code : NULL;
            //函数指针
            if ('@' == *code)
            {
                if (!(code=czl_fun_addr_match(gp, code+1, &node)))
                    return NULL;
            }
            //常量: 数字或字符串
            else if (czl_is_constant(gp, &code, &node)) {}
            //变量或函数
            else if (czl_is_var_or_fun(gp, &code, &node, NULL)) {}
            //数组list或表list [ {
            else if (czl_is_array_or_table(gp, &code, &node)) {}
            else //表达式树完整性检查
                return czl_exp_integrity_check(gp, h) ? code : NULL;
        }
        if (!czl_exp_node_insert(gp, node, h))
            return NULL;
    }

    return czl_exp_integrity_check(gp, h) ? code : NULL;
}

char* czl_expression_analysis
(
    czl_gp *gp,
    char *code
)
{
	czl_exp_handle h = {NULL, NULL, 0, 0};
    unsigned long exp_node_cnt_backup = gp->tmp->exp_node_cnt;
    unsigned long reg_cnt_backup = gp->reg_cnt;
    unsigned char reg_flag_backup = gp->tmp->reg_flag;
    unsigned long line = gp->error_line;

    gp->tmp->reg_flag = 0;
    gp->tmp->reg_sign = 0;

    if (!(code=czl_exp_analysis(gp, code, &h)))
    {
        gp->error_line = line;
        return NULL;
    }

    gp->tmp->exp_node_cnt = exp_node_cnt_backup;
    gp->reg_cnt = reg_cnt_backup;
    gp->tmp->reg_flag = reg_flag_backup;

    //树转栈，目的是为了计算表达式时避免递归耗时和
    //减少表达式节点里没价值的指针域占用的内存。
    if (!czl_exp_tree_to_stack(gp, h.root))
    {
        gp->error_line = line;
        return NULL;
    }

    return code;
}

char* czl_exp_sentence_match(czl_gp *gp, char *code)
{
    if (!(code=czl_is_exp_right(gp, code)))
        return NULL;

    if (!(code=czl_end_sign_check(gp, code)))
        return NULL;

    return code;
}

char* czl_sentence_match(czl_gp *gp, char *code)
{
    if (!(code=czl_exp_sentence_match(gp, code)))
        return NULL;

    if (gp->tmp->exp_head) //不存空语句
    {
        //创建表达式语句块
        if (!czl_sentence_block_create(gp, CZL_EXP_SENTENCE))
            return NULL;
    }

    return code;
}

char* czl_var_create
(
    czl_gp *gp,
    char *code,
    czl_var **node,
    char quality,
    char ot
)
{
    char name[CZL_NAME_MAX_SIZE];
    unsigned char var_field_backup;

    //过滤变量名
    if (!(code=czl_name_match(gp, code, name)))
    {
        sprintf(gp->log_buf, "variable name should be within _/a~z/A~Z/0~9 and can not be 0~9 firstly, ");
        return NULL;
    }
    //变量名不能与系统关键字同名
    if (czl_is_keyword(gp, name))
    {
        sprintf(gp->log_buf, "%s is a keyword, ", name);
        return NULL;
    }
    //变量与常量在同一作用域不能同名
    var_field_backup = gp->tmp->variable_field;
    gp->tmp->variable_field = CZL_IN_ANY;
    //变量是否存在
    if (czl_var_find(gp, name, CZL_LOCAL_FIND))
    {
        sprintf(gp->log_buf, "redefined variable %s, ", name);
        return NULL;
    }
    //检查是否已有存储器
    if (!czl_store_device_check(gp))
        return NULL;
    //创建变量节点
    if (!(*node=czl_var_create_by_field(gp, name, quality, ot)))
        return NULL;
    gp->tmp->variable_field = var_field_backup;

    return code;
}

char czl_default_para_type_check(czl_gp *gp, char ot)
{
    czl_exp_ele *exp = gp->tmp->exp_head;

    if (CZL_NIL == ot)
        return 1;

    switch (exp->kind)
    {
    case CZL_FUN_REF:
        return (CZL_FUN_REF == ot ? 1 : 0);
    case CZL_ARRAY_LIST:
        return (CZL_ARRAY == ot ? 1 : 0);
    case CZL_TABLE_LIST:
        return (CZL_TABLE == ot ? 1 : 0);
    case CZL_NEW:
        return (((czl_new_sentence*)exp->res->val.Obj)->type == ot ? 1 : 0);
    default:
        switch (exp->res->type)
        {
        case CZL_INT: case CZL_FLOAT:
            switch (ot) {
            case CZL_INT: case CZL_FLOAT: case CZL_NUM: return 1;
            default: return 0;
            }
        default: //CZL_STRING
            return (CZL_STRING == ot ? 1 : 0);
        }
    }
}

char* czl_var_def(czl_gp *gp, char *code, char quality, char ot)
{
    char const_init_flag = 0;
	czl_var *last_num = NULL;

    if (quality != CZL_DYNAMIC_VAR || CZL_IN_CLASS == gp->tmp->analysis_field)
    {
        const_init_flag = 1;
        gp->tmp->variable_field = CZL_IN_CONSTANT;
    }

    for (;;)
    {
        char *tmp = code;
        int error_line_backup = gp->error_line;
        czl_var *var;
        if (!(code=czl_var_create(gp, code, &var, quality, ot)))
            return NULL;

        if (const_init_flag)
        {
            code = czl_ignore_sign_filt(gp, code);
            if ('=' == *code && *(code+1) != '=')
            {
                if (!(code=czl_is_exp_right(gp, code+1)))
                    return NULL;
                if (!gp->tmp->exp_head)
                {
                    sprintf(gp->log_buf, "init para of %s should not be empty, ", var->name);
                    return NULL;
                }
                if (!(czl_default_para_type_check(gp, ot)))
                {
                    sprintf(gp->log_buf, "type of init para not matched %s, ", var->name);
                    return NULL;
                }
                if (!czl_const_exp_init(gp, var, quality))
                    return NULL;
            }
            else if (CZL_CONST_VAR == quality && last_num)
            {
                //没初始化枚举常量默认是前一个数值常量加一，如果没有则为零
                var->type = var->ot = last_num->type;
                if (CZL_INT == last_num->type)
                    var->val.inum = last_num->val.inum + 1;
                else //CZL_FLOAT
                    var->val.fnum = last_num->val.fnum + 1;
            }
            if (CZL_CONST_VAR == quality &&
                (CZL_INT == var->type || CZL_FLOAT == var->type))
            {
                var->ot = var->type;
                last_num = var;
            }
        }
        else
        {
            code = czl_ignore_sign_filt(gp, code);
            if ('=' == *code && *(code+1) != '=')
            {
                gp->error_line = error_line_backup;
                if (!(code=czl_is_exp_right(gp, tmp)))
                    return NULL;
                //创建变量定义语句块
                if (!czl_sentence_block_create(gp, CZL_EXP_SENTENCE))
                    return NULL;
            }
        }

        code = czl_ignore_sign_filt(gp, code);
        if (',' == *code)
            ++code;
        else
        {
            gp->tmp->variable_field = CZL_IN_ANY;
            return ';' == *code ? code+1 : code;
        }
    }
}

char* czl_const_def(czl_gp *gp, char *code)
{
    czl_enum *node;

    if (!czl_store_device_check(gp))
        return NULL;
    if (!(node=czl_enum_node_create(gp)))
        return NULL;
    czl_enum_insert(gp, node);
    return czl_var_def(gp, code+1, CZL_CONST_VAR, CZL_NIL);
}

char czl_create_new_sentence
(
    czl_gp *gp,
    czl_var *var,
    czl_new_sentence *newObj,
    char const_init_flag
)
{
	czl_exp_ele *exp;
    czl_var *ele = czl_ele_create(gp, CZL_NEW, newObj);
    if (!ele)
    {
        czl_new_sentence_delete(gp, newObj);
        return 0;
    }

    if (!(exp=(czl_exp_ele*)CZL_RT_MALLOC(gp, sizeof(czl_exp_ele))))
        return 0;

    exp->pl.msg.line = gp->error_line;
    exp->pl.msg.cnt = 1;
    exp->next = NULL;

    if (const_init_flag)
    {
        exp->res = ele;
        exp->flag = CZL_OPERAND;
        exp->lt = CZL_OPERAND;
    }
    else
    {
        exp->flag = CZL_BINARY_OPT;
        exp->kind = CZL_ASS;
        exp->lt = CZL_LG_VAR;
        exp->lo = var;
        exp->rt = CZL_NEW;
        exp->ro = ele;
    }

    gp->tmp->exp_head = exp;
    return 1;
}

char* czl_obj_paras_def
(
    czl_gp *gp,
    char *code,
    czl_var *var,
    char type,
    char const_init_flag,
    czl_class *pclass
)
{
	czl_exp_ele *len = NULL;
    czl_new_sentence *newObj = czl_new_sentence_create(gp, type);
    if (!newObj)
        return NULL;

    if (type != CZL_TABLE)
    {
        if (!(code=czl_get_arr_len(gp, code, &len)))
            goto CZL_SYNTAX_ERROR;
        code = czl_ignore_sign_filt(gp, code);
    }

    switch (type)
    {
    case CZL_TABLE:
        break;
    case CZL_ARRAY: case CZL_STACK: case CZL_QUEUE:
        newObj->new_obj.arr.len = len;
        if ('=' == *code && *(code+1) != '=')
        {
            code = czl_ignore_sign_filt(gp, code+1);
            if (*code != '[')
            {
                sprintf(gp->log_buf, "init para of variable %s should be an array, ", var->name);
                goto CZL_SYNTAX_ERROR;
            }
            if (!(code=czl_array_list_build
                  (gp, code+1, &newObj->new_obj.arr.init_list)))
                goto CZL_SYNTAX_ERROR;
        }
        break;
    case CZL_STRING:
        newObj->new_obj.arr.len = len;
        if ('=' == *code && *(code+1) != '=')
        {
			czl_var *str;
            code = czl_ignore_sign_filt(gp, code+1);
            if (*code != '\"')
            {
                sprintf(gp->log_buf, "init para of variable %s should be a string, ", var->name);
                goto CZL_SYNTAX_ERROR;
            }
            if (!(code=czl_str_match(gp, code, &str)))
                goto CZL_SYNTAX_ERROR;
            newObj->new_obj.arr.init_list = (czl_array_list*)str;
        }
        break;
    default:
        newObj->new_obj.ins.pclass = pclass;
        newObj->new_obj.ins.len = len;
        if ('(' == *code)
        {
            czl_fun *fun;
            if (const_init_flag)
            {
                sprintf(gp->log_buf, "init static instance should not be with structure function, ");
                goto CZL_SYNTAX_ERROR;
            }
            if (!(fun=czl_fun_node_find(newObj->new_obj.ins.pclass->name,
                                        &newObj->new_obj.ins.pclass->funs_hash)))
            {
                sprintf(gp->log_buf, "undefined structure function in class %s, ", pclass->name);
                goto CZL_SYNTAX_ERROR;
            }
            if (!(code=czl_exp_fun_build(gp, code+1,
                                         &newObj->new_obj.ins.init_fun,
                                         fun, CZL_STATIC_FUN)))
                goto CZL_SYNTAX_ERROR;
        }
        break;
    }

    if (!czl_create_new_sentence(gp, var, newObj, const_init_flag))
        return NULL;

    return code;

CZL_SYNTAX_ERROR:
    czl_new_sentence_delete(gp, newObj);
    return NULL;
}

char* czl_objs_def
(
    czl_gp *gp,
    char *code,
    char quality,
    char type,
    czl_class *pclass
)
{
    char const_init_flag = 0;

    if (CZL_STATIC_VAR == quality || CZL_IN_CLASS == gp->tmp->analysis_field)
    {
        const_init_flag = 1;
        gp->tmp->variable_field = CZL_IN_CONSTANT;
    }

    for (;;)
    {
		czl_var *var;
        char *tmp = code = czl_ignore_sign_filt(gp, code);
		unsigned long error_line_backup = gp->error_line;

        if (!(code=czl_var_create(gp, code, &var, quality, type)))
            return NULL;

        if (pclass)
            ((czl_loc_var*)var)->pclass = pclass;

        code = czl_ignore_sign_filt(gp, code);
        if ('=' == *code && *(code+1) != '=')
        {
            if (const_init_flag)
            {
                if (!(code=czl_is_exp_right(gp, code+1)))
                    return NULL;
            }
            else
            {
                gp->error_line = error_line_backup;
                if (!(code=czl_is_exp_right(gp, tmp)))
                    return NULL;
            }
        }
        else if (!(code=czl_obj_paras_def(gp, code, var, type, const_init_flag, pclass)))
            return NULL;

        if (!(const_init_flag ? czl_const_exp_init(gp, var, quality) :
            czl_sentence_block_create(gp, CZL_EXP_SENTENCE)))
            return NULL;

        code = czl_ignore_sign_filt(gp, code);
        if (',' == *code)
            ++code;
        else
        {
            gp->tmp->variable_field = CZL_IN_ANY;
            return ';' == *code ? code+1 : code;
        }
    }
}

char* czl_instance_def(czl_gp *gp, char *code, char quality)
{
    char flag;
    char name[CZL_NAME_MAX_SIZE*2];
    czl_class *pclass;

    if (!(code=czl_class_name_match(gp, code, name, &flag)))
        return NULL;

    if (!(pclass=czl_class_find(gp, name, flag)))
    {
        sprintf(gp->log_buf, "undefined class %s, ", name);
        return NULL;
    }
    if (CZL_NOT_SURE == pclass->flag)
    {
        sprintf(gp->log_buf, "unsure class %s, ", name);
        return NULL;
    }

    return czl_objs_def(gp, code, quality, pclass->ot_num, pclass);
}

char* czl_obj_def(czl_gp *gp, char *code, int index, char quality)
{
    switch (index)
    {
    case CZL_VAR_INDEX:
        return czl_var_def(gp, code, quality, CZL_NIL);
    case CZL_INT_INDEX:
        return czl_var_def(gp, code, quality, CZL_INT);
    case CZL_FLOAT_INDEX:
        return czl_var_def(gp, code, quality, CZL_FLOAT);
    case CZL_NUM_INDEX:
        return czl_var_def(gp, code, quality, CZL_NUM);
    case CZL_FILE_INDEX:
        return czl_var_def(gp, code, quality, CZL_FILE);
    case CZL_SOURCE_INDEX:
        return czl_var_def(gp, code, quality, CZL_SOURCE);
    case CZL_FUN_INDEX:
        return czl_var_def(gp, code, quality, CZL_FUN_REF);
    case CZL_INS_INDEX:
        return czl_instance_def(gp, code, quality);
    case CZL_TABLE_INDEX:
        return czl_objs_def(gp, code, quality, CZL_TABLE, NULL);
    case CZL_STR_INDEX:
        return czl_objs_def(gp, code, quality, CZL_STRING, NULL);
    case CZL_ARR_INDEX:
        return czl_objs_def(gp, code, quality, CZL_ARRAY, NULL);
    case CZL_STACK_INDEX:
        return czl_objs_def(gp, code, quality, CZL_STACK, NULL);
    case CZL_QUEUE_INDEX:
        return czl_objs_def(gp, code, quality, CZL_QUEUE, NULL);
    case CZL_CONST_INDEX:
        return czl_const_def(gp, code);
    default:
        if (CZL_STATIC_VAR == quality)
            return czl_var_def(gp, code, quality, CZL_NIL);
        sprintf(gp->log_buf, "%s used error in my scope, ", czl_keyword_table[index-1].name);
        return NULL;
    }
}

char* czl_foreach_dynamic_var_create
(
    czl_gp *gp,
    char *code,
    czl_exp_node **node
)
{
	czl_var *var;
    char name[CZL_NAME_MAX_SIZE];

    if (!(code=czl_name_match(gp, code, name)))
    {
        sprintf(gp->log_buf, "missed variable after for, ");
        return NULL;
    }
    if (!(var=czl_dynamic_var_create(gp, name)))
        return NULL;
    if (!(*node=czl_opr_node_create(gp, CZL_LG_VAR, var, gp->error_line)))
        return NULL;

    return code;
}

char* czl_foreach_obj_start_para(czl_gp *gp, char *code, char flag)
{
	czl_exp_ele *opr;
    czl_exp_node *node = NULL;

    code = czl_ignore_sign_filt(gp, code);
    if (!czl_is_var_or_fun(gp, &code, &node, NULL) || !node)
    {
        if (3 == flag)
        {
            if (!(*code >= '0' && *code <= '9') ||
                !(code=czl_number_match(gp, code, &node)) || !node)
                return NULL;
        }
        else
        {
            sprintf(gp->log_buf, "syntax error, ");
            if (!(code=czl_foreach_dynamic_var_create(gp, code, &node)))
                return NULL;
        }
    }
    else if (3 == flag)
    {
        unsigned char ot = czl_get_opr_ot(node->type, node->op.obj);
        if (ot != CZL_NIL && ot != CZL_INT  && ot != CZL_FLOAT)
        {
            sprintf(gp->log_buf, "para of for count should be int/float, ");
            return NULL;
        }
    }
    else if (node->type != CZL_LG_VAR && node->type != CZL_INS_VAR)
    {
        sprintf(gp->log_buf, "para of for should be a simple variable, ");
        return NULL;
    }

    if (!(opr=czl_opr_create(gp, node)))
        return NULL;
    --gp->tmp->exp_node_cnt;

    switch (flag)
    {
    case 1: gp->tmp->for_paras_start = (czl_para*)opr; break;
    case 2: gp->tmp->for_condition = opr; break;
    default: gp->tmp->for_cnt = opr; break;
    }

    return code;
}

char* czl_foreach_obj_start(czl_gp *gp, char *code)
{
    if (!(code=czl_foreach_obj_start_para(gp, code, 1)))
        return NULL;

    code = czl_ignore_sign_filt(gp, code);
    if (',' == *code && !(code=czl_foreach_obj_start_para(gp, code+1, 2)))
        return NULL;

    return code;
}

char* czl_foreach_obj_end(czl_gp *gp, char *code)
{
    unsigned long cur_line;
    czl_exp_node *node = NULL;
    char flag = 0;

    code = czl_ignore_sign_filt(gp, code);
    cur_line = gp->error_line;

    if (!czl_is_var_or_fun(gp, &code, &node, &flag) || !node)
    {
        if (gp->tmp->foreach_type)
        {
            sprintf(gp->log_buf, "& should be with a variable, ");
            return NULL;
        }
        if ('\"' == *code && !flag)
        {
            if (!(code=czl_string_match(gp, code, &node)))
                return NULL;
        }
        else
        {
            if ('[' == *code && !flag)
            {
                if (!(code=czl_array_list_match(gp, code, &node)))
                    return NULL;
            }
            else if ('(' == *code)
            {
                unsigned long paras_count;
                if (0 == flag || 1 == flag)
                    gp->tmp->foreach_type = 4;
                else
                    gp->tmp->foreach_type = 5;
                if (!(code=czl_paras_match(gp, code+1,
                                           &gp->tmp->for_paras_end,
                                           &paras_count, 0, 0, ')')))
                    return NULL;
                if (paras_count != 2 && paras_count != 3)
                {
                    sprintf(gp->log_buf, "paras number of for range should be 2/3, ");
                    return NULL;
                }
            }
            else
                return NULL;
        }
        if (gp->tmp->for_paras_end)
            return code;
    }
    else if (CZL_IS_NOT_VAR(node))
    {
        if (CZL_FUNCTION == node->type || CZL_MEMBER == node->type)
            gp->tmp->foreach_type = 3;
        else if (2 == gp->tmp->foreach_type)
        {
            sprintf(gp->log_buf, "end para of for should be a variable or function, ");
            return NULL;
        }
    }

    if (!(gp->tmp->for_paras_end=(czl_para*)czl_opr_create(gp, node)))
        return NULL;
    --gp->tmp->exp_node_cnt;
    ((czl_exp_ele*)gp->tmp->for_paras_end)->pl.msg.line = cur_line;

    code = czl_ignore_sign_filt(gp, code);
    if (',' == *code)
    {
        if (CZL_ENUM == node->type ||
            CZL_ARRAY_LIST == node->type ||
            !(code=czl_foreach_obj_start_para(gp, code+1, 3)))
            return NULL;
    }

    return code;
}

char* czl_foreach_paras_match(czl_gp *gp, char *code)
{
	char name[CZL_NAME_MAX_SIZE];

    if (!(code=czl_foreach_obj_start(gp, code)))
        return NULL;
    
    if (!(code=czl_name_match(gp, code, name)))
    {
        sprintf(gp->log_buf, "missed in/key after for, ");
        return NULL;
    }

    if (strcmp("in", name) == 0)
    {
        code = czl_ignore_sign_filt(gp, code);
        if ('&' == *code)
        {
            ++code;
            gp->tmp->foreach_type = 2;
        }
        else
            gp->tmp->foreach_type = 0;
    }
    else if (strcmp("key", name) == 0)
        gp->tmp->foreach_type = 1;
    else
    {
        sprintf(gp->log_buf, "keyword of for should be in/key, ");
        return NULL;
    }

    return czl_foreach_obj_end(gp, code);
}

char* czl_for_condition_match(czl_gp *gp, char *code)
{
    code = czl_ignore_sign_filt(gp, code);
    if (*code != '(')
    {
        gp->tmp->cur_loop_type = CZL_FOREACH_LOOP;
        return czl_foreach_paras_match(gp, code);
    }

    gp->tmp->cur_loop_type = CZL_FOR_LOOP;
    if (!(code=czl_paras_match(gp, code+1,
                               &gp->tmp->for_paras_start,
                               NULL, 2, 0, ';')))
        return NULL;

    if (!(code=czl_exp_sentence_match(gp, code)))
        return NULL;
    gp->tmp->for_condition = gp->tmp->exp_head;
    gp->tmp->exp_head = NULL;

    return czl_paras_match(gp, code,
                           &gp->tmp->for_paras_end,
                           NULL, 2, 0, ')');
}

char* czl_try_flag_match(czl_gp *gp, char *code)
{
    char name[CZL_NAME_MAX_SIZE];

    if (!(code=czl_name_match(gp, code, name)))
    {
        sprintf(gp->log_buf, "try missed exit/break/continue or goto_flag, ");
        return NULL;
    }

    if (strcmp("exit", name) == 0)
        gp->tmp->try_type = CZL_TRY_EXIT;
    else if (strcmp("break", name) == 0)
        gp->tmp->try_type = CZL_TRY_BREAK;
    else if (strcmp("continue", name) == 0)
        gp->tmp->try_type = CZL_TRY_CONTINUE;
    else
    {
        gp->tmp->try_type = CZL_TRY_GOTO;
        if (!(gp->tmp->goto_flag_name=(char*)CZL_TMP_MALLOC(gp, strlen(name)+1)))
            return NULL;
        strcpy(gp->tmp->goto_flag_name, name);
    }

    return code;
}

char* czl_try_condition_match(czl_gp *gp, char *code)
{
    gp->tmp->try_type = CZL_TRY_CONTINUE;

    code = czl_ignore_sign_filt(gp, code);
    if (*code != '(')
        return code;

    if (!(code=czl_try_flag_match(gp, code+1)))
        return NULL;

    code = czl_ignore_sign_filt(gp, code);
    if (')' == *code)
        return code+1;
    else if (',' == *code)
        code++;

    return czl_paras_match(gp, code,
                           &gp->tmp->try_paras,
                           NULL, 0, 0, ')');
}

char* czl_condition_match(czl_gp *gp, char *code, char while_flag)
{
    gp->tmp->condition_flag = 1;
    if (!(code=czl_expression_analysis(gp, code)))
        return NULL;
    gp->tmp->condition_flag = 0;

    if (!gp->tmp->exp_head && !while_flag)
    {
        sprintf(gp->log_buf, "logic condition should not be empty, ");
        return NULL;
    }

    return code;
}

char* czl_cases_condition_match(czl_gp *gp, char *code)
{
    return czl_paras_match(gp, code, &gp->tmp->branch_child_paras, NULL, 0, 0, ':');
}

char czl_elif_condition_create(czl_gp *gp)
{
    //之所以将elif独立处理，是为了适配 elif(else if) 与 case 数据结构格式
    gp->tmp->branch_child_paras = czl_para_node_create(gp, 0);
    return (gp->tmp->branch_child_paras ? 1 : 0);
}

char czl_condition_block_check
(
    czl_gp *gp,
    int index,
    char **code,
    char *do_while_end
)
{
	czl_block_struct *p;

    if (!gp->tmp->block_count)
        return 0;

    p = gp->tmp->block_stack + gp->tmp->block_count - 1;

    if (CZL_NOT_SURE == p->flag)
    {
        if ((CZL_IF_INDEX == index ||
             CZL_ELIF_INDEX == index ||
             CZL_ELSE_INDEX == index) &&
            (CZL_IF_INDEX == p->index ||
             CZL_ELIF_INDEX == p->index ||
             CZL_ELSE_INDEX == p->index))
        {
            if (CZL_IF_INDEX == index)
            {
                //else if 等效 elif 处理
                if (CZL_ELSE_INDEX == p->index)
                {
                    p->index = CZL_ELIF_INDEX;
                    return 1;
                }
                //if if 或 elif if 第一个 if/elif 标记不为空
                else
                    p->flag = CZL_SURE;
            }
            else
            {
                //if elif else 语句均不能为空
                sprintf(gp->log_buf, "block of branch should bot be empty, ");
                return -1;
            }
        }
        else
        {
            p->flag = CZL_SURE;
        }
    }
    else
    {
        if (CZL_FOR_INDEX == p->index ||
            CZL_WHILE_INDEX == p->index ||
            CZL_DO_INDEX == p->index ||
            CZL_TIMER_INDEX == p->index ||
            CZL_TASK_INDEX == p->index ||
            CZL_TRY_INDEX == p->index)
        {
            //do while 语句结束
            if (CZL_DO_INDEX == p->index)
            {
                if (index != CZL_WHILE_INDEX)
                {
                    sprintf(gp->log_buf, "missed while for do, ");
                    return -1;
                }
                if (!(*code=czl_condition_match(gp, *code, 0)))
                    return -1;
                gp->tmp->code_blocks[gp->tmp->code_blocks_count-1].
                                     block.loop->condition = gp->tmp->exp_head;
                gp->tmp->exp_head = NULL;
                *do_while_end = 1;
            }
            gp->tmp->block_count--;
            //退出for while语句块
            gp->tmp->code_blocks_count--;
            return czl_condition_block_check(gp, index, code, do_while_end);
        }
        else if ((CZL_ELSE_INDEX == p->index) ||
                  ((CZL_IF_INDEX == p->index ||
                    CZL_ELIF_INDEX == p->index) &&
                   (index != CZL_ELIF_INDEX &&
                    index != CZL_ELSE_INDEX)))
        {
            czl_if_elif_else_pop(gp);
            //退出if/elif/else语句块
            gp->tmp->code_blocks_count--;
            return czl_condition_block_check(gp, index, code, do_while_end);
        }
        //上面递归调用 czl_condition_block_check(gp, index);
        //是为了纠正以下解释错误情况：
        /*
        fun ()
        {
            if (1) //或 while
                if (1) //或 while
                    ;
        }
        */
    }

    return 0;
}

char* czl_goto_flag_match(czl_gp *gp, char *code)
{
    char name[CZL_NAME_MAX_SIZE];

    if (!(code=czl_name_match(gp, code, name)))
    {
        sprintf(gp->log_buf, "missed goto_flag for goto, ");
        return NULL;
    }

    if (!(gp->tmp->goto_flag_name=(char*)CZL_TMP_MALLOC(gp, strlen(name)+1)))
        return NULL;
    strcpy(gp->tmp->goto_flag_name, name);

    return code;
}

char czl_code_block_create(czl_gp *gp, int index)
{
    char block_type;

    switch (index)
    {
    case CZL_IF_INDEX:
        block_type = CZL_BRANCH_BLOCK;
        gp->tmp->cur_branch_type = CZL_IF_BRANCH;
        break;
    case CZL_ELIF_INDEX:
        block_type = CZL_BRANCH_CHILD_BLOCK;
        break;
    case CZL_ELSE_INDEX:
        block_type = CZL_BRANCH_CHILD_BLOCK;
        break;
    case CZL_TRY_INDEX:
        block_type = CZL_TRY_BLOCK;
        break;
    case CZL_FOR_INDEX:
        block_type = CZL_LOOP_BLOCK;
        break;
    case CZL_WHILE_INDEX:
        block_type = CZL_LOOP_BLOCK;
        gp->tmp->cur_loop_type = CZL_WHILE_LOOP;
        break;
    case CZL_DO_INDEX:
        block_type = CZL_LOOP_BLOCK;
        gp->tmp->cur_loop_type = CZL_DO_WHILE_LOOP;
        break;
    case CZL_TIMER_INDEX:
        block_type = CZL_LOOP_BLOCK;
        gp->tmp->cur_loop_type = CZL_TIMER_LOOP;
        break;
    case CZL_TASK_INDEX:
        block_type = CZL_TASK_BLOCK;
        break;
    case CZL_SWITCH_INDEX:
        block_type = CZL_BRANCH_BLOCK;
        gp->tmp->cur_branch_type = CZL_SWITCH_BRANCH;
        break;
    case CZL_CASE_INDEX:
        block_type = CZL_BRANCH_CHILD_BLOCK;
        break;
    case CZL_DEFAULT_INDEX:
        block_type = CZL_BRANCH_CHILD_BLOCK;
        break;
    case CZL_BREAK_INDEX:
        block_type = CZL_BREAK_SENTENCE;
        break;
    case CZL_CONTINUE_INDEX:
        block_type = CZL_CONTINUE_SENTENCE;
        break;
    case CZL_RETURN_INDEX:
        block_type = CZL_RETURN_SENTENCE;
        break;
    case CZL_YEILD_INDEX:
        block_type = CZL_YEILD_SENTENCE;
        break;
    default: //CZL_GOTO_INDEX
        block_type = CZL_GOTO_SENTENCE;
        break;
    }

    return czl_sentence_block_create(gp, block_type);
}

char czl_switch_check(czl_gp *gp, char *code)
{
    int index;
    int error_line_backup = gp->error_line;

    code = czl_ignore_sign_filt(gp, code);
    if (*code != '{')
    {
        sprintf(gp->log_buf, "missed '{' for switch, ");
        return 0;
    }

    czl_keyword_find(gp, code+1, &index, 0);
    if (index != CZL_CASE_INDEX)
    {
        sprintf(gp->log_buf, "missed case for switch, ");
        return 0;
    }

    gp->error_line = error_line_backup;

    return 1;
}

char* czl_code_block_check(czl_gp *gp, char *code, int index)
{
    if (!czl_block_context_check(gp, index))
        return NULL;

    if (!czl_code_blocks_count(gp, index))
        return NULL;

    switch (index)
    {
    case CZL_DO_INDEX: case CZL_ELSE_INDEX:
        break;
    case CZL_DEFAULT_INDEX:
        code = czl_ignore_sign_filt(gp, code);
        if (':' == *code)
            ++code;
        break;
    case CZL_FOR_INDEX:
        if (!(code=czl_for_condition_match(gp, code)))
            return NULL;
        break;
    case CZL_TASK_INDEX:
        if (!(code=czl_condition_match(gp, code, 0)))
            return NULL;
        break;
    case CZL_TRY_INDEX:
        if (!(code=czl_try_condition_match(gp, code)))
            return NULL;
        break;
    case CZL_WHILE_INDEX: case CZL_TIMER_INDEX:
        if (!(code=czl_condition_match(gp, code, 1)))
            return NULL;
        break;
    case CZL_CASE_INDEX:
        if (!(code=czl_cases_condition_match(gp, code)))
            return NULL;
        break;
    //IF_INDEX/ELIF_INDEX/SWITCH_INDEX
    default:
        if (!(code=czl_condition_match(gp, code, 0)))
            return NULL;
        if (CZL_ELIF_INDEX == index)
        {
            if (!czl_elif_condition_create(gp))
                return NULL;
        }
        else if (CZL_SWITCH_INDEX == index)
        {
            if (!czl_switch_check(gp, code))
                return NULL;
        }
        break;
    }

    //创建条件语句块
    if (!czl_code_block_create(gp, index))
        return NULL;

    return code;
}

char* czl_context_analysis(czl_gp *gp, char *code, int index)
{
    char do_while_end = 0;
    int ret = czl_condition_block_check(gp, index, &code, &do_while_end);

    if (-1 == ret)
        return NULL;

    if (1 == ret)
    {
        //修正 else 语句块为 elif 语句块
        if (!(code=czl_condition_match(gp, code, 0)) ||
            !czl_elif_condition_create(gp))
            return NULL;
        czl_else_block_to_elif(gp);
        return code;
    }

    if (do_while_end)
        return code;

    if (CZL_GOTO_FLAG == index)
        return czl_goto_flag_insert(gp, code);

    switch (index)
    {
    case 0:
        return czl_sentence_match(gp, code);
    case CZL_IF_INDEX: case CZL_ELIF_INDEX: case CZL_ELSE_INDEX:
    case CZL_FOR_INDEX: case CZL_WHILE_INDEX: case CZL_DO_INDEX:
    case CZL_SWITCH_INDEX: case CZL_CASE_INDEX: case CZL_DEFAULT_INDEX:
    case CZL_TRY_INDEX: case CZL_TIMER_INDEX: case CZL_TASK_INDEX:
        return czl_code_block_check(gp, code, index);
    case CZL_LBRACKET_INDEX:
        if (!czl_code_blocks_count(gp, index))
            return NULL;
        break;
    case CZL_RBRACKET_INDEX:
        if (!czl_block_context_check(gp, index))
            return NULL;
        break;
    case CZL_BREAK_INDEX: case CZL_CONTINUE_INDEX:
        if (!czl_block_context_check(gp, index))
            return NULL;
        if (!(code=czl_end_sign_check(gp, code)))
            return NULL;
        if (!czl_code_block_create(gp, index))
            return NULL;
        break;
    case CZL_RETURN_INDEX: case CZL_YEILD_INDEX:
        gp->ry_flag = 1;
        if (!(code=czl_exp_sentence_match(gp, code)))
            return NULL;
        gp->ry_flag = 0;
        if (!czl_code_block_create(gp, index))
            return NULL;
        break;
    case CZL_GOTO_INDEX:
        if (!(code=czl_goto_flag_match(gp, code)))
            return NULL;
        if (!czl_code_block_create(gp, index))
            return NULL;
        return czl_end_sign_check(gp, code);
    case CZL_MY_INDEX:
        return czl_sentence_match(gp, code-4);
    case CZL_STATIC_INDEX:
        code = czl_keyword_find(gp, code, &index, 0);
        return czl_obj_def(gp, code, index, CZL_STATIC_VAR);
    default:
        return czl_obj_def(gp, code, index, CZL_DYNAMIC_VAR);
    }

    return code;
}

char* czl_fun_body_analysis(czl_gp *gp, char *code)
{
    int index;

    while (*code != '\0')
    {
        if (!(code=czl_keyword_find(gp, code, &index, 1)))
            return NULL;
        if (!(code=czl_context_analysis(gp, code, index)))
            return NULL;
        code = czl_ignore_sign_filt(gp, code);
        if (0 == gp->tmp->block_count)
            break;
    }

    if (gp->tmp->block_count)
    {
        sprintf(gp->log_buf, "missed '}' for end defined a function, ");
        return NULL;
    }

    return code;
}

czl_fun* czl_usr_fun_create(czl_gp *gp, char *name, char state, char ot)
{
	char fun_type;
	czl_fun *node;

    if (czl_is_keyword(gp, name))
    {
        sprintf(gp->log_buf, "%s is a keyword, ", name);
        return NULL;
    }
    if (CZL_IN_GLOBAL == gp->tmp->analysis_field ||
            CZL_IN_GLOBAL_FUN == gp->tmp->analysis_field)
        fun_type = CZL_USR_GLOBAL_FUN;
    else
        fun_type = CZL_USR_CLASS_FUN;
    node = czl_fun_node_create(gp, name, state, fun_type, ot, NULL);
    if (!node || !czl_fun_insert(gp, node))
        return NULL;

    return node;
}

char czl_get_para_ot(czl_gp *gp, char *name)
{
    switch (czl_is_keyword(gp, name))
    {
    case CZL_INT_INDEX: return CZL_INT;
    case CZL_FLOAT_INDEX: return CZL_FLOAT;
    case CZL_NUM_INDEX: return CZL_NUM;
    case CZL_STR_INDEX: return CZL_STRING;
    case CZL_TABLE_INDEX: return CZL_TABLE;
    case CZL_ARR_INDEX: return CZL_ARRAY;
    case CZL_STACK_INDEX: return CZL_STACK;
    case CZL_QUEUE_INDEX: return CZL_QUEUE;
    case CZL_FILE_INDEX: return CZL_FILE;
    case CZL_SOURCE_INDEX: return CZL_SOURCE;
    case CZL_FUN_INDEX: return CZL_FUN_REF;
    default: return CZL_NIL;
    }
}

char* czl_para_dec_check
(
    czl_gp *gp,
    char *code,
    unsigned char *ot,
    unsigned char *ref_flag,
    czl_class **pclass
)
{
	char name[CZL_NAME_MAX_SIZE];
    char *tmp = code = czl_ignore_sign_filt(gp, code);

    *ot = CZL_NIL;
    *ref_flag = 0;

    if ((code=czl_name_match(gp, code, name)))
    {
        code = czl_ignore_sign_filt(gp, code);
        *ot = czl_get_para_ot(gp, name);
        if (CZL_NIL == *ot)
        {
            if (',' == *code || ')' == *code || '=' == *code)
                return tmp;
            else
            {
                char flag;
                char name[CZL_NAME_MAX_SIZE*2];
                if (!(code=czl_class_name_match(gp, tmp, name, &flag)))
                    return NULL;
                if (!(*pclass=czl_class_find(gp, name, flag)))
                {
                    sprintf(gp->log_buf, "undefined class %s, ", name);
                    return NULL;
                }
                *ot = (*pclass)->ot_num;
                code = czl_ignore_sign_filt(gp, code);
            }
        }
    }
    else
        code = tmp;

    if ('&' == *code)
        *ref_flag = *code++;

    return code;
}

char* czl_fun_paras_match(czl_gp *gp, char *code, czl_fun *fun)
{
    czl_class *pclass;
    unsigned long paras_count = 0;

	gp->tmp->variable_field = CZL_IN_CONSTANT; //函数默认参数只能是常量

    code = czl_ignore_sign_filt(gp, code);
    if ('(' == *code)
    {
        char name[CZL_NAME_MAX_SIZE];
        czl_loc_var *tail;
        czl_para_explain *para_explain_tail = NULL;
        czl_para_explain *para;
        unsigned char ref_flag;
        unsigned char ot;
        code = czl_ignore_sign_filt(gp, code+1);
        if (')' == *code)
        {
            ++code;
            goto CZL_SYNTAX_ERROR;
        }
        for (;;)
        {
            if (!(code=czl_para_dec_check(gp, code, &ot, &ref_flag, &pclass)))
                return NULL;
            if (!(code=czl_name_match(gp, code, name)))
            {
                if (0 == paras_count && CZL_NIL == ot)
                    sprintf(gp->log_buf, "missed ')' for end defined function paras, ");
                else
                    sprintf(gp->log_buf, "missed variable after ',', ");
                return NULL;
            }
            if (czl_is_keyword(gp, name))
            {
                sprintf(gp->log_buf, "%s is a keyword, ", name);
                return NULL;
            }
            if (czl_loc_var_find(name, (czl_loc_var*)fun->loc_vars))
            {
                sprintf(gp->log_buf, "redefined variable %s, ", name);
                return NULL;
            }
            if (!czl_loc_var_create(gp, &fun->loc_vars, &tail,
                                    name, ot, CZL_DYNAMIC_VAR, !ref_flag))
                return NULL;
            if (ot > CZL_NIL)
                tail->pclass = pclass;

            code = czl_ignore_sign_filt(gp, code);
            if ('=' == *code && *(code+1) != '=')
            {
                if (ref_flag)
                    return NULL;
                code = czl_expression_analysis(gp, code+1);
                if (!code || !gp->tmp->exp_head)
                    return NULL;
                if (!czl_default_para_type_check(gp, ot))
                {
                    sprintf(gp->log_buf, "type of default para not matched %s, ", name);
                    return NULL;
                }
            }
            else
            {
                gp->tmp->exp_head = NULL;
            }

            if (ref_flag || gp->tmp->exp_head)
            {
                if (!(para=czl_para_explain_create(gp, paras_count,
                                                   ref_flag, gp->tmp->exp_head)))
                    return NULL;
                czl_para_explain_insert(&fun->para_explain, &para_explain_tail, para);
                gp->tmp->exp_head = NULL;
            }

            paras_count++;

            code = czl_ignore_sign_filt(gp, code);
            if (',' == *code)
                ++code;
            else if (')' == *code++)
                break;
            else
            {
                sprintf(gp->log_buf, "missed ')' for end defined function paras, ");
                return NULL;
            }
        }
    }

CZL_SYNTAX_ERROR:

    //全局main函数规定入参不得多于一个
    if (CZL_IN_GLOBAL_FUN == gp->tmp->analysis_field &&
        0 == strcmp(CZL_MAIN_FUN_NAME, fun->name))
    {
        if (paras_count > 1)
        {
            sprintf(gp->log_buf, "paras number of function main should not be much 1, ");
            return NULL;
        }
        if (gp->tmp->cur_usrlib != gp->tmp->glo_lib)
        {
            sprintf(gp->log_buf, "main function defined should be in global, ");
            return NULL;
        }
        gp->main_err_line = gp->error_line;
    }

    code = czl_ignore_sign_filt(gp, code);
    if (*code != '{')
    {
        sprintf(gp->log_buf, "missed '{' for begin defined function body, ");
        return NULL;
    }

    fun->enter_vars_cnt = paras_count;

    gp->tmp->variable_field = CZL_IN_ANY;

    return code;
}

char* czl_fun_def(czl_gp *gp, char *code, char ot)
{
    char name[CZL_NAME_MAX_SIZE];
    czl_fun *fun;
    czl_class *pclass = NULL;
    unsigned long line = gp->error_line;

    if (!(code=czl_name_match(gp, code, name)))
    {
        sprintf(gp->log_buf, "function name should be within _/a~z/A~Z/0~9 and can not be 0~9 firstly, ");
        return NULL;
    }
    code = czl_ignore_sign_filt(gp, code);
    if (':' == *code)
    {
        if (CZL_IN_CLASS == gp->tmp->analysis_field || *(code+1) != ':')
            goto CZL_SYNTAX_ERROR;
        if (!(pclass=czl_class_find_in_local(gp, name)))
        {
            sprintf(gp->log_buf, "undefined class %s, ", name);
            goto CZL_SYNTAX_ERROR;
        }
        if (!(code=czl_name_match(gp, code+2, name)))
        {
            sprintf(gp->log_buf, "missed function name after ':', ");
            goto CZL_SYNTAX_ERROR;
        }
        gp->tmp->cur_class = pclass;
        gp->tmp->analysis_field = CZL_IN_CLASS_FUN;
    }
    else
    {
        if (CZL_IN_GLOBAL == gp->tmp->analysis_field)
            gp->tmp->analysis_field = CZL_IN_GLOBAL_FUN;
        else //CZL_IN_CLASS
            gp->tmp->analysis_field = CZL_IN_CLASS_FUN;
    }

    if ((fun=czl_fun_find(gp, name, CZL_LOCAL_FIND)))
    {
        if (fun->state != CZL_IN_STATEMENT)
        {
            sprintf(gp->log_buf, "redefined function %s, ", name);
            goto CZL_SYNTAX_ERROR;
        }
        fun->state = CZL_IN_IDLE;
        fun->ret.ot = ot;
    }
    else
    {
        //类成员函数必须要声明，因为需要标记函数的访问权限
        if (pclass)
        {
            sprintf(gp->log_buf, "undeclared function %s in class %s, ", name, pclass->name);
            goto CZL_SYNTAX_ERROR;
        }
        if (!(fun=czl_usr_fun_create(gp, name, CZL_IN_IDLE, ot)))
            goto CZL_SYNTAX_ERROR;
    }

    //初始化函数解析作用域参数
    gp->cur_fun = fun;
    gp->tmp->code_blocks[0].type = CZL_FUN_BLOCK;
    gp->tmp->code_blocks[0].block.fun = fun;
    gp->tmp->code_blocks_count = 1;

    gp->tmp->int_reg_cnt = gp->tmp->float_reg_cnt = 0;

    if (!(code=czl_fun_paras_match(gp, code, fun)))
        return NULL;

    code = czl_fun_body_analysis(gp, code);
    czl_save_fun_enum_list(gp, fun->store_device_head);
    if (!code)
        return NULL;

    //设置my变量
    fun->my_vars = gp->tmp->ins_vars_head;
    gp->tmp->ins_vars_head = NULL;

    //编译抽象语法树ast为opcode
    if (!czl_ast_serialize(gp, fun))
        return NULL;

    return code;

CZL_SYNTAX_ERROR:
    gp->error_line = line;
    return NULL;
}

char* czl_fun_dec(czl_gp *gp, char *code)
{
    char name[CZL_NAME_MAX_SIZE];

    for (;;)
    {
		czl_fun *fun;
        if (!(code=czl_name_match(gp, code, name)))
        {
            sprintf(gp->log_buf, "missed function name after func, ");
            return NULL;
        }
        if (czl_is_keyword(gp, name))
        {
            sprintf(gp->log_buf, "%s is a keyword, ", name);
            return NULL;
        }
        if (czl_sys_hash_find(CZL_STRING, CZL_NIL,
                              name, &gp->tmp->cur_class->funs_hash))
        {
            sprintf(gp->log_buf, "redeclared function %s, ", name);
            return NULL;
        }
		fun = czl_undef_static_fun_create(gp, name);
        if (!fun || !czl_nsef_create(gp, fun, 0, gp->error_line))
            return NULL;
        code = czl_ignore_sign_filt(gp, code);
        if (',' == *code)
            ++code;
        else
            return ';' == *code ? code+1 : code;
    }
}

//读取脚本
char* czl_read_shell(czl_gp *gp, const char *path, unsigned long *code_size)
{
	int i;
    char ch;
	char *code;
    unsigned long line = 0;
    FILE *out = fopen(path, "r");
    if (!out)
    {
        sprintf(gp->log_buf, "Can't open file %s, ", path);
        return NULL;
    }

    if (fseek(out, 0, SEEK_END))
        goto CZL_SYNTAX_ERROR;

    *code_size = ftell(out)+1;
    if (!(code=(char*)CZL_TMP_MALLOC(gp, *code_size)))
        goto CZL_SYNTAX_ERROR;

    rewind(out);

    for (i = 0; (ch=fgetc(out)) != EOF; i++)
    {
        code[i] = ch;
        if ('\n' == ch && ++line > 60000)
        {
            sprintf(gp->log_buf, "file line limit 60000, ");
            goto CZL_SYNTAX_ERROR;
        }
    }
    code[i] = '\0';

    fclose(out);
    return code;

CZL_SYNTAX_ERROR:
    fclose(out);
    return NULL;
}

//跳转到指定文件名进行解析
char* czl_load_shell(czl_gp *gp, char *old_code)
{
	char *path;
	char *new_code;
    char *error_file_backup;
	unsigned long error_line_backup;
    czl_usrlib *usrlib_backup;
	unsigned long code_size;

    old_code = czl_ignore_sign_filt(gp, old_code);
    if (*old_code != '\"')
    {
        sprintf(gp->log_buf, "missed file path after load, ");
        return NULL;
    }

    if (!(old_code=czl_string_filt(gp, old_code, &path)))
        return NULL;

    if (czl_shell_name_find(gp, path))
    {
        CZL_TMP_FREE(gp, path, strlen(path));
        return old_code;
    }

    new_code = czl_read_shell(gp, path, &code_size);
    if (!new_code || !czl_shell_name_save(gp, path, 1))
    {
        CZL_TMP_FREE(gp, path, strlen(path));
        CZL_TMP_FREE(gp, new_code, code_size);
        return NULL;
    }

    error_line_backup = gp->error_line;
    error_file_backup = gp->error_file;
    usrlib_backup = gp->tmp->cur_usrlib;
    gp->error_line = 1;
    gp->error_file = gp->huds.sn_head->name;

    if (!czl_shell_analysis(gp, new_code))
        old_code = NULL;
    else
    {
        gp->error_line = error_line_backup;
        gp->error_file = error_file_backup;
        gp->tmp->cur_usrlib = usrlib_backup;
    }

    CZL_TMP_FREE(gp, new_code, code_size);

    return old_code;
}

char* czl_permission_dec(czl_gp *gp, char *code, int index)
{
    code = czl_ignore_sign_filt(gp, code);
    if (*(code++) != ':')
    {
        sprintf(gp->log_buf, "missed ':'' after class permission declared, ");
        return NULL;
    }

    switch (index)
    {
    case CZL_PUBLIC_INDEX:
        gp->tmp->permission = CZL_PUBLIC;
        break;
    case CZL_PROTECTED_INDEX:
        gp->tmp->permission = CZL_PROTECTED;
        break;
    default: //CZL_PRIVATE_INDEX
        gp->tmp->permission = CZL_PRIVATE;
        break;
    }

    return code;
}

//给类、函数起别名
char* czl_as_build(czl_gp *gp, char *code, char *old_name)
{
    char new_name[CZL_NAME_MAX_SIZE];

    if (gp->tmp->analysis_field != CZL_IN_GLOBAL)
        return NULL;

    if (!(code=czl_name_match(gp, code, new_name)))
        return NULL;

    if (czl_sys_hash_find(CZL_STRING, CZL_NIL, new_name, &gp->tmp->cur_usrlib->as_hash))
    {
        sprintf(gp->log_buf, "redefined as %s, ", new_name);
        return NULL;
    }

    return czl_as_save(gp, new_name, old_name) ? code : NULL;
}

char* czl_is_fun_obj_def(czl_gp *gp, char *code, char ot, char *error)
{
    char name[CZL_NAME_MAX_SIZE];
    char obj_name[CZL_NAME_MAX_SIZE*2];
    unsigned long line;
    char *tmp;
    char flag;
    czl_class *pclass = NULL;

	if (CZL_IN_GLOBAL_FUN == gp->tmp->analysis_field ||
        CZL_IN_CLASS_FUN == gp->tmp->analysis_field)
        return NULL;

    if (CZL_INSTANCE == ot && !(code=czl_class_name_match(gp, code, obj_name, &flag)))
    {
        *error = 1;
        return code;
    }

    line = gp->error_line;
    tmp = code;

    if ((code=czl_name_match(gp, code, name)))
    {
        if (strcmp("as", name) == 0)
        {
            if (!(code=czl_as_build(gp, code, obj_name)))
                goto CZL_SYNTAX_ERROR;
            return code;
        }
        if (CZL_INSTANCE == ot)
        {
            if (!(pclass=czl_class_find(gp, obj_name, flag)))
            {
                sprintf(gp->log_buf, "undefined class %s, ", obj_name);
                goto CZL_SYNTAX_ERROR;
            }
            ot = pclass->ot_num;
        }
        code = czl_ignore_sign_filt(gp, code);
        if ('(' == *code || '{' == *code || ':' == *code)
        {
            gp->error_line = line;
            if (!(tmp=czl_fun_def(gp, tmp, ot)))
            {
                *error = 1;
                return code;
            }
            return tmp;
        }
        else if (pclass)
        {
            if (CZL_NOT_SURE == pclass->flag)
            {
                sprintf(gp->log_buf, "unsure class %s, ", name);
                goto CZL_SYNTAX_ERROR;
            }
            gp->error_line = line;
            if (!(code=czl_objs_def(gp, tmp, CZL_DYNAMIC_VAR, pclass->ot_num, pclass)))
                goto CZL_SYNTAX_ERROR;
            return code;
        }
    }
    else if (CZL_INSTANCE == ot)
        goto CZL_SYNTAX_ERROR;

    gp->error_line = line;
    code = tmp;
    return NULL;

CZL_SYNTAX_ERROR:
    gp->error_line = line;
    *error = 1;
    return code;
}

char* czl_is_fun_or_obj_def(czl_gp *gp, char *code, int index)
{
    char ot, error = 0;
	char *tmp;

    switch (index)
    {
    case CZL_INT_INDEX: ot = CZL_INT; break;
    case CZL_FLOAT_INDEX: ot = CZL_FLOAT; break;
    case CZL_NUM_INDEX: ot = CZL_NUM; break;
    case CZL_FILE_INDEX: ot = CZL_FILE; break;
    case CZL_SOURCE_INDEX: ot = CZL_SOURCE; break;
    case CZL_FUN_INDEX: ot = CZL_FUN_REF; break;
    case CZL_INS_INDEX: ot = CZL_INSTANCE; break;
    case CZL_TABLE_INDEX: ot = CZL_TABLE; break;
    case CZL_STR_INDEX: ot = CZL_STRING; break;
    case CZL_ARR_INDEX: ot = CZL_ARRAY; break;
    case CZL_STACK_INDEX: ot = CZL_STACK; break;
    case CZL_QUEUE_INDEX: ot = CZL_QUEUE; break;
    case CZL_VAR_INDEX: return czl_var_def(gp, code, CZL_DYNAMIC_VAR, CZL_NIL);
    case CZL_CONST_INDEX: return czl_const_def(gp, code);
    default: return index ? NULL : czl_fun_def(gp, code, CZL_NIL);
    }

    if ((tmp=czl_is_fun_obj_def(gp, code, ot, &error)))
        return error ? NULL : tmp;

    switch (index)
    {
    case CZL_INT_INDEX:
        return czl_var_def(gp, code, CZL_DYNAMIC_VAR, CZL_INT);
    case CZL_FLOAT_INDEX:
        return czl_var_def(gp, code, CZL_DYNAMIC_VAR, CZL_FLOAT);
    case CZL_NUM_INDEX:
        return czl_var_def(gp, code, CZL_DYNAMIC_VAR, CZL_NUM);
    case CZL_FILE_INDEX:
        return czl_var_def(gp, code, CZL_DYNAMIC_VAR, CZL_FILE);
    case CZL_SOURCE_INDEX:
        return czl_var_def(gp, code, CZL_DYNAMIC_VAR, CZL_SOURCE);
    case CZL_FUN_INDEX:
        return czl_var_def(gp, code, CZL_DYNAMIC_VAR, CZL_FUN_REF);
    case CZL_TABLE_INDEX:
        return czl_objs_def(gp, code, CZL_DYNAMIC_VAR, CZL_TABLE, NULL);
    default: //CZL_STR_INDEX/CZL_ARR_INDEX/
             //CZL_STACK_INDEX/CZL_QUEUE_INDEX/CZL_LIS_INDEX
        return czl_objs_def(gp, code, CZL_DYNAMIC_VAR, ot, NULL);
    }
}

char* czl_class_body_match
(
    czl_gp *gp,
    char *code,
    czl_class *pclass,
    char analysis_field,
    char permission,
    czl_class *parent
)
{
    int index;
    czl_class_var_list var_list_backup = gp->tmp->class_vars_tail;

    gp->tmp->cur_class = pclass;
    gp->tmp->permission = CZL_PUBLIC;

    for (;;)
    {
        code = czl_keyword_find(gp, code, &index, 2);
        gp->tmp->analysis_field = CZL_IN_CLASS;
        switch (index)
        {
        case CZL_RBRACKET_INDEX:    //类定义结束
            czl_set_class_total_cnt(pclass);
        #ifdef CZL_MM_MODULE
            czl_mm_pool_init(&pclass->pool, CZL_IL(pclass->total_count), CZL_MM_OBJ_SP);
        #endif
            pclass->flag = CZL_SURE;
            gp->tmp->analysis_field = analysis_field;
            gp->tmp->permission = permission;
            gp->tmp->cur_class = parent;
            gp->tmp->class_vars_tail = var_list_backup;
            return code;
        case CZL_PUBLIC_INDEX:         //访问权限声明
        case CZL_PROTECTED_INDEX: case CZL_PRIVATE_INDEX:
            code = czl_permission_dec(gp, code, index);
            break;
        case CZL_STATIC_INDEX:      //静态变量、实例定义
            code = czl_keyword_find(gp, code, &index, 0);
            code = czl_obj_def(gp, code, index, CZL_STATIC_VAR);
            break;
        case CZL_CLASS_INDEX: //类定义
            code = czl_class_def(gp, code, CZL_IN_CLASS, 0,
                                 gp->tmp->permission, pclass);
            break;
        case CZL_FINAL_INDEX:
            code = czl_keyword_find(gp, code, &index, 0);
            if (index != CZL_CLASS_INDEX)
                return NULL;
            code = czl_class_def(gp, code, CZL_IN_CLASS, 1,
                                 gp->tmp->permission, pclass);
            break;
        case CZL_FUNC_INDEX:
            code = czl_fun_dec(gp, code);
            break;
        default:
            code = czl_is_fun_or_obj_def(gp, code, index);
            break;
        }
        if (!code)
            return NULL;
    }
}

char* czl_class_inherit_match(czl_gp *gp, char *code, czl_class *pclass)
{
    char flag;
    char name[CZL_NAME_MAX_SIZE*2];
    int index;
    czl_class *parent;
    czl_class_parent *node;
    czl_class_parent *parents_tail;
    char permission;

    for (;;)
    {
        code = czl_keyword_find(gp, code, &index, 0);
        switch (index)
        {
        case CZL_PUBLIC_INDEX:
            permission = CZL_PUBLIC;
            break;
        case CZL_PROTECTED_INDEX:
            permission = CZL_PROTECTED;
            break;
        case CZL_PRIVATE_INDEX:
            permission = CZL_PRIVATE;
            break;
        default:
            permission = CZL_PUBLIC;
            break;
        }

        if (!(code=czl_class_name_match(gp, code, name, &flag)))
            return NULL;
        if (!(parent=czl_class_find(gp, name, flag)))
        {
            sprintf(gp->log_buf, "undefined class %s, ", name);
            return NULL;
        }
        if (parent->final_flag)
        {
            sprintf(gp->log_buf, "class %s is final, ", name);
            return NULL;
        }
        if (czl_if_class_parent_repeat(name, pclass))
        {
            sprintf(gp->log_buf, "redeclared parent %s, ", name);
            return NULL;
        }
        if (!(node=czl_class_parent_node_create(gp, parent, permission)))
            return NULL;
        czl_class_parent_node_insert(&pclass->parents, &parents_tail, node);
        code = czl_ignore_sign_filt(gp, code);
        switch (*code)
        {
        case ',':
            code++;
            break;
        case '{':
            return code;
        default:
            sprintf(gp->log_buf, "missed '{' for begin define class body, ");
            return NULL;
        }
    }
}

char* czl_class_def
(
    czl_gp *gp,
    char *code,
    char analysis_field,
    char final_flag,
    char permission,
    czl_class *parent
)
{
    char name[CZL_NAME_MAX_SIZE];
    czl_class *node;

    if (!(code=czl_name_match(gp, code, name)))
    {
        sprintf(gp->log_buf, "missed class name after class, ");
        return NULL;
    }
    if (czl_is_keyword(gp, name))
    {
        sprintf(gp->log_buf, "%s is a keyword, ", name);
        return NULL;
    }
    if (czl_class_find_in_local(gp, name))
    {
        sprintf(gp->log_buf, "redefined class %s, ", name);
        return NULL;
    }
    code = czl_ignore_sign_filt(gp, code);
    if (*code != '{' && *code != ':')
    {
        sprintf(gp->log_buf, "missed '{' or ':' after class, ");
        return NULL;
    }
    if (!(node=czl_class_create(gp, name, final_flag)))
        return NULL;
    if (!czl_class_insert(gp, node))
        return NULL;
    if (':' == *code && !(code=czl_class_inherit_match(gp, code+1, node)))
        return NULL;

    return czl_class_body_match(gp, code+1, node, analysis_field, permission, parent);
}

char* czl_lib_def(czl_gp *gp, char *code, char *flag)
{
    char name[CZL_NAME_MAX_SIZE];
    czl_usrlib *lib;

    if (!(code=czl_name_match(gp, code, name)))
    {
        sprintf(gp->log_buf, "missed library name after lib, ");
        return NULL;
    }
    if (czl_is_keyword(gp, name))
    {
        sprintf(gp->log_buf, "library name should not be keyword, ");
        return NULL;
    }
    code = czl_ignore_sign_filt(gp, code);
    if ('{' == *code)
    {
        ++code;
        *flag = 1;
    }
    else
    {
        *flag = 0;
    }

    lib = (czl_usrlib*)czl_sys_hash_find(CZL_STRING, CZL_NIL, name, &gp->tmp->usrlibs_hash);
    if (!lib && !(lib=czl_usrlib_create(gp, name)))
        return NULL;

    gp->tmp->cur_usrlib = lib;
    return code;
}

//解析脚本生成可执行逻辑结构
char czl_shell_analysis(czl_gp *gp, char *code)
{
    int index;
    char libFlag = 0;

    while (*code != '\0')
    {
        code = czl_keyword_find(gp, code, &index, 0);
        gp->tmp->analysis_field = CZL_IN_GLOBAL;
        switch (index)
        {
        case CZL_LOAD_INDEX: //加载脚本文件
            code = czl_load_shell(gp, code);
            break;
        case CZL_LIB_INDEX:
            if (!(code=czl_lib_def(gp, code, &libFlag)))
                return 0;
            break;
        case CZL_CLASS_INDEX: //类定义
            code = czl_class_def(gp, code, CZL_IN_GLOBAL, 0, CZL_PUBLIC, NULL);
            break;
        case CZL_FINAL_INDEX:
            code = czl_keyword_find(gp, code, &index, 0);
            if (index != CZL_CLASS_INDEX)
                return 0;
            code = czl_class_def(gp, code, CZL_IN_GLOBAL, 1, CZL_PUBLIC, NULL);
            break;
        default:
            if ('}' == *code && libFlag)
            {
                ++code;
                libFlag = 0;
                gp->tmp->cur_usrlib = gp->tmp->glo_lib;
            }
            else
                code = czl_is_fun_or_obj_def(gp, code, index);
            break;
        }
        if (!code)
            return 0;
        code = czl_ignore_sign_filt(gp, code);
    }

    if (libFlag)
    {
        sprintf(gp->log_buf, "missed } for library end, ");
        return 0;
    }

    return 1;
}

char czl_exec_shell_prepare(czl_gp *gp, char *path, char flag)
{
    char ret = 1;

    //读取脚本
    unsigned long code_size;
    char *code = czl_read_shell(gp, path, &code_size);
    if (!code)
        ret = 0;

    //解析脚本
    if (ret && czl_shell_name_save(gp, path, 0))
    {
        gp->error_file = gp->huds.sn_head->name;
        ret = czl_shell_analysis(gp, code);
    }
    else
        gp->error_file = path;

    CZL_TMP_FREE(gp, code, code_size);

    //脚本完整性检查
    ret = czl_integrity_check(gp, ret);

#ifdef CZL_MULT_THREAD
    if (ret && !flag && !czl_thread_para_get(gp, &gp->enter_var, gp->thread_pipe))
        return 0;
#endif //#ifdef CZL_MULT_THREAD

    return ret;
}

czl_fun* czl_exec_shell(czl_gp *gp, char *path, char flag)
{
    if (!czl_exec_shell_prepare(gp, path, flag))
        return NULL;
    return czl_run(gp);
}

#ifndef CZL_CONSOLE
char czl_exec_prepare(czl_gp *gp)
{
    if (czl_exec_shell_prepare(gp, gp->shell_path, 1))
    {
        if (gp->class_name && !(gp->pclass=czl_class_find(gp, gp->class_name, 1)))
            return 0;
        gp->main_fun = gp->cur_fun;
        return 1;
    }
    return 0;
}
#endif //#ifndef CZL_CONSOLE

void czl_mm_print(czl_gp *gp)
{
    char mode[8];
    char modify[64];

    if (0 == gp->mm_cnt) return;

#ifdef CZL_SYSTEM_LINUX
    if (8 == sizeof(czl_ulong))
        strcpy(mode, "%lld");
    else
        strcpy(mode, "%ld");
#elif defined CZL_SYSTEM_WINDOWS
    if (8 == sizeof(czl_ulong))
        strcpy(mode, "%I64d");
    else
        strcpy(mode, "%ld");
#else
    strcpy(mode, "%ld");
#endif

    strcpy(modify, "memory error, ");
    strcat(modify, mode);
    strcat(modify, "\n");
    sprintf(gp->log_buf, modify, gp->mm_cnt);
    czl_log(gp, gp->log_buf);
}

char czl_global_paras_init(czl_gp *gp)
{
    //全局域创建
    if (!(gp->tmp->glo_lib = gp->tmp->cur_usrlib = czl_usrlib_create(gp, "g")))
        return 0;

    //初始化log缓存
    strcpy(gp->log_buf, "syntax error, ");

    //初始化脚本语法错误行号、错误提示
    gp->error_line = 1;

    //脚本中断退出标志位初始化
    gp->exit_code = CZL_EXIT_ABNORMAL;

    //初始化异常码
    gp->exceptionCode = CZL_EXCEPTION_NO;

    //变量区域限制标志位初始化
    gp->tmp->variable_field = CZL_IN_ANY;

    //初始化类强类型声明编号
    gp->tmp->class_ot_num = CZL_NIL;

    return 1;
}

void czl_expfun_init(czl_gp *gp)
{
    //初始化运行时构造函数
    gp->ef0.paras = NULL;
    gp->ef0.paras_count = 0;
    gp->ef0.quality = CZL_STATIC_FUN;
    gp->ef0.flag = 1;

    gp->ef1.paras = &gp->efp2;
    gp->ef1.paras_count = 1;
    gp->ef1.quality = CZL_STATIC_FUN;
    gp->ef1.flag = 1;

    gp->ef2.paras = &gp->efp1;
    gp->ef2.paras_count = 2;
    gp->ef2.quality = CZL_STATIC_FUN;
    gp->ef2.flag = 1;

    gp->efp1.para = &gp->efe1;
    gp->efp1.next = &gp->efp2;
    gp->efp2.para = &gp->efe2;
    gp->efp2.next = NULL;

    gp->efe1.flag = gp->efe1.lt = CZL_OPERAND;
    gp->efe1.kind = CZL_REG_VAR;
    gp->efe2.flag = gp->efe2.lt = CZL_OPERAND;
    gp->efe2.kind = CZL_REG_VAR;

#ifdef CZL_LIB_TCP
    //初始化运行时tcp事件回调构造函数
    gp->tcp_ef.paras = &gp->tcp_efp1;
    gp->tcp_ef.paras_count = 2;
    gp->tcp_ef.quality = CZL_STATIC_FUN;
    gp->tcp_ef.flag = 1;

    gp->tcp_efp1.para = &gp->tcp_efe1;
    gp->tcp_efp1.next = &gp->tcp_efp2;
    gp->tcp_efp2.para = &gp->tcp_efe2;
    gp->tcp_efp2.next = NULL;

    gp->tcp_efe1.flag = gp->tcp_efe1.lt = CZL_OPERAND;
    gp->tcp_efe1.kind = CZL_REG_VAR;
    gp->tcp_efe1.res = &gp->tcp_v1;
    gp->tcp_efe2.flag = gp->tcp_efe2.lt = CZL_OPERAND;
    gp->tcp_efe2.kind = CZL_REG_VAR;
    gp->tcp_efe2.res = &gp->tcp_v2;

    gp->tcp_v1.type = CZL_SOURCE;
#endif //#ifdef CZL_LIB_TCP
}

#if (defined CZL_MULT_THREAD || defined CZL_LIB_TCP)
static unsigned char czl_global_lock_init = 0;
#endif //#if (defined CZL_MULT_THREAD || defined CZL_LIB_TCP)

char czl_sys_init(czl_gp *gp)
{
#if defined CZL_SYSTEM_LINUX && (defined CZL_LIB_COM || \
                                 defined CZL_LIB_TCP || defined CZL_LIB_UDP)
    if ((gp->kdpfd=epoll_create(1)) < 0)
        return 0;
#endif

    if (!(gp->tmp=(czl_analysis_gp*)malloc(sizeof(czl_analysis_gp))))
        return 0;
    memset(gp->tmp, 0, sizeof(czl_analysis_gp));

#ifdef CZL_MM_MODULE
    //初始化内存池： 这个必须先执行
    czl_mm_module_init(gp, CZL_MM_SP_HEAP_NUM_MAX/2, 1, CZL_MM_4MB
                       #ifdef CZL_MM_CACHE
                       , 0
                       #endif
                       );
#endif //#ifdef CZL_MM_MODULE

    gp->mm_limit = CZL_MM_3GB;

#if (defined CZL_MULT_THREAD || defined CZL_LIB_TCP)
    if (!czl_global_lock_init)
    {
        czl_global_lock_init = 1;
    #ifdef CZL_MULT_THREAD
        #ifdef CZL_SYSTEM_WINDOWS
            InitializeCriticalSection(&czl_global_cs);
        #elif defined CZL_SYSTEM_LINUX
            pthread_mutex_init(&czl_global_mutex, NULL);
        #endif
    #endif //#ifdef CZL_MULT_THREAD
    #ifdef CZL_LIB_TCP
        #ifdef CZL_SYSTEM_WINDOWS
            InitializeCriticalSection(&czl_tcp_cs);
        #elif defined CZL_SYSTEM_LINUX
            pthread_mutex_init(&czl_tcp_mutex, NULL);
        #endif
    #endif //#ifdef CZL_LIB_TCP
    }
#endif //#if (defined CZL_MULT_THREAD || defined CZL_LIB_TCP)

#ifdef CZL_TIMER
    #ifdef CZL_SYSTEM_WINDOWS
        InitializeCriticalSection(&gp->timer_cs);
    #elif defined CZL_SYSTEM_LINUX
        pthread_mutex_init(&gp->timer_mutex, NULL);
    #endif
#endif //#ifdef CZL_TIMER

#ifndef CZL_CONSOLE
    if (!(gp->table=czl_table_create(gp, 1, 0, 0)))
        return 0;
#endif //#ifndef CZL_CONSOLE

    //字符串元素缓冲区链表头初始化
    if (!(gp->ch_head=(czl_char_var*)CZL_STACK_MALLOC(gp, sizeof(czl_char_var))))
        return 0;
    gp->ch_head->type = gp->ch_head->ot = CZL_INT;
    gp->ch_head->quality = CZL_DYNAMIC_VAR;
    gp->ch_head->next = NULL;

    //系统函数哈希表创建
    if (!czl_sys_lib_hash_create(gp))
        return 0;

    //系统关键字哈希表创建
    if (!czl_sys_keyword_hash_create(gp, czl_keyword_table, czl_keyword_table_num))
        return 0;

    //初始化czl_global_paras结构
    if (!czl_global_paras_init(gp))
        return 0;

    //初始化临时构造函数结构
    czl_expfun_init(gp);

    return 1;
}
