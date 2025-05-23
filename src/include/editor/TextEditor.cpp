#include <algorithm>
#include <chrono>
#include <string>
#include <regex>
#include <cmath>

#include "TextEditor.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h" // for imGui::GetCurrentWindow()

#define FOR_VEC(a, vec) for(auto a = (vec).begin(); a < (vec).end(); a++)
int clamp(int a, int b, int c)
{
	if (a <= b) return b;
	if (a >= c) return c;
	return a;
}

// TODO
// - multiline comments vs single-line: latter is blocking start of a ML

template<class InputIt1, class InputIt2, class BinaryPredicate>
bool equals(InputIt1 first1, InputIt1 last1,
	InputIt2 first2, InputIt2 last2, BinaryPredicate p)
{
	for (; first1 != last1 && first2 != last2; ++first1, ++first2)
	{
		if (!p(*first1, *first2))
			return false;
	}
	return first1 == last1 && first2 == last2;
}

TextEditor::TextEditor()
	: mLineSpacing(1.0f)
	, mUndoIndex(0)
	, mTabSize(4)
	, mOverwrite(false)
	, mReadOnly(false)
	, mWithinRender(false)
	, mScrollToCursor(false)
	, mScrollToTop(false)
	, mTextChanged(false)
	, mColorizerEnabled(true)
	, mTextStart(20.0f)
	, mLeftMargin(10)
	, mCursorPositionChanged(false)
	, mColorRangeMin(0)
	, mColorRangeMax(0)
	, mSelectionMode(SelectionMode::Normal)
	, mCheckComments(true)
	, mLastClick(-1.0f)
	, mHandleKeyboardInputs(true)
	, mHandleMouseInputs(true)
	, mIgnoreImGuiChild(false)
	, mShowWhitespaces(true)
	, mStartTime(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
{
	SetPalette(GetDarkPalette());
	SetLanguageDefinition(LanguageDefinition::HLSL());
	mLines.push_back(Line());
}

TextEditor::~TextEditor()
{
}

void TextEditor::SetLanguageDefinition(const LanguageDefinition & aLanguageDef)
{
	mLanguageDefinition = aLanguageDef;
	mRegexList.clear();

	for (auto& r : mLanguageDefinition.mTokenRegexStrings)
		mRegexList.push_back(std::make_pair(std::regex(r.first, std::regex_constants::optimize), r.second));

	Colorize();
}

void TextEditor::SetPalette(const Palette & aValue)
{
	mPaletteBase = aValue;
}

own_std::string TextEditor::GetText2(const Coordinates & aStart, const Coordinates & aEnd) const
{
	own_std::string result;

	auto lstart = aStart.mLine;
	auto lend = aEnd.mLine;
	auto istart = aStart.mColumn;
	auto iend = aEnd.mColumn;
	size_t s = 0;

	for (size_t i = lstart; i < lend; i++)
		s += mLines[i].size();

	result.reserve(s + s / 8);

	while (istart <= iend || lstart < lend)
	{
		if (lstart >= (int)mLines.size())
			break;

		auto& line = mLines[lstart];
		if (istart < (int)line.size())
		{
			result += line[istart].mChar;
			istart++;
		}
		else
		{
			istart = 0;
			++lstart;
			result += '\n';
		}
	}

	return result;
}
own_std::string TextEditor::GetText(const Coordinates & aStart, const Coordinates & aEnd) const
{
	own_std::string result;

	auto lstart = aStart.mLine;
	auto lend = aEnd.mLine;
	auto istart = GetCharacterIndex(aStart);
	auto iend = GetCharacterIndex(aEnd);
	size_t s = 0;

	for (size_t i = lstart; i < lend; i++)
		s += mLines[i].size();

	result.reserve(s + s / 8);

	while (istart < iend || lstart < lend)
	{
		if (lstart >= (int)mLines.size())
			break;

		auto& line = mLines[lstart];
		if (istart < (int)line.size())
		{
			result += line[istart].mChar;
			istart++;
		}
		else
		{
			istart = 0;
			++lstart;
			result += '\n';
		}
	}

	return result;
}

TextEditor::Coordinates TextEditor::GetActualCursorCoordinates() const
{
	return SanitizeCoordinates(mState.mCursorPosition);
}

TextEditor::Coordinates TextEditor::SanitizeCoordinates(const Coordinates & aValue) const
{
	auto line = aValue.mLine;
	auto column = aValue.mColumn;
	if (line >= (int)mLines.size())
	{
		if (mLines.empty())
		{
			line = 0;
			column = 0;
		}
		else
		{
			line = (int)mLines.size() - 1;
			column = GetLineMaxColumn(line);
		}
		return Coordinates(line, column);
	}
	else
	{
		column = mLines.empty() ? 0 : std::min(column, GetLineMaxColumn(line));
		return Coordinates(line, column);
	}
}

// https://en.wikipedia.org/wiki/UTF-8
// We assume that the char is a standalone character (<128) or a leading byte of an UTF-8 code sequence (non-10xxxxxx code)
static int UTF8CharLength(TextEditor::Char c)
{
	if ((c & 0xFE) == 0xFC)
		return 6;
	if ((c & 0xFC) == 0xF8)
		return 5;
	if ((c & 0xF8) == 0xF0)
		return 4;
	else if ((c & 0xF0) == 0xE0)
		return 3;
	else if ((c & 0xE0) == 0xC0)
		return 2;
	return 1;
}

// "Borrowed" from ImGui source
static inline int ImTextCharToUtf8(char* buf, int buf_size, unsigned int c)
{
	if (c < 0x80)
	{
		buf[0] = (char)c;
		return 1;
	}
	if (c < 0x800)
	{
		if (buf_size < 2) return 0;
		buf[0] = (char)(0xc0 + (c >> 6));
		buf[1] = (char)(0x80 + (c & 0x3f));
		return 2;
	}
	if (c >= 0xdc00 && c < 0xe000)
	{
		return 0;
	}
	if (c >= 0xd800 && c < 0xdc00)
	{
		if (buf_size < 4) return 0;
		buf[0] = (char)(0xf0 + (c >> 18));
		buf[1] = (char)(0x80 + ((c >> 12) & 0x3f));
		buf[2] = (char)(0x80 + ((c >> 6) & 0x3f));
		buf[3] = (char)(0x80 + ((c) & 0x3f));
		return 4;
	}
	//else if (c < 0x10000)
	{
		if (buf_size < 3) return 0;
		buf[0] = (char)(0xe0 + (c >> 12));
		buf[1] = (char)(0x80 + ((c >> 6) & 0x3f));
		buf[2] = (char)(0x80 + ((c) & 0x3f));
		return 3;
	}
}

void TextEditor::GoBackOne(Coordinates & aCoordinates) const
{
	if (aCoordinates.mLine < (int)mLines.size())
	{
		auto& line = mLines[aCoordinates.mLine];
		auto cindex = GetCharacterIndex(aCoordinates);

		if (cindex - 1 > 0)
		{
			auto delta = UTF8CharLength(line[cindex].mChar);
			cindex = std::min(cindex - delta, 0);
		}
		else
		{
			--aCoordinates.mLine;
			cindex = GetLineMaxColumn(aCoordinates.mLine);
		}
		aCoordinates.mColumn = GetCharacterColumn(aCoordinates.mLine, cindex);
	}
}
void TextEditor::Advance(Coordinates & aCoordinates) const
{
	if (aCoordinates.mLine < (int)mLines.size())
	{
		auto& line = mLines[aCoordinates.mLine];
		auto cindex = GetCharacterIndex(aCoordinates);

		if (cindex + 1 < (int)line.size())
		{
			auto delta = UTF8CharLength(line[cindex].mChar);
			cindex = std::min(cindex + delta, (int)line.size() - 1);
		}
		else
		{
			++aCoordinates.mLine;
			cindex = 0;
		}
		aCoordinates.mColumn = GetCharacterColumn(aCoordinates.mLine, cindex);
	}
}

void TextEditor::DeleteRange2(const Coordinates & aStart, Coordinates & aEnd)
{
	assert(aEnd >= aStart);
	assert(!mReadOnly);

	//printf("D(%d.%d)-(%d.%d)\n", aStart.mLine, aStart.mColumn, aEnd.mLine, aEnd.mColumn);

	if (aEnd == aStart)
		return;

	auto start = aStart.mColumn;
	auto end = aEnd.mColumn;

	UndoRecord u;
	u.mBefore = mState;

	
	if (aEnd.mColumn > mLines[aEnd.mLine].size())
	{
		aEnd.mColumn = 0;
		aEnd.mLine++;
		u.mRemoved = GetText2(aStart, aEnd);
		aEnd.mLine--;
	}
	else
		u.mRemoved = GetText2(aStart, aEnd);

	u.mRemovedStart = aStart;
	u.mRemovedStart.mColumn = GetCharacterColumn(aStart.mLine, aStart.mColumn);
	u.mRemovedEnd = aEnd;
	u.mRemovedEnd.mColumn = GetCharacterColumn(aEnd.mLine, aEnd.mColumn);

	//u.mRemovedEnd.mColumn = GetC
	AddUndo(u);

	if (aStart.mLine == aEnd.mLine)
	{
		auto& line = mLines[aStart.mLine];
		auto n = GetLineCharacterCount(aStart.mLine);
		if (aEnd.mColumn > n)
		{
			line.erase(line.begin() + start, line.end());
			RemoveLine(aStart.mLine);
		}
		else
			line.erase(line.begin() + start, line.begin() + end + 1);

	}
	else
	{
		auto& firstLine = mLines[aStart.mLine];
		auto& lastLine = mLines[aEnd.mLine];

		firstLine.erase(firstLine.begin() + start, firstLine.end());
		lastLine.erase(lastLine.begin(), lastLine.begin() + end);

		if (aStart.mLine < aEnd.mLine)
			firstLine.insert(firstLine.end(), lastLine.begin(), lastLine.end());

		if (aStart.mLine < aEnd.mLine)
			RemoveLine(aStart.mLine + 1, aEnd.mLine + 1);
	}

	mTextChanged = true;
}
void TextEditor::DeleteRange(const Coordinates & aStart, const Coordinates & aEnd, bool makeUndo)
{
	assert(aEnd >= aStart);
	assert(!mReadOnly);

	//printf("D(%d.%d)-(%d.%d)\n", aStart.mLine, aStart.mColumn, aEnd.mLine, aEnd.mColumn);

	if (aEnd == aStart)
		return;

	auto start = GetCharacterIndex(aStart);
	auto end = GetCharacterIndex(aEnd);


	if (aStart.mLine == aEnd.mLine)
	{
		auto& line = mLines[aStart.mLine];
		auto n = GetLineMaxColumn(aStart.mLine);
		if (aEnd.mColumn > n)
		{
			line.erase(line.begin() + start, line.end());
			RemoveLine(aStart.mLine);
		}
		else
			line.erase(line.begin() + start, line.begin() + end);

	}
	else
	{
		auto& firstLine = mLines[aStart.mLine];
		auto& lastLine = mLines[aEnd.mLine];

		firstLine.erase(firstLine.begin() + start, firstLine.end());
		lastLine.erase(lastLine.begin(), lastLine.begin() + end);

		if (aStart.mLine < aEnd.mLine)
			firstLine.insert(firstLine.end(), lastLine.begin(), lastLine.end());

		if (aStart.mLine < aEnd.mLine)
			RemoveLine(aStart.mLine + 1, aEnd.mLine + 1);
	}

	mTextChanged = true;
}

int TextEditor::InsertTextAt(Coordinates& /* inout */ aWhere, const char * aValue)
{
	assert(!mReadOnly);

	int cindex = GetCharacterIndex(aWhere);
	int totalLines = 0;
	while (*aValue != '\0')
	{
		assert(!mLines.empty());

		if (*aValue == '\r')
		{
			// skip
			++aValue;
		}
		else if (*aValue == '\n')
		{
			if (cindex < (int)mLines[aWhere.mLine].size())
			{
				auto& newLine = InsertLine(aWhere.mLine + 1);
				auto& line = mLines[aWhere.mLine];
				newLine.insert(newLine.begin(), line.begin() + cindex, line.end());
				line.erase(line.begin() + cindex, line.end());
			}
			else
			{
				InsertLine(aWhere.mLine + 1);
			}
			++aWhere.mLine;
			aWhere.mColumn = 0;
			cindex = 0;
			++totalLines;
			++aValue;
		}
		else
		{
			auto& line = mLines[aWhere.mLine];
			auto d = UTF8CharLength(*aValue);
			while (d-- > 0 && *aValue != '\0')
				line.insert(line.begin() + cindex++, Glyph(*aValue++, PaletteIndex::Default));
			++aWhere.mColumn;
		}

		mTextChanged = true;
	}

	return totalLines;
}

void TextEditor::AddUndo(UndoRecord& aValue)
{
	assert(!mReadOnly);
	//printf("AddUndo: (@%d.%d) +\'%s' [%d.%d .. %d.%d], -\'%s', [%d.%d .. %d.%d] (@%d.%d)\n",
	//	aValue.mBefore.mCursorPosition.mLine, aValue.mBefore.mCursorPosition.mColumn,
	//	aValue.mAdded.c_str(), aValue.mAddedStart.mLine, aValue.mAddedStart.mColumn, aValue.mAddedEnd.mLine, aValue.mAddedEnd.mColumn,
	//	aValue.mRemoved.c_str(), aValue.mRemovedStart.mLine, aValue.mRemovedStart.mColumn, aValue.mRemovedEnd.mLine, aValue.mRemovedEnd.mColumn,
	//	aValue.mAfter.mCursorPosition.mLine, aValue.mAfter.mCursorPosition.mColumn
	//	);

	mUndoBuffer.resize((size_t)(mUndoIndex + 1));
	mUndoBuffer.back() = aValue;
	++mUndoIndex;
}

TextEditor::Coordinates TextEditor::ScreenPosToCoordinates(const ImVec2& aPosition) const
{
	ImVec2 origin = ImGui::GetCursorScreenPos();
	ImVec2 local(aPosition.x - origin.x, aPosition.y - origin.y);

	int lineNo = std::max(0, (int)floor(local.y / mCharAdvance.y));

	int columnCoord = 0;

	if (lineNo >= 0 && lineNo < (int)mLines.size())
	{
		auto& line = mLines.at(lineNo);

		int columnIndex = 0;
		float columnX = 0.0f;

		while ((size_t)columnIndex < line.size())
		{
			float columnWidth = 0.0f;

			if (line[columnIndex].mChar == '\t')
			{
				float spaceSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, " ").x;
				float oldX = columnX;
				float newColumnX = (1.0f + std::floor((1.0f + columnX) / (float(mTabSize) * spaceSize))) * (float(mTabSize) * spaceSize);
				columnWidth = newColumnX - oldX;
				if (mTextStart + columnX + columnWidth * 0.5f > local.x)
					break;
				columnX = newColumnX;
				columnCoord = (columnCoord / mTabSize) * mTabSize + mTabSize;
				columnIndex++;
			}
			else
			{
				char buf[7];
				auto d = UTF8CharLength(line[columnIndex].mChar);
				int i = 0;
				while (i < 6 && d-- > 0)
					buf[i++] = line[columnIndex++].mChar;
				buf[i] = '\0';
				columnWidth = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, buf).x;
				if (mTextStart + columnX + columnWidth * 0.5f > local.x)
					break;
				columnX += columnWidth;
				columnCoord++;
			}
		}
	}

	return SanitizeCoordinates(Coordinates(lineNo, columnCoord));
}

TextEditor::Coordinates TextEditor::FindWordEnd2(const Coordinates & aFrom) const
{
	Coordinates at = aFrom;
	if (at.mLine >= (int)mLines.size())
		return at;

	auto& line = mLines[at.mLine];
	auto cindex = GetCharacterIndex(at);


	if (cindex >= (int)line.size())
	{
		cindex = (int)line.size() - 1;
	}

	int tkn_idx=0;
	FOR_VEC(t, line_tokens)
	{
		bool less = cindex <= t->line_offset;
		auto next = t + 1;
		bool is_word = t->type == T_WORD;
		int sz = 0;
		if (is_word)
			sz = t->str.size();
		if (!is_word && less || is_word && 
								cindex >= (t->line_offset) &&
								cindex <= (t->line_offset + sz))
			break;

		tkn_idx++;
	}
	
	tkn_idx = tkn_idx % line_tokens.size();
	auto t = &line_tokens[tkn_idx];
	/*
	if (t->type == T_WORD && cindex == t->line_offset)
	{
		tkn_idx = (tkn_idx + 1) % line_tokens.size();
		t = &line_tokens[tkn_idx];
	}
	*/
	cindex = t->line_offset;
	if (t->type == T_WORD)
		cindex += t->str.size();
	
	return Coordinates(at.mLine, GetCharacterColumn(at.mLine, cindex));
}
TextEditor::Coordinates TextEditor::FindWordStart2(const Coordinates & aFrom) const
{
	Coordinates at = aFrom;
	if (at.mLine >= (int)mLines.size())
		return at;

	auto& line = mLines[at.mLine];
	auto cindex = GetCharacterIndex(at);

	if (line.size() == 0)
		return at;

	if (cindex >= (int)line.size())
	{
		cindex = (int)line.size() - 1;
	}

	int tkn_idx=0;
	FOR_VEC(t, line_tokens)
	{
		bool less = cindex <= t->line_offset;
		auto next = t + 1;
		bool is_word = t->type == T_WORD;
		int sz = 0;
		if (is_word)
			sz = t->str.size();
		if (!is_word && less || is_word && 
								cindex >= (t->line_offset) &&
								cindex <= (t->line_offset + sz))
			break;

		tkn_idx++;
	}
	tkn_idx = tkn_idx % line_tokens.size();
	
	cindex = line_tokens[tkn_idx].line_offset;
	
	return Coordinates(at.mLine, GetCharacterColumn(at.mLine, cindex));
}
TextEditor::Coordinates TextEditor::FindPrevWordStart(const Coordinates & aFrom) const
{
	Coordinates at = aFrom;
	if (at.mLine >= (int)mLines.size())
		return at;

	auto& line = mLines[at.mLine];
	auto cindex = GetCharacterIndex(at);


	if (cindex >= (int)line.size())
	{
		cindex = (int)line.size() - 1;
	}

	int tkn_idx=0;
	FOR_VEC(t, line_tokens)
	{
		if (cindex <= t->line_offset)
			break;

		tkn_idx++;
	}
	tkn_idx = clamp(tkn_idx - 1, 0, line_tokens.size() - 2);
	
	if (tkn_idx < 0)
	{
		at.mLine--;
		at.mLine = clamp(at.mLine - 1, 0, mLines.size() - 1);
		cindex = GetLineMaxColumn(at.mLine) - 1;
	}
	else
		cindex = line_tokens[tkn_idx].line_offset;
	
	return Coordinates(at.mLine, GetCharacterColumn(at.mLine, cindex));
}
TextEditor::Coordinates TextEditor::FindWordStart(const Coordinates & aFrom) const
{
	Coordinates at = aFrom;
	if (at.mLine >= (int)mLines.size())
		return at;

	auto& line = mLines[at.mLine];
	auto cindex = GetCharacterIndex(at);

	if (cindex >= (int)line.size())
		return at;

	while (cindex > 0 && isspace(line[cindex].mChar))
		--cindex;

	auto cstart = (PaletteIndex)line[cindex].mColorIndex;
	while (cindex > 0)
	{
		auto c = line[cindex].mChar;
		if ((c & 0xC0) != 0x80)	// not UTF code sequence 10xxxxxx
		{
			if (c <= 32 && isspace(c))
			{
				cindex++;
				break;
			}
			if (cstart != (PaletteIndex)line[size_t(cindex - 1)].mColorIndex)
				break;
		}
		--cindex;
	}
	return Coordinates(at.mLine, GetCharacterColumn(at.mLine, cindex));
}

