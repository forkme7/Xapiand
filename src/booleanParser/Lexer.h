/*
 * Copyright (C):
 *  2014 furan,
 *  2016,2017 deipi.com LLC and contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#pragma once

#include "ContentReader.h"
#include "LexicalException.h"
#include "Token.h"


enum class LexerState : uint8_t {
	INIT,
	TOKEN,
	TOKEN_QUOTE,
	INIT_SQUARE_BRACKET,
	END_SQUARE_BRACKET,
	ESCAPE,
	SYMBOL_OP,
	EOFILE
};


class Lexer {
	ContentReader contentReader;
	Symbol currentSymbol;

	void InitDictionary();
	void IsStringOperator(Token& token) const;
	bool IsSymbolOp(char c) const;

public:
	explicit Lexer(char* input);
	Token NextToken();
};
