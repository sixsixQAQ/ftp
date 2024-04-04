#pragma once

#include <memory>

class AbstractFd {
public:
	AbstractFd (int fd);

	int64_t read (void *buf, size_t size);
	int64_t write (void *buf, size_t size);
	void close ();
	bool isOpened () const;

	enum class Whence {
		CUR,
		END,
		BEG,
	};

	operator int () const
	{
		return getFd();
	}

	int64_t lseek (int64_t offset, Whence whence);
	int getFd () const;
protected:
	class Impl;
	std::shared_ptr<Impl> m_pImpl = nullptr;
};

class BackableFd : protected AbstractFd {
public:
	using AbstractFd::AbstractFd;
	void unread (void *buf, size_t size);

private:
	using AbstractFd::lseek; // delete
};

class ControlFd : public BackableFd {
public:
	using BackableFd::BackableFd;
};

class DataFd : public BackableFd {
public:
	using BackableFd::BackableFd;
};