TextEditor::Coordinates TextEditor::FindWordEnd(const Coordinates & aFrom) const
{
	Coordinates at = aFrom;
	if (at.mLine >= (int)mLines.size())
		return at;

	auto& line = mLines[at.mLine];
	auto cindex = GetCharacterIndex(at);

	if (line.size() == 0)
		return at;

	if (cindex >= (int)line.size())
	{
		cindex = (int)line.size() - 1;
	}

	int tkn_idx=0;
	FOR_VEC(t, line_tokens)
	{
		bool less = cindex <= t->line_offset;
		bool is_word = t->type == T_WORD;
		int sz = 0;
		if (is_word)
			sz = t->str.size();
		if (!is_word && less || is_word && 
								cindex >= (t->line_offset) &&
								cindex <= (t->line_offset + sz))
			break;

		tkn_idx++;
	}
	tkn_idx = tkn_idx % line_tokens.size();

	auto t = &line_tokens[tkn_idx];
	
	cindex = line_tokens[tkn_idx].line_offset;

	if (t->type == T_WORD)
		cindex += t->str.size();
	
	return Coordinates(at.mLine, GetCharacterColumn(at.mLine, cindex));
}

TextEditor::Coordinates TextEditor::FindNextWord(const Coordinates & aFrom) const
{
	Coordinates at = aFrom;
	if (at.mLine >= (int)mLines.size())
		return at;

	// skip to the next non-word character
	auto cindex = GetCharacterIndex(aFrom);
	bool isword = false;
	bool skip = false;
	if (cindex < (int)mLines[at.mLine].size())
	{
		auto& line = mLines[at.mLine];
		isword = isalnum(line[cindex].mChar);
		skip = isword;
	}
	if (!isword)
	{
		auto &line = mLines[at.mLine];
		do
		{
			cindex++;
		} while (cindex < line.size() && line[cindex].mChar == ' ');
	}

	while (!isword || skip)
	{
		if (at.mLine >= mLines.size())
		{
			auto l = std::max(0, (int) mLines.size() - 1);
			return Coordinates(l, GetLineMaxColumn(l));
		}
		auto &line = mLines[at.mLine];
		while (cindex < line.size() && line[cindex].mChar == '\t')
		{
			cindex++;
		}

		if (cindex < (int)line.size())
		{
			isword = isalnum(line[cindex].mChar);
			isword = isword || line[cindex].mChar == '_';

			if (isword && !skip)
				return Coordinates(at.mLine, GetCharacterColumn(at.mLine, cindex));

			if (!isword)
			{
				skip = false;
				at.mColumn = GetCharacterColumn(aFrom.mLine, cindex);
				break;
			}


			cindex++;
		}
		else
		{
			cindex = 0;
			++at.mLine;
			skip = false;
			isword = false;
		}
	}

	return at;
}

int TextEditor::GetCharacterIndex2(const Coordinates& aCoordinates) const
{
	if (aCoordinates.mLine >= mLines.size())
		return -1;
	auto& line = mLines[aCoordinates.mLine];
	int c = 0;
	int i = 0;
	for (; i < line.size() && c <= aCoordinates.mColumn;)
	{
		if (line[i].mChar == '\t')
		{
			c = (c / mTabSize) * mTabSize + mTabSize;
		}
		else
			++c;
		i += UTF8CharLength(line[i].mChar);
	}
	return i;
}
int TextEditor::GetCharacterIndex(const Coordinates& aCoordinates) const
{
	if (aCoordinates.mLine >= mLines.size())
		return -1;
	auto& line = mLines[aCoordinates.mLine];
	int c = 0;
	int i = 0;
	for (; i < line.size() && c < aCoordinates.mColumn;)
	{
		if (line[i].mChar == '\t')
			c = (c / mTabSize) * mTabSize + mTabSize;
		else
			++c;
		i += UTF8CharLength(line[i].mChar);
	}
	return i;
}

int TextEditor::GetCharacterColumn(int aLine, int aIndex) const
{
	if (aLine >= mLines.size())
		return 0;
	auto& line = mLines[aLine];
	int col = 0;
	int i = 0;
	while (i < aIndex && i < (int)line.size())
	{
		auto c = line[i].mChar;
		i += UTF8CharLength(c);
		if (c == '\t')
			col = (col / mTabSize) * mTabSize + mTabSize;
		else
			col++;
	}
	return col;
}

int TextEditor::GetLineCharacterCount(int aLine) const
{
	if (aLine >= mLines.size())
		return 0;
	auto& line = mLines[aLine];
	int c = 0;
	for (unsigned i = 0; i < line.size(); c++)
		i += UTF8CharLength(line[i].mChar);
	return c;
}

int TextEditor::GetLineMaxColumn(int aLine) const
{
	if (aLine >= mLines.size())
		return 0;
	auto& line = mLines[aLine];
	int col = 0;
	for (unsigned i = 0; i < line.size(); )
	{
		auto c = line[i].mChar;
		if (c == '\t')
			col = (col / mTabSize) * mTabSize + mTabSize;
		else
			col++;
		i += UTF8CharLength(c);
	}
	return col;
}

bool TextEditor::IsOnWordBoundary(const Coordinates & aAt) const
{
	if (aAt.mLine >= (int)mLines.size() || aAt.mColumn == 0)
		return true;

	auto& line = mLines[aAt.mLine];
	auto cindex = GetCharacterIndex(aAt);
	if (cindex >= (int)line.size())
		return true;

	if (mColorizerEnabled)
		return line[cindex].mColorIndex != line[size_t(cindex - 1)].mColorIndex;

	return isspace(line[cindex].mChar) != isspace(line[cindex - 1].mChar);
}

void TextEditor::RemoveLine(int aStart, int aEnd)
{
	assert(!mReadOnly);
	assert(aEnd >= aStart);
	assert(mLines.size() > (size_t)(aEnd - aStart));

	ErrorMarkers etmp;
	for (auto& i : mErrorMarkers)
	{
		ErrorMarkers::value_type e(i.first >= aStart ? i.first - 1 : i.first, i.second);
		if (e.first >= aStart && e.first <= aEnd)
			continue;
		etmp.insert(e);
	}
	mErrorMarkers = std::move(etmp);

	Breakpoints btmp;
	for (auto i : mBreakpoints)
	{
		if (i >= aStart && i <= aEnd)
			continue;
		btmp.insert(i >= aStart ? i - 1 : i);
	}
	mBreakpoints = std::move(btmp);

	mLines.erase(mLines.begin() + aStart, mLines.begin() + aEnd);
	assert(!mLines.empty());

	mTextChanged = true;
}

void TextEditor::RemoveLine(int aIndex)
{
	assert(!mReadOnly);
	assert(mLines.size() > 1);

	ErrorMarkers etmp;
	for (auto& i : mErrorMarkers)
	{
		ErrorMarkers::value_type e(i.first > aIndex ? i.first - 1 : i.first, i.second);
		if (e.first - 1 == aIndex)
			continue;
		etmp.insert(e);
	}
	mErrorMarkers = std::move(etmp);

	Breakpoints btmp;
	for (auto i : mBreakpoints)
	{
		if (i == aIndex)
			continue;
		btmp.insert(i >= aIndex ? i - 1 : i);
	}
	mBreakpoints = std::move(btmp);

	FOR_VEC(m, gotoMarks)
	{
		if(m->coor.mLine >= aIndex)
			m->coor.mLine--;
	}

	mLines.erase(mLines.begin() + aIndex);
	assert(!mLines.empty());

	mTextChanged = true;
}

TextEditor::Line& TextEditor::InsertLine(int aIndex)
{
	assert(!mReadOnly);

	auto& result = *mLines.insert(mLines.begin() + aIndex, Line());

	ErrorMarkers etmp;
	for (auto& i : mErrorMarkers)
		etmp.insert(ErrorMarkers::value_type(i.first >= aIndex ? i.first + 1 : i.first, i.second));
	mErrorMarkers = std::move(etmp);

	Breakpoints btmp;
	for (auto i : mBreakpoints)
		btmp.insert(i >= aIndex ? i + 1 : i);
	mBreakpoints = std::move(btmp);

	FOR_VEC(m, gotoMarks)
	{
		if(m->coor.mLine >= aIndex)
			m->coor.mLine++;
	}

	return result;
}

own_std::string TextEditor::GetWordUnderCursor() const
{
	auto c = GetCursorPosition();
	return GetWordAt(c);
}

own_std::string TextEditor::GetWordAt(const Coordinates & aCoords) const
{
	auto start = FindWordStart2(aCoords);
	auto end = FindWordEnd2(aCoords);

	own_std::string r;

	auto istart = GetCharacterIndex(start);
	auto iend = GetCharacterIndex(end);

	for (auto it = istart; it < iend; ++it)
		r.push_back(mLines[aCoords.mLine][it].mChar);

	return r;
}

ImU32 TextEditor::GetGlyphColor(const Glyph & aGlyph) const
{
	return aGlyph.color;
	if (!mColorizerEnabled)
		return mPalette[(int)PaletteIndex::Default];
	if (aGlyph.mComment)
		return mPalette[(int)PaletteIndex::Comment];
	if (aGlyph.mMultiLineComment)
		return mPalette[(int)PaletteIndex::MultiLineComment];
	auto const color = mPalette[(int)aGlyph.mColorIndex];
	if (aGlyph.mPreprocessor)
	{
		const auto ppcolor = mPalette[(int)PaletteIndex::Preprocessor];
		const int c0 = ((ppcolor & 0xff) + (color & 0xff)) / 2;
		const int c1 = (((ppcolor >> 8) & 0xff) + ((color >> 8) & 0xff)) / 2;
		const int c2 = (((ppcolor >> 16) & 0xff) + ((color >> 16) & 0xff)) / 2;
		const int c3 = (((ppcolor >> 24) & 0xff) + ((color >> 24) & 0xff)) / 2;
		return ImU32(c0 | (c1 << 8) | (c2 << 16) | (c3 << 24));
	}
	return color;
}

VIM_mode_enum TextEditor::GetVimMode()
{
	return mVimMode;
}
void TextEditor::BasicMovs(int movAmount)
{
	bool bNotEmpty = insertBuffer.size() > 0;
	ImGuiIO& io = ImGui::GetIO();
	auto shift = io.KeyShift;
	auto ctrl = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
	auto alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;
	bool isCapF = bNotEmpty && insertBuffer[0] == 'F';
	bool isF = bNotEmpty && insertBuffer[0] == 'f';
	
	
	bool moved = false;
	if(mVimMode == VI_CHANGE || mVimMode == VI_VISUAL || mVimMode == VI_YANK)
	{
		if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGuiKey_LeftBracket))
		{
			if (bNotEmpty && insertBuffer.back() == 'i')
			{
				SelectInnerLevel('{', mState.mCursorPosition);
				mState.mCursorPosition = mState.mSelectionEnd;
				mInteractiveStart = mState.mSelectionStart;
				mInteractiveEnd = mState.mSelectionEnd;
				insertBuffer.clear();
				return;
			}
		}
		else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_W))
		{
			if (bNotEmpty && insertBuffer.back() == 'i')
			{
				Coordinates start = FindPrevWordStart(mState.mCursorPosition);
				SelectWordUnderCursor();
				mInteractiveStart = mState.mSelectionStart;
				mInteractiveEnd = mState.mSelectionEnd;
				mState.mCursorPosition = mState.mSelectionEnd;
				insertBuffer.clear();
				return;
			}
			else
			{
				Coordinates start = FindWordStart(mState.mCursorPosition);
				if(mVimMode != VI_VISUAL && mVimMode != VI_LINE_VISUAL)
					mInteractiveStart = mState.mCursorPosition;
				mInteractiveEnd = FindWordEnd2(mState.mCursorPosition);
				mState.mCursorPosition = mInteractiveEnd;
				insertBuffer.clear();
				return;
			}
		}
		else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGuiKey_LeftBracket))
		{
			if (bNotEmpty && insertBuffer.back() == 'i')
			{
				SelectInnerLevel('{', mState.mCursorPosition);
				mInteractiveStart = mState.mSelectionStart;
				mInteractiveEnd = mState.mSelectionEnd;
				mState.mCursorPosition = mState.mSelectionEnd;
			}
			else
			{

			}
		}
	}
	else if (mVimMode == VI_NORMAL)
	{
		if(isQuotes)
		{
			if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_H))
				insertBuffer += 'h';
			else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_A))
				insertBuffer += 'a';
			else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_O))
				insertBuffer += 'o';
			return;
		}
	}

	if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_L))
	{
		MoveRight(movAmount, shift, ctrl);
		moved = true;
	}
	else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_H))
	{
		MoveLeft(movAmount, shift, ctrl);
		moved = true;
	}
	else if(isF && !io.InputQueueCharacters.empty())
	{
		char ch = io.InputQueueCharacters[0];
		int column = GetCharacterIndex(mState.mCursorPosition);
		int cline = mState.mCursorPosition.mLine;
		auto &line = mLines[cline];
		int max = line.size();
		for(int i = column + 1; i < max; i++)
		{
			if(line[i].mChar == ch)
			{
				int ch_column = GetCharacterColumn(cline, i);
				SetCursorPosition(Coordinates(cline, ch_column));
				break;
			}
		}
		mInteractiveEnd = mState.mCursorPosition;
		insertBuffer.clear();
	}
	else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_F))
	{
		if (!bNotEmpty)
		{
			insertBuffer += 'f';
		}
	}
	else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGuiKey_F))
	{
		if (!bNotEmpty)
		{
			insertBuffer += 'F';
		}
	}
	else if(isCapF && !io.InputQueueCharacters.empty())
	{
		char ch = io.InputQueueCharacters[0];
		int column = GetCharacterIndex(mState.mCursorPosition);
		int cline = mState.mCursorPosition.mLine;
		auto &line = mLines[cline];
		int max = line.size();
		for(int i = column -1; i > 0; i--)
		{
			if(line[i].mChar == ch)
			{
				int ch_column = GetCharacterColumn(cline, i);
				SetCursorPosition(Coordinates(cline, ch_column));
				break;
			}
		}
		mInteractiveEnd = mState.mCursorPosition;
		insertBuffer.clear();
	}
	else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_K))
	{
		MoveUp(movAmount);
		moved = true;
	}
	else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_J))
	{
		MoveDown(movAmount);
		moved = true;
	}
	else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_RightBracket))
	{
		if(insertBuffer == "]")
		{
			gotoFuncSrcLine = 1;
			insertBuffer.clear();
		}
		else
			insertBuffer += ']';
	}
	else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_I))
	{
		if (bNotEmpty)
		{
			insertBuffer += 'i';
		}
	}
	else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_LeftBracket))
	{
		if(insertBuffer == "[")
		{
			gotoFuncSrcLine = -1;
			insertBuffer.clear();
		}
		else
			insertBuffer += '[';
	}
	else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGuiKey_RightBracket))
	{
		if(bNotEmpty && insertBuffer[0] == ']')
		{
			MoveToCharAtSamelevel('}');
		}
		else
		{
			int dst_line = mState.mCursorPosition.mLine + 1;
			for (; dst_line < mLines.size(); dst_line++)
			{
				if (IsLineOnlyTab(dst_line) || GetLineCharacterCount(dst_line) == 0)
					break;
			}
			SetCursorPosition(Coordinates(dst_line, 0));
		}
	}
	else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGuiKey_LeftBracket))
	{
		if(bNotEmpty && insertBuffer[0] == '[')
		{
			MoveToCharAtSamelevel('{');
		}
		else
		{
			int dst_line = mState.mCursorPosition.mLine - 1;
			for (; dst_line >= 0; dst_line--)
			{
				if (IsLineOnlyTab(dst_line) || GetLineCharacterCount(dst_line) == 0)
					break;
			}
			SetCursorPosition(Coordinates(dst_line, 0));
		}
	}
	else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_B))
	{
		Coordinates ncoord = FindPrevWordStart(mState.mCursorPosition);
		SetCursorPosition(ncoord);
	}
	else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_W))
	{
		Coordinates ncoord = FindNextWord(mState.mCursorPosition);
		SetCursorPosition(ncoord);
	}
	else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_E))
	{
		Coordinates ncoord = FindWordEnd2(mState.mCursorPosition);
		ncoord.mColumn--;
		ncoord.mColumn = clamp(ncoord.mColumn, 0, mLines[mState.mCursorPosition.mLine].size() - 1);
		SetCursorPosition(ncoord);
	}
	else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Minus))
	{
		Coordinates pos = mState.mCursorPosition;
		pos.mColumn = 0;
		pos = FindNextWord(pos);
		SetCursorPosition(pos);
	}
	else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGuiKey_4))
	{
		MoveEnd();
	}
	if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_X) && !isCapF && !isF)
	{
		Delete();
		mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
		mVimMode = VI_NORMAL;
		mSelectionMode = SelectionMode::Normal;
		SetSelection(mState.mCursorPosition, mState.mCursorPosition, mSelectionMode);
	}
	else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_I))
	{
		if(mVimMode == VI_VISUAL || mVimMode == VI_LINE_VISUAL || mVimMode == VI_CHANGE)
			insertBuffer += 'i';
	}
	else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_W))
	{
		if (insertBuffer == "i")
		{
			Coordinates start = FindWordStart(mState.mCursorPosition);
			SelectWordUnderCursor();
			mState.mCursorPosition = mState.mSelectionEnd;
		}
	}
	for (int i = 1; i <= 9; i++)
	{
		if (!ctrl && !shift && !alt && ImGui::IsKeyPressed((ImGuiKey)(ImGuiKey_0 + i)))
		{
			insertBuffer += '0' + i;
		}
	}
	if(moved)
	{
		insertBuffer.clear();
	}
}


void TextEditor::InsertLineAddIndent(int line, int indentOfLine)
{
	//int line = mState.mCursorPosition.mLine + 1;
	InsertLine(line);
	char buffer[64];
	int indent = GetLineIndent(indentOfLine);
	// 62 because below we're doing adding a new line character at the end
	// for undo
	assert(indent < 62);
	memset(buffer, '\t', indent);
	buffer[indent] = 0;
	Coordinates coor = Coordinates(line, 0);
	InsertTextAt(coor, (const char *)buffer);
	int col = GetCharacterColumn(line, indent);

	UndoRecord u;
	buffer[indent] = '\n';
	buffer[indent+1] = '\0';
	u.mAdded = buffer;
	u.mAddedStart = coor;
	u.mAddedEnd = Coordinates(line, col + 1);
	u.mBefore.mCursorPosition = mState.mCursorPosition;

	AddUndo(u);
	SetCursorPosition(Coordinates(line, col));
	//mLines[line] += buffer;
	mVimMode = VI_INSERT;
}

void GlobalClearSearchStringHighlight(void* data);
bool GlobalIsCurCmdBuffer(void *);
void RenderFuncDef(void* data, float screen_x, float screen_y);
void RenderIntellisenseSuggestions(void* data, float screen_x, float screen_y);
void GotoPrevBuffer(void* data);
bool OnIntellisenseSuggestions(void* data);
void AcceptIntellisenseSeggestion(void* data);
void ClearIntellisenseSeggestion(void* data);
void MoveSelectedIllisenseSuggestions(void* data, int add, bool absolute);
void GlobalChangeToCmdBuffer(void *);
void GlobalExitCmdBuffer(void *, bool restoreOriginolPos = false);
void GlobalGetFileName(void *, own_std::string *);
void GlobalGetCurBufferFileLines(void *, own_std::string *);
void WriteFileLang(char* name, void* data, int size);
void SaveFile(void*, char* contents, int size, own_std::string *file_name);

