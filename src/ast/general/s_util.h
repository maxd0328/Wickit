#pragma once

#include "buildw/parser.h"

namespace wckt::ast
{
	class S_StaticSymbol : public build::ASTNode
	{
		public:
			S_StaticSymbol();
			sym::Locator getLocator() const;
			
			void parse(build::Parser& parser) override;
	};
	
	class S_Identifier : public build::ASTNode
	{
		public:
			enum special_t
			{
				OPERATOR_ADD			= 0x00,
				OPERATOR_SUB			= 0x01,
				OPERATOR_MUL			= 0x02,
				OPERATOR_DIV			= 0x03,
				OPERATOR_MOD			= 0x04,
				OPERATOR_AND			= 0x05,
				OPERATOR_OR				= 0x06,
				OPERATOR_XOR			= 0x07,
				OPERATOR_SHL			= 0x08,
				OPERATOR_SHR			= 0x09,
				OPERATOR_LOGICAL_NOT	= 0x0a,
				OPERATOR_BITWISE_NOT	= 0x0b,
				OPERATOR_POS			= 0x0c,
				OPERATOR_NEG			= 0x0d,
				OPERATOR_EQU			= 0x0e,
				OPERATOR_NEQ			= 0x0f,
				OPERATOR_GRT			= 0x10,
				OPERATOR_LST			= 0x11,
				OPERATOR_GTE			= 0x12,
				OPERATOR_LTE			= 0x13,
				OPERATOR_ADD_SELF		= 0x14,
				OPERATOR_SUB_SELF		= 0x15,
				OPERATOR_MUL_SELF		= 0x16,
				OPERATOR_DIV_SELF		= 0x17,
				OPERATOR_MOD_SELF		= 0x18,
				OPERATOR_AND_SELF		= 0x19,
				OPERATOR_OR_SELF		= 0x1a,
				OPERATOR_XOR_SELF		= 0x1b,
				OPERATOR_SHL_SELF		= 0x1c,
				OPERATOR_SHR_SELF		= 0x1d,
				OPERATOR_INC			= 0x1e,
				OPERATOR_DEC			= 0x1f,
				OPERATOR_RSUBSCRIPT		= 0x20,
				OPERATOR_LSUBSCRIPT		= 0x21,
				OPERATOR_RFUNCTOR		= 0x22,
				OPERATOR_LFUNCTOR		= 0x23,
				NO_NAME					= 0x24
			};
			
			static bool isIdentifier(build::Parser& parser);
		
		private:
			std::string value;
			special_t special;
		
		public:
			S_Identifier();
			std::string getValue() const;
			
			void parse(build::Parser& parser) override;
			std::string toString() const override;
	};
}
