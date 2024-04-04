#include "FTPResponseUtil.hpp"

#include <fstream>
#include <regex>
#include <sstream>
#include <string.h>
#include <unistd.h>

// "214-The following commands are recognized.\r\n"
// " ABOR ACCT ALLO APPE CDUP CWD  DELE EPRT EPSV FEAT HELP LIST MDTM MKD\r\n"
// " MODE NLST NOOP OPTS PASS PASV PORT PWD  QUIT REIN REST RETR RMD  RNFR\r\n"
// " RNTO SITE SIZE SMNT STAT STOR STOU STRU SYST TYPE USER XCUP XCWD XMKD\r\n"
// "214 Help OK.\r\n"

class FTPResponseUtilImpl : ErrorUtil {
public:
	static std::vector<std::string> getResponse (int connFd);

private:
	enum WaitingChar {
		BEG_DIGIT_1,
		BEG_DIGIT_2,
		BEG_DIGIT_3,
		HEAD_SPACE,

		LINE_R,
		LINE_N,
		LINE_PREFIX_SPACE,

		END_DIGIT_1,
		END_DIGIT_2,
		END_DIGIT_3,
		END_SPACE,

		END_LINE_R,
		END_LINE_N,
		ERROR,
		FINISHED,
	};

	enum CharEvent { DIGIT, SPACE, MINUS, R, N, OTHER };

	struct StateTransform {
		WaitingChar currState;
		CharEvent event;
		WaitingChar nextState;
	};

	static void transform (
		WaitingChar *pCurrState, CharEvent event, WaitingChar *pLastState /*NULLAble*/
	);
	static CharEvent eventFromChar (char ch);
};

void
FTPResponseUtilImpl::transform (WaitingChar *pCurrState, CharEvent event, WaitingChar *pLastState)
{
	static const StateTransform stateTransMap[] = {
		{BEG_DIGIT_1, DIGIT, BEG_DIGIT_2},
		{BEG_DIGIT_1, SPACE, ERROR},
		{BEG_DIGIT_1, MINUS, ERROR},
		{BEG_DIGIT_1, R, ERROR},
		{BEG_DIGIT_1, N, ERROR},
		{BEG_DIGIT_1, OTHER, ERROR},

		{BEG_DIGIT_2, DIGIT, BEG_DIGIT_3},
		{BEG_DIGIT_2, SPACE, ERROR},
		{BEG_DIGIT_2, MINUS, ERROR},
		{BEG_DIGIT_2, R, ERROR},
		{BEG_DIGIT_2, N, ERROR},
		{BEG_DIGIT_2, OTHER, ERROR},

		{BEG_DIGIT_3, DIGIT, HEAD_SPACE},
		{BEG_DIGIT_3, SPACE, ERROR},
		{BEG_DIGIT_3, MINUS, ERROR},
		{BEG_DIGIT_3, R, ERROR},
		{BEG_DIGIT_3, N, ERROR},
		{BEG_DIGIT_3, OTHER, ERROR},

		{HEAD_SPACE, DIGIT, ERROR},
		{HEAD_SPACE, SPACE, END_LINE_R},
		{HEAD_SPACE, MINUS, LINE_R},
		{HEAD_SPACE, R, ERROR},
		{HEAD_SPACE, N, ERROR},
		{HEAD_SPACE, OTHER, ERROR},

		{LINE_R, DIGIT, LINE_R},
		{LINE_R, SPACE, LINE_R},
		{LINE_R, MINUS, LINE_R},
		{LINE_R, R, LINE_N},
		{LINE_R, N, LINE_R},
		{LINE_R, OTHER, LINE_R},

		{LINE_N, DIGIT, LINE_R},
		{LINE_N, SPACE, LINE_R},
		{LINE_N, MINUS, LINE_R},
		{LINE_N, R, LINE_R},
		{LINE_N, N, LINE_PREFIX_SPACE},
		{LINE_N, OTHER, LINE_R},

		{LINE_PREFIX_SPACE, DIGIT, END_DIGIT_2},
		{LINE_PREFIX_SPACE, SPACE, LINE_R},
		{LINE_PREFIX_SPACE, MINUS, ERROR},
		{LINE_PREFIX_SPACE, R, ERROR},
		{LINE_PREFIX_SPACE, N, ERROR},
		{LINE_PREFIX_SPACE, OTHER, ERROR},

		{END_DIGIT_2, DIGIT, END_DIGIT_3},
		{END_DIGIT_2, SPACE, ERROR},
		{END_DIGIT_2, MINUS, ERROR},
		{END_DIGIT_2, R, ERROR},
		{END_DIGIT_2, N, ERROR},
		{END_DIGIT_2, OTHER, ERROR},

		{END_DIGIT_3, DIGIT, END_SPACE},
		{END_DIGIT_3, SPACE, ERROR},
		{END_DIGIT_3, MINUS, ERROR},
		{END_DIGIT_3, R, ERROR},
		{END_DIGIT_3, N, ERROR},
		{END_DIGIT_3, OTHER, ERROR},

		{END_SPACE, DIGIT, ERROR},
		{END_SPACE, SPACE, END_LINE_R},
		{END_SPACE, MINUS, ERROR},
		{END_SPACE, R, ERROR},
		{END_SPACE, N, ERROR},
		{END_SPACE, OTHER, ERROR},

		{END_LINE_R, DIGIT, END_LINE_R},
		{END_LINE_R, SPACE, END_LINE_R},
		{END_LINE_R, MINUS, END_LINE_R},
		{END_LINE_R, R, END_LINE_N},
		{END_LINE_R, N, END_LINE_R},
		{END_LINE_R, OTHER, END_LINE_R},

		{END_LINE_N, DIGIT, END_LINE_R},
		{END_LINE_N, SPACE, END_LINE_R},
		{END_LINE_N, MINUS, END_LINE_R},
		{END_LINE_N, R, END_LINE_R},
		{END_LINE_N, N, FINISHED},
		{END_LINE_N, OTHER, END_LINE_R},

	};
	static WaitingChar NULL_ABLE;
	if (pCurrState == nullptr)
		return;
	if (pLastState == nullptr)
		pLastState = &NULL_ABLE;

	for (size_t i = 0; i < sizeof (stateTransMap) / sizeof (stateTransMap[0]); ++i) {
		if (stateTransMap[i].currState == *pCurrState && stateTransMap[i].event == event) {
			*pLastState = *pCurrState;
			*pCurrState = stateTransMap[i].nextState;
			return;
		}
	}
	*pLastState = *pCurrState;
	*pCurrState = ERROR;
}

static std::vector<std::string>
split (const std::string &text, const std::string &delimStr)
{
	std::vector<std::string> tokens;

	size_t nextLineBeg = 0;
	while (true) {
		size_t nextDelim = text.find (delimStr, nextLineBeg);
		if (nextDelim == std::string::npos) // no more delim
		{
			tokens.emplace_back (text.substr (nextLineBeg));
			break;
		} else {
			tokens.emplace_back (text.substr (nextLineBeg, nextDelim + delimStr.length()));
			nextLineBeg = nextDelim + delimStr.length();
		}
	}
	return tokens;
}

std::vector<std::string>
FTPResponseUtilImpl::getResponse (int connfd)
{
	char buf[4096];
	std::stringstream response;

	for (WaitingChar currState = BEG_DIGIT_1;;) {
		int nRead = read (connfd, buf, sizeof (buf));

		if (nRead < 0) {
			setError (strerror (errno));
			goto FAILED;
		} else if (nRead == 0) {
			goto FAILED;
		}

		// TODO:这里没有对粘包进行处理，需要处理
		// //存储报文信息
		// stream.write(buf, nRead);
		// if (stream.bad())
		// {
		//     setError("Response text is too long to save.");
		// }

		//针对每个字符事件执行状态机转换。
		for (int i = 0; i < nRead; ++i) {
			response << buf[i];

			CharEvent event = eventFromChar (buf[i]);
			transform (&currState, event, NULL);
			if (currState == ERROR) {
				setError ("Invalid Response Format");
				goto FAILED;
			} else if (currState == FINISHED) {

				goto SUCCEED;
			}
		}
	}

FAILED:
	return {};

SUCCEED:
	return split (response.str(), "\r\n");
}

FTPResponseUtilImpl::CharEvent
FTPResponseUtilImpl::eventFromChar (char ch)
{
	CharEvent event;
	if (isdigit (ch))
		event = DIGIT;
	else if (ch == ' ')
		event = SPACE;
	else if (ch == '-')
		event = MINUS;
	else if (ch == '\r')
		event = R;
	else if (ch == '\n')
		event = N;
	else
		event = OTHER;
	return event;
}

std::vector<std::string>
FTPResponseUtil::getResponse (AbstractFd connFd)
{
	return FTPResponseUtilImpl::getResponse (connFd);
}

std::vector<std::string>
FTPResponseUtil::echoResponse (AbstractFd connFd, std::ostream &out)
{
	auto response = FTPResponseUtil::getResponse (connFd);
	for (auto line : response) {
		out << line;
	}
	return response;
}

/**
 * "227 Entering Passive Mode (139,199,176,107,143,179).\r\n"
 */
void
FTPResponseUtil::PASVResponse (AbstractFd connFd, std::string &ip, uint16_t &port)
{
	std::string response;
	{
		auto responseVector = FTPResponseUtil::getResponse (connFd);
		if (responseVector.size() < 1) {
			return;
		} else {
			response = responseVector[0];
		}
	}
	static std::regex pattern (R"I_LOVE_YOU(\((.*?,.*?,.*?,.*?),(.*?),(.*?)\))I_LOVE_YOU");

	std::string rawIp;
	std::string headByteStr;
	std::string tailByteStr;
	for (std::sregex_iterator it (response.begin(), response.end(), pattern), end_it; it != end_it; ++it) {
		if ((*it)[1].matched) {
			rawIp = it->str (1);
		}
		if ((*it)[2].matched) {
			headByteStr = it->str (2);
		}
		if ((*it)[3].matched) {
			tailByteStr = it->str (3);
		}
	}
	try {
		uint8_t headByte = (uint8_t)std::stoi (headByteStr);
		uint8_t tailByte = (uint8_t)std::stoi (tailByteStr);
		// little-endian
		if (__ORDER_LITTLE_ENDIAN__) {
			((uint8_t *)&port)[1] = headByte;
			((uint8_t *)&port)[0] = tailByte;
		} else if (__ORDER_BIG_ENDIAN__) {
			((uint8_t *)&port)[0] = headByte;
			((uint8_t *)&port)[1] = tailByte;
		}
		std::replace (rawIp.begin(), rawIp.end(), ',', '.');
		ip = rawIp;
	} catch (...) {
		std::cerr << "response:[" << response << "]\n";
		throw;
	}
}

long
FTPResponseUtil::sizeResponse (AbstractFd connFd)
{
	constexpr int COULD_NOT_GET_FILE_SIZE = 550;
	std::vector<std::string> result		  = getResponse (connFd);
	if (result.size() < 1)
		return -1;
	std::stringstream stream (result[0]);
	std::string text;
	stream >> text;
	if (std::stol (text) == COULD_NOT_GET_FILE_SIZE) {
		return -1;
	}
	text.clear();
	stream >> text;
	return std::stol (text);
}