void GetMarkPressKey(char &ch, bool &pressed)
{
	ImGuiIO& io = ImGui::GetIO();
	auto shift = io.KeyShift;
	auto ctrl = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
	auto alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;
	pressed = false;
	if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_A))
	{
		pressed = true;
		ch = 'a';
	}
	else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_O))
	{
		pressed = true;
		ch = 'o';
	}
	else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_E))
	{
		pressed = true;
		ch = 'e';
	}
}

int min(int a, int b)
{
	if (a < b)
		return a;
	return b;
}
void TextEditor::RemoveLineComplete(YankBuffer *yb)
{
	MoveHome();
	mState.mSelectionStart = mState.mCursorPosition;
	//mState.mSelectionStart.mColumn = -1;

	mState.mSelectionEnd.mLine = mState.mCursorPosition.mLine;
	mState.mSelectionEnd.mColumn = 0;
	mState.mSelectionEnd.mLine++;
	int line = mState.mSelectionEnd.mLine;
	UndoRecord u;
	u.mRemovedStart = mState.mSelectionStart;
	u.mRemovedEnd = mState.mSelectionEnd;
	if (line >= mLines.size())
	{
		mState.mCursorPosition.mLine = min(line, mLines.size() - 1);
		mState.mSelectionEnd.mLine = mState.mCursorPosition.mLine;
		mState.mSelectionEnd.mColumn = GetLineMaxColumn(mState.mCursorPosition.mLine)+1;
		u.mBefore = mState;
		u.mRemovedStart.mLine--;
		u.mRemovedEnd.mLine--;
	}
	else
	{
		u.mBefore = mState;
		u.mRemovedStart = mState.mSelectionStart;
		u.mRemovedEnd = mState.mSelectionEnd;
	}
	yb->str = GetCurrentLineText();
	yb->str += '\n';
	u.mRemoved = yb->str;
	yb->str.insert(0, "\n");
	AddUndo(u);
	DeleteSelection();
}
void TextEditor::HandleKeyboardInputs()
{

	ImGuiIO& io = ImGui::GetIO();
	auto shift = io.KeyShift;
	auto ctrl = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
	auto alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;

	bool bNotEmpty = insertBuffer.size() > 0;
	bool isD = bNotEmpty && insertBuffer[0] == 'd';
	bool isG = bNotEmpty && insertBuffer[0] == 'g';
	bool isM = bNotEmpty && insertBuffer[0] == 'm';
	bool isCapF = bNotEmpty && insertBuffer[0] == 'F';
	bool isF = bNotEmpty && insertBuffer[0] == 'f';
	bool isApostrophe = bNotEmpty && insertBuffer[0] == '\'';
	isQuotes = bNotEmpty && insertBuffer[0] == '\"' && insertBuffer.size() < 2;
	firstChInInsertBufferIsSlash = bNotEmpty && insertBuffer[0] == '/';;

	bool isCmdBuffer = GlobalIsCurCmdBuffer(data);
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && (!firstChInInsertBufferIsSlash && !isCmdBuffer || haveKeyboardFocusAnyway || isCmdBuffer))
	{
		if (ImGui::IsWindowHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
		//ImGui::CaptureKeyboardFromApp(true);

		char ch = 0;
		if (insertBuffer.size() > 1 && insertBuffer[0] == '\"')
			ch = insertBuffer[1];
		YankBuffer *yb = GetYankBuffer(ch);

		io.WantCaptureKeyboard = true;
		io.WantTextInput = true;

		if (isCmdBuffer)
		{
			if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Enter))
			{
				GlobalExitCmdBuffer(data);
				MoveTop();
				own_std::string cmd = GetCurrentLineText();
				if (!firstChInInsertBufferIsSlash)
				{
					if (cmd == "w")
					{
						own_std::string fname;
						GlobalGetFileName(data, &fname);
						own_std::string file;
						GlobalGetCurBufferFileLines(data, &file);

						SaveFile(data, (char*)file.c_str(), file.size(), &fname);
						ClearLines();
					}
					// clear selected search
					else if (cmd == "cs")
					{
						GlobalClearSearchStringHighlight(data);
						ClearLines();
						GlobalExitCmdBuffer(data);
					}
				}
				return;
			}
			else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Semicolon))
			{
				GlobalExitCmdBuffer(data, true);
				return;
			}
		}

		int movAmount = 1;
		if (insertBuffer.size() > 0 && insertBuffer[0] >= '0' && insertBuffer[0] <= '9')
		{
			movAmount = atoi(insertBuffer.c_str());
		}
		if (isM)
		{
			bool pressed = false;
			char ch = 0;
			GetMarkPressKey(ch, pressed);
			if (pressed)
			{
				GotoMark* mark = nullptr;
				FOR_VEC(m, gotoMarks)
				{
					if (m->ch == ch)
					{
						mark = &*m;
						break;
					}
				}
				if (mark)
				{
					mark->coor = mState.mCursorPosition;
				}
				else
				{
					GotoMark m;
					m.ch = ch;
					m.coor = mState.mCursorPosition;
					gotoMarks.emplace_back(m);
				}
				insertBuffer.clear();
				return;
			}
		}
		else if (isApostrophe)
		{
			bool pressed = false;
			char ch = 0;
			GetMarkPressKey(ch, pressed);
			if (pressed)
			{
				GotoMark* mark = nullptr;
				FOR_VEC(m, gotoMarks)
				{
					if (m->ch == ch)
					{
						mark = &*m;
						break;
					}
				}
				if (mark)
				{
					SetCursorPosition(mark->coor);
				}
				insertBuffer.clear();
				return;
			}
		}
		switch (mVimMode)
		{
		case VI_CHANGE:
		{
			BasicMovs(movAmount);
			if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Escape))
			{
				mVimMode = VI_NORMAL;
				mInteractiveEnd = mInteractiveStart;
				SetSelection(mInteractiveStart, mInteractiveStart);
				return;
			}
			if(mState.mCursorPosition != mState.mSelectionStart)
			{
				mState.mSelectionStart = mInteractiveStart;
				mState.mSelectionEnd = mInteractiveEnd;
				mInteractiveEnd = mState.mCursorPosition;
				//SetSelection(mInteractiveStart, mState.mCursorPosition, mSelectionMode);
				Delete();
				mVimMode = VI_INSERT;
				return;
			}
		}break;
		case VI_YANK:
		{
			BasicMovs(movAmount);
			bool yanked = false;
			if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Escape))
			{
				yanked = true;
			}
			if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_I))
			{

				insertBuffer += 'i';
			}
			if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Y))
			{
				yb->str = GetCurrentLineText();
				yb->str.insert(0, "\n");
				yb->str += '\n';
				mVimMode = VI_NORMAL;
				mInteractiveEnd = mInteractiveStart;
				SetSelection(mInteractiveStart, mInteractiveStart);
				yanked = true;

			}
			if(mState.mCursorPosition != mState.mSelectionStart && !yanked)
			{
				yb->str = GetSelectedText();
				yanked = true;
			}

			if(yanked)
			{
				insertBuffer.clear();
				mInteractiveEnd = mState.mCursorPosition;
				SetSelection(mInteractiveStart, mInteractiveStart);
				mVimMode = VI_NORMAL;
				return;
			}
		}break;
		case VI_NORMAL:
		{
			BasicMovs(movAmount);
			if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_I))
			{
				mVimMode = VI_INSERT;
			}
			else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Apostrophe))
			{
				if (isApostrophe)
					insertBuffer.clear();
				else
					insertBuffer += '\'';
			}
			else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_P) && !isQuotes)
			{
				bool pasteInNewLine = false;
				if (yb->str[0] == '\n')
				{
					mState.mCursorPosition.mLine++;
					pasteInNewLine = true;
				}
				else
				{
					MoveRight(1);
				}
				Paste(yb->str);

				if(pasteInNewLine)
					mState.mCursorPosition.mLine--;
				else
				{
					MoveRight(yb->str.size());
				}
			}
			else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGuiKey_P) && !isQuotes)
			{
				bool pasteInNewLine = false;
				if(yb->str[0] == '\n')
				{
					//mState.mCursorPosition.mLine++;
					mState.mCursorPosition.mColumn = 0;
					pasteInNewLine = true;
				}
				Paste(yb->str);
				if (pasteInNewLine)
					mState.mCursorPosition.mLine--;
			}
			else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Y) && !isQuotes)
			{
				mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
				mSelectionMode = SelectionMode::Normal;
				SetSelection(mState.mCursorPosition, mState.mCursorPosition, mSelectionMode);
				mVimMode = VI_YANK;
			}
			else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGuiKey_C))
			{
				mState.mSelectionStart = mState.mCursorPosition;
				MoveEnd();
				mState.mSelectionEnd = mState.mCursorPosition;
				Delete();
				mVimMode = VI_INSERT;
			}
			else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Apostrophe))
			{
				if (isQuotes)
					insertBuffer.clear();
				else
					insertBuffer += '\"';
			}
			else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Slash))
			{
				insertBuffer += '/';
				//mVimMode = VI_CMD;
				if (!isCmdBuffer)
				{
					originalCPosBeforeSearchString = mState.mCursorPosition;
					GlobalChangeToCmdBuffer(data);
				}
			}
			else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGuiKey_I))
			{
				Coordinates start = mState.mCursorPosition;
				start.mColumn = 0;
				start = FindNextWord(start);
				//start.mColumn = GetCharacterColumn(start.mLine, start.mColumn);

				mState.mCursorPosition = start;
				

				mInteractiveEnd = mInteractiveStart;
				SetSelection(mInteractiveStart, mInteractiveStart);
				mVimMode = VI_INSERT;
			}
			else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGuiKey_S))
			{
				Coordinates start = mState.mCursorPosition;
				start.mColumn = 0;
				start = FindNextWord(start);
				start.mColumn = GetCharacterIndex(start);

				Coordinates end;
				end.mColumn = GetLineCharacterCount(mState.mCursorPosition.mLine) - 1;
				end.mLine = mState.mCursorPosition.mLine;
				DeleteRange2(start, end);
				//DeleteRange(pos, mState.mCursorPosition);
				mInteractiveEnd = mInteractiveStart;
				SetSelection(mInteractiveStart, mInteractiveStart);
				mVimMode = VI_INSERT;
			}
			else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGuiKey_N))
			{
				if (matchedStrings.size() > 0)
				{
					int i = 0;

					FOR_VEC(matched, matchedStrings)
					{
						if (matched->pos >= mState.mCursorPosition)
							break;
						i++;
					}
					i--;
					if(i < 0)
						i = matchedStrings.size() - 1;
					else
						i = i % matchedStrings.size();
					SetCursorPosition(matchedStrings[i].pos);
				}

			}
			else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_N))
			{
				if (matchedStrings.size() > 0)
				{
					int i = 0;

					FOR_VEC(matched, matchedStrings)
					{
						if (matched->pos > mState.mCursorPosition)
							break;
						i++;
					}
					i = i % matchedStrings.size();
					SetCursorPosition(matchedStrings[i].pos);
				}

			}
			else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_A))
			{
				MoveRight(1);
				mVimMode = VI_INSERT;
			}
			else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGuiKey_A))
			{
				MoveEnd();
				mVimMode = VI_INSERT;
			}
			else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_M))
			{
				insertBuffer = 'm';
			}
			if (!isD && !ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_V))
			{
				mVimMode = VI_VISUAL;
				mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
				mSelectionMode = SelectionMode::Normal;
				SetSelection(mState.mCursorPosition, mState.mCursorPosition, mSelectionMode);
			}
			else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Semicolon) && !isF && !isCapF)
			{
				//mVimMode = VI_CMD;
				if (!isCmdBuffer)
					GlobalChangeToCmdBuffer(data);
			}
			else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_W))
			{
				if (insertBuffer == "ci")
				{
					Coordinates start = FindWordStart(mState.mCursorPosition);
					SelectWordUnderCursor();
					Delete();
					insertBuffer.clear();
					SetCursorPosition(start);
				}
				else if (bNotEmpty)
				{
					insertBuffer += 'w';
				}
			}
			else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_C))
			{
				mVimMode = VI_CHANGE;
				mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
				mSelectionMode = SelectionMode::Normal;
				SetSelection(mState.mCursorPosition, mState.mCursorPosition, mSelectionMode);
			}
			else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGuiKey_8))
			{
				int l, col = 0;
				SearchStringRange(GetWordUnderCursor(), &l, &col);
				//GotoPrevBuffer(data);
			}
			else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_6))
			{
				GotoPrevBuffer(data);
			}
			else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_U))
				Undo();
			else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGuiKey_V))
			{
				mVimMode = VI_LINE_VISUAL;
				mInteractiveStart.mColumn = 0;
				mInteractiveStart.mLine = mState.mCursorPosition.mLine;
				mInteractiveEnd.mColumn = GetLineMaxColumn(mState.mCursorPosition.mLine);
				mInteractiveEnd.mLine = mState.mCursorPosition.mLine;
				mStartLineVisual = mState.mCursorPosition;
				SetSelection(mInteractiveStart, mInteractiveEnd, mSelectionMode);

			}
			else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGuiKey_G))
			{
				MoveBottom();
			}
			else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_G))
			{
				if (isG)
				{
					MoveTop();
					insertBuffer.clear();
				}
				else
				{
					if(bNotEmpty && IsNumber(insertBuffer[0]))
					{
						int clamped = clamp(movAmount - 1, 0, mLines.size() - 1);
						SetCursorPosition(Coordinates(clamped, 0));
						insertBuffer.clear();
					}
					else
						insertBuffer += 'g';
				}
				//InsertLineAddIndent(mState.mCursorPosition.mLine, mState.mCursorPosition.mLine + 1);
			}
			else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGuiKey_O))
			{
				InsertLineAddIndent(mState.mCursorPosition.mLine, mState.mCursorPosition.mLine + 1);
			}
			else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_O))
			{
				InsertLineAddIndent(mState.mCursorPosition.mLine + 1, mState.mCursorPosition.mLine);
			}
			else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_R))
				Redo();
			else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_0))
			{
				if (insertBuffer.size() > 0)
				{
					insertBuffer += '0';
				}
			}
			else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_D))
			{
				if (isD)
				{
					RemoveLineComplete(yb);
					insertBuffer.clear();
				}
				else
					insertBuffer += 'd';
			}
			else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGuiKey_D))
			{
				//Coordinates end = mState.mCursorPosition;
				//end.mColumn = mLines[mState.mCursorPosition.mLine].size();
				RemoveLineComplete(yb);
			}
			else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Escape))
			{
				insertBuffer.clear();

			}
		}break;
		case VI_LINE_VISUAL:
		{
			BasicMovs(movAmount);

			if (mState.mCursorPosition.mLine < mStartLineVisual.mLine)
			{
				mInteractiveStart = mState.mCursorPosition;
				mInteractiveStart.mColumn = 0;
				mInteractiveEnd = mStartLineVisual;
				mInteractiveEnd.mColumn = GetLineMaxColumn(mStartLineVisual.mLine);
			}
			else if (mState.mCursorPosition.mLine > mStartLineVisual.mLine)
			{
				mInteractiveStart = mStartLineVisual;
				mInteractiveStart.mColumn = 0;
				mInteractiveEnd = mState.mCursorPosition;
				mInteractiveEnd.mColumn = GetLineMaxColumn(mState.mCursorPosition.mLine);
			}
			else
			{
				mInteractiveStart = mStartLineVisual;
				mInteractiveStart.mColumn = 0;
				mInteractiveEnd = mStartLineVisual;
				mInteractiveEnd.mColumn = GetLineMaxColumn(mState.mCursorPosition.mLine);
			}

			if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Y) && !isQuotes)
			{
				yb->str = GetSelectedText();
				yb->str.insert(0, "\n");
				yb->str += '\n';
				mVimMode = VI_NORMAL;
				mInteractiveEnd = mInteractiveStart;
				SetSelection(mInteractiveStart, mInteractiveStart);

			}

			SetSelection(mInteractiveStart, mInteractiveEnd, mSelectionMode);

			FromVisualToNormalMode(yb);
		}break;
		case VI_VISUAL:
		{
			BasicMovs(movAmount);

			if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Y) && !isQuotes)
			{
				mState.mSelectionStart.mColumn = GetCharacterIndex(mState.mSelectionStart);
				mState.mSelectionEnd.mColumn = GetCharacterIndex(mState.mSelectionEnd);
				yb->str = GetSelectedText2();
				mVimMode = VI_NORMAL;
				mInteractiveEnd = mInteractiveStart;
				SetSelection(mInteractiveStart, mInteractiveStart);
				return;

			}
			mInteractiveEnd = mState.mCursorPosition;
			SetSelection(mInteractiveStart, mState.mCursorPosition, mSelectionMode);

			FromVisualToNormalMode(yb);
			/*
			printf("start_x %d, start_y %d, end_x %d, end_y %d\n",
								mState.mSelectionStart.mColumn,
								mState.mSelectionStart.mLine,
								mState.mSelectionEnd.mColumn,
								mState.mSelectionEnd.mLine);
			else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_U))
				Undo();
			else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_R))
				Redo();
				*/
		}break;
		case VI_INSERT:
		{
			if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Escape))
			{
				mVimMode = VI_NORMAL;
				MoveLeft(1);
				ClearIntellisenseSeggestion(data);
				if (isCmdBuffer)
				{
					GlobalExitCmdBuffer(data, true);
				}
			}
			else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGuiKey_Tab))
				EnterCharacter('\t', shift);
			else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Delete))
			{
				Delete();
			}
			else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Backspace))
			{
				Backspace();
			}
			else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Enter))
			{
				if (OnIntellisenseSuggestions(data))
				{
					AcceptIntellisenseSeggestion(data);
				}
				else
				{
					EnterCharacter('\n', false);
				}
			}
			else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_UpArrow))
			{
				if (OnIntellisenseSuggestions(data))
				{
					MoveSelectedIllisenseSuggestions(data, 1, false);
				}
			}
			else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_DownArrow))
			{
				if (OnIntellisenseSuggestions(data))
				{
					MoveSelectedIllisenseSuggestions(data, -1, false);
				}
			}
			else if (!IsReadOnly() && !io.InputQueueCharacters.empty())
			{
				for (int i = 0; i < io.InputQueueCharacters.Size; i++)
				{
					auto c = io.InputQueueCharacters[i];
					if (c != 0 && (c == '\n' || c >= 32))
					{
						EnterCharacter(c, shift);
					}

					lastInsertedChar = c;
				}
				io.InputQueueCharacters.resize(0);
			}
		}break;
		}
		if(mVimMode != VI_INSERT)
		{
			if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Comma))
			{
				Coordinates prev_cur = mState.mCursorPosition;
				mState.mCursorPosition.mColumn = 0;
				if (HasSelection())
				{
					int start_line = mState.mSelectionStart.mLine;
					int lines = mState.mSelectionEnd.mLine - mState.mSelectionStart.mLine;
					lines++;
					for (int i = 0; i < lines; i++)
					{
						mState.mCursorPosition.mLine = i + start_line;
						auto& line = mLines[i + start_line];
						if (line.size() == 0)
							continue;
						for (int j = 0; j < movAmount; j++)
						{
							if (line[0].mChar == '\t')
							{
								line.erase(line.begin());
							}
							else
								break;
						}
					}
				}
				else
				{
					auto& line = mLines[mState.mCursorPosition.mLine];
					for (int j = 0; j < movAmount; j++)
					{
						if (line[0].mChar == '\t')
						{
							line.erase(line.begin());
						}
						else
							break;
					}
				}
				mState.mCursorPosition = prev_cur;
			}
			else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Period))
			{
				char buffer[64];
				memset(buffer, '\t', movAmount);
				buffer[movAmount] = 0;

				if (HasSelection())
				{
					int start_line = mState.mSelectionStart.mLine;
					int lines = mState.mSelectionEnd.mLine - mState.mSelectionStart.mLine;
					lines++;
					for (int i = 0; i < lines; i++)
					{
						InsertTextAt(Coordinates(start_line + i, 0), buffer);
					}
				}
				else
				{
					InsertTextAt(Coordinates(mState.mCursorPosition.mLine, 0), buffer);
				}
			}
			else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_S) && !isCapF && !isF)
			{
				Coordinates start = mState.mSelectionStart;

				Coordinates end = mState.mSelectionEnd;

				if(start == end)
				{
					Delete();

				}
				else
				{

					start.mColumn = GetCharacterIndex(start);
					end.mColumn = GetCharacterIndex(end);
					end.mLine = end.mLine;
					DeleteRange2(start, end);
					mState.mCursorPosition = mState.mSelectionStart;
				}
				mVimMode = VI_INSERT;

				mInteractiveEnd = mInteractiveStart;
				SetSelection(mInteractiveStart, mInteractiveStart);
			}
		}
	}
}

