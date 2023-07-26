#pragma once

#include "include/definitions.h"
#include "include/exception.h"
#include "base/url.h"

namespace wckt::err
{
	class ErrorContextLayer;
	
	typedef std::unique_ptr<ErrorContextLayer> PTR_ErrorContextLayer;
	
	class ErrorContextLayer
	{
		private:
			PTR_ErrorContextLayer next;
		
		protected:
			ErrorContextLayer(PTR_ErrorContextLayer next);
		
		public:
			virtual ~ErrorContextLayer() = default;
			
			const ErrorContextLayer* getNext() const;
			virtual std::string what() const = 0;
	};
	
	class StandardError : public ErrorContextLayer
	{
		private:
			std::string message;
			
		public:
			StandardError(const std::string& message);
			~StandardError() override = default;
			
			std::string what() const override;
	};
	
	class ErrorWrapper : public std::exception
	{
		private:
			PTR_ErrorContextLayer top;
		
		public:
			ErrorWrapper(PTR_ErrorContextLayer top);
			~ErrorWrapper() = default;
			
			const ErrorContextLayer& getTop() const;
			PTR_ErrorContextLayer releaseTop();
			const char* what() const noexcept override;
	};
	
	#define _MAKE_STD_ERR(_Msg)			std::make_unique<wckt::err::StandardError>(std::string(_Msg))
	#define _MAKE_ERR(_Class, _Args)	std::make_unique<_Class>(_Args)
	
	struct TaskFailed : public std::exception
	{};
	
	class ErrorSentinel
	{
		public:
			enum behavior_t
			{
				COLLECT,
				THROW,
				IGNORE
			};
			
			typedef std::function<PTR_ErrorContextLayer(PTR_ErrorContextLayer)> errctx_fn_t;
			
			static errctx_fn_t NO_CONTEXT_FN;
			
		private:
			behavior_t behavior;
			std::vector<PTR_ErrorContextLayer> errors;
			errctx_fn_t contextFunction;
			
		public:
			ErrorSentinel(behavior_t behavior, const errctx_fn_t& contextFunction);
			~ErrorSentinel() = default;
			
			void raise(PTR_ErrorContextLayer error);
			
			template<typename _Ty, typename... _Args>
			void raise(_Args... args)
			{
				raise(_MAKE_ERR(_Ty, args));
			}
			
			template<auto __F_Action, typename... _Args>
			auto guard(_Args... args) -> decltype(__F_Action(args...))
			{
				try { return __F_Action(args); }
				catch(const APIException& e)
				{
					raise(contextFunction(_MAKE_STD_ERR(e.what())));
					throw TaskFailed();
				}
				catch(ErrorWrapper& e)
				{
					raise(contextFunction(e.releaseTop()));
					throw TaskFailed();
				}
			}
			
			template<auto __F_Action, typename... _Args>
			auto guardWeak(_Args... args) -> decltype(__F_Action(args...))
			{
				try { return __F_Action(args); }
				catch(ErrorWrapper& e)
				{
					raise(contextFunction(e.releaseTop()));
					throw TaskFailed();
				}
			}
			
			template<auto __F_Action, typename... _Args>
			void guardNoExcept(_Args... args)
			{
				try { __F_Action(args); }
				catch(const APIException& e) { raise(contextFunction(_MAKE_STD_ERR(e.what()))); }
				catch(ErrorWrapper& e) { raise(contextFunction(e.releaseTop())); }
			}
			
			template<auto __F_Action, typename... _Args>
			void guardWeakNoExcept(_Args... args)
			{
				try { __F_Action(args); }
				catch(ErrorWrapper& e) { raise(contextFunction(e.releaseTop())); }
			}
			
			
	};
};
