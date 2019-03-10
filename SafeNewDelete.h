#pragma once

#include <iostream>
#include <cassert>
#include <vector>

class SafeNewDelete
{
	struct STraceList
	{
		void* pointer{};
		bool is_deleted{ false };

		STraceList() {};
		STraceList(void* _pointer) : pointer(_pointer) {};
	};

public:
	SafeNewDelete() {};
	~SafeNewDelete()
	{
		std::cout << std::endl << "[LOG] Terminating the process ..." << std::endl;
		std::cout << std::endl;

		if (m_new_count != m_delete_count)
		{
			std::cout << "[LOG ERROR!] memory leak detected." << std::endl;
			std::cout << "             total new count    : " << m_new_count << std::endl;
			std::cout << "             total delete count : " << m_delete_count << std::endl;

			long long iterator_index{};
			for (auto& iterator : m_trace_list)
			{
				if (!iterator.is_deleted)
				{
					iterator.is_deleted = true;

					std::cout << "             memory leaked with : < LOG_NEW #" << iterator_index << " >" << std::endl;
				}

				++iterator_index;
			}
			std::cout << std::endl;
			
			abort();
		}
		else
		{
			std::cout << "[LOG] No memory leak detected." << std::endl;
			std::cout << "[LOG] Process terminated successfully." << std::endl;

			std::cin.get();
		}

	};

	template<typename T>
	void new_s(T** p_pointer) noexcept
	{
		try
		{
			*p_pointer = new T{};

			m_trace_list.push_back(STraceList(p_pointer));
		}
		catch (std::bad_alloc e)
		{
			std::cout << e.what() << std::endl;

			abort();
		}

		++m_new_count;
	}

	template<typename T>
	void new_s(T** p_pointer, T&& init_value) noexcept
	{
		try
		{
			*p_pointer = new T{ init_value };

			m_trace_list.push_back(STraceList(p_pointer));
		}
		catch (std::bad_alloc e)
		{
			std::cout << e.what() << std::endl;

			abort();
		}

		++m_new_count;
	}

	template<typename T>
	void delete_s(T** p_pointer) noexcept
	{
		if (*p_pointer)
		{
			delete *p_pointer;
			*p_pointer = nullptr;
			
			long long iterator_index{};
			for (auto& iterator : m_trace_list)
			{
				if (iterator.pointer == p_pointer)
				{
					iterator.is_deleted = true;
					std::cout << "   => SEE < LOG_NEW #" << iterator_index << " >" << std::endl;
					std::cout << std::endl;

					break;
				}
				++iterator_index;
			}

			++m_delete_count;
		}
		else
		{
			std::cout << std::endl << std::endl;
			std::cout << "[LOG ERROR!] you just attempted to delete a null pointer." << std::endl;
			std::cout << std::endl;

			abort();
		}
	}

	void log_new(long long nd_id, const std::type_info& type_id, const char* var_name, const char* function, const char* file, int line)
	{
		std::cout << "[LOG #" << m_log_id << "  < LOG_NEW #" << nd_id << " > ]" << std::endl;
		std::cout << "  -FILE (" << file << ")" << std::endl;
		std::cout << "  -LINE (" << line << ") - " << function << "()" << std::endl;
		std::cout << "  new (" << type_id.name() << ") " << var_name << ";" << std::endl;
		std::cout << std::endl;

		++m_log_id;
	}

	void log_delete(const std::type_info& type_id, const char* var_name, const char* function, const char* file, int line)
	{
		std::cout << "[LOG #" << m_log_id << "]" << std::endl;
		std::cout << "  -FILE (" << file << ")" << std::endl;
		std::cout << "  -LINE (" << line << ") - " << function << "()" << std::endl;
		std::cout << "  delete (" << type_id.name() << ") " << var_name << "; ";

		++m_log_id;
	}

	auto get_new_id() const noexcept { return m_new_count; };


private:
	long long m_log_id;

	long long m_new_count{};
	long long m_delete_count{};

	std::vector<STraceList> m_trace_list;
};

static SafeNewDelete snd;

#define new_s(pPointer) { snd.log_new(snd.get_new_id(), typeid(pPointer), #pPointer, __func__, __FILE__, __LINE__); snd.new_s<std::remove_pointer<decltype(pPointer)>::type>(&pPointer); }
#define new_init_s(pPointer, Value) { snd.log_new(snd.get_new_id(), typeid(pPointer), #pPointer, __func__, __FILE__, __LINE__); snd.new_s<std::remove_pointer<decltype(pPointer)>::type>(&pPointer, Value); }
#define delete_s(pPointer) { snd.log_delete(typeid(pPointer), #pPointer, __func__, __FILE__, __LINE__); snd.delete_s(&pPointer); }