void TextEditor::HandleMouseInputs()
{
	ImGuiIO& io = ImGui::GetIO();
	auto shift = io.KeyShift;
	auto ctrl = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
	auto alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;

	if (ImGui::IsWindowHovered())
	{
		if (!shift && !alt)
		{
			auto click = ImGui::IsMouseClicked(0);
			auto doubleClick = ImGui::IsMouseDoubleClicked(0);
			auto t = ImGui::GetTime();
			auto tripleClick = click && !doubleClick && (mLastClick != -1.0f && (t - mLastClick) < io.MouseDoubleClickTime);

			/*
			Left mouse button triple click
			*/

			if (tripleClick)
			{
				if (!ctrl)
				{
					mState.mCursorPosition = mInteractiveStart = mInteractiveEnd = ScreenPosToCoordinates(ImGui::GetMousePos());
					mSelectionMode = SelectionMode::Line;
					SetSelection(mInteractiveStart, mInteractiveEnd, mSelectionMode);
				}

				mLastClick = -1.0f;
			}

			/*
			Left mouse button double click
			*/

			else if (doubleClick)
			{
				if (!ctrl)
				{
					mState.mCursorPosition = mInteractiveStart = mInteractiveEnd = ScreenPosToCoordinates(ImGui::GetMousePos());
					if (mSelectionMode == SelectionMode::Line)
						mSelectionMode = SelectionMode::Normal;
					else
						mSelectionMode = SelectionMode::Word;
					SetSelection(mInteractiveStart, mInteractiveEnd, mSelectionMode);
				}

				mLastClick = (float)ImGui::GetTime();
			}

			/*
			Left mouse button click
			*/
			else if (click)
			{
				mState.mCursorPosition = mInteractiveStart = mInteractiveEnd = ScreenPosToCoordinates(ImGui::GetMousePos());
				if (ctrl)
					mSelectionMode = SelectionMode::Word;
				else
					mSelectionMode = SelectionMode::Normal;
				SetSelection(mInteractiveStart, mInteractiveEnd, mSelectionMode);

				mLastClick = (float)ImGui::GetTime();
			}
			// Mouse left button dragging (=> update selection)
			else if (ImGui::IsMouseDragging(0) && ImGui::IsMouseDown(0))
			{
				io.WantCaptureMouse = true;
				mState.mCursorPosition = mInteractiveEnd = ScreenPosToCoordinates(ImGui::GetMousePos());
				SetSelection(mInteractiveStart, mInteractiveEnd, mSelectionMode);
			}
		}
	}
}

void TextEditor::Render()
{
	/* Compute mCharAdvance regarding to scaled font size (Ctrl + mouse wheel)*/
	const float fontSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, "#", nullptr, nullptr).x;
	mCharAdvance = ImVec2(fontSize, ImGui::GetTextLineHeightWithSpacing() * mLineSpacing);

	/* Update palette with the current alpha from style */
	for (int i = 0; i < (int)PaletteIndex::Max; ++i)
	{
		auto color = ImGui::ColorConvertU32ToFloat4(mPaletteBase[i]);
		color.w *= ImGui::GetStyle().Alpha;
		mPalette[i] = ImGui::ColorConvertFloat4ToU32(color);
	}

	assert(mLineBuffer.empty());

	auto contentSize = ImGui::GetWindowContentRegionMax();
	auto drawList = ImGui::GetWindowDrawList();
	float longest(mTextStart);

	if (mScrollToTop)
	{
		mScrollToTop = false;
		ImGui::SetScrollY(0.f);
	}

	ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
	auto scrollX = ImGui::GetScrollX();
	auto scrollY = ImGui::GetScrollY();

	auto lineNo = (int)floor(scrollY / mCharAdvance.y);
	auto globalLineMax = (int)mLines.size();
	auto lineMax = std::max(0, std::min((int)mLines.size() - 1, lineNo + (int)floor((scrollY + contentSize.y) / mCharAdvance.y)));

	// Deduce mTextStart by evaluating mLines size (global lineMax) plus two spaces as text width
	char buf[16];
	snprintf(buf, 16, " %d ", globalLineMax);
	mTextStart = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, buf, nullptr, nullptr).x + mLeftMargin;

	if (!mLines.empty())
	{
		float spaceSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, " ", nullptr, nullptr).x;

		while (lineNo <= lineMax)
		{
			ImVec2 lineStartScreenPos = ImVec2(cursorScreenPos.x, cursorScreenPos.y + lineNo * mCharAdvance.y);
			ImVec2 textScreenPos = ImVec2(lineStartScreenPos.x + mTextStart, lineStartScreenPos.y);

			auto& line = mLines[lineNo];
			longest = std::max(mTextStart + TextDistanceToLineStart(Coordinates(lineNo, GetLineMaxColumn(lineNo))), longest);
			auto columnNo = 0;
			Coordinates lineStartCoord(lineNo, 0);
			Coordinates lineEndCoord(lineNo, GetLineMaxColumn(lineNo));

			// Draw selection for the current line
			float sstart = -1.0f;
			float ssend = -1.0f;

			assert(mState.mSelectionStart <= mState.mSelectionEnd);
			if (mState.mSelectionStart <= lineEndCoord)
				sstart = mState.mSelectionStart > lineStartCoord ? TextDistanceToLineStart(mState.mSelectionStart) : 0.0f;
			if (mState.mSelectionEnd > lineStartCoord)
				ssend = TextDistanceToLineStart(mState.mSelectionEnd < lineEndCoord ? mState.mSelectionEnd : lineEndCoord);

			if (mState.mSelectionEnd.mLine > lineNo)
				ssend += mCharAdvance.x;

			if (sstart != -1 && ssend != -1 && sstart < ssend)
			{
				ImVec2 vstart(lineStartScreenPos.x + mTextStart + sstart, lineStartScreenPos.y);
				ImVec2 vend(lineStartScreenPos.x + mTextStart + ssend, lineStartScreenPos.y + mCharAdvance.y);
				drawList->AddRectFilled(vstart, vend, mPalette[(int)PaletteIndex::Selection]);
			}

			// Draw breakpoints
			auto start = ImVec2(lineStartScreenPos.x + scrollX, lineStartScreenPos.y);

			if (mBreakpoints.count(lineNo + 1) != 0)
			{
				auto end = ImVec2(lineStartScreenPos.x + contentSize.x + 2.0f * scrollX, lineStartScreenPos.y + mCharAdvance.y);
				drawList->AddRectFilled(start, end, mPalette[(int)PaletteIndex::Breakpoint]);
			}

			// Draw error markers
			auto errorIt = mErrorMarkers.find(lineNo + 1);
			if (errorIt != mErrorMarkers.end())
			{
				auto end = ImVec2(lineStartScreenPos.x + contentSize.x + 2.0f * scrollX, lineStartScreenPos.y + mCharAdvance.y);
				drawList->AddRectFilled(start, end, mPalette[(int)PaletteIndex::ErrorMarker]);

				if (ImGui::IsMouseHoveringRect(lineStartScreenPos, end))
				{
					ImGui::BeginTooltip();
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
					ImGui::Text("Error at line %d:", errorIt->first);
					ImGui::PopStyleColor();
					ImGui::Separator();
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.2f, 1.0f));
					ImGui::Text("%s", errorIt->second.c_str());
					ImGui::PopStyleColor();
					ImGui::EndTooltip();
				}
			}

			// Draw line number (right aligned)
			switch(lnMode)
			{
			case LINE_RELATIVE:
			{
				if(lineNo != mState.mCursorPosition.mLine)
				{
					snprintf(buf, 16, "%d  ", abs(mState.mCursorPosition.mLine - lineNo));
				}
				else
					snprintf(buf, 16, "%d  ", lineNo + 1);

			}break;
			}

			auto lineNoWidth = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, buf, nullptr, nullptr).x;
			drawList->AddText(ImVec2(lineStartScreenPos.x + mTextStart - lineNoWidth, lineStartScreenPos.y), mPalette[(int)PaletteIndex::LineNumber], buf);

			if (mState.mCursorPosition.mLine == lineNo)
			{
				auto focused = ImGui::IsWindowFocused( ImGuiFocusedFlags_RootAndChildWindows) && hasCursorFocus;

				// Highlight the current line (where the cursor is)
				if (!HasSelection())
				{
					auto end = ImVec2(start.x + contentSize.x + scrollX, start.y + mCharAdvance.y);
					drawList->AddRectFilled(start, end, mPalette[(int)(focused ? PaletteIndex::CurrentLineFill : PaletteIndex::CurrentLineFillInactive)]);
					drawList->AddRect(start, end, mPalette[(int)PaletteIndex::CurrentLineEdge], 1.0f);
				}

				// Render the cursor
				if (focused)
				{
					auto timeEnd = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
					auto elapsed = timeEnd - mStartTime;
					auto cindex = GetCharacterIndex(mState.mCursorPosition);
					float cx = TextDistanceToLineStart(mState.mCursorPosition);
					if (elapsed > 400 || mCursorPositionChanged)
					{
						float width = 1.0f;

						char buf2[2];
						if (mOverwrite && cindex < (int)line.size())
						{
							auto c = line[cindex].mChar;
							if (c == '\t')
							{
								auto x = (1.0f + std::floor((1.0f + cx) / (float(mTabSize) * spaceSize))) * (float(mTabSize) * spaceSize);
								width = x - cx;
							}
							else
							{
								buf2[0] = line[cindex].mChar;
								buf2[1] = '\0';
								width = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, buf2).x;
							}
						}
						// highlights the whole character cell under the curser
						if (mVimMode == VI_NORMAL || mVimMode == VI_VISUAL || mVimMode == VI_LINE_VISUAL)
						{
							if (line.size() > 0)
							{
								cindex = clamp(cindex, 0, line.size() - 1);
								buf2[0] = line[cindex].mChar;
								buf2[1] = '\0';
								width = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, buf2).x;;
							}
							else
							{
								width = 10.0;
							}
						}

						ImVec2 cstart(textScreenPos.x + cx, lineStartScreenPos.y);
						ImVec2 cend(textScreenPos.x + cx + width, lineStartScreenPos.y + mCharAdvance.y);

						if (mVimMode == VI_CHANGE || mVimMode == VI_YANK)
							cstart.y += 10;
						drawList->AddRectFilled(cstart, cend, mPalette[(int)PaletteIndex::Cursor]);
						if (elapsed > 800)
							mStartTime = timeEnd;
					}
				}
			}

			// Render colorized text
			auto prevColor = line.empty() ? mPalette[(int)PaletteIndex::Default] : GetGlyphColor(line[0]);
			ImVec2 bufferOffset;

			int cursorCol = GetCharacterIndex(mState.mCursorPosition);
			bool cursorSameLine = mState.mCursorPosition.mLine == lineNo;
			for (int i = 0; i < line.size();)
			{
				auto& glyph = line[i];
				auto color = GetGlyphColor(glyph);

				const ImVec2 newOffset(textScreenPos.x + bufferOffset.x, textScreenPos.y + bufferOffset.y);
				short ch = glyph.mChar;
				bool cursorSameCol = cursorCol == i;
				bool cursorNotHere = cursorSameLine && !cursorSameCol || !cursorSameLine;
					
				if(glyph.backgroundColor != 0 && cursorNotHere)
				{
					ImVec2 cstart = newOffset;
					ImVec2 cend;
					ImVec2 textSize;
					if (mLineBuffer.size() > 0)
					{
						textSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, mLineBuffer.c_str(), nullptr, nullptr);

						cstart.x += textSize.x;
					}
					cend = cstart;
					textSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, (char*)&ch, nullptr, nullptr);
					cend.x += textSize.x;
					cend.y += textSize.y;
					drawList->AddRectFilled(cstart, cend, glyph.backgroundColor);
				}
				if ((color != prevColor || glyph.mChar == '\t' || glyph.mChar == ' ') && !mLineBuffer.empty())
				{
					auto textSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, mLineBuffer.c_str(), nullptr, nullptr);
					drawList->AddText(newOffset, prevColor, mLineBuffer.c_str());
					bufferOffset.x += textSize.x;
					mLineBuffer.clear();
				}
				prevColor = color;

				if (glyph.mChar == '\t')
				{
					auto oldX = bufferOffset.x;
					bufferOffset.x = (1.0f + std::floor((1.0f + bufferOffset.x) / (float(mTabSize) * spaceSize))) * (float(mTabSize) * spaceSize);
					++i;

					if (mShowWhitespaces)
					{
						const auto s = ImGui::GetFontSize();
						const auto x1 = textScreenPos.x + oldX + 1.0f;
						const auto x2 = textScreenPos.x + bufferOffset.x - 1.0f;
						const auto y = textScreenPos.y + bufferOffset.y + s * 0.5f;
						const ImVec2 p1(x1, y);
						const ImVec2 p2(x2, y);
						const ImVec2 p3(x2 - s * 0.2f, y - s * 0.2f);
						const ImVec2 p4(x2 - s * 0.2f, y + s * 0.2f);
						drawList->AddLine(p1, p2, 0x90909090);
						drawList->AddLine(p2, p3, 0x90909090);
						drawList->AddLine(p2, p4, 0x90909090);
					}
				}
				else if (glyph.mChar == ' ')
				{
					if (mShowWhitespaces)
					{
						const auto s = ImGui::GetFontSize();
						const auto x = textScreenPos.x + bufferOffset.x + spaceSize * 0.5f;
						const auto y = textScreenPos.y + bufferOffset.y + s * 0.5f;
						drawList->AddCircleFilled(ImVec2(x, y), 1.5f, 0x80808080, 4);
					}
					bufferOffset.x += spaceSize;
					i++;
				}
				else
				{
					auto l = UTF8CharLength(glyph.mChar);
					while (l-- > 0)
						mLineBuffer.push_back(line[i++].mChar);
				}
				++columnNo;
			}

			if (!mLineBuffer.empty())
			{
				const ImVec2 newOffset(textScreenPos.x + bufferOffset.x, textScreenPos.y + bufferOffset.y);
				drawList->AddText(newOffset, prevColor, mLineBuffer.c_str());
				mLineBuffer.clear();
			}

			++lineNo;
		}

		/*
		// Draw a tooltip on known identifiers/preprocessor symbols
		if (ImGui::IsMousePosValid())
		{
			auto id = GetWordAt(ScreenPosToCoordinates(ImGui::GetMousePos()));
			if (!id.empty())
			{
				auto it = mLanguageDefinition.mIdentifiers.find(id);
				if (it != mLanguageDefinition.mIdentifiers.end())
				{
					ImGui::BeginTooltip();
					ImGui::TextUnformatted(it->second.mDeclaration.c_str());
					ImGui::EndTooltip();
				}
				else
				{
					auto pi = mLanguageDefinition.mPreprocIdentifiers.find(id);
					if (pi != mLanguageDefinition.mPreprocIdentifiers.end())
					{
						ImGui::BeginTooltip();
						ImGui::TextUnformatted(pi->second.mDeclaration.c_str());
						ImGui::EndTooltip();
					}
				}
			}
		}
		*/
	}


	ImGui::Dummy(ImVec2((longest + 2), mLines.size() * mCharAdvance.y));

	if (mScrollToCursor)
	{
		EnsureCursorVisible();
		ImGui::SetWindowFocus();
		mScrollToCursor = false;
	}

	float y_pos = cursorScreenPos.y + mState.mCursorPosition.mLine * mCharAdvance.y;
	ImVec2 lineStartScreenPos = ImVec2(cursorScreenPos.x, y_pos);
	ImVec2 textScreenPos = ImVec2(lineStartScreenPos.x + mTextStart, lineStartScreenPos.y);
	float cx = TextDistanceToLineStart(mState.mCursorPosition);
	ImVec2 cstart(textScreenPos.x + cx, lineStartScreenPos.y);

	mCursorScreenPos = cstart;
	if (hasCursorFocus)
	{
		RenderIntellisenseSuggestions(data, cstart.x, cstart.y);
	}
}

void TokenizeLine(own_std::string& line, std::vector<token2> *tkns);

void TextEditor::Render(const char* aTitle, const ImVec2& aSize, int flags, bool aBorder)
{
	lastInsertedChar = 0;
	mWithinRender = true;
	mTextChanged = false;
	mCursorPositionChanged = false;
	gotoFuncSrcLine = 0;

	if(cur_tkns_are_from_line != mState.mCursorPosition.mLine)
	{
		own_std::string line = GetCurrentLineText();
		line_tokens.clear();
		TokenizeLine(line, &line_tokens);
		cur_tkns_are_from_line = mState.mCursorPosition.mLine;
	}

	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::ColorConvertU32ToFloat4(mPalette[(int)PaletteIndex::Background]));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	bool hasFocus = (flags & TEXT_ED_DONT_HAVE_CURSOR_FOCUS) == 0 || haveKeyboardFocusAnyway;
	hasCursorFocus = hasFocus;

	bool bNotEmpty = insertBuffer.size() > 0;
	firstChInInsertBufferIsSlash = bNotEmpty && insertBuffer[0] == '/';;
	if (!mIgnoreImGuiChild)
	{
		auto new_sz = aSize;
		ImGui::BeginChild(aTitle, new_sz, aBorder, ImGuiWindowFlags_NoNav);
	}

	if ((flags & TEXT_ED_ALLOW_ARRAW_NAVIGATION_EVEN_WHEN_NOT_FOCUS) != 0)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto shift = io.KeyShift;
		auto ctrl = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
		auto alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;
		if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_RightArrow))
		{
			MoveRight(1, shift, ctrl);
		}
		else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_LeftArrow))
		{
			MoveLeft(1, shift, ctrl);
		}
		else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_UpArrow))
		{
			MoveUp(1);
		}
		else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_DownArrow))
		{
			MoveDown(1);
		}
	}
	if (mHandleKeyboardInputs && hasFocus)
	{
		HandleKeyboardInputs();
		ImGui::PushAllowKeyboardFocus(true);
	}

	if (mHandleMouseInputs && hasFocus)
		HandleMouseInputs();

	ColorizeInternal();
	Render();

	if (mHandleKeyboardInputs && hasFocus)
		ImGui::PopAllowKeyboardFocus();

	if (!mIgnoreImGuiChild)
		ImGui::EndChild();

	ImGui::PopStyleVar();
	ImGui::PopStyleColor();

	mWithinRender = false;


	if (mTextChanged)
	{
		int line = mState.mCursorPosition.mLine;
		int column = 0;
		ColorizeLine(line, column);

		own_std::string line_str = GetCurrentLineText();
		line_tokens.clear();
		TokenizeLine(line_str, &line_tokens);
	}
}

