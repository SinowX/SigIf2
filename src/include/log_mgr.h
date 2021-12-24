#ifndef LOG_MGR_H
#define LOG_MGR_H

#include <ctime>
#include <cstring>
#include <memory>
#include <vector>
#include <mutex>
#include <condition_variable>

#include "conf_mgr.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"


enum LEVEL
{
	INFO=0,
	WARN,
	ERROR
};

class LogMgr
{
private:
	class LogBuffer
	{
		public:
			LogBuffer(const std::size_t max_size=4096,
					const std::size_t safe_limit=100)
				:max_size_(0)
			{
				Reset(max_size, safe_limit);
			}

			LogBuffer(LogBuffer&)=delete;
			void operator=(LogBuffer&)=delete;
			~LogBuffer()
			{
				delete[] buff_;
			}
			
			int Append(const char* str, const std::size_t size)
			{
				if(offset_+size > max_size_)
					return -1;

				strncpy(buff_+offset_, str, size);

				if((offset_+=size)+safe_limit_>max_size_)
					isFull_=true;
				return 0;
			}

			const char* GetBuff() const
			{
				return buff_;
			}
			const std::size_t GetSize() const
			{
				return strlen(buff_);
			}

			void Reset(const std::size_t max_size=4096,
					const std::size_t safe_limit=100)
			{
				if(max_size != max_size_)
				{
					delete[] buff_;
					buff_ = new char[max_size];
					max_size_ = max_size;
				}
				offset_ = 0;
				safe_limit_ = safe_limit;
				isFull_ = false;
			}
			
			std::size_t GetLeftSize()
			{
				return max_size_ - offset_;
			}
			
			std::size_t GetLeftSafeSize()
			{
				return max_size_ - safe_limit_ - offset_;
			}

		private:
			std::size_t max_size_{0};
			std::size_t offset_{0};
			std::size_t safe_limit_{100};
			char* buff_{nullptr};
			bool isFull_{false};
	};

	using LogBufferPtr=std::unique_ptr<LogBuffer>;
	using LogBufferVec=std::vector<LogBufferPtr>;

public:
	static LogMgr& getInstance()
	{
		static LogMgr instance_;
		return instance_;
	}
	
	LogMgr(LogMgr&)=delete;
	void operator=(LogMgr&)=delete;
	~LogMgr()
	{
		if(logfd>=0) close(logfd);
	}

	/* LogMgr& operator<<(const char* content) */
	/* { */
	/* 	std::time_t t = std::time(nullptr); */
	/* 	std::strftime(time_str, sizeof time_str, "%Y-%m-%d %OH:%M:%S", std::localtime(&t)); */
	/* 	snprintf(content_str, sizeof(content_str), "%s ") */
	/* } */
	void LOG(const int level, const char* content, const char* filename, const int line)
	{
		char time_str[50];
		char content_str[100];

		std::time_t t = std::time(nullptr);
		std::strftime(time_str, sizeof time_str, "%Y-%m-%d %OH:%M:%S", std::localtime(&t));
		snprintf(content_str, sizeof(content_str), "%s %s %s %s:%d\n", time_str,get_level(level),content,filename, line);

		WriteLog(content_str);
	}

	// single thread invoke WriteDown()
	void WriteDown()
	{
		std::unique_lock<std::mutex> full_lk(full_mtx_);
		while(full_buffer.empty())
		{
			full_condi.wait(full_lk);
		}
		
		LogBufferPtr ptr = std::move(full_buffer.back());
		full_buffer.pop_back();
		full_mtx_.unlock();
		
		// write to log file
		if(logfd>=0)
			write(logfd, ptr->GetBuff(), ptr->GetSize()); 
		
		std::unique_lock<std::mutex> empty_lk(empty_mtx_);
		empty_buffer_.push_back(std::move(ptr));
		empty_lk.unlock();
	}

private:
	
	const char* INFO_S = "INFO ";
	const char* WARN_S = "WARN ";
	const char* ERROR_S = "ERROR";

	const char* get_level(const int level) const
	{
		switch (level)
		{
			case LEVEL::INFO: return INFO_S;
			case LEVEL::WARN: return WARN_S;
			case LEVEL::ERROR: return ERROR_S;
		}
	}



	int WriteLog(const char* content)
	{
		std::unique_lock<std::mutex> lk(current_mtx_);
			if(current_buffer_->GetLeftSize()-strlen(content)<0)
				SwitchBuffer();
			if(current_buffer_->GetLeftSize()-strlen(content)<0)
				// this content is too long, skip
				return -1;
			else
			{
				current_buffer_->Append(content, strlen(content));
				return 0;
			}
	}

	int SwitchBuffer()
	{
		{
			bool full_queue_empty=false;
			std::unique_lock<std::mutex> full_lk(full_mtx_);
			full_queue_empty = full_buffer.empty();
			full_buffer.push_back(std::move(current_buffer_));
			full_lk.unlock();
			if(full_queue_empty) full_condi.notify_one();
		}

		{
			std::unique_lock<std::mutex> empty_lk(empty_mtx_);
			while(empty_buffer_.empty())
			{
				empty_condi.wait(empty_lk);
			}
			current_buffer_ = std::move(empty_buffer_.back());
			empty_buffer_.pop_back();
			empty_lk.unlock();
		}
		return 0;
	}

	std::mutex current_mtx_;
	std::condition_variable current_condi;
	LogBufferPtr current_buffer_;
	std::mutex empty_mtx_;
	std::condition_variable empty_condi;
	LogBufferVec empty_buffer_;
	std::mutex full_mtx_;
	std::condition_variable full_condi;
	LogBufferVec full_buffer;
	
	int logfd{-1};

	LogMgr()
	{
		logfd = open(ConfMgr::getInstance().get_log_path(),
				O_WRONLY|O_CREAT|O_APPEND);
		current_buffer_ = std::make_unique<LogBuffer>();
	}
};

#define LOGINFO(content) LogMgr::getInstance().LOG(LEVEL::INFO, content, __FILE__, __LINE__)
#define LOGWARN(content) LogMgr::getInstance().LOG(LEVEL::WARN, content, __FILE__, __LINE__)
#define LOGERROR(content) LogMgr::getInstance().LOG(LEVEL::ERROR, content, __FILE__, __LINE__)

#endif
