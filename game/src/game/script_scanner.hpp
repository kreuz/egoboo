//********************************************************************************************
//*
//*    This file is part of Egoboo.
//*
//*    Egoboo is free software: you can redistribute it and/or modify it
//*    under the terms of the GNU General Public License as published by
//*    the Free Software Foundation, either version 3 of the License, or
//*    (at your option) any later version.
//*
//*    Egoboo is distributed in the hope that it will be useful, but
//*    WITHOUT ANY WARRANTY; without even the implied warranty of
//*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//*    General Public License for more details.
//*
//*    You should have received a copy of the GNU General Public License
//*    along with Egoboo.  If not, see <http://www.gnu.org/licenses/>.
//*
//********************************************************************************************
#pragma once

#include "game/egoboo.h"

#define MAX_OPCODE 1024 ///< Number of lines in AICODES.TXT

/// @brief A token.
struct Token {
	enum class Type {
		Unknown = '?',
		Function = 'F',
		Variable = 'V',
		Constant = 'C',
		Operator = 'O',
	};

private:
	/// @brief The type of this token.
	Type _type;

	/// @brief The line number.
    /// @todo Use Id::Location.
	size_t _line;

	/// @brief The index of this token.
    int _index;

	/// @brief The value of this token.
    int _value;

    /// @brief The text of this token.
    std::string _text;

public:
	/// @brief Get the index of this token.
	/// @return the index of this token
    /// @see setIndex
	int getIndex() const {
		return _index;
	}

	/// @brief Set the index of this token.
	/// @param index the index
	/// @see getIndex
	void setIndex(int index) {
		_index = index;
	}

	/// @brief Get the value of this token.
	/// @return the value of this token
    /// @see setValue
	int getValue() const {
		return _value;
	}

	/// @brief Set the value of this token.
	/// @param value the value
	/// @see getValue
	void setValue(int value) {
		_value = value;
	}

	/// @brief Get the line number of this token.
    /// @return the line number of this token
    /// @see setLine
    /// @remark The line number is the line number of the line in which the lexeme of this token starts in.
	size_t getLine() const {
		return _line;
	}

	/// @brief Set the line number of this token.
	/// @param line the line number
	/// @see getLine
	void setLine(size_t line) {
		_line = line;
	}

	/// @brief Get the type of this token.
	/// @return the type of this token
    /// @see setType
	Type getType() const {
		return _type;
	}

	/// @brief Set the type of this token.
	/// @param type the type
    /// @see getType
	void setType(Type type) {
		_type = type;
	}

	/// @brief Set the text of this token.
	/// @param text the text
    /// @see getText
	void setText(const std::string& text) {
		_text = text;
	}

    /// @brief Get the text of this token.
    /// @return the text of this token
    /// @see setText
    const std::string& getText() const {
        return _text;
    }

	/// @brief Construct this token with default values.
	Token();

	/// @brief Construct this token with values of another token.
	/// @param other the other token
	Token(const Token& other);

	/// @brief Destruct this token.
	~Token();

	friend std::ostream& operator<<(std::ostream& os, const Token& token);
};

/// @brief Overloaded &lt;&lt; operator for a token type.
/// @param ostream the output stream to write to
/// @param tokenType the token type to write
/// @return the outputstream
std::ostream& operator<<(std::ostream& os, const Token::Type& tokenType);

/// @brief Overloaded &lt;&lt; operator for a token.
/// @param ostream the output stream to write to
/// @param token the token to write
/// @return the output stream
std::ostream& operator<<(std::ostream& os, const Token& token);