bool TextEditor::IsCloseCommentBlock(int i, Line &l)
{
	if ((i + 1) >= l.size())
		return false;
	if (l[i].mChar == '*' && l[i + 1].mChar == '/')
		return true;
	return false;
}
bool TextEditor::IsOpenCommentBlock(int i, Line &l)
{
	if ((i + 1) >= l.size())
		return false;
	if (l[i].mChar == '/' && l[i + 1].mChar == '*')
		return true;
	return false;
}
void TextEditor::ColorizeLine(int &line, int &column)
{
	auto l = &mLines[line];
	int lsz = l->size();
	int start_word = 0;
	if (lsz != 0)
	{

		int block_level = 0;
		Glyph* g = &(*l)[0];
		auto i = column;
		for (; i < lsz; i++)
		{
			g[i].backgroundColor = 0;
			g[i].color = 0xffffffff;
			if(g[i].mChar == '/' && g[i + 1].mChar == '/')
			{
				for (;i < lsz; i++)
				{
					g[i].color = commentColor;
				}

			}
			if(IsOpenCommentBlock(i, *l))
			{
				while (line < mLines.size() && i < lsz)
				{
					(*l)[i].color = commentColor;
					if (IsOpenCommentBlock(i, *l))
					{
						block_level++;
						i++;
					}
					if (IsCloseCommentBlock(i, *l))
					{
						block_level--;
						i++;

						if (block_level == 0)
						{
							i++;
							return;
						}
					}

					i++;

					if(i >= lsz)
					{
						do
						{
							line++;
							if (line >= mLines.size())
								return;
							i = 0;
							l++;
							lsz = l->size();
						} while (l->size() == 0);
						g = &(*l)[0];
					}
				}
			}
			else if(g[i].mChar == '\"')
			{
				i++;
				while (i < lsz && g[i].mChar != '\"')
				{
					g[i].color = IM_COL32(255, 244, 38, 255);
					i++;
				}
				i++;

			}
			else if (IsNumber(g[i].mChar))
			{
				start_word = i;
				i++;
				if (g[i].mChar == 'x')
					i++;
				while (i < lsz && (IsNumber(g[i].mChar) || g[i].mChar == '.'))
				{
					i++;
				}

				int word_len = i - start_word;
				for (int j = 0; j < word_len; j++)
				{
					g[start_word + j].color = IM_COL32(208, 150, 255, 255);;
				}
			}
			else if (IsLetter(g[i].mChar))
			{
				start_word = i;
				i++;
				while (i < lsz && (IsLetter(g[i].mChar) || IsNumber(g[i].mChar) || g[i].mChar == '_'))
				{
					i++;
				}

				char buffer[256];

				int word_len = i - start_word;
				for (int j = 0; j < word_len; j++)
				{
					buffer[j] = g[start_word + j].mChar;
				}

				buffer[word_len] = 0;
				if (strcmp("if", buffer) == 0 ||
					strcmp("else", buffer) == 0||
					strcmp("continue", buffer) == 0||
					strcmp("return", buffer) == 0||
					strcmp("true", buffer) == 0||
					strcmp("false", buffer) == 0||
					strcmp("while", buffer) == 0||
					strcmp("for", buffer) == 0
					)
				{
					for (int j = 0; j < word_len; j++)
					{
						g[start_word + j].color = IM_COL32(145, 200, 255, 255);
					}
				}
				else if (strcmp("union", buffer) == 0 ||
						 strcmp("struct", buffer) == 0 ||
						 strcmp("import", buffer) == 0 ||
						 strcmp("fn", buffer) == 0)
				{
					for (int j = 0; j < word_len; j++)
					{
						g[start_word + j].color = IM_COL32(145, 255, 184, 255);
					}
				}
				else if (strcmp("u64", buffer) == 0 ||
						 strcmp("s64", buffer) == 0 ||
						 strcmp("u32", buffer) == 0 ||
						 strcmp("s32", buffer) == 0 ||
						 strcmp("u16", buffer) == 0 ||
						 strcmp("s16", buffer) == 0 ||
						 strcmp("u8", buffer) == 0 ||
						 strcmp("s8", buffer) == 0||
						 strcmp("f32", buffer) == 0 ||
						 strcmp("bool", buffer) == 0
						 )
				{
					for (int j = 0; j < word_len; j++)
					{
						g[start_word + j].color = IM_COL32(239, 255, 150, 255);
					}
				}
			}
		}
	}
}
void TextEditor::SetText(const own_std::string & aText)
{
	mLines.clear();
	mLines.emplace_back(Line());
	for (auto chr : aText)
	{
		if (chr == '\r')
		{
			// ignore the carriage return character
		}
		else if (chr == '\n')
			mLines.emplace_back(Line());
		else
		{
			mLines.back().emplace_back(Glyph(chr, PaletteIndex::Default));
		}
	}


	mTextChanged = true;
	mScrollToTop = true;

	mUndoBuffer.clear();
	mUndoIndex = 0;

	Colorize();

	int l_idx = 0;
	int column_idx = 0;
	for(int l_idx = 0; l_idx < mLines.size(); l_idx++)
	{
		ColorizeLine(l_idx, column_idx);
		column_idx = column_idx % mLines.size();
	}
}

void TextEditor::SetTextLines(const std::vector<own_std::string> & aLines)
{
	mLines.clear();

	if (aLines.empty())
	{
		mLines.emplace_back(Line());
	}
	else
	{
		mLines.resize(aLines.size());

		for (size_t i = 0; i < aLines.size(); ++i)
		{
			const own_std::string & aLine = aLines[i];

			mLines[i].reserve(aLine.size());
			for (size_t j = 0; j < aLine.size(); ++j)
				mLines[i].emplace_back(Glyph(aLine[j], PaletteIndex::Default));
		}
	}

	mTextChanged = true;
	mScrollToTop = true;

	mUndoBuffer.clear();
	mUndoIndex = 0;

	Colorize();
}

void TextEditor::EnterCharacter(ImWchar aChar, bool aShift)
{
	assert(!mReadOnly);

	UndoRecord u;

	u.mBefore = mState;

	if (HasSelection())
	{
		if (aChar == '\t' && mState.mSelectionStart.mLine != mState.mSelectionEnd.mLine)
		{

			auto start = mState.mSelectionStart;
			auto end = mState.mSelectionEnd;
			auto originalEnd = end;

			if (start > end)
				std::swap(start, end);
			start.mColumn = 0;
			//			end.mColumn = end.mLine < mLines.size() ? mLines[end.mLine].size() : 0;
			if (end.mColumn == 0 && end.mLine > 0)
				--end.mLine;
			if (end.mLine >= (int)mLines.size())
				end.mLine = mLines.empty() ? 0 : (int)mLines.size() - 1;
			end.mColumn = GetLineMaxColumn(end.mLine);

			//if (end.mColumn >= GetLineMaxColumn(end.mLine))
			//	end.mColumn = GetLineMaxColumn(end.mLine) - 1;

			u.mRemovedStart = start;
			u.mRemovedEnd = end;
			u.mRemoved = GetText(start, end);

			bool modified = false;

			for (int i = start.mLine; i <= end.mLine; i++)
			{
				auto& line = mLines[i];
				if (aShift)
				{
					if (!line.empty())
					{
						if (line.front().mChar == '\t')
						{
							line.erase(line.begin());
							modified = true;
						}
						else
						{
							for (int j = 0; j < mTabSize && !line.empty() && line.front().mChar == ' '; j++)
							{
								line.erase(line.begin());
								modified = true;
							}
						}
					}
				}
				else
				{
					line.insert(line.begin(), Glyph('\t', TextEditor::PaletteIndex::Background));
					modified = true;
				}
			}

			if (modified)
			{
				start = Coordinates(start.mLine, GetCharacterColumn(start.mLine, 0));
				Coordinates rangeEnd;
				if (originalEnd.mColumn != 0)
				{
					end = Coordinates(end.mLine, GetLineMaxColumn(end.mLine));
					rangeEnd = end;
					u.mAdded = GetText(start, end);
				}
				else
				{
					end = Coordinates(originalEnd.mLine, 0);
					rangeEnd = Coordinates(end.mLine - 1, GetLineMaxColumn(end.mLine - 1));
					u.mAdded = GetText(start, rangeEnd);
				}

				u.mAddedStart = start;
				u.mAddedEnd = rangeEnd;
				u.mAfter = mState;

				mState.mSelectionStart = start;
				mState.mSelectionEnd = end;
				AddUndo(u);

				mTextChanged = true;

				EnsureCursorVisible();
			}

			return;
		} // c == '\t'
		else
		{
			u.mRemoved = GetSelectedText();
			u.mRemovedStart = mState.mSelectionStart;
			u.mRemovedEnd = mState.mSelectionEnd;
			DeleteSelection();
		}
	} // HasSelection

	auto coord = GetActualCursorCoordinates();
	u.mAddedStart = coord;

	assert(!mLines.empty());

	if (aChar == '\n')
	{
		InsertLine(coord.mLine + 1);
		auto& line = mLines[coord.mLine];
		auto& newLine = mLines[coord.mLine + 1];

		if (mLanguageDefinition.mAutoIndentation)
			for (size_t it = 0; it < line.size() && isascii(line[it].mChar) && isblank(line[it].mChar); ++it)
				newLine.push_back(line[it]);

		const size_t whitespaceSize = newLine.size();
		auto cindex = GetCharacterIndex(coord);
		newLine.insert(newLine.end(), line.begin() + cindex, line.end());
		line.erase(line.begin() + cindex, line.begin() + line.size());
		SetCursorPosition(Coordinates(coord.mLine + 1, GetCharacterColumn(coord.mLine + 1, (int)whitespaceSize)));
		u.mAdded = (char)aChar;
	}
	else
	{
		char buf[7];
		int e = ImTextCharToUtf8(buf, 7, aChar);
		if (e > 0)
		{
			buf[e] = '\0';
			auto& line = mLines[coord.mLine];
			auto cindex = GetCharacterIndex(coord);

			if (mOverwrite && cindex < (int)line.size())
			{
				auto d = UTF8CharLength(line[cindex].mChar);

				u.mRemovedStart = mState.mCursorPosition;
				u.mRemovedEnd = Coordinates(coord.mLine, GetCharacterColumn(coord.mLine, cindex + d));

				while (d-- > 0 && cindex < (int)line.size())
				{
					u.mRemoved += line[cindex].mChar;
					line.erase(line.begin() + cindex);
				}
			}

			for (auto p = buf; *p != '\0'; p++, ++cindex)
				line.insert(line.begin() + cindex, Glyph(*p, PaletteIndex::Default));
			u.mAdded = buf;

			SetCursorPosition(Coordinates(coord.mLine, GetCharacterColumn(coord.mLine, cindex)));
		}
		else
			return;
	}

	mTextChanged = true;

	u.mAddedEnd = GetActualCursorCoordinates();
	u.mAfter = mState;

	AddUndo(u);

	Colorize(coord.mLine - 1, 3);
	EnsureCursorVisible();
}

void TextEditor::SetReadOnly(bool aValue)
{
	mReadOnly = aValue;
}

void TextEditor::SetColorizerEnable(bool aValue)
{
	mColorizerEnabled = aValue;
}

void TextEditor::SetCursorPosition(const Coordinates & aPosition)
{
	if (mState.mCursorPosition != aPosition)
	{
		mState.mCursorPosition = aPosition;
		mCursorPositionChanged = true;
		EnsureCursorVisible();
	}
}

void TextEditor::SetSelectionStart(const Coordinates & aPosition)
{
	mState.mSelectionStart = SanitizeCoordinates(aPosition);
	if (mState.mSelectionStart > mState.mSelectionEnd)
		std::swap(mState.mSelectionStart, mState.mSelectionEnd);
}

void TextEditor::SetSelectionEnd(const Coordinates & aPosition)
{
	mState.mSelectionEnd = SanitizeCoordinates(aPosition);
	if (mState.mSelectionStart > mState.mSelectionEnd)
		std::swap(mState.mSelectionStart, mState.mSelectionEnd);
}

void TextEditor::SetSelection(const Coordinates & aStart, const Coordinates & aEnd, SelectionMode aMode)
{
	auto oldSelStart = mState.mSelectionStart;
	auto oldSelEnd = mState.mSelectionEnd;

	mState.mSelectionStart = SanitizeCoordinates(aStart);
	mState.mSelectionEnd = SanitizeCoordinates(aEnd);

	if (mState.mSelectionStart > mState.mSelectionEnd)
		std::swap(mState.mSelectionStart, mState.mSelectionEnd);

	switch (aMode)
	{
	case TextEditor::SelectionMode::Normal:
		break;
	case TextEditor::SelectionMode::Word:
	{
		mState.mSelectionStart = FindWordStart(mState.mSelectionStart);
		if (!IsOnWordBoundary(mState.mSelectionEnd))
			mState.mSelectionEnd = FindWordEnd(FindWordStart(mState.mSelectionEnd));
		break;
	}
	case TextEditor::SelectionMode::Line:
	{
		const auto lineNo = mState.mSelectionEnd.mLine;
		const auto lineSize = (size_t)lineNo < mLines.size() ? mLines[lineNo].size() : 0;
		mState.mSelectionStart = Coordinates(mState.mSelectionStart.mLine, 0);
		mState.mSelectionEnd = Coordinates(lineNo, GetLineMaxColumn(lineNo));
		break;
	}
	default:
		break;
	}

	if (mState.mSelectionStart != oldSelStart ||
		mState.mSelectionEnd != oldSelEnd)
		mCursorPositionChanged = true;
}

void TextEditor::SetTabSize(int aValue)
{
	mTabSize = std::max(0, std::min(32, aValue));
}

void TextEditor::InsertText(const own_std::string & aValue)
{
	InsertText(aValue.c_str());
}

/*
void TextEditor::AppendLine(const char * aValue)
{
	if (aValue == nullptr)
		return;

	InsertLine()
	auto pos = GetActualCursorCoordinates();
	auto start = std::min(pos, mState.mSelectionStart);
	int totalLines = pos.mLine - start.mLine;

	totalLines += InsertTextAt(pos, aValue);

	SetSelection(pos, pos);
	SetCursorPosition(pos);
	Colorize(start.mLine - 1, totalLines + 2);
}
*/
void TextEditor::InsertText(const char * aValue)
{
	if (aValue == nullptr)
		return;

	auto pos = GetActualCursorCoordinates();
	auto start = std::min(pos, mState.mSelectionStart);
	int totalLines = pos.mLine - start.mLine;

	totalLines += InsertTextAt(pos, aValue);

	SetSelection(pos, pos);
	SetCursorPosition(pos);
	Colorize(start.mLine - 1, totalLines + 2);
}

void TextEditor::DeleteSelection()
{
	assert(mState.mSelectionEnd >= mState.mSelectionStart);

	if (mState.mSelectionEnd == mState.mSelectionStart)
		return;

	DeleteRange(mState.mSelectionStart, mState.mSelectionEnd);

	SetSelection(mState.mSelectionStart, mState.mSelectionStart);
	SetCursorPosition(mState.mSelectionStart);
	Colorize(mState.mSelectionStart.mLine, 1);
}

void TextEditor::MoveUp(int aAmount, bool aSelect)
{
	auto oldPos = mState.mCursorPosition;
	mState.mCursorPosition.mLine = std::max(0, mState.mCursorPosition.mLine - aAmount);
	if (oldPos != mState.mCursorPosition)
	{
		/*
		if (aSelect)
		{
			if (oldPos == mInteractiveStart)
				mInteractiveStart = mState.mCursorPosition;
			else if (oldPos == mInteractiveEnd)
				mInteractiveEnd = mState.mCursorPosition;
			else
			{
				mInteractiveStart = mState.mCursorPosition;
				mInteractiveEnd = oldPos;
			}
		}
		else
			mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
		SetSelection(mInteractiveStart, mInteractiveEnd);
		*/

		EnsureCursorVisible();
	}
}

void TextEditor::MoveDown(int aAmount, bool aSelect)
{
	assert(mState.mCursorPosition.mColumn >= 0);
	auto oldPos = mState.mCursorPosition;
	mState.mCursorPosition.mLine = std::max(0, std::min((int)mLines.size() - 1, mState.mCursorPosition.mLine + aAmount));

	if (mState.mCursorPosition != oldPos)
	{
		/*
		if (aSelect)
		{
			if (oldPos == mInteractiveEnd)
				mInteractiveEnd = mState.mCursorPosition;
			else if (oldPos == mInteractiveStart)
				mInteractiveStart = mState.mCursorPosition;
			else
			{
				mInteractiveStart = oldPos;
				mInteractiveEnd = mState.mCursorPosition;
			}
		}
		else
			mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
		SetSelection(mInteractiveStart, mInteractiveEnd);
		*/

		EnsureCursorVisible();
	}
}

static bool IsUTFSequence(char c)
{
	return (c & 0xC0) == 0x80;
}

void TextEditor::MoveLeft(int aAmount, bool aSelect, bool aWordMode)
{
	if (mLines.empty())
		return;


	auto oldPos = mState.mCursorPosition;
	mState.mCursorPosition = GetActualCursorCoordinates();
	auto line = mState.mCursorPosition.mLine;
	auto cindex = GetCharacterIndex(mState.mCursorPosition);

	while (aAmount-- > 0)
	{
		if (cindex == 0)
		{
			/*
			if (line > 0)
			{
				--line;
				if ((int)mLines.size() > line)
					cindex = (int)mLines[line].size();
				else
					cindex = 0;
			}
			*/
		}
		else
		{
			--cindex;
			if (cindex > 0)
			{
				if ((int)mLines.size() > line)
				{
					while (cindex > 0 && IsUTFSequence(mLines[line][cindex].mChar))
						--cindex;
				}
			}
		}

		mState.mCursorPosition = Coordinates(line, GetCharacterColumn(line, cindex));
		if (aWordMode)
		{
			mState.mCursorPosition = FindWordStart(mState.mCursorPosition);
			cindex = GetCharacterIndex(mState.mCursorPosition);
		}
	}

	mState.mCursorPosition = Coordinates(line, GetCharacterColumn(line, cindex));

	assert(mState.mCursorPosition.mColumn >= 0);
	/*
	if (aSelect)
	{
		if (oldPos == mInteractiveStart)
			mInteractiveStart = mState.mCursorPosition;
		else if (oldPos == mInteractiveEnd)
			mInteractiveEnd = mState.mCursorPosition;
		else
		{
			mInteractiveStart = mState.mCursorPosition;
			mInteractiveEnd = oldPos;
		}
	}
	else
		mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
		*/
	//SetSelection(mInteractiveStart, mInteractiveEnd, aSelect && aWordMode ? SelectionMode::Word : SelectionMode::Normal);

	EnsureCursorVisible();
}

void TextEditor::MoveRight(int aAmount, bool aSelect, bool aWordMode)
{
	auto oldPos = mState.mCursorPosition;

	if (mLines.empty() || oldPos.mLine >= mLines.size())
		return;

	auto cindex = GetCharacterIndex(mState.mCursorPosition);
	while (aAmount-- > 0)
	{
		auto lindex = mState.mCursorPosition.mLine;
		auto& line = mLines[lindex];

		if (cindex >= line.size())
		{
			cindex = line.size() - 1;
			/*
			if (mState.mCursorPosition.mLine < mLines.size() - 1)
			{
				mState.mCursorPosition.mLine = std::max(0, std::min((int)mLines.size() - 1, mState.mCursorPosition.mLine + 1));
				mState.mCursorPosition.mColumn = 0;
			}
			else
				return;
				*/
		}
		else
		{
			cindex += UTF8CharLength(line[cindex].mChar);
			mState.mCursorPosition = Coordinates(lindex, GetCharacterColumn(lindex, cindex));
			if (aWordMode)
				mState.mCursorPosition = FindNextWord(mState.mCursorPosition);
		}
	}

	/*
	if (aSelect)
	{
		if (oldPos == mInteractiveEnd)
			mInteractiveEnd = SanitizeCoordinates(mState.mCursorPosition);
		else if (oldPos == mInteractiveStart)
			mInteractiveStart = mState.mCursorPosition;
		else
		{
			mInteractiveStart = oldPos;
			mInteractiveEnd = mState.mCursorPosition;
		}
	}
	else
		mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
	SetSelection(mInteractiveStart, mInteractiveEnd, aSelect && aWordMode ? SelectionMode::Word : SelectionMode::Normal);
	*/

	EnsureCursorVisible();
}

void TextEditor::MoveTop(bool aSelect)
{
	auto oldPos = mState.mCursorPosition;
	SetCursorPosition(Coordinates(0, 0));

	/*
	if (mState.mCursorPosition != oldPos)
	{
		if (aSelect)
		{
			mInteractiveEnd = oldPos;
			mInteractiveStart = mState.mCursorPosition;
		}
		else
			mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
		SetSelection(mInteractiveStart, mInteractiveEnd);
	}
	*/
	EnsureCursorVisible();
}

void TextEditor::TextEditor::MoveBottom(bool aSelect)
{
	auto oldPos = GetCursorPosition();
	auto newPos = Coordinates((int)mLines.size() - 1, 0);
	SetCursorPosition(newPos);
	/*
	if (aSelect)
	{
		mInteractiveStart = oldPos;
		mInteractiveEnd = newPos;
	}
	else
		mInteractiveStart = mInteractiveEnd = newPos;
	SetSelection(mInteractiveStart, mInteractiveEnd);
	*/
	EnsureCursorVisible();
}

void TextEditor::MoveHome(bool aSelect)
{
	auto oldPos = mState.mCursorPosition;
	SetCursorPosition(Coordinates(mState.mCursorPosition.mLine, 0));

	if (mState.mCursorPosition != oldPos)
	{
		if (aSelect)
		{
			if (oldPos == mInteractiveStart)
				mInteractiveStart = mState.mCursorPosition;
			else if (oldPos == mInteractiveEnd)
				mInteractiveEnd = mState.mCursorPosition;
			else
			{
				mInteractiveStart = mState.mCursorPosition;
				mInteractiveEnd = oldPos;
			}
		}
		else
			mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
		SetSelection(mInteractiveStart, mInteractiveEnd);
	}
}

void TextEditor::MoveEnd(bool aSelect)
{
	auto oldPos = mState.mCursorPosition;
	SetCursorPosition(Coordinates(mState.mCursorPosition.mLine, GetLineMaxColumn(oldPos.mLine)));

	/*
	if (mState.mCursorPosition != oldPos)
	{
		if (aSelect)
		{
			if (oldPos == mInteractiveEnd)
				mInteractiveEnd = mState.mCursorPosition;
			else if (oldPos == mInteractiveStart)
				mInteractiveStart = mState.mCursorPosition;
			else
			{
				mInteractiveStart = oldPos;
				mInteractiveEnd = mState.mCursorPosition;
			}
		}
		else
			mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
		SetSelection(mInteractiveStart, mInteractiveEnd);
	}
*/
}

void TextEditor::Delete()
{
	assert(!mReadOnly);

	if (mLines.empty())
		return;

	UndoRecord u;
	u.mBefore = mState;

	if (HasSelection())
	{
		u.mRemoved = GetSelectedText();
		u.mRemovedStart = mState.mSelectionStart;
		u.mRemovedEnd = mState.mSelectionEnd;

		DeleteSelection();
	}
	else
	{
		auto pos = GetActualCursorCoordinates();
		SetCursorPosition(pos);
		auto& line = mLines[pos.mLine];

		if (pos.mColumn == GetLineMaxColumn(pos.mLine))
		{
			if (pos.mLine == (int)mLines.size() - 1)
				return;

			u.mRemoved = '\n';
			u.mRemovedStart = u.mRemovedEnd = GetActualCursorCoordinates();
			Advance(u.mRemovedEnd);

			auto& nextLine = mLines[pos.mLine + 1];
			line.insert(line.end(), nextLine.begin(), nextLine.end());
			RemoveLine(pos.mLine + 1);
		}
		else
		{
			auto cindex = GetCharacterIndex(pos);
			u.mRemovedStart = u.mRemovedEnd = GetActualCursorCoordinates();
			u.mRemovedEnd.mColumn++;
			u.mRemoved = GetText(u.mRemovedStart, u.mRemovedEnd);

			auto d = UTF8CharLength(line[cindex].mChar);
			while (d-- > 0 && cindex < (int)line.size())
				line.erase(line.begin() + cindex);
		}

		mTextChanged = true;

		Colorize(pos.mLine, 1);
	}

	u.mAfter = mState;
	AddUndo(u);
}

void TextEditor::Backspace()
{
	assert(!mReadOnly);

	if (mLines.empty())
		return;

	UndoRecord u;
	u.mBefore = mState;

	if (HasSelection())
	{
		u.mRemoved = GetSelectedText();
		u.mRemovedStart = mState.mSelectionStart;
		u.mRemovedEnd = mState.mSelectionEnd;

		DeleteSelection();
	}
	else
	{
		auto pos = GetActualCursorCoordinates();
		SetCursorPosition(pos);

		if (mState.mCursorPosition.mColumn == 0)
		{
			if (mState.mCursorPosition.mLine == 0)
				return;

			u.mRemoved = '\n';
			u.mRemovedStart = u.mRemovedEnd = Coordinates(pos.mLine - 1, GetLineMaxColumn(pos.mLine - 1));
			Advance(u.mRemovedEnd);

			auto& line = mLines[mState.mCursorPosition.mLine];
			auto& prevLine = mLines[mState.mCursorPosition.mLine - 1];
			auto prevSize = GetLineMaxColumn(mState.mCursorPosition.mLine - 1);
			prevLine.insert(prevLine.end(), line.begin(), line.end());

			ErrorMarkers etmp;
			for (auto& i : mErrorMarkers)
				etmp.insert(ErrorMarkers::value_type(i.first - 1 == mState.mCursorPosition.mLine ? i.first - 1 : i.first, i.second));
			mErrorMarkers = std::move(etmp);

			RemoveLine(mState.mCursorPosition.mLine);
			--mState.mCursorPosition.mLine;
			mState.mCursorPosition.mColumn = prevSize;
		}
		else
		{
			auto& line = mLines[mState.mCursorPosition.mLine];
			auto cindex = GetCharacterIndex(pos) - 1;
			auto cend = cindex + 1;
			while (cindex > 0 && IsUTFSequence(line[cindex].mChar))
				--cindex;

			//if (cindex > 0 && UTF8CharLength(line[cindex].mChar) > 1)
			//	--cindex;

			u.mRemovedStart = u.mRemovedEnd = GetActualCursorCoordinates();
			--u.mRemovedStart.mColumn;
			--mState.mCursorPosition.mColumn;

			while (cindex < line.size() && cend-- > cindex)
			{
				u.mRemoved += line[cindex].mChar;
				line.erase(line.begin() + cindex);
			}
		}

		mTextChanged = true;

		EnsureCursorVisible();
		Colorize(mState.mCursorPosition.mLine, 1);
	}

	u.mAfter = mState;
	AddUndo(u);
}

void TextEditor::SelectWordUnderCursor()
{
	auto c = GetCursorPosition();
	SetSelection(FindWordStart2(c), FindWordEnd2(c));
}

void TextEditor::SelectAll()
{
	SetSelection(Coordinates(0, 0), Coordinates((int)mLines.size(), 0));
}

bool TextEditor::HasSelection() const
{
	return mState.mSelectionEnd > mState.mSelectionStart;
}

void TextEditor::Copy()
{
	if (HasSelection())
	{
		ImGui::SetClipboardText(GetSelectedText().c_str());
	}
	else
	{
		if (!mLines.empty())
		{
			own_std::string str;
			auto& line = mLines[GetActualCursorCoordinates().mLine];
			for (auto& g : line)
				str.push_back(g.mChar);
			ImGui::SetClipboardText(str.c_str());
		}
	}
}

void TextEditor::Cut()
{
	if (IsReadOnly())
	{
		Copy();
	}
	else
	{
		if (HasSelection())
		{
			UndoRecord u;
			u.mBefore = mState;
			u.mRemoved = GetSelectedText();
			u.mRemovedStart = mState.mSelectionStart;
			u.mRemovedEnd = mState.mSelectionEnd;

			Copy();
			DeleteSelection();

			u.mAfter = mState;
			AddUndo(u);
		}
	}
}

void TextEditor::Paste(own_std::string clipText)
{
	if (IsReadOnly())
		return;

	if (clipText.size())
	{
		int prev_col = mState.mCursorPosition.mColumn;
		if(clipText[0] == '\n')
		{
			mState.mCursorPosition.mColumn = 0;
			clipText.erase(0, 1);
		}
		UndoRecord u;
		u.mBefore = mState;

		if (HasSelection())
		{
			u.mRemoved = GetSelectedText();
			u.mRemovedStart = mState.mSelectionStart;
			u.mRemovedEnd = mState.mSelectionEnd;
			DeleteSelection();
		}

		u.mAdded = clipText;
		u.mAddedStart = GetActualCursorCoordinates();

		InsertText(clipText);

		u.mAddedEnd = GetActualCursorCoordinates();
		u.mAfter = mState;
		AddUndo(u);

		mState.mCursorPosition.mColumn = prev_col;
	}
}
void TextEditor::Paste()
{
	if (IsReadOnly())
		return;

	auto clipText = ImGui::GetClipboardText();
	if (clipText != nullptr && strlen(clipText) > 0)
	{
		UndoRecord u;
		u.mBefore = mState;

		if (HasSelection())
		{
			u.mRemoved = GetSelectedText();
			u.mRemovedStart = mState.mSelectionStart;
			u.mRemovedEnd = mState.mSelectionEnd;
			DeleteSelection();
		}

		u.mAdded = clipText;
		u.mAddedStart = GetActualCursorCoordinates();

		InsertText(clipText);

		u.mAddedEnd = GetActualCursorCoordinates();
		u.mAfter = mState;
		AddUndo(u);
	}
}

bool TextEditor::CanUndo() const
{
	return !mReadOnly && mUndoIndex > 0;
}

bool TextEditor::CanRedo() const
{
	return !mReadOnly && mUndoIndex < (int)mUndoBuffer.size();
}

void TextEditor::Undo(int aSteps)
{
	while (CanUndo() && aSteps-- > 0)
		mUndoBuffer[--mUndoIndex].Undo(this);
}

void TextEditor::Redo(int aSteps)
{
	while (CanRedo() && aSteps-- > 0)
		mUndoBuffer[mUndoIndex++].Redo(this);
}

const TextEditor::Palette & TextEditor::GetDarkPalette()
{
	const static Palette p = { {
			0xff7f7f7f,	// Default
			0xffd69c56,	// Keyword	
			0xff00ff00,	// Number
			0xff7070e0,	// String
			0xff70a0e0, // Char literal
			0xffffffff, // Punctuation
			0xff408080,	// Preprocessor
			0xffaaaaaa, // Identifier
			0xff9bc64d, // Known identifier
			0xffc040a0, // Preproc identifier
			0xff206020, // Comment (single line)
			0xff406020, // Comment (multi line)
			0xff101010, // Background
			0xffe0e0e0, // Cursor
			0x80a06020, // Selection
			0x800020ff, // ErrorMarker
			0x400000ff, // Breakpoint
			0xff707000, // Line number
			0x40000000, // Current line fill
			0x40808080, // Current line fill (inactive)
			0x40a0a0a0, // Current line edge
		} };
	return p;
}

const TextEditor::Palette & TextEditor::GetLightPalette()
{
	const static Palette p = { {
			0xff7f7f7f,	// None
			0xffff0c06,	// Keyword	
			0xff008000,	// Number
			0xff2020a0,	// String
			0xff304070, // Char literal
			0xff000000, // Punctuation
			0xff406060,	// Preprocessor
			0xff404040, // Identifier
			0xff606010, // Known identifier
			0xffc040a0, // Preproc identifier
			0xff205020, // Comment (single line)
			0xff405020, // Comment (multi line)
			0xffffffff, // Background
			0xff000000, // Cursor
			0x80600000, // Selection
			0xa00010ff, // ErrorMarker
			0x80f08000, // Breakpoint
			0xff505000, // Line number
			0x40000000, // Current line fill
			0x40808080, // Current line fill (inactive)
			0x40000000, // Current line edge
		} };
	return p;
}

const TextEditor::Palette & TextEditor::GetRetroBluePalette()
{
	const static Palette p = { {
			0xff00ffff,	// None
			0xffffff00,	// Keyword	
			0xff00ff00,	// Number
			0xff808000,	// String
			0xff808000, // Char literal
			0xffffffff, // Punctuation
			0xff008000,	// Preprocessor
			0xff00ffff, // Identifier
			0xffffffff, // Known identifier
			0xffff00ff, // Preproc identifier
			0xff808080, // Comment (single line)
			0xff404040, // Comment (multi line)
			0xff800000, // Background
			0xff0080ff, // Cursor
			0x80ffff00, // Selection
			0xa00000ff, // ErrorMarker
			0x80ff8000, // Breakpoint
			0xff808000, // Line number
			0x40000000, // Current line fill
			0x40808080, // Current line fill (inactive)
			0x40000000, // Current line edge
		} };
	return p;
}


own_std::string TextEditor::GetText() const
{
	return GetText(Coordinates(), Coordinates((int)mLines.size(), 0));
}

std::vector<own_std::string> TextEditor::GetTextLines() const
{
	std::vector<own_std::string> result;

	result.reserve(mLines.size());

	for (auto & line : mLines)
	{
		own_std::string text;

		text.resize(line.size());

		for (size_t i = 0; i < line.size(); ++i)
			text[i] = line[i].mChar;

		result.emplace_back(std::move(text));
	}

	return result;
}

own_std::string TextEditor::GetSelectedText() const
{
	return GetText(mState.mSelectionStart, mState.mSelectionEnd);
}
own_std::string TextEditor::GetSelectedText2() const
{
	return GetText2(mState.mSelectionStart, mState.mSelectionEnd);
}

own_std::string TextEditor::GetCurrentLineText()const
{
	auto lineLength = GetLineMaxColumn(mState.mCursorPosition.mLine);
	return GetText(
		Coordinates(mState.mCursorPosition.mLine, 0),
		Coordinates(mState.mCursorPosition.mLine, lineLength));
}

void TextEditor::ProcessInputs()
{
}

void TextEditor::Colorize(int aFromLine, int aLines)
{
	int toLine = aLines == -1 ? (int)mLines.size() : std::min((int)mLines.size(), aFromLine + aLines);
	mColorRangeMin = std::min(mColorRangeMin, aFromLine);
	mColorRangeMax = std::max(mColorRangeMax, toLine);
	mColorRangeMin = std::max(0, mColorRangeMin);
	mColorRangeMax = std::max(mColorRangeMin, mColorRangeMax);
	mCheckComments = true;
}

void TextEditor::ColorizeRange(int aFromLine, int aToLine)
{
	if (mLines.empty() || aFromLine >= aToLine)
		return;

	own_std::string buffer;
	std::cmatch results;
	own_std::string id;

	int endLine = std::max(0, std::min((int)mLines.size(), aToLine));
	for (int i = aFromLine; i < endLine; ++i)
	{
		auto& line = mLines[i];

		if (line.empty())
			continue;

		buffer.resize(line.size());
		for (size_t j = 0; j < line.size(); ++j)
		{
			auto& col = line[j];
			buffer[j] = col.mChar;
			col.mColorIndex = PaletteIndex::Default;
		}

		const char * bufferBegin = &buffer.front();
		const char * bufferEnd = bufferBegin + buffer.size();

		auto last = bufferEnd;

		for (auto first = bufferBegin; first != last; )
		{
			const char * token_begin = nullptr;
			const char * token_end = nullptr;
			PaletteIndex token_color = PaletteIndex::Default;

			bool hasTokenizeResult = false;

			if (mLanguageDefinition.mTokenize != nullptr)
			{
				if (mLanguageDefinition.mTokenize(first, last, token_begin, token_end, token_color))
					hasTokenizeResult = true;
			}

			if (hasTokenizeResult == false)
			{
				// todo : remove
				//printf("using regex for %.*s\n", first + 10 < last ? 10 : int(last - first), first);

				/*
				for (auto& p : mRegexList)
				{
					if (std::regex_search(first, last, results, p.first, std::regex_constants::match_continuous))
					{
						hasTokenizeResult = true;

						auto& v = *results.begin();
						token_begin = v.first;
						token_end = v.second;
						token_color = p.second;
						break;
					}
				}
				*/
			}

			if (hasTokenizeResult == false)
			{
				first++;
			}
			else
			{
				const size_t token_length = token_end - token_begin;

				if (token_color == PaletteIndex::Identifier)
				{
					id.assign(token_begin, token_end);

					// todo : allmost all language definitions use lower case to specify keywords, so shouldn't this use ::tolower ?
					if (!mLanguageDefinition.mCaseSensitive)
						std::transform(id.begin(), id.end(), id.begin(), ::toupper);

					if (!line[first - bufferBegin].mPreprocessor)
					{
						if (mLanguageDefinition.mKeywords.count(id) != 0)
							token_color = PaletteIndex::Keyword;
						else if (mLanguageDefinition.mIdentifiers.count(id) != 0)
							token_color = PaletteIndex::KnownIdentifier;
						else if (mLanguageDefinition.mPreprocIdentifiers.count(id) != 0)
							token_color = PaletteIndex::PreprocIdentifier;
					}
					else
					{
						if (mLanguageDefinition.mPreprocIdentifiers.count(id) != 0)
							token_color = PaletteIndex::PreprocIdentifier;
					}
				}

				for (size_t j = 0; j < token_length; ++j)
					line[(token_begin - bufferBegin) + j].mColorIndex = token_color;

				first = token_end;
			}
		}
	}
}

void TextEditor::ColorizeInternal()
{
	if (mLines.empty() || !mColorizerEnabled)
		return;

	if (mCheckComments)
	{
		auto endLine = mLines.size();
		auto endIndex = 0;
		auto commentStartLine = endLine;
		auto commentStartIndex = endIndex;
		auto withinString = false;
		auto withinSingleLineComment = false;
		auto withinPreproc = false;
		auto firstChar = true;			// there is no other non-whitespace characters in the line before
		auto concatenate = false;		// '\' on the very end of the line
		auto currentLine = 0;
		auto currentIndex = 0;
		while (currentLine < endLine || currentIndex < endIndex)
		{
			auto& line = mLines[currentLine];

			if (currentIndex == 0 && !concatenate)
			{
				withinSingleLineComment = false;
				withinPreproc = false;
				firstChar = true;
			}

			concatenate = false;

			if (!line.empty())
			{
				auto& g = line[currentIndex];
				auto c = g.mChar;

				if (c != mLanguageDefinition.mPreprocChar && !isspace(c))
					firstChar = false;

				if (currentIndex == (int)line.size() - 1 && line[line.size() - 1].mChar == '\\')
					concatenate = true;

				bool inComment = (commentStartLine < currentLine || (commentStartLine == currentLine && commentStartIndex <= currentIndex));

				if (withinString)
				{
					line[currentIndex].mMultiLineComment = inComment;

					if (c == '\"')
					{
						if (currentIndex + 1 < (int)line.size() && line[currentIndex + 1].mChar == '\"')
						{
							currentIndex += 1;
							if (currentIndex < (int)line.size())
								line[currentIndex].mMultiLineComment = inComment;
						}
						else
							withinString = false;
					}
					else if (c == '\\')
					{
						currentIndex += 1;
						if (currentIndex < (int)line.size())
							line[currentIndex].mMultiLineComment = inComment;
					}
				}
				else
				{
					if (firstChar && c == mLanguageDefinition.mPreprocChar)
						withinPreproc = true;

					if (c == '\"')
					{
						withinString = true;
						line[currentIndex].mMultiLineComment = inComment;
					}
					else
					{
						auto pred = [](const char& a, const Glyph& b) { return a == b.mChar; };
						auto from = line.begin() + currentIndex;
						auto& startStr = mLanguageDefinition.mCommentStart;
						auto& singleStartStr = mLanguageDefinition.mSingleLineComment;

						if (singleStartStr.size() > 0 &&
							currentIndex + singleStartStr.size() <= line.size() &&
							equals(singleStartStr.begin(), singleStartStr.end(), from, from + singleStartStr.size(), pred))
						{
							withinSingleLineComment = true;
						}
						else if (!withinSingleLineComment && currentIndex + startStr.size() <= line.size() &&
							equals(startStr.begin(), startStr.end(), from, from + startStr.size(), pred))
						{
							commentStartLine = currentLine;
							commentStartIndex = currentIndex;
						}

						inComment = inComment = (commentStartLine < currentLine || (commentStartLine == currentLine && commentStartIndex <= currentIndex));

						line[currentIndex].mMultiLineComment = inComment;
						line[currentIndex].mComment = withinSingleLineComment;

						auto& endStr = mLanguageDefinition.mCommentEnd;
						if (currentIndex + 1 >= (int)endStr.size() &&
							equals(endStr.begin(), endStr.end(), from + 1 - endStr.size(), from + 1, pred))
						{
							commentStartIndex = endIndex;
							commentStartLine = endLine;
						}
					}
				}
				line[currentIndex].mPreprocessor = withinPreproc;
				currentIndex += UTF8CharLength(c);
				if (currentIndex >= (int)line.size())
				{
					currentIndex = 0;
					++currentLine;
				}
			}
			else
			{
				currentIndex = 0;
				++currentLine;
			}
		}
		mCheckComments = false;
	}

	if (mColorRangeMin < mColorRangeMax)
	{
		const int increment = (mLanguageDefinition.mTokenize == nullptr) ? 10 : 10000;
		const int to = std::min(mColorRangeMin + increment, mColorRangeMax);
		ColorizeRange(mColorRangeMin, to);
		mColorRangeMin = to;

		if (mColorRangeMax == mColorRangeMin)
		{
			mColorRangeMin = std::numeric_limits<int>::max();
			mColorRangeMax = 0;
		}
		return;
	}
}

float TextEditor::TextDistanceToLineStart(const Coordinates& aFrom) const
{
	auto& line = mLines[aFrom.mLine];
	float distance = 0.0f;
	float spaceSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, " ", nullptr, nullptr).x;
	int colIndex = GetCharacterIndex(aFrom);
	for (size_t it = 0u; it < line.size() && it < colIndex; )
	{
		if (line[it].mChar == '\t')
		{
			distance = (1.0f + std::floor((1.0f + distance) / (float(mTabSize) * spaceSize))) * (float(mTabSize) * spaceSize);
			++it;
		}
		else
		{
			auto d = UTF8CharLength(line[it].mChar);
			char tempCString[7];
			int i = 0;
			for (; i < 6 && d-- > 0 && it < (int)line.size(); i++, it++)
				tempCString[i] = line[it].mChar;

			tempCString[i] = '\0';
			distance += ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, tempCString, nullptr, nullptr).x;
		}
	}

	return distance;
}

void TextEditor::EnsureCursorVisible()
{
	if (!mWithinRender)
	{
		mScrollToCursor = true;
		return;
	}

	float scrollX = ImGui::GetScrollX();
	float scrollY = ImGui::GetScrollY();

	auto height = ImGui::GetWindowHeight();
	auto width = ImGui::GetWindowWidth();

	auto top = 1 + (int)ceil(scrollY / mCharAdvance.y);
	auto bottom = (int)ceil((scrollY + height) / mCharAdvance.y);

	auto left = (int)ceil(scrollX / mCharAdvance.x);
	auto right = (int)ceil((scrollX + width) / mCharAdvance.x);

	auto pos = GetActualCursorCoordinates();
	auto len = TextDistanceToLineStart(pos);

	if (pos.mLine < top)
		ImGui::SetScrollY(std::max(0.0f, (pos.mLine - 1) * mCharAdvance.y));
	if (pos.mLine > bottom - 4)
		ImGui::SetScrollY(std::max(0.0f, (pos.mLine + 4) * mCharAdvance.y - height));
	if (len + mTextStart < left + 4)
		ImGui::SetScrollX(std::max(0.0f, len + mTextStart - 4));
	if (len + mTextStart > right - 4)
		ImGui::SetScrollX(std::max(0.0f, len + mTextStart + 4 - width));

	
}

int TextEditor::GetPageSize() const
{
	auto height = ImGui::GetWindowHeight() - 20.0f;
	return (int)floor(height / mCharAdvance.y);
}

TextEditor::UndoRecord::UndoRecord(
	const own_std::string& aAdded,
	const TextEditor::Coordinates aAddedStart,
	const TextEditor::Coordinates aAddedEnd,
	const own_std::string& aRemoved,
	const TextEditor::Coordinates aRemovedStart,
	const TextEditor::Coordinates aRemovedEnd,
	TextEditor::EditorState& aBefore,
	TextEditor::EditorState& aAfter)
	: mAdded(aAdded)
	, mAddedStart(aAddedStart)
	, mAddedEnd(aAddedEnd)
	, mRemoved(aRemoved)
	, mRemovedStart(aRemovedStart)
	, mRemovedEnd(aRemovedEnd)
	, mBefore(aBefore)
	, mAfter(aAfter)
{
	assert(mAddedStart <= mAddedEnd);
	assert(mRemovedStart <= mRemovedEnd);
}

void TextEditor::UndoRecord::Undo(TextEditor * aEditor)
{
	if (!mAdded.empty())
	{
		aEditor->DeleteRange(mAddedStart, mAddedEnd, false);
		aEditor->Colorize(mAddedStart.mLine - 1, mAddedEnd.mLine - mAddedStart.mLine + 2);
	}

	if (!mRemoved.empty())
	{
		auto start = mRemovedStart;
		aEditor->InsertTextAt(start, mRemoved.c_str());
		aEditor->Colorize(mRemovedStart.mLine - 1, mRemovedEnd.mLine - mRemovedStart.mLine + 2);
	}

	aEditor->mState = mBefore;
	aEditor->mState.mSelectionStart = aEditor->mState.mSelectionEnd;
	aEditor->EnsureCursorVisible();

}

void TextEditor::UndoRecord::Redo(TextEditor * aEditor)
{
	if (!mRemoved.empty())
	{
		aEditor->DeleteRange(mRemovedStart, mRemovedEnd);
		aEditor->Colorize(mRemovedStart.mLine - 1, mRemovedEnd.mLine - mRemovedStart.mLine + 1);
	}

	if (!mAdded.empty())
	{
		auto start = mAddedStart;
		aEditor->InsertTextAt(start, mAdded.c_str());
		aEditor->Colorize(mAddedStart.mLine - 1, mAddedEnd.mLine - mAddedStart.mLine + 1);
	}

	aEditor->mState = mAfter;
	aEditor->EnsureCursorVisible();
}

static bool TokenizeCStyleString(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end)
{
	const char * p = in_begin;

	if (*p == '"')
	{
		p++;

		while (p < in_end)
		{
			// handle end of string
			if (*p == '"')
			{
				out_begin = in_begin;
				out_end = p + 1;
				return true;
			}

			// handle escape character for "
			if (*p == '\\' && p + 1 < in_end && p[1] == '"')
				p++;

			p++;
		}
	}

	return false;
}

static bool TokenizeCStyleCharacterLiteral(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end)
{
	const char * p = in_begin;

	if (*p == '\'')
	{
		p++;

		// handle escape characters
		if (p < in_end && *p == '\\')
			p++;

		if (p < in_end)
			p++;

		// handle end of character literal
		if (p < in_end && *p == '\'')
		{
			out_begin = in_begin;
			out_end = p + 1;
			return true;
		}
	}

	return false;
}

static bool TokenizeCStyleIdentifier(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end)
{
	const char * p = in_begin;

	if ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
	{
		p++;

		while ((p < in_end) && ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9') || *p == '_'))
			p++;

		out_begin = in_begin;
		out_end = p;
		return true;
	}

	return false;
}

static bool TokenizeCStyleNumber(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end)
{
	const char * p = in_begin;

	const bool startsWithNumber = *p >= '0' && *p <= '9';

	if (*p != '+' && *p != '-' && !startsWithNumber)
		return false;

	p++;

	bool hasNumber = startsWithNumber;

	while (p < in_end && (*p >= '0' && *p <= '9'))
	{
		hasNumber = true;

		p++;
	}

	if (hasNumber == false)
		return false;

	bool isFloat = false;
	bool isHex = false;
	bool isBinary = false;

	if (p < in_end)
	{
		if (*p == '.')
		{
			isFloat = true;

			p++;

			while (p < in_end && (*p >= '0' && *p <= '9'))
				p++;
		}
		else if (*p == 'x' || *p == 'X')
		{
			// hex formatted integer of the type 0xef80

			isHex = true;

			p++;

			while (p < in_end && ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F')))
				p++;
		}
		else if (*p == 'b' || *p == 'B')
		{
			// binary formatted integer of the type 0b01011101

			isBinary = true;

			p++;

			while (p < in_end && (*p >= '0' && *p <= '1'))
				p++;
		}
	}

	if (isHex == false && isBinary == false)
	{
		// floating point exponent
		if (p < in_end && (*p == 'e' || *p == 'E'))
		{
			isFloat = true;

			p++;

			if (p < in_end && (*p == '+' || *p == '-'))
				p++;

			bool hasDigits = false;

			while (p < in_end && (*p >= '0' && *p <= '9'))
			{
				hasDigits = true;

				p++;
			}

			if (hasDigits == false)
				return false;
		}

		// single precision floating point type
		if (p < in_end && *p == 'f')
			p++;
	}

	if (isFloat == false)
	{
		// integer size type
		while (p < in_end && (*p == 'u' || *p == 'U' || *p == 'l' || *p == 'L'))
			p++;
	}

	out_begin = in_begin;
	out_end = p;
	return true;
}

static bool TokenizeCStylePunctuation(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end)
{
	(void)in_end;

	switch (*in_begin)
	{
	case '[':
	case ']':
	case '{':
	case '}':
	case '!':
	case '%':
	case '^':
	case '&':
	case '*':
	case '(':
	case ')':
	case '-':
	case '+':
	case '=':
	case '~':
	case '|':
	case '<':
	case '>':
	case '?':
	case ':':
	case '/':
	case ';':
	case ',':
	case '.':
		out_begin = in_begin;
		out_end = in_begin + 1;
		return true;
	}

	return false;
}

const TextEditor::LanguageDefinition& TextEditor::LanguageDefinition::CPlusPlus()
{
	static bool inited = false;
	static LanguageDefinition langDef;
	if (!inited)
	{
		/*
		static const char* const cppKeywords[] = {
			"alignas", "alignof", "and", "and_eq", "asm", "atomic_cancel", "atomic_commit", "atomic_noexcept", "auto", "bitand", "bitor", "bool", "break", "case", "catch", "char", "char16_t", "char32_t", "class",
			"compl", "concept", "const", "constexpr", "const_cast", "continue", "decltype", "default", "delete", "do", "double", "dynamic_cast", "else", "enum", "explicit", "export", "extern", "false", "float",
			"for", "friend", "goto", "if", "import", "inline", "int", "long", "module", "mutable", "namespace", "new", "noexcept", "not", "not_eq", "nullptr", "operator", "or", "or_eq", "private", "protected", "public",
			"register", "reinterpret_cast", "requires", "return", "short", "signed", "sizeof", "static", "static_assert", "static_cast", "struct", "switch", "synchronized", "template", "this", "thread_local",
			"throw", "true", "try", "typedef", "typeid", "typename", "union", "unsigned", "using", "virtual", "void", "volatile", "wchar_t", "while", "xor", "xor_eq"
		};
		for (auto& k : cppKeywords)
			langDef.mKeywords.insert(k);

		static const char* const identifiers[] = {
			"abort", "abs", "acos", "asin", "atan", "atexit", "atof", "atoi", "atol", "ceil", "clock", "cosh", "ctime", "div", "exit", "fabs", "floor", "fmod", "getchar", "getenv", "isalnum", "isalpha", "isdigit", "isgraph",
			"ispunct", "isspace", "isupper", "kbhit", "log10", "log2", "log", "memcmp", "modf", "pow", "printf", "sprintf", "snprintf", "putchar", "putenv", "puts", "rand", "remove", "rename", "sinh", "sqrt", "srand", "strcat", "strcmp", "strerror", "time", "tolower", "toupper",
			"std", "string", "vector", "map", "unordered_map", "set", "unordered_set", "min", "max"
		};
		for (auto& k : identifiers)
		{
			Identifier id;
			id.mDeclaration = "Built-in function";
			langDef.mIdentifiers.insert(std::make_pair(own_std::string(k), id));
		}
		*/

		langDef.mTokenize = [](const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end, PaletteIndex & paletteIndex) -> bool
		{
			paletteIndex = PaletteIndex::Max;

			while (in_begin < in_end && isascii(*in_begin) && isblank(*in_begin))
				in_begin++;

			if (in_begin == in_end)
			{
				out_begin = in_end;
				out_end = in_end;
				paletteIndex = PaletteIndex::Default;
			}
			else if (TokenizeCStyleString(in_begin, in_end, out_begin, out_end))
				paletteIndex = PaletteIndex::String;
			else if (TokenizeCStyleCharacterLiteral(in_begin, in_end, out_begin, out_end))
				paletteIndex = PaletteIndex::CharLiteral;
			else if (TokenizeCStyleIdentifier(in_begin, in_end, out_begin, out_end))
				paletteIndex = PaletteIndex::Identifier;
			else if (TokenizeCStyleNumber(in_begin, in_end, out_begin, out_end))
				paletteIndex = PaletteIndex::Number;
			else if (TokenizeCStylePunctuation(in_begin, in_end, out_begin, out_end))
				paletteIndex = PaletteIndex::Punctuation;

			return paletteIndex != PaletteIndex::Max;
		};

		langDef.mCommentStart = "/*";
		langDef.mCommentEnd = "*/";
		langDef.mSingleLineComment = "//";

		langDef.mCaseSensitive = true;
		langDef.mAutoIndentation = true;

		langDef.mName = "C++";

		inited = true;
	}
	return langDef;
}

const TextEditor::LanguageDefinition& TextEditor::LanguageDefinition::HLSL()
{
	static bool inited = false;
	static LanguageDefinition langDef;
	if (!inited)
	{
		/*
		static const char* const keywords[] = {
			"AppendStructuredBuffer", "asm", "asm_fragment", "BlendState", "bool", "break", "Buffer", "ByteAddressBuffer", "case", "cbuffer", "centroid", "class", "column_major", "compile", "compile_fragment",
			"CompileShader", "const", "continue", "ComputeShader", "ConsumeStructuredBuffer", "default", "DepthStencilState", "DepthStencilView", "discard", "do", "double", "DomainShader", "dword", "else",
			"export", "extern", "false", "float", "for", "fxgroup", "GeometryShader", "groupshared", "half", "Hullshader", "if", "in", "inline", "inout", "InputPatch", "int", "interface", "line", "lineadj",
			"linear", "LineStream", "matrix", "min16float", "min10float", "min16int", "min12int", "min16uint", "namespace", "nointerpolation", "noperspective", "NULL", "out", "OutputPatch", "packoffset",
			"pass", "pixelfragment", "PixelShader", "point", "PointStream", "precise", "RasterizerState", "RenderTargetView", "return", "register", "row_major", "RWBuffer", "RWByteAddressBuffer", "RWStructuredBuffer",
			"RWTexture1D", "RWTexture1DArray", "RWTexture2D", "RWTexture2DArray", "RWTexture3D", "sample", "sampler", "SamplerState", "SamplerComparisonState", "shared", "snorm", "stateblock", "stateblock_state",
			"static", "string", "struct", "switch", "StructuredBuffer", "tbuffer", "technique", "technique10", "technique11", "texture", "Texture1D", "Texture1DArray", "Texture2D", "Texture2DArray", "Texture2DMS",
			"Texture2DMSArray", "Texture3D", "TextureCube", "TextureCubeArray", "true", "typedef", "triangle", "triangleadj", "TriangleStream", "uint", "uniform", "unorm", "unsigned", "vector", "vertexfragment",
			"VertexShader", "void", "volatile", "while",
			"bool1","bool2","bool3","bool4","double1","double2","double3","double4", "float1", "float2", "float3", "float4", "int1", "int2", "int3", "int4", "in", "out", "inout",
			"uint1", "uint2", "uint3", "uint4", "dword1", "dword2", "dword3", "dword4", "half1", "half2", "half3", "half4",
			"float1x1","float2x1","float3x1","float4x1","float1x2","float2x2","float3x2","float4x2",
			"float1x3","float2x3","float3x3","float4x3","float1x4","float2x4","float3x4","float4x4",
			"half1x1","half2x1","half3x1","half4x1","half1x2","half2x2","half3x2","half4x2",
			"half1x3","half2x3","half3x3","half4x3","half1x4","half2x4","half3x4","half4x4",
		};
		for (auto& k : keywords)
			langDef.mKeywords.insert(k);

		static const char* const identifiers[] = {
			"abort", "abs", "acos", "all", "AllMemoryBarrier", "AllMemoryBarrierWithGroupSync", "any", "asdouble", "asfloat", "asin", "asint", "asint", "asuint",
			"asuint", "atan", "atan2", "ceil", "CheckAccessFullyMapped", "clamp", "clip", "cos", "cosh", "countbits", "cross", "D3DCOLORtoUBYTE4", "ddx",
			"ddx_coarse", "ddx_fine", "ddy", "ddy_coarse", "ddy_fine", "degrees", "determinant", "DeviceMemoryBarrier", "DeviceMemoryBarrierWithGroupSync",
			"distance", "dot", "dst", "errorf", "EvaluateAttributeAtCentroid", "EvaluateAttributeAtSample", "EvaluateAttributeSnapped", "exp", "exp2",
			"f16tof32", "f32tof16", "faceforward", "firstbithigh", "firstbitlow", "floor", "fma", "fmod", "frac", "frexp", "fwidth", "GetRenderTargetSampleCount",
			"GetRenderTargetSamplePosition", "GroupMemoryBarrier", "GroupMemoryBarrierWithGroupSync", "InterlockedAdd", "InterlockedAnd", "InterlockedCompareExchange",
			"InterlockedCompareStore", "InterlockedExchange", "InterlockedMax", "InterlockedMin", "InterlockedOr", "InterlockedXor", "isfinite", "isinf", "isnan",
			"ldexp", "length", "lerp", "lit", "log", "log10", "log2", "mad", "max", "min", "modf", "msad4", "mul", "noise", "normalize", "pow", "printf",
			"Process2DQuadTessFactorsAvg", "Process2DQuadTessFactorsMax", "Process2DQuadTessFactorsMin", "ProcessIsolineTessFactors", "ProcessQuadTessFactorsAvg",
			"ProcessQuadTessFactorsMax", "ProcessQuadTessFactorsMin", "ProcessTriTessFactorsAvg", "ProcessTriTessFactorsMax", "ProcessTriTessFactorsMin",
			"radians", "rcp", "reflect", "refract", "reversebits", "round", "rsqrt", "saturate", "sign", "sin", "sincos", "sinh", "smoothstep", "sqrt", "step",
			"tan", "tanh", "tex1D", "tex1D", "tex1Dbias", "tex1Dgrad", "tex1Dlod", "tex1Dproj", "tex2D", "tex2D", "tex2Dbias", "tex2Dgrad", "tex2Dlod", "tex2Dproj",
			"tex3D", "tex3D", "tex3Dbias", "tex3Dgrad", "tex3Dlod", "tex3Dproj", "texCUBE", "texCUBE", "texCUBEbias", "texCUBEgrad", "texCUBElod", "texCUBEproj", "transpose", "trunc"
		};
		for (auto& k : identifiers)
		{
			Identifier id;
			id.mDeclaration = "Built-in function";
			langDef.mIdentifiers.insert(std::make_pair(own_std::string(k), id));
		}
		*/

		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("[ \\t]*#[ \\t]*[a-zA-Z_]+", PaletteIndex::Preprocessor));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("L?\\\"(\\\\.|[^\\\"])*\\\"", PaletteIndex::String));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("\\'\\\\?[^\\']\\'", PaletteIndex::CharLiteral));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("[+-]?[0-9]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("0[0-7]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\:\\,\\.]", PaletteIndex::Punctuation));

		langDef.mCommentStart = "/*";
		langDef.mCommentEnd = "*/";
		langDef.mSingleLineComment = "//";

		langDef.mCaseSensitive = true;
		langDef.mAutoIndentation = true;

		langDef.mName = "HLSL";

		inited = true;
	}
	return langDef;
}

