#include "context_data.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// 报文样例
// "214-The following commands are recognized.\r\n"
// " ABOR ACCT ALLO APPE CDUP CWD  DELE EPRT EPSV FEAT HELP LIST MDTM MKD\r\n"
// " MODE NLST NOOP OPTS PASS PASV PORT PWD  QUIT REIN REST RETR RMD  RNFR\r\n"
// " RNTO SITE SIZE SMNT STAT STOR STOU STRU SYST TYPE USER XCUP XCWD XMKD\r\n"
// "214 Help OK.\r\n"

typedef enum WaitState
{
    waitingForHeadFirstDigit,
    waitingForHeadSecondDigit,
    waitingForHeadThirdDigit,
    waitingForHeadSpace,

    waitingForLineR,
    waitingForLineN,
    waitingForPrefixSpace,
    ///////////////////
    waitingForTailFirstDigit,
    waitingForTailSecondDigit,
    waitingForTailThirdDigit,
    waitingForTailSpace,

    waitingForEndR,
    waitingForEndN,
    ERROR,
    Finished,
} WaitState;

typedef enum
{
    Digit,
    Space,
    Minus,
    R,
    N,
    Other
} CharEvent;

typedef struct StateTransform
{
    WaitState currState;
    CharEvent event;
    WaitState nextState;
} StateTransform;

static void transform(WaitState *pCurrState, CharEvent event, WaitState *pLastState /*NULLAble*/)
{
    static const StateTransform stateTransMap[] = {
        {waitingForHeadFirstDigit, Digit, waitingForHeadSecondDigit},
        {waitingForHeadFirstDigit, Space, ERROR},
        {waitingForHeadFirstDigit, Minus, ERROR},
        {waitingForHeadFirstDigit, R, ERROR},
        {waitingForHeadFirstDigit, N, ERROR},
        {waitingForHeadFirstDigit, Other, ERROR},

        {waitingForHeadSecondDigit, Digit, waitingForHeadThirdDigit},
        {waitingForHeadSecondDigit, Space, ERROR},
        {waitingForHeadSecondDigit, Minus, ERROR},
        {waitingForHeadSecondDigit, R, ERROR},
        {waitingForHeadSecondDigit, N, ERROR},
        {waitingForHeadSecondDigit, Other, ERROR},

        {waitingForHeadThirdDigit, Digit, waitingForHeadSpace},
        {waitingForHeadThirdDigit, Space, ERROR},
        {waitingForHeadThirdDigit, Minus, ERROR},
        {waitingForHeadThirdDigit, R, ERROR},
        {waitingForHeadThirdDigit, N, ERROR},
        {waitingForHeadThirdDigit, Other, ERROR},

        {waitingForHeadSpace, Digit, ERROR},
        {waitingForHeadSpace, Space, waitingForEndR},
        {waitingForHeadSpace, Minus, waitingForLineR},
        {waitingForHeadSpace, R, ERROR},
        {waitingForHeadSpace, N, ERROR},
        {waitingForHeadSpace, Other, ERROR},

        {waitingForLineR, Digit, waitingForLineR},
        {waitingForLineR, Space, waitingForLineR},
        {waitingForLineR, Minus, waitingForLineR},
        {waitingForLineR, R, waitingForLineN},
        {waitingForLineR, N, waitingForLineR},
        {waitingForLineR, Other, waitingForLineR},

        {waitingForLineN, Digit, waitingForLineR},
        {waitingForLineN, Space, waitingForLineR},
        {waitingForLineN, Minus, waitingForLineR},
        {waitingForLineN, R, waitingForLineR},
        {waitingForLineN, N, waitingForPrefixSpace},
        {waitingForLineN, Other, waitingForLineR},

        {waitingForPrefixSpace, Digit, waitingForTailSecondDigit},
        {waitingForPrefixSpace, Space, waitingForLineR},
        {waitingForPrefixSpace, Minus, ERROR},
        {waitingForPrefixSpace, R, ERROR},
        {waitingForPrefixSpace, N, ERROR},
        {waitingForPrefixSpace, Other, ERROR},

        {waitingForTailSecondDigit, Digit, waitingForTailThirdDigit},
        {waitingForTailSecondDigit, Space, ERROR},
        {waitingForTailSecondDigit, Minus, ERROR},
        {waitingForTailSecondDigit, R, ERROR},
        {waitingForTailSecondDigit, N, ERROR},
        {waitingForTailSecondDigit, Other, ERROR},

        {waitingForTailThirdDigit, Digit, waitingForTailSpace},
        {waitingForTailThirdDigit, Space, ERROR},
        {waitingForTailThirdDigit, Minus, ERROR},
        {waitingForTailThirdDigit, R, ERROR},
        {waitingForTailThirdDigit, N, ERROR},
        {waitingForTailThirdDigit, Other, ERROR},

        {waitingForTailSpace, Digit, ERROR},
        {waitingForTailSpace, Space, waitingForEndR},
        {waitingForTailSpace, Minus, ERROR},
        {waitingForTailSpace, R, ERROR},
        {waitingForTailSpace, N, ERROR},
        {waitingForTailSpace, Other, ERROR},

        {waitingForEndR, Digit, waitingForEndR},
        {waitingForEndR, Space, waitingForEndR},
        {waitingForEndR, Minus, waitingForEndR},
        {waitingForEndR, R, waitingForEndN},
        {waitingForEndR, N, waitingForEndR},
        {waitingForEndR, Other, waitingForEndR},

        {waitingForEndN, Digit, waitingForEndR},
        {waitingForEndN, Space, waitingForEndR},
        {waitingForEndN, Minus, waitingForEndR},
        {waitingForEndN, R, waitingForEndR},
        {waitingForEndN, N, Finished},
        {waitingForEndN, Other, waitingForEndR},

    };
    static WaitState NULL_ABLE;
    if (pCurrState == NULL)
        return;
    if (pLastState == NULL)
        pLastState = &NULL_ABLE;

    for (int i = 0; i < sizeof(stateTransMap) / sizeof(stateTransMap[0]); ++i)
    {
        if (stateTransMap[i].currState == *pCurrState && stateTransMap[i].event == event)
        {
            *pLastState = *pCurrState;
            *pCurrState = stateTransMap[i].nextState;
            return;
        }
    }
    *pLastState = *pCurrState;
    *pCurrState = ERROR;
}

