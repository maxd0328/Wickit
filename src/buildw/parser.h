#pragma once

#include "include/definitions.h"
#include "buildw/build.h"
#include "buildw/tokenizer.h"
#include "error/error.h"

namespace wckt::build
{
	class ParseObject
	{
		public:
			virtual ~ParseObject() = default;
			
			virtual std::string toString() const = 0;
			virtual std::vector<const ParseObject*> getElements() const = 0;
			virtual std::vector<ParseObject*> getElements() = 0;
			
			std::string toTreeString(uint32_t tabSize = 3) const;
	};
	
	class DummyObject : public ParseObject
	{
		public:
			inline DummyObject() {}
			~DummyObject() override = default;
			
			inline std::string toString() const override
			{ return "DummyObject"; }
			inline std::vector<const ParseObject*> getElements() const override
			{ return {}; }
			inline std::vector<ParseObject*> getElements() override
			{ return {}; }
	};
	
	template<typename _Ty>
	class ContainerObject : public ParseObject
	{
		private:
			_Ty value;
			
		public:
			ContainerObject(const _Ty& value): value(value) {}
			ContainerObject(_Ty&& value): value(std::move(value)) {}
			~ContainerObject() override = default;
			
			const _Ty& get() const		{ return value; }
			_Ty& get()					{ return value; }
			
			inline std::string toString() const override
			{ return std::string("ContainerObject"); }
			inline std::vector<const ParseObject*> getElements() const override
			{ return {}; }
			inline std::vector<ParseObject*> getElements() override
			{ return {}; }
	};
    
	typedef std::unique_ptr<ParseObject>(*psem_action_t)(std::vector<std::unique_ptr<ParseObject>>&&);
	
	#define	UPTR(_Type)							std::unique_ptr<_Type>
	
	#define __PXELEM(_Index)					__xelem ## _Index ## __
	
	#define PNULL								nullptr
	#define PMAKE_UNIQUE(_Class)				std::make_unique<_Class>
	#define PMAKE_UNIQUE_OF(_Type, _Args...)	std::make_unique<_Type>(_Args)
	#define PSEM_ACTION(__Name)					UPTR(ParseObject) __Name(std::vector<UPTR(ParseObject)>&& __xelems__)
	#define PXELEM(_Index)						( std::move(__PXELEM(_Index)) )
	#define PMAKE_XELEM(_Type, _Index)			UPTR(_Type) __PXELEM(_Index) = UPTR(_Type)(static_cast<_Type*>(__xelems__[_Index].release()));
	
    typedef struct
    {
        uint32_t nterm;
        uint32_t length;
		psem_action_t action;
    } production_t;

    class TokenIterator
    {
        private:
            std::shared_ptr<std::vector<Token>> tokenSequence;
            size_t position;
			
			Token insertedToken;

        public:
            TokenIterator(std::shared_ptr<std::vector<Token>> tokenSequence, size_t position = 0);
            ~TokenIterator() = default;

            std::shared_ptr<std::vector<Token>> getTokenSequence() const;
            size_t getPosition() const;

            Token next();
            Token lookAhead() const;
            Token latest() const;
			
			void insert(const Token& token);
    };

    namespace services
    {
        void parse(build_info_t& buildInfo, err::ErrorSentinel* parentSentinel);
    }
}
