#pragma once

#include "Context.hpp"

#include <functional>
#include <map>

class ClientCmd {
public:
	using ArgList = std::vector<std::string>;
	using CmdList = std::vector<std::shared_ptr<ClientCmd>>;
	ClientCmd (Context &context, const ArgList &args);
	virtual ~ClientCmd() = default;

	static std::shared_ptr<ClientCmd> cmdFactory (Context &context, const ArgList &args);
	void templateExecution ();
	virtual std::string help () = 0;
protected:

	using Factory = std::function<std::shared_ptr<ClientCmd> (Context &, const ArgList &)>;

	static const std::map<std::string, Factory> &getFactoryMap ();

	virtual bool isDataConnectionNeeded ();
	virtual bool isControlConnectionNeeded ();
	virtual void exec ()													   = 0;
	virtual std::pair<bool, std::function<void()>> checkArgsWhenAboutToExec () = 0;

	// Note: 这里的返回值回调千万不要依赖于销毁的变量

	Context &m_context;
	ArgList m_args;

private:
	void enterPASVMode ();
	void enterPORTMode ();
};