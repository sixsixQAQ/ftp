#include "CliHandler.hpp"

#include <functional>
#include <sstream>
#include <vector>

#include "ProtocolConfig.hpp"
#include "cmd.hpp"

static void execHandler(Context &context, const std::vector<std::string> args);
static void openHandler(Context &context, const std::vector<std::string> &args);
static void passiveHandler(Context &context, const std::vector<std::string> &args);
static void pwdHandler(Context &context, const std::vector<std::string> &args);
static void deleteHandler(Context &context, const std::vector<std::string> &args);
static void rmdirHandler(Context &context, const std::vector<std::string> &args);
static void mkdirHandler(Context &context, const std::vector<std::string> &args);
static void lsHandler(Context &context, const std::vector<std::string> &args);
static void quitHandler(Context &context, const std::vector<std::string> &args);

static std::vector<std::string> split(const std::string &str)
{
    std::vector<std::string> tokens;
    std::stringstream stream(str);

    std::string token;
    while (stream >> token)
    {
        tokens.emplace_back(token);
    }
    return tokens;
}

CliHandler::CliHandler(std::istream &inStream, std::ostream &outStream) : m_cliContext(inStream, outStream)
{
}

void CliHandler::exec()
{
    std::string cmdLine;

    for (;;)
    {
        m_cliContext.outStream << "ftp>";
        std::vector<std::string> args = parseOneLine(std::cin);
        execHandler(m_cliContext, args);
    }
}

std::vector<std::string> CliHandler::parseOneLine(std::istream &inStream)
{
    std::string cmdLine;
    getline(inStream, cmdLine);
    return split(cmdLine);
}

void execHandler(Context &context, const std::vector<std::string> args)
{
    if (args.size() == 0)
        return;

    using Handler = std::function<void(Context &, const std::vector<std::string>)>;
    static std::unordered_map<std::string, Handler> cmdHandlerMap{
        {"open", openHandler},   {"passive", passiveHandler}, {"pwd", pwdHandler}, {"delete", deleteHandler},
        {"rmdir", rmdirHandler}, {"mkdir", mkdirHandler},     {"ls", lsHandler},   {"quit", quitHandler},
    };

    auto cmdPair = cmdHandlerMap.find(args[0]);
    if (cmdPair != cmdHandlerMap.end())
    {
        cmdPair->second(context, args);
    }
    else
    {
        context.outStream << "?Invalid command.\n";
    }
}

void openHandler(Context &context, const std::vector<std::string> &args)
{
    if (args.size() == 2)
    {
        context.ctrlFd = openImpl(args[1], SERVER_DEFAULT_CONTROL_CONN_PORT);
    }
    else if (args.size() == 3)
    {

        uint16_t port;
        try
        {
            port = std::stoul(args[2]);
            if (port < 1 || port > 65535)
                goto USAGE;
        }
        catch (const std::exception &)
        {
            goto USAGE;
        }

        context.ctrlFd = openImpl(args[1], std::stoul(args[2]));
    }
    {
        context.outStream << "Username:";
        std::string userName;
        getline(context.inStream, userName);
        _userImpl_(context.ctrlFd, userName);

        context.outStream << "Password:";
        std::string passwd;
        getline(context.inStream, passwd);
        _passwordImpl_(context.ctrlFd, passwd);

        return;
    }

USAGE:
    context.outStream << "Usage: open <ip> [port]";
}

void passiveHandler(Context &context, const std::vector<std::string> &args)
{
    if (args.size() != 1)
    {
        context.outStream << "Usage: passive";
        return;
    }
    passiveImpl(context.PASV_Toggle);
    context.outStream << "PASV:" << std::boolalpha << context.PASV_Toggle << "\n";
}
void pwdHandler(Context &context, const std::vector<std::string> &args)
{
    if (args.size() != 1)
    {
        context.outStream << "Usage: pwd";
        return;
    }

    pwdImpl(context.ctrlFd);
}
void deleteHandler(Context &context, const std::vector<std::string> &args)
{
    if (args.size() != 2)
    {
        context.outStream << "Usage: delete <filename>";
        return;
    }
    deleteImpl(context.ctrlFd, args[1]);
}
void rmdirHandler(Context &context, const std::vector<std::string> &args)
{
    if (args.size() != 2)
    {
        context.outStream << "Usage: rmdir <dirname>";
        return;
    }
    rmdirImpl(context.ctrlFd, args[1]);
}
void mkdirHandler(Context &context, const std::vector<std::string> &args)
{
    if (args.size() != 2)
    {
        context.outStream << "Usage: mkdir <dirname>";
        return;
    }
    mkdirImpl(context.ctrlFd, args[1]);
}
void lsHandler(Context &context, const std::vector<std::string> &args)
{
    if (args.size() == 1)
    {
        context.outStream << lsImpl(context.ctrlFd, context.PASV_Toggle);
    }
    else if (args.size() == 2)
    {
        context.outStream << lsImpl(context.ctrlFd, context.PASV_Toggle, args[1]);
    }
    else
    {
        context.outStream << "Usage: ls [path]";
    }
}
void quitHandler(Context &context, const std::vector<std::string> &args)
{
    if (args.size() != 1)
    {
        context.outStream << "Usage: quit";
    }
    quitImpl(context.ctrlFd);
}