static CharEvent eventFromChar(char ch)
{
    CharEvent event;
    if (isdigit(ch))
        event = Digit;
    else if (ch == ' ')
        event = Space;
    else if (ch == '-')
        event = Minus;
    else if (ch == '\r')
        event = R;
    else if (ch == '\n')
        event = N;
    else
        event = Other;
    return event;
}

void echo_response(int connfd)
{
    char *last_response = get_shared_context()->last_response;
    int totalRead = 0;

    char buf[4096];
    WaitState currState = waitingForHeadFirstDigit;
    for (;;)
    {
        int nRead = read(connfd, buf, sizeof(buf));

        if (nRead < 0)
        {
            perror("read()");
            break;
        }
        else if (nRead == 0)
        {
            break;
        }

        //存储报文信息。
        if (totalRead + nRead <= RESPONSE_MAX_SIZE)
        {
            memcpy(last_response, buf, nRead);
            totalRead += nRead;
        }
        else
        {
            fprintf(stderr, "Response buffer is too small.\n");
        }

        //针对每个字符事件执行状态机转换。
        for (int i = 0; i < nRead; ++i)
        {
            putchar(buf[i]);

            CharEvent event = eventFromChar(buf[i]);
            transform(&currState, event, NULL);
            if (currState == ERROR)
            {
                fprintf(stderr, "Parse Error.");
                return;
            }
            else if (currState == Finished)
            {
                return;
            }
        }
    }
}