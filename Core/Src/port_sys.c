#include "port_sys.h"
#include "485.h"
#include <stdarg.h>
/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(u32 errorCode)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
	PRINTF("ST:%d,ERRCODE:%d\r\n",localSubAddr,errorCode);
    __disable_irq();

    while (1)
    {
		
    }

    /* USER CODE END Error_Handler_Debug */
}

#define DEBUG_MAX_TEXT_LEN 255
static void InterAct_OutputDebug(char *buf)
{
	LogTo485((u8 *)buf,strlen(buf));
}

typedef void (*printfCb)(char *buf, int32_t *indicator, char val, int len);
static void InterAct_RelocateLog(char *buf, int32_t *indicator, char val, int len)
{
    int i = 0;

    for (i = 0; i < len; i++)
    {
        if ((*indicator + 1) >= DEBUG_MAX_TEXT_LEN)
        {
            InterAct_OutputDebug(buf);//, *indicator);
			AppMemSet(buf,0,DEBUG_MAX_TEXT_LEN);
            *indicator = 0U;
        }

        buf[*indicator] = val;
        (*indicator)++;
    }
}
static int32_t ConvertRadixNumToString(char *numstr, void *nump, int32_t neg, int32_t radix, bool use_caps)
{
#if PRINTF_ADVANCED_ENABLE
    int64_t a;
    int64_t b;
    int64_t c;

    uint64_t ua;
    uint64_t ub;
    uint64_t uc;
#else
    int32_t a;
    int32_t b;
    int32_t c;

    uint32_t ua;
    uint32_t ub;
    uint32_t uc;
#endif /* PRINTF_ADVANCED_ENABLE */

    int32_t nlen;
    char *nstrp;

    nlen = 0;
    nstrp = numstr;
    *nstrp++ = '\0';

    if (neg)
    {
#if PRINTF_ADVANCED_ENABLE
        a = *(int64_t *)nump;
#else
        a = *(int32_t *)nump;
#endif /* PRINTF_ADVANCED_ENABLE */
        if (a == 0)
        {
            *nstrp = '0';
            ++nlen;
            return nlen;
        }
        while (a != 0)
        {
#if PRINTF_ADVANCED_ENABLE
            b = (int64_t)a / (int64_t)radix;
            c = (int64_t)a - ((int64_t)b * (int64_t)radix);
            if (c < 0)
            {
                uc = (uint64_t)c;
                c = (int64_t)(~uc) + 1 + '0';
            }
#else
            b = a / radix;
            c = a - (b * radix);
            if (c < 0)
            {
                uc = (uint32_t)c;
                c = (uint32_t)(~uc) + 1 + '0';
            }
#endif /* PRINTF_ADVANCED_ENABLE */
            else
            {
                c = c + '0';
            }
            a = b;
            *nstrp++ = (char)c;
            ++nlen;
        }
    }
    else
    {
#if PRINTF_ADVANCED_ENABLE
        ua = *(uint64_t *)nump;
#else
        ua = *(uint32_t *)nump;
#endif /* PRINTF_ADVANCED_ENABLE */
        if (ua == 0)
        {
            *nstrp = '0';
            ++nlen;
            return nlen;
        }
        while (ua != 0)
        {
#if PRINTF_ADVANCED_ENABLE
            ub = (uint64_t)ua / (uint64_t)radix;
            uc = (uint64_t)ua - ((uint64_t)ub * (uint64_t)radix);
#else
            ub = ua / (uint32_t)radix;
            uc = ua - (ub * (uint32_t)radix);
#endif /* PRINTF_ADVANCED_ENABLE */

            if (uc < 10)
            {
                uc = uc + '0';
            }
            else
            {
                uc = uc - 10 + (use_caps ? 'A' : 'a');
            }
            ua = ub;
            *nstrp++ = (char)uc;
            ++nlen;
        }
    }
    return nlen;
}