const TextEditor::LanguageDefinition& TextEditor::LanguageDefinition::GLSL()
{
	static bool inited = false;
	static LanguageDefinition langDef;
	if (!inited)
	{
		static const char* const keywords[] = {
			"auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern", "float", "for", "goto", "if", "inline", "int", "long", "register", "restrict", "return", "short",
			"signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while", "_Alignas", "_Alignof", "_Atomic", "_Bool", "_Complex", "_Generic", "_Imaginary",
			"_Noreturn", "_Static_assert", "_Thread_local"
		};
		for (auto& k : keywords)
			langDef.mKeywords.insert(k);

		static const char* const identifiers[] = {
			"abort", "abs", "acos", "asin", "atan", "atexit", "atof", "atoi", "atol", "ceil", "clock", "cosh", "ctime", "div", "exit", "fabs", "floor", "fmod", "getchar", "getenv", "isalnum", "isalpha", "isdigit", "isgraph",
			"ispunct", "isspace", "isupper", "kbhit", "log10", "log2", "log", "memcmp", "modf", "pow", "putchar", "putenv", "puts", "rand", "remove", "rename", "sinh", "sqrt", "srand", "strcat", "strcmp", "strerror", "time", "tolower", "toupper"
		};
		for (auto& k : identifiers)
		{
			Identifier id;
			id.mDeclaration = "Built-in function";
			langDef.mIdentifiers.insert(std::make_pair(own_std::string(k), id));
		}

		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("[ \\t]*#[ \\t]*[a-zA-Z_]+", PaletteIndex::Preprocessor));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("L?\\\"(\\\\.|[^\\\"])*\\\"", PaletteIndex::String));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("\\'\\\\?[^\\']\\'", PaletteIndex::CharLiteral));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("[+-]?[0-9]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("0[0-7]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", PaletteIndex::Punctuation));

		langDef.mCommentStart = "/*";
		langDef.mCommentEnd = "*/";
		langDef.mSingleLineComment = "//";

		langDef.mCaseSensitive = true;
		langDef.mAutoIndentation = true;

		langDef.mName = "GLSL";

		inited = true;
	}
	return langDef;
}

const TextEditor::LanguageDefinition& TextEditor::LanguageDefinition::C()
{
	static bool inited = false;
	static LanguageDefinition langDef;
	if (!inited)
	{
		static const char* const keywords[] = {
			"auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern", "float", "for", "goto", "if", "inline", "int", "long", "register", "restrict", "return", "short",
			"signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while", "_Alignas", "_Alignof", "_Atomic", "_Bool", "_Complex", "_Generic", "_Imaginary",
			"_Noreturn", "_Static_assert", "_Thread_local"
		};
		for (auto& k : keywords)
			langDef.mKeywords.insert(k);

		static const char* const identifiers[] = {
			"abort", "abs", "acos", "asin", "atan", "atexit", "atof", "atoi", "atol", "ceil", "clock", "cosh", "ctime", "div", "exit", "fabs", "floor", "fmod", "getchar", "getenv", "isalnum", "isalpha", "isdigit", "isgraph",
			"ispunct", "isspace", "isupper", "kbhit", "log10", "log2", "log", "memcmp", "modf", "pow", "putchar", "putenv", "puts", "rand", "remove", "rename", "sinh", "sqrt", "srand", "strcat", "strcmp", "strerror", "time", "tolower", "toupper"
		};
		for (auto& k : identifiers)
		{
			Identifier id;
			id.mDeclaration = "Built-in function";
			langDef.mIdentifiers.insert(std::make_pair(own_std::string(k), id));
		}

		langDef.mTokenize = [](const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end, PaletteIndex & paletteIndex) -> bool
		{
			paletteIndex = PaletteIndex::Max;

			while (in_begin < in_end && isascii(*in_begin) && isblank(*in_begin))
				in_begin++;

			if (in_begin == in_end)
			{
				out_begin = in_end;
				out_end = in_end;
				paletteIndex = PaletteIndex::Default;
			}
			else if (TokenizeCStyleString(in_begin, in_end, out_begin, out_end))
				paletteIndex = PaletteIndex::String;
			else if (TokenizeCStyleCharacterLiteral(in_begin, in_end, out_begin, out_end))
				paletteIndex = PaletteIndex::CharLiteral;
			else if (TokenizeCStyleIdentifier(in_begin, in_end, out_begin, out_end))
				paletteIndex = PaletteIndex::Identifier;
			else if (TokenizeCStyleNumber(in_begin, in_end, out_begin, out_end))
				paletteIndex = PaletteIndex::Number;
			else if (TokenizeCStylePunctuation(in_begin, in_end, out_begin, out_end))
				paletteIndex = PaletteIndex::Punctuation;

			return paletteIndex != PaletteIndex::Max;
		};

		langDef.mCommentStart = "/*";
		langDef.mCommentEnd = "*/";
		langDef.mSingleLineComment = "//";

		langDef.mCaseSensitive = true;
		langDef.mAutoIndentation = true;

		langDef.mName = "C";

		inited = true;
	}
	return langDef;
}

const TextEditor::LanguageDefinition& TextEditor::LanguageDefinition::SQL()
{
	static bool inited = false;
	static LanguageDefinition langDef;
	if (!inited)
	{
		static const char* const keywords[] = {
			"ADD", "EXCEPT", "PERCENT", "ALL", "EXEC", "PLAN", "ALTER", "EXECUTE", "PRECISION", "AND", "EXISTS", "PRIMARY", "ANY", "EXIT", "PRINT", "AS", "FETCH", "PROC", "ASC", "FILE", "PROCEDURE",
			"AUTHORIZATION", "FILLFACTOR", "PUBLIC", "BACKUP", "FOR", "RAISERROR", "BEGIN", "FOREIGN", "READ", "BETWEEN", "FREETEXT", "READTEXT", "BREAK", "FREETEXTTABLE", "RECONFIGURE",
			"BROWSE", "FROM", "REFERENCES", "BULK", "FULL", "REPLICATION", "BY", "FUNCTION", "RESTORE", "CASCADE", "GOTO", "RESTRICT", "CASE", "GRANT", "RETURN", "CHECK", "GROUP", "REVOKE",
			"CHECKPOINT", "HAVING", "RIGHT", "CLOSE", "HOLDLOCK", "ROLLBACK", "CLUSTERED", "IDENTITY", "ROWCOUNT", "COALESCE", "IDENTITY_INSERT", "ROWGUIDCOL", "COLLATE", "IDENTITYCOL", "RULE",
			"COLUMN", "IF", "SAVE", "COMMIT", "IN", "SCHEMA", "COMPUTE", "INDEX", "SELECT", "CONSTRAINT", "INNER", "SESSION_USER", "CONTAINS", "INSERT", "SET", "CONTAINSTABLE", "INTERSECT", "SETUSER",
			"CONTINUE", "INTO", "SHUTDOWN", "CONVERT", "IS", "SOME", "CREATE", "JOIN", "STATISTICS", "CROSS", "KEY", "SYSTEM_USER", "CURRENT", "KILL", "TABLE", "CURRENT_DATE", "LEFT", "TEXTSIZE",
			"CURRENT_TIME", "LIKE", "THEN", "CURRENT_TIMESTAMP", "LINENO", "TO", "CURRENT_USER", "LOAD", "TOP", "CURSOR", "NATIONAL", "TRAN", "DATABASE", "NOCHECK", "TRANSACTION",
			"DBCC", "NONCLUSTERED", "TRIGGER", "DEALLOCATE", "NOT", "TRUNCATE", "DECLARE", "NULL", "TSEQUAL", "DEFAULT", "NULLIF", "UNION", "DELETE", "OF", "UNIQUE", "DENY", "OFF", "UPDATE",
			"DESC", "OFFSETS", "UPDATETEXT", "DISK", "ON", "USE", "DISTINCT", "OPEN", "USER", "DISTRIBUTED", "OPENDATASOURCE", "VALUES", "DOUBLE", "OPENQUERY", "VARYING","DROP", "OPENROWSET", "VIEW",
			"DUMMY", "OPENXML", "WAITFOR", "DUMP", "OPTION", "WHEN", "ELSE", "OR", "WHERE", "END", "ORDER", "WHILE", "ERRLVL", "OUTER", "WITH", "ESCAPE", "OVER", "WRITETEXT"
		};

		for (auto& k : keywords)
			langDef.mKeywords.insert(k);

		static const char* const identifiers[] = {
			"ABS",  "ACOS",  "ADD_MONTHS",  "ASCII",  "ASCIISTR",  "ASIN",  "ATAN",  "ATAN2",  "AVG",  "BFILENAME",  "BIN_TO_NUM",  "BITAND",  "CARDINALITY",  "CASE",  "CAST",  "CEIL",
			"CHARTOROWID",  "CHR",  "COALESCE",  "COMPOSE",  "CONCAT",  "CONVERT",  "CORR",  "COS",  "COSH",  "COUNT",  "COVAR_POP",  "COVAR_SAMP",  "CUME_DIST",  "CURRENT_DATE",
			"CURRENT_TIMESTAMP",  "DBTIMEZONE",  "DECODE",  "DECOMPOSE",  "DENSE_RANK",  "DUMP",  "EMPTY_BLOB",  "EMPTY_CLOB",  "EXP",  "EXTRACT",  "FIRST_VALUE",  "FLOOR",  "FROM_TZ",  "GREATEST",
			"GROUP_ID",  "HEXTORAW",  "INITCAP",  "INSTR",  "INSTR2",  "INSTR4",  "INSTRB",  "INSTRC",  "LAG",  "LAST_DAY",  "LAST_VALUE",  "LEAD",  "LEAST",  "LENGTH",  "LENGTH2",  "LENGTH4",
			"LENGTHB",  "LENGTHC",  "LISTAGG",  "LN",  "LNNVL",  "LOCALTIMESTAMP",  "LOG",  "LOWER",  "LPAD",  "LTRIM",  "MAX",  "MEDIAN",  "MIN",  "MOD",  "MONTHS_BETWEEN",  "NANVL",  "NCHR",
			"NEW_TIME",  "NEXT_DAY",  "NTH_VALUE",  "NULLIF",  "NUMTODSINTERVAL",  "NUMTOYMINTERVAL",  "NVL",  "NVL2",  "POWER",  "RANK",  "RAWTOHEX",  "REGEXP_COUNT",  "REGEXP_INSTR",
			"REGEXP_REPLACE",  "REGEXP_SUBSTR",  "REMAINDER",  "REPLACE",  "ROUND",  "ROWNUM",  "RPAD",  "RTRIM",  "SESSIONTIMEZONE",  "SIGN",  "SIN",  "SINH",
			"SOUNDEX",  "SQRT",  "STDDEV",  "SUBSTR",  "SUM",  "SYS_CONTEXT",  "SYSDATE",  "SYSTIMESTAMP",  "TAN",  "TANH",  "TO_CHAR",  "TO_CLOB",  "TO_DATE",  "TO_DSINTERVAL",  "TO_LOB",
			"TO_MULTI_BYTE",  "TO_NCLOB",  "TO_NUMBER",  "TO_SINGLE_BYTE",  "TO_TIMESTAMP",  "TO_TIMESTAMP_TZ",  "TO_YMINTERVAL",  "TRANSLATE",  "TRIM",  "TRUNC", "TZ_OFFSET",  "UID",  "UPPER",
			"USER",  "USERENV",  "VAR_POP",  "VAR_SAMP",  "VARIANCE",  "VSIZE "
		};
		for (auto& k : identifiers)
		{
			Identifier id;
			id.mDeclaration = "Built-in function";
			langDef.mIdentifiers.insert(std::make_pair(own_std::string(k), id));
		}

		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("L?\\\"(\\\\.|[^\\\"])*\\\"", PaletteIndex::String));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("\\\'[^\\\']*\\\'", PaletteIndex::String));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("[+-]?[0-9]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("0[0-7]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", PaletteIndex::Punctuation));

		langDef.mCommentStart = "/*";
		langDef.mCommentEnd = "*/";
		langDef.mSingleLineComment = "//";

		langDef.mCaseSensitive = false;
		langDef.mAutoIndentation = false;

		langDef.mName = "SQL";

		inited = true;
	}
	return langDef;
}

const TextEditor::LanguageDefinition& TextEditor::LanguageDefinition::AngelScript()
{
	static bool inited = false;
	static LanguageDefinition langDef;
	if (!inited)
	{
		static const char* const keywords[] = {
			"and", "abstract", "auto", "bool", "break", "case", "cast", "class", "const", "continue", "default", "do", "double", "else", "enum", "false", "final", "float", "for",
			"from", "funcdef", "function", "get", "if", "import", "in", "inout", "int", "interface", "int8", "int16", "int32", "int64", "is", "mixin", "namespace", "not",
			"null", "or", "out", "override", "private", "protected", "return", "set", "shared", "super", "switch", "this ", "true", "typedef", "uint", "uint8", "uint16", "uint32",
			"uint64", "void", "while", "xor"
		};

		for (auto& k : keywords)
			langDef.mKeywords.insert(k);

		static const char* const identifiers[] = {
			"cos", "sin", "tab", "acos", "asin", "atan", "atan2", "cosh", "sinh", "tanh", "log", "log10", "pow", "sqrt", "abs", "ceil", "floor", "fraction", "closeTo", "fpFromIEEE", "fpToIEEE",
			"complex", "opEquals", "opAddAssign", "opSubAssign", "opMulAssign", "opDivAssign", "opAdd", "opSub", "opMul", "opDiv"
		};
		for (auto& k : identifiers)
		{
			Identifier id;
			id.mDeclaration = "Built-in function";
			langDef.mIdentifiers.insert(std::make_pair(own_std::string(k), id));
		}

		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("L?\\\"(\\\\.|[^\\\"])*\\\"", PaletteIndex::String));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("\\'\\\\?[^\\']\\'", PaletteIndex::String));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("[+-]?[0-9]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("0[0-7]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", PaletteIndex::Punctuation));

		langDef.mCommentStart = "/*";
		langDef.mCommentEnd = "*/";
		langDef.mSingleLineComment = "//";

		langDef.mCaseSensitive = true;
		langDef.mAutoIndentation = true;

		langDef.mName = "AngelScript";

		inited = true;
	}
	return langDef;
}

const TextEditor::LanguageDefinition& TextEditor::LanguageDefinition::Lua()
{
	static bool inited = false;
	static LanguageDefinition langDef;
	if (!inited)
	{
		static const char* const keywords[] = {
			"and", "break", "do", "", "else", "elseif", "end", "false", "for", "function", "if", "in", "", "local", "nil", "not", "or", "repeat", "return", "then", "true", "until", "while"
		};

		for (auto& k : keywords)
			langDef.mKeywords.insert(k);

		static const char* const identifiers[] = {
			"assert", "collectgarbage", "dofile", "error", "getmetatable", "ipairs", "loadfile", "load", "loadstring",  "next",  "pairs",  "pcall",  "print",  "rawequal",  "rawlen",  "rawget",  "rawset",
			"select",  "setmetatable",  "tonumber",  "tostring",  "type",  "xpcall",  "_G",  "_VERSION","arshift", "band", "bnot", "bor", "bxor", "btest", "extract", "lrotate", "lshift", "replace",
			"rrotate", "rshift", "create", "resume", "running", "status", "wrap", "yield", "isyieldable", "debug","getuservalue", "gethook", "getinfo", "getlocal", "getregistry", "getmetatable",
			"getupvalue", "upvaluejoin", "upvalueid", "setuservalue", "sethook", "setlocal", "setmetatable", "setupvalue", "traceback", "close", "flush", "input", "lines", "open", "output", "popen",
			"read", "tmpfile", "type", "write", "close", "flush", "lines", "read", "seek", "setvbuf", "write", "__gc", "__tostring", "abs", "acos", "asin", "atan", "ceil", "cos", "deg", "exp", "tointeger",
			"floor", "fmod", "ult", "log", "max", "min", "modf", "rad", "random", "randomseed", "sin", "sqrt", "string", "tan", "type", "atan2", "cosh", "sinh", "tanh",
			"pow", "frexp", "ldexp", "log10", "pi", "huge", "maxinteger", "mininteger", "loadlib", "searchpath", "seeall", "preload", "cpath", "path", "searchers", "loaded", "module", "require", "clock",
			"date", "difftime", "execute", "exit", "getenv", "remove", "rename", "setlocale", "time", "tmpname", "byte", "char", "dump", "find", "format", "gmatch", "gsub", "len", "lower", "match", "rep",
			"reverse", "sub", "upper", "pack", "packsize", "unpack", "concat", "maxn", "insert", "pack", "unpack", "remove", "move", "sort", "offset", "codepoint", "char", "len", "codes", "charpattern",
			"coroutine", "table", "io", "os", "string", "utf8", "bit32", "math", "debug", "package"
		};
		for (auto& k : identifiers)
		{
			Identifier id;
			id.mDeclaration = "Built-in function";
			langDef.mIdentifiers.insert(std::make_pair(own_std::string(k), id));
		}

		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("L?\\\"(\\\\.|[^\\\"])*\\\"", PaletteIndex::String));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("\\\'[^\\\']*\\\'", PaletteIndex::String));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("[+-]?[0-9]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));
		langDef.mTokenRegexStrings.push_back(std::make_pair<own_std::string, PaletteIndex>("[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", PaletteIndex::Punctuation));

		langDef.mCommentStart = "--[[";
		langDef.mCommentEnd = "]]";
		langDef.mSingleLineComment = "--";

		langDef.mCaseSensitive = true;
		langDef.mAutoIndentation = false;

		langDef.mName = "Lua";

		inited = true;
	}
	return langDef;
}
