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
	
	#define BASIC_CONTEXT_LAYER(_Name, _Str)				\
		struct _Name : public wckt::err::ErrorContextLayer	\
		{													\
			_Name(wckt::err::PTR_ErrorContextLayer next)	\
			: ErrorContextLayer(std::move(next))			\
			{}												\
			std::string what() const override				\
			{ return _Str + this->getNext()->what(); }		\
		};
	
	#define USE_BASIC_CONTEXT_LAYER(_Name)					\
		[](wckt::err::PTR_ErrorContextLayer ptr)			\
		{ return _MAKE_ERR(_Name, std::move(ptr)); }		
	
	class StandardError : public ErrorContextLayer
	{
		private:
			std::string message;
			
		public:
			StandardError(const std::string& message);
			~StandardError() override = default;
			
			std::string what() const override;
	};
	
	class ErrorPackage : public ErrorContextLayer
	{
		private:
			std::vector<PTR_ErrorContextLayer> errors;
			
			ErrorPackage(std::vector<PTR_ErrorContextLayer>& errors);
			
		public:
			~ErrorPackage() override = default;
			
			const std::vector<PTR_ErrorContextLayer>& getErrors() const;
			std::string what() const override;
			
			friend class ErrorSentinel;
	};
	
	class WickitError : public std::exception
	{
		private:
			PTR_ErrorContextLayer top;
		
		public:
			WickitError(PTR_ErrorContextLayer top);
			~WickitError() = default;
			
			const ErrorContextLayer& getTop() const;
			PTR_ErrorContextLayer releaseTop();
			const char* what() const noexcept override;
	};
	
	#define _MAKE_STD_ERR(_Msg)			std::make_unique<wckt::err::StandardError>(std::string(_Msg))
	#define _MAKE_ERR(_Class, _Args...)	std::make_unique<_Class>(_Args)
	
	class GuardCounter
	{
		private:
			uint32_t& ctr;
		public:
			GuardCounter(uint32_t& ctr);
			~GuardCounter();
	};
	
	/**
	 * Error Sentinel Rules:
	 * 
	 * When subordinate sentinel is in THROW mode:
	 *  - Guard lifetime of subordinate
	 * When subordinate sentinel is in COLLECT mode:
	 *  - Guard lifetime of subordinate if no supervisor reference is given
	 *  - Otherwise, do not guard if current sentinel is in THROW mode
	*/
	class ErrorSentinel
	{
		public:
			enum behavior_t
			{
				COLLECT,
				THROW,
				IGNORE
			};
			
			struct no_except : public APIError
			{
				private:
					no_except();
			};
			
			typedef std::function<PTR_ErrorContextLayer(PTR_ErrorContextLayer)> errctx_fn_t;
			
			static errctx_fn_t NO_CONTEXT_FN;
			
		private:
			ErrorSentinel* prev;
			behavior_t behavior;
			std::vector<PTR_ErrorContextLayer> errors;
			errctx_fn_t contextFunction;
			
			uint32_t guardctr;
			
		public:
			ErrorSentinel(behavior_t behavior, const errctx_fn_t& contextFunction);
			ErrorSentinel(ErrorSentinel* prev, behavior_t behavior, const errctx_fn_t& contextFunction);
			~ErrorSentinel();
			
			ErrorSentinel* getPrev() const;
			behavior_t getBehavior() const;
			const std::vector<PTR_ErrorContextLayer>& getErrors() const;
			errctx_fn_t getContextFunction() const;
			
			bool hasErrors() const;
			
			void raise(PTR_ErrorContextLayer error);
			void raise(const APIError& error);
			
			template<typename _Ty, typename... _Args>
			void raise(_Args... args)
			{
				raise(_MAKE_ERR(_Ty, args...));
			}
			
			template<typename _Exc = APIError>
			bool guard(const std::function<void(ErrorSentinel&)>& fn,
				const std::function<void(const _Exc&)>& onExc = nullptr)
			{
				try
				{
					{ GuardCounter __counter(this->guardctr);
					  fn(*this); }
					return true;
				}
				catch(const _Exc& e)
				{
					if(onExc)
						onExc(e);
					raise(e);
					return false;
				}
				catch(WickitError& e)
				{
					raise(e.releaseTop());
					return false;
				}
			}
			
			void forward(ErrorSentinel* sentinel = nullptr);
			void clear();
			
			void flush(std::ostream& out = std::cout);
			void flushAndPreserve(std::ostream& out = std::cout) const;
	};
};