static int StrFormatPrintf(const char *fmt, va_list ap, char *buf, printfCb cb)
{
    /* va_list ap; */
    char *p;
    int32_t c;

    char vstr[33];
    char *vstrp = NULL;
    int32_t vlen = 0;

    int32_t done;
    int32_t count = 0;

    uint32_t field_width;
    uint32_t precision_width;
    char *sval;
    int32_t cval;
    bool use_caps;
    uint8_t radix = 0;

#if PRINTF_ADVANCED_ENABLE
    uint32_t flags_used;
    int32_t schar, dschar;
    int64_t ival;
    uint64_t uval = 0;
    bool valid_precision_width;
#else
    int32_t ival;
    uint32_t uval = 0;
#endif /* PRINTF_ADVANCED_ENABLE */

#if PRINTF_FLOAT_ENABLE
    double fval;
#endif /* PRINTF_FLOAT_ENABLE */

    /* Start parsing apart the format string and display appropriate formats and data. */
    for (p = (char *)fmt; (c = *p) != 0; p++)
    {
        /*
         * All formats begin with a '%' marker.  Special chars like
         * '\n' or '\t' are normally converted to the appropriate
         * character by the __compiler__.  Thus, no need for this
         * routine to account for the '\' character.
         */
        if (c != '%')
        {
            cb(buf, &count, c, 1);
            /* By using 'continue', the next iteration of the loop is used, skipping the code that follows. */
            continue;
        }

        use_caps = true;

#if PRINTF_ADVANCED_ENABLE
        /* First check for specification modifier flags. */
        flags_used = 0;
        done = false;
        while (!done)
        {
            switch (*++p)
            {
                case '-':
                    flags_used |= kPRINTF_Minus;
                    break;
                case '+':
                    flags_used |= kPRINTF_Plus;
                    break;
                case ' ':
                    flags_used |= kPRINTF_Space;
                    break;
                case '0':
                    flags_used |= kPRINTF_Zero;
                    break;
                case '#':
                    flags_used |= kPRINTF_Pound;
                    break;
                default:
                    /* We've gone one char too far. */
                    --p;
                    done = true;
                    break;
            }
        }
#endif /* PRINTF_ADVANCED_ENABLE */

        /* Next check for minimum field width. */
        field_width = 0;
        done = false;
        while (!done)
        {
            c = *++p;
            if ((c >= '0') && (c <= '9'))
            {
                field_width = (field_width * 10) + (c - '0');
            }
#if PRINTF_ADVANCED_ENABLE
            else if (c == '*')
            {
                field_width = (uint32_t)va_arg(ap, uint32_t);
            }
#endif /* PRINTF_ADVANCED_ENABLE */
            else
            {
                /* We've gone one char too far. */
                --p;
                done = true;
            }
        }
        /* Next check for the width and precision field separator. */
        precision_width = 6;
#if PRINTF_ADVANCED_ENABLE
        valid_precision_width = false;
#endif /* PRINTF_ADVANCED_ENABLE */
        if (*++p == '.')
        {
            /* Must get precision field width, if present. */
            precision_width = 0;
            done = false;
            while (!done)
            {
                c = *++p;
                if ((c >= '0') && (c <= '9'))
                {
                    precision_width = (precision_width * 10) + (c - '0');
#if PRINTF_ADVANCED_ENABLE
                    valid_precision_width = true;
#endif /* PRINTF_ADVANCED_ENABLE */
                }
#if PRINTF_ADVANCED_ENABLE
                else if (c == '*')
                {
                    precision_width = (uint32_t)va_arg(ap, uint32_t);
                    valid_precision_width = true;
                }
#endif /* PRINTF_ADVANCED_ENABLE */
                else
                {
                    /* We've gone one char too far. */
                    --p;
                    done = true;
                }
            }
        }
        else
        {
            /* We've gone one char too far. */
            --p;
        }
#if PRINTF_ADVANCED_ENABLE
        /*
         * Check for the length modifier.
         */
        switch (/* c = */ *++p)
        {
            case 'h':
                if (*++p != 'h')
                {
                    flags_used |= kPRINTF_LengthShortInt;
                    --p;
                }
                else
                {
                    flags_used |= kPRINTF_LengthChar;
                }
                break;
            case 'l':
                if (*++p != 'l')
                {
                    flags_used |= kPRINTF_LengthLongInt;
                    --p;
                }
                else
                {
                    flags_used |= kPRINTF_LengthLongLongInt;
                }
                break;
            default:
                /* we've gone one char too far */
                --p;
                break;
        }
#endif /* PRINTF_ADVANCED_ENABLE */
        /* Now we're ready to examine the format. */
        c = *++p;
        {
            if ((c == 'd') || (c == 'i') || (c == 'f') || (c == 'F') || (c == 'x') || (c == 'X') || (c == 'o') ||
                (c == 'b') || (c == 'p') || (c == 'u'))
            {
                if ((c == 'd') || (c == 'i'))
                {
#if PRINTF_ADVANCED_ENABLE
                    if (flags_used & kPRINTF_LengthLongLongInt)
                    {
                        ival = (int64_t)va_arg(ap, int64_t);
                    }
                    else
#endif /* PRINTF_ADVANCED_ENABLE */
                    {
                        ival = (int32_t)va_arg(ap, int32_t);
                    }
                    vlen = ConvertRadixNumToString(vstr, &ival, true, 10, use_caps);
                    vstrp = &vstr[vlen];
#if PRINTF_ADVANCED_ENABLE
                    if (ival < 0)
                    {
                        schar = '-';
                        ++vlen;
                    }
                    else
                    {
                        if (flags_used & kPRINTF_Plus)
                        {
                            schar = '+';
                            ++vlen;
                        }
                        else
                        {
                            if (flags_used & kPRINTF_Space)
                            {
                                schar = ' ';
                                ++vlen;
                            }
                            else
                            {
                                schar = 0;
                            }
                        }
                    }
                    dschar = false;
                    /* Do the ZERO pad. */
                    if (flags_used & kPRINTF_Zero)
                    {
                        if (schar)
                        {
                            cb(buf, &count, schar, 1);
                        }
                        dschar = true;

                        cb(buf, &count, '0', field_width - vlen);
                        vlen = field_width;
                    }
                    else
                    {
                        if (!(flags_used & kPRINTF_Minus))
                        {
                            cb(buf, &count, ' ', field_width - vlen);
                            if (schar)
                            {
                                cb(buf, &count, schar, 1);
                            }
                            dschar = true;
                        }
                    }
                    /* The string was built in reverse order, now display in correct order. */
                    if ((!dschar) && schar)
                    {
                        cb(buf, &count, schar, 1);
                    }
#endif /* PRINTF_ADVANCED_ENABLE */
                }

#if PRINTF_FLOAT_ENABLE
                if ((c == 'f') || (c == 'F'))
                {
                    fval = (double)va_arg(ap, double);
                    vlen = ConvertFloatRadixNumToString(vstr, &fval, 10, precision_width);
                    vstrp = &vstr[vlen];

#if PRINTF_ADVANCED_ENABLE
                    if (fval < 0)
                    {
                        schar = '-';
                        ++vlen;
                    }
                    else
                    {
                        if (flags_used & kPRINTF_Plus)
                        {
                            schar = '+';
                            ++vlen;
                        }
                        else
                        {
                            if (flags_used & kPRINTF_Space)
                            {
                                schar = ' ';
                                ++vlen;
                            }
                            else
                            {
                                schar = 0;
                            }
                        }
                    }
                    dschar = false;
                    if (flags_used & kPRINTF_Zero)
                    {
                        if (schar)
                        {
                            cb(buf, &count, schar, 1);
                        }
                        dschar = true;
                        cb(buf, &count, '0', field_width - vlen);
                        vlen = field_width;
                    }
                    else
                    {
                        if (!(flags_used & kPRINTF_Minus))
                        {
                            cb(buf, &count, ' ', field_width - vlen);
                            if (schar)
                            {
                                cb(buf, &count, schar, 1);
                            }
                            dschar = true;
                        }
                    }
                    if ((!dschar) && schar)
                    {
                        cb(buf, &count, schar, 1);
                    }
#endif /* PRINTF_ADVANCED_ENABLE */
                }
#endif /* PRINTF_FLOAT_ENABLE */
                if ((c == 'X') || (c == 'x'))
                {
                    if (c == 'x')
                    {
                        use_caps = false;
                    }
#if PRINTF_ADVANCED_ENABLE
                    if (flags_used & kPRINTF_LengthLongLongInt)
                    {
                        uval = (uint64_t)va_arg(ap, uint64_t);
                    }
                    else
#endif /* PRINTF_ADVANCED_ENABLE */
                    {
                        uval = (uint32_t)va_arg(ap, uint32_t);
                    }
                    vlen = ConvertRadixNumToString(vstr, &uval, false, 16, use_caps);
                    vstrp = &vstr[vlen];

#if PRINTF_ADVANCED_ENABLE
                    dschar = false;
                    if (flags_used & kPRINTF_Zero)
                    {
                        if (flags_used & kPRINTF_Pound)
                        {
                            cb(buf, &count, '0', 1);
                            cb(buf, &count, (use_caps ? 'X' : 'x'), 1);
                            dschar = true;
                        }
                        cb(buf, &count, '0', field_width - vlen);
                        vlen = field_width;
                    }
                    else
                    {
                        if (!(flags_used & kPRINTF_Minus))
                        {
                            if (flags_used & kPRINTF_Pound)
                            {
                                vlen += 2;
                            }
                            cb(buf, &count, ' ', field_width - vlen);
                            if (flags_used & kPRINTF_Pound)
                            {
                                cb(buf, &count, '0', 1);
                                cb(buf, &count, (use_caps ? 'X' : 'x'), 1);
                                dschar = true;
                            }
                        }
                    }

                    if ((flags_used & kPRINTF_Pound) && (!dschar))
                    {
                        cb(buf, &count, '0', 1);
                        cb(buf, &count, (use_caps ? 'X' : 'x'), 1);
                        vlen += 2;
                    }
#endif /* PRINTF_ADVANCED_ENABLE */
                }
                if ((c == 'o') || (c == 'b') || (c == 'p') || (c == 'u'))
                {
#if PRINTF_ADVANCED_ENABLE
                    if (flags_used & kPRINTF_LengthLongLongInt)
                    {
                        uval = (uint64_t)va_arg(ap, uint64_t);
                    }
                    else
#endif /* PRINTF_ADVANCED_ENABLE */
                    {
                        uval = (uint32_t)va_arg(ap, uint32_t);
                    }

                    if (c == 'o')
                    {
                        radix = 8;
                    }
                    else if (c == 'b')
                    {
                        radix = 2;
                    }
                    else if (c == 'p')
                    {
                        radix = 16;
                    }
                    else
                    {
                        radix = 10;
                    }

                    vlen = ConvertRadixNumToString(vstr, &uval, false, radix, use_caps);
                    vstrp = &vstr[vlen];
#if PRINTF_ADVANCED_ENABLE
                    if (flags_used & kPRINTF_Zero)
                    {
                        cb(buf, &count, '0', field_width - vlen);
                        vlen = field_width;
                    }
                    else
                    {
                        if (!(flags_used & kPRINTF_Minus))
                        {
                            cb(buf, &count, ' ', field_width - vlen);
                        }
                    }
#endif /* PRINTF_ADVANCED_ENABLE */
                }
#if !PRINTF_ADVANCED_ENABLE
                cb(buf, &count, ' ', field_width - vlen);
#endif /* !PRINTF_ADVANCED_ENABLE */
                if (vstrp != NULL)
                {
                    while (*vstrp)
                    {
                        cb(buf, &count, *vstrp--, 1);
                    }
                }
#if PRINTF_ADVANCED_ENABLE
                if (flags_used & kPRINTF_Minus)
                {
                    cb(buf, &count, ' ', field_width - vlen);
                }
#endif /* PRINTF_ADVANCED_ENABLE */
            }
            else if (c == 'c')
            {
                cval = (char)va_arg(ap, uint32_t);
                cb(buf, &count, cval, 1);
            }
            else if (c == 's')
            {
                sval = (char *)va_arg(ap, char *);
                if (sval)
                {
#if PRINTF_ADVANCED_ENABLE
                    if (valid_precision_width)
                    {
                        vlen = precision_width;
                    }
                    else
                    {
                        vlen = strlen(sval);
                    }
#else
                    vlen = strlen(sval);
#endif /* PRINTF_ADVANCED_ENABLE */
#if PRINTF_ADVANCED_ENABLE
                    if (!(flags_used & kPRINTF_Minus))
#endif /* PRINTF_ADVANCED_ENABLE */
                    {
                        cb(buf, &count, ' ', field_width - vlen);
                    }

#if PRINTF_ADVANCED_ENABLE
                    if (valid_precision_width)
                    {
                        while ((*sval) && (vlen > 0))
                        {
                            cb(buf, &count, *sval++, 1);
                            vlen--;
                        }
                        /* In case that vlen sval is shorter than vlen */
                        vlen = precision_width - vlen;
                    }
                    else
                    {
#endif /* PRINTF_ADVANCED_ENABLE */
                        while (*sval)
                        {
                            cb(buf, &count, *sval++, 1);
                        }
#if PRINTF_ADVANCED_ENABLE
                    }
#endif /* PRINTF_ADVANCED_ENABLE */

#if PRINTF_ADVANCED_ENABLE
                    if (flags_used & kPRINTF_Minus)
                    {
                        cb(buf, &count, ' ', field_width - vlen);
                    }
#endif /* PRINTF_ADVANCED_ENABLE */
                }
            }
            else
            {
                cb(buf, &count, c, 1);
            }
        }
    }

    return count;
}
bool DebugLogBy485=false;
void PRINTF(char *info,...)
{
	#ifndef DEBUG
	if(!DebugLogBy485)
		return;
	#endif
	va_list ap;
    int logLength = 0U;//, result = 0U;
    char printBuf[DEBUG_MAX_TEXT_LEN] = {0U};

    va_start(ap, info);
    /* format print log first */
    logLength = StrFormatPrintf(info, ap, printBuf, InterAct_RelocateLog);
    /* print log */
    InterAct_OutputDebug(printBuf);

    va_end(ap);
}
