#pragma once

#include <string>
#include <vector>
#include <array>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <regex>
#include "imgui.h"

int clamp(int a, int b, int c);
enum VIM_mode_enum
{
	VI_NORMAL,
	VI_INSERT,
	VI_CHANGE,
	VI_VISUAL,
	VI_LINE_VISUAL,
	VI_YANK,
	VI_CMD,
};
enum line_mode
{
	LINE_RELATIVE,
	LINE_ABS,
};

#define TEXT_ED_DONT_HAVE_CURSOR_FOCUS 1
#define TEXT_ED_ALLOW_ARRAW_NAVIGATION_EVEN_WHEN_NOT_FOCUS 2
class TextEditor
{
	typedef unsigned long long u64;
	typedef unsigned int u32;
	typedef unsigned char u8;
	typedef long long s64;
public:
	enum class PaletteIndex
	{
		Default,
		Keyword,
		Number,
		String,
		CharLiteral,
		Punctuation,
		Preprocessor,
		Identifier,
		KnownIdentifier,
		PreprocIdentifier,
		Comment,
		MultiLineComment,
		Background,
		Cursor,
		Selection,
		ErrorMarker,
		Breakpoint,
		LineNumber,
		CurrentLineFill,
		CurrentLineFillInactive,
		CurrentLineEdge,
		Max
	};
	enum class SelectionMode
	{
		Normal,
		Word,
		Line
	};

	struct Breakpoint
	{
		int mLine;
		bool mEnabled;
		std::string mCondition;

		Breakpoint()
			: mLine(-1)
			, mEnabled(false)
		{}
	};

	// Represents a character coordinate from the user's point of view,
	// i. e. consider an uniform grid (assuming fixed-width font) on the
	// screen as it is rendered, and each cell has its own coordinate, starting from 0.
	// Tabs are counted as [1..mTabSize] count empty spaces, depending on
	// how many space is necessary to reach the next tab stop.
	// For example, coordinate (1, 5) represents the character 'B' in a line "\tABC", when mTabSize = 4,
	// because it is rendered as "    ABC" on the screen.
	struct Coordinates
	{
		int mLine, mColumn;
		Coordinates() : mLine(0), mColumn(0) {}
		Coordinates(int aLine, int aColumn) : mLine(aLine), mColumn(aColumn)
		{
			assert(aLine >= 0);
			assert(aColumn >= 0);
		}
		static Coordinates Invalid() { static Coordinates invalid(-1, -1); return invalid; }

		bool operator ==(const Coordinates& o) const
		{
			return
				mLine == o.mLine &&
				mColumn == o.mColumn;
		}

		bool operator !=(const Coordinates& o) const
		{
			return
				mLine != o.mLine ||
				mColumn != o.mColumn;
		}

		bool operator <(const Coordinates& o) const
		{
			if (mLine != o.mLine)
				return mLine < o.mLine;
			return mColumn < o.mColumn;
		}

		bool operator >(const Coordinates& o) const
		{
			if (mLine != o.mLine)
				return mLine > o.mLine;
			return mColumn > o.mColumn;
		}

		bool operator <=(const Coordinates& o) const
		{
			if (mLine != o.mLine)
				return mLine < o.mLine;
			return mColumn <= o.mColumn;
		}

		bool operator >=(const Coordinates& o) const
		{
			if (mLine != o.mLine)
				return mLine > o.mLine;
			return mColumn >= o.mColumn;
		}
	};

	struct YankBuffer
	{
		char ch;
		std::string str;
	};
	struct GotoMark
	{
		char ch;
		Coordinates coor;
	};

	struct Identifier
	{
		Coordinates mLocation;
		std::string mDeclaration;
	};

	typedef std::string String;
	typedef std::unordered_map<std::string, Identifier> Identifiers;
	typedef std::unordered_set<std::string> Keywords;
	typedef std::map<int, std::string> ErrorMarkers;
	typedef std::unordered_set<int> Breakpoints;
	typedef std::array<ImU32, (unsigned)PaletteIndex::Max> Palette;
	typedef uint8_t Char;

	struct Glyph
	{
		Char mChar;
		PaletteIndex mColorIndex = PaletteIndex::Default;
		bool mComment : 1;
		bool mMultiLineComment : 1;
		bool mPreprocessor : 1;
		int color;
		int backgroundColor;

		Glyph(Char aChar, PaletteIndex aColorIndex) : mChar(aChar), mColorIndex(aColorIndex),
			mComment(false), mMultiLineComment(false), mPreprocessor(false) {
			color = 0xffffffff;
			backgroundColor = 0;
		}
	};

	typedef std::vector<Glyph> Line;
	typedef std::vector<Line> Lines;

	struct LanguageDefinition
	{
		typedef std::pair<std::string, PaletteIndex> TokenRegexString;
		typedef std::vector<TokenRegexString> TokenRegexStrings;
		typedef bool(*TokenizeCallback)(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end, PaletteIndex & paletteIndex);

		std::string mName;
		Keywords mKeywords;
		Identifiers mIdentifiers;
		Identifiers mPreprocIdentifiers;
		std::string mCommentStart, mCommentEnd, mSingleLineComment;
		char mPreprocChar;
		bool mAutoIndentation;

		TokenizeCallback mTokenize;

		TokenRegexStrings mTokenRegexStrings;

		bool mCaseSensitive;

		LanguageDefinition()
			: mPreprocChar('#'), mAutoIndentation(true), mTokenize(nullptr), mCaseSensitive(true)
		{
		}

		static const LanguageDefinition& CPlusPlus();
		static const LanguageDefinition& HLSL();
		static const LanguageDefinition& GLSL();
		static const LanguageDefinition& C();
		static const LanguageDefinition& SQL();
		static const LanguageDefinition& AngelScript();
		static const LanguageDefinition& Lua();
	};

	TextEditor();
	~TextEditor();

	void SetLanguageDefinition(const LanguageDefinition& aLanguageDef);
	const LanguageDefinition& GetLanguageDefinition() const { return mLanguageDefinition; }

	const Palette& GetPalette() const { return mPaletteBase; }
	void SetPalette(const Palette& aValue);

	
	void MoveToCharAtSamelevel(char target_ch)
	{
		int out_line, out_column;
		int cline = mState.mCursorPosition.mLine;
		int ccolumn = mState.mCursorPosition.mColumn;
		int in_column = GetCharacterIndex(Coordinates(cline, ccolumn));

		if(CheckMatchLevelsOfChar(target_ch, mState.mCursorPosition.mLine, in_column, &out_line, &out_column))
		{
			SetCursorPosition(Coordinates(out_line, out_column));
			auto a = 0;
		}
	}
	bool CheckMatchLevelsOfCharLogic(char target_ch, int line, int column, int *out_line, int *out_column, int *level)
	{
		auto& cur_line = mLines[line];
		if (cur_line.size() > 0)
		{
			for (int i = column; i >= 0; i--)
			{
				//int idx = GetCharacterIndex(Coordinates(line, i));
				Glyph* gl = &cur_line[i];

				switch (target_ch)
				{
				case '}':
				{
					if (gl->mChar == '{')
					{
						(*level)++;
					}
					else if (gl->mChar == '}')
					{
						if (*level == 0)
						{
							*out_line = line;
							*out_column = i;
							return true;
						}
						(*level)--;
					}
				}break;
				case '(':
				{
					if (gl->mChar == '(')
					{
						if (*level == 0)
						{
							*out_line = line;
							*out_column = i;
							return true;
						}
						(*level)++;
					}
					else if (gl->mChar == ')')
					{
						(*level)--;
					}
				}break;
				case '{':
				{
					if (gl->mChar == '{')
					{
						if (*level == 0)
						{
							*out_line = line;
							*out_column = i;
							return true;
						}
						(*level)++;
					}
					else if (gl->mChar == '}')
					{
						(*level)--;
					}
				}break;
				default:
					assert(false);
				}
			}
		}
		return false;
	}
	bool CheckMatchLevelsOfChar(char target_ch, int line, int column, int *out_line, int *out_column)
	{
		int max_column = mLines[line].size();
		column = clamp(column, 0, max_column - 1);
		
		int level = 0;
		switch(target_ch)
		{
		case '(':
		case '{':
		{
			while (line > 0)
			{
				if(CheckMatchLevelsOfCharLogic(target_ch, line, column, out_line, out_column, &level))
					return true;
				line--;
				int last_ch = GetLineMaxColumn(line);
				column = GetCharacterIndex(Coordinates(line, last_ch))-1;
			}
		}break;
		case '}':
		{
			while (line < mLines.size())
			{
				if(CheckMatchLevelsOfCharLogic('}', line, column, out_line, out_column, &level))
					return true;
				line++;
				int last_ch = GetLineMaxColumn(line);
				column = GetCharacterIndex(Coordinates(line, last_ch))-1;
			}
		}break;
		default:
			assert(false);
		}
		return false;
	}

	void ClearLines()
	{
		MoveBottom();
		MoveEnd();
		Coordinates end = mState.mCursorPosition;
		MoveTop();
		Coordinates start = mState.mCursorPosition;
		DeleteRange(start, end);
		mLines[0].clear();
	}

	void SetErrorMarkers(const ErrorMarkers& aMarkers) { mErrorMarkers = aMarkers; }
	void SetBreakpoints(const Breakpoints& aMarkers) { mBreakpoints = aMarkers; }
	void SetBreakpoint(int line) { mBreakpoints.insert(line); }
	bool HasBreakpoint(int line) { return mBreakpoints.count(line); }
	void RemoveBreakpoint(int line) { mBreakpoints.erase(line); }

	void Render(const char* aTitle, const ImVec2& aSize = ImVec2(), int flags = 0, bool aBorder = false);
	void SetText(const std::string& aText);
	std::string GetText() const;
	std::string GetText2(const Coordinates& aStart, const Coordinates& aEnd) const;

	void SetTextLines(const std::vector<std::string>& aLines);
	std::vector<std::string> GetTextLines() const;

	VIM_mode_enum GetVimMode();

	std::string GetSelectedText() const;
	std::string GetCurrentLineText()const;

	int GetCharacterIndex(const Coordinates& aCoordinates) const;
	int GetCharacterIndex2(const Coordinates& aCoordinates) const;

	int GetTotalLines() const { return (int)mLines.size(); }
	bool IsOverwrite() const { return mOverwrite; }

	void SetReadOnly(bool aValue);
	bool IsReadOnly() const { return mReadOnly; }
	bool IsTextChanged() const { return mTextChanged; }
	bool IsCursorPositionChanged() const { return mCursorPositionChanged; }

	bool IsColorizerEnabled() const { return mColorizerEnabled; }
	void SetColorizerEnable(bool aValue);

	Coordinates GetCursorPosition() const { return GetActualCursorCoordinates(); }
	void SetCursorPosition(const Coordinates& aPosition);

	inline void SetHandleMouseInputs    (bool aValue){ mHandleMouseInputs    = aValue;}
	inline bool IsHandleMouseInputsEnabled() const { return mHandleKeyboardInputs; }

	inline void SetHandleKeyboardInputs (bool aValue){ mHandleKeyboardInputs = aValue;}
	inline bool IsHandleKeyboardInputsEnabled() const { return mHandleKeyboardInputs; }

	inline void SetImGuiChildIgnored    (bool aValue){ mIgnoreImGuiChild     = aValue;}
	inline bool IsImGuiChildIgnored() const { return mIgnoreImGuiChild; }

	inline void SetShowWhitespaces(bool aValue) { mShowWhitespaces = aValue; }
	inline bool IsShowingWhitespaces() const { return mShowWhitespaces; }

	void SetTabSize(int aValue);
	inline int GetTabSize() const { return mTabSize; }
	void InsertLineAddIndent(int line, int indentOfLine);
	inline int GetLineIndent(int line) const 
	{ 
		line = clamp(line, 0, mLines.size() - 1);
		int tab = 0;

		for(int i =0; i < mLines[line].size();i++)
		{
			if (mLines[line][i].mChar == '\t')
				tab++;
			else
				break;
		}
		return tab;
	}

	void InsertText(const std::string& aValue);
	void InsertText(const char* aValue);

	void MoveUp(int aAmount = 1, bool aSelect = false);
	void MoveDown(int aAmount = 1, bool aSelect = false);
	void MoveLeft(int aAmount = 1, bool aSelect = false, bool aWordMode = false);
	void MoveRight(int aAmount = 1, bool aSelect = false, bool aWordMode = false);
	void MoveTop(bool aSelect = false);
	void MoveBottom(bool aSelect = false);
	void MoveHome(bool aSelect = false);
	void MoveEnd(bool aSelect = false);
	void MoveEndOfLine();

	void SetSelectionStart(const Coordinates& aPosition);
	void SetSelectionEnd(const Coordinates& aPosition);
	void SetSelection(const Coordinates& aStart, const Coordinates& aEnd, SelectionMode aMode = SelectionMode::Normal);
	void SelectWordUnderCursor();
	void SelectAll();
	bool HasSelection() const;


	void DeleteRange(const Coordinates& aStart, const Coordinates& aEnd, bool makeUndo = true);
	void DeleteRange2(const Coordinates& aStart, Coordinates& aEnd);
	int InsertTextAt(Coordinates& aWhere, const char* aValue);

	void Copy();
	void Cut();
	void Paste();
	void Paste(std::string);
	void Delete();

	bool CanUndo() const;
	bool CanRedo() const;
	void Undo(int aSteps = 1);
	void Redo(int aSteps = 1);

	static const Palette& GetDarkPalette();
	static const Palette& GetLightPalette();
	static const Palette& GetRetroBluePalette();

	void FromVisualToNormalMode(YankBuffer *yb)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto shift = io.KeyShift;
		auto ctrl = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
		auto alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;
		if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGuiKey_V))
		{
			mVimMode = VI_NORMAL;
			mInteractiveEnd = mInteractiveStart;
			SetSelection(mInteractiveStart, mInteractiveStart);
		}
		else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_D))
		{
			Delete();
			mInteractiveEnd = mInteractiveStart;
			SetSelection(mInteractiveStart, mInteractiveStart);
			if(mVimMode == VI_LINE_VISUAL)
			{
				RemoveLine(mStartLineVisual.mLine);
				SetCursorPosition(mStartLineVisual);
			}
			mVimMode = VI_NORMAL;
			//RemoveLine(mState.mSelectionStart.mLine, mState.mSelectionEnd.mLine);
		}
		else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Escape))
		{
			mVimMode = VI_NORMAL;
			mInteractiveEnd = mInteractiveStart;
			SetSelection(mInteractiveStart, mInteractiveStart);
		}
	}

	std::string GetWordUnderCursor() const;

	void ClearSearchStringHighlight()
	{
		int cur_line = 0;
		
		
		for(int matched = 0; matched < matchedStrings.size();)
		{
			SearchStringPos* m = &matchedStrings[matched];

			int matchedLine = m->pos.mLine;
			auto& line = mLines[matchedLine];
			int lsz = line.size();
			for(int i= 0; i < lsz; i++)
			{
				if (line[i].backgroundColor == selectedSearch)
					line[i].backgroundColor = 0;
			}


			do
			{
				matched++;
				m++;
			} while (m->pos.mLine == matchedLine);

		}
	}

	bool SearchStringRange(std::string str, int *line, int *column, int start = -1, int end = -1)
	{
#define FOR_VEC(a, vec) for(auto a = (vec).begin(); a < (vec).end(); a++)
		searchWord = false;
		if(str[0]=='/')
		{
			str = str.substr(1);
			searchWord = true;
		}
		int line_idx = 0;
		matchedStrings.clear();
		if (start == -1)
			start = 0;
		if (end == -1)
			end = mLines.size();

		bool found = false;
		int str_sz = str.size();
		for(int line_idx = start; line_idx < end; line_idx++)
		{
			auto l = &mLines[line_idx];
			int lsz = l->size();
			int cur_column = 0;
			if (lsz != 0)
			{
				while (true)
				{
					if (SearchStringInLine(str, 0, line_idx, cur_column, column) && cur_column < lsz)
					{
						*line = line_idx;
						SearchStringPos matched;
						matched.pos.mLine = line_idx;
						matched.pos.mColumn = GetCharacterColumn(line_idx, *column);

						for (int i = *column; i < (*column + str_sz); i++)
						{
							(*l)[i].backgroundColor = selectedSearch;
						}

						matchedStrings.emplace_back(matched);
						found = true;
						cur_column = *column + str_sz;

					}
					else
						break;
				}
			}
		}
		if (found)
		{
			int i = 0;
			FOR_VEC(matched, matchedStrings)
			{
				if (matched->pos.mLine >= mState.mCursorPosition.mLine)
					break;
				i++;
			}
			i = i % matchedStrings.size();

			//int ccolumn = ed->GetCharacterColumn(line, column);
			SetCursorPosition(matchedStrings[i].pos);
		}
		return found;
	}
	u64 GetStrHash(char *str, int sz)
	{
		u64 ret = 0;
		for(int i = 0; i< sz; i++)
		{
			
		}
		return ret;
	}
	bool CanBeWord(char ch)
	{
		return IsLetter(ch) || IsNumber(ch) || ch == '_';
	}
	bool SearchStringInLine(std::string str, u64 str_hash, int line, int column_start, int *column)
	{
		char buffer[256];

		auto l = &mLines[line];
		int lsz = l->size();
		int start_word = 0;
		int str_sz = str.size();
		char* str_data = (char *)str.data();
		if (str_sz == 0 && searchWord)
			str_sz = 1;

		Glyph* g = &(*l)[0];
		for (int i = column_start; i < lsz; i++)
		{
			if (g[i].backgroundColor == selectedSearch)
				g[i].backgroundColor = 0;

			buffer[i] = g[i].mChar;
		}

		for (int i = column_start; i < (lsz - str_sz); i++)
		{
			if(memcmp(buffer + i, str_data, str_sz)==0)
			{
				if(searchWord)
				{
					if(i > 0)
					{
						bool canBeWord = CanBeWord(g[i - 1].mChar);
						if (canBeWord)
							continue;
					}
					if((i + str_sz) < lsz)
					{
						bool canBeWord = CanBeWord(g[i + str_sz].mChar);
						if (canBeWord)
							continue;
					}
				}
				*column = i;
				i += str_sz;
				return true;
			}
		}
		return false;
	}

	int GetLineMaxColumn(int aLine) const;
	std::string GetText(const Coordinates& aStart, const Coordinates& aEnd) const;

	line_mode lnMode;
	std::string insertBuffer;
	bool firstChInInsertBufferIsSlash;
	bool haveKeyboardFocusAnyway;

	struct EditorState
	{
		Coordinates mSelectionStart;
		Coordinates mSelectionEnd;
		Coordinates mCursorPosition;
	};
	EditorState mState;
	Lines mLines;

	std::string cmdBuffer;
	VIM_mode_enum mVimMode;
	void* data;
	char lastInsertedChar;

	ImVec2 mCursorScreenPos;
	


//private:
	typedef std::vector<std::pair<std::regex, PaletteIndex>> RegexList;


	class UndoRecord
	{
	public:
		UndoRecord() {}
		~UndoRecord() {}

		UndoRecord(
			const std::string& aAdded,
			const TextEditor::Coordinates aAddedStart,
			const TextEditor::Coordinates aAddedEnd,

			const std::string& aRemoved,
			const TextEditor::Coordinates aRemovedStart,
			const TextEditor::Coordinates aRemovedEnd,

			TextEditor::EditorState& aBefore,
			TextEditor::EditorState& aAfter);

		void Undo(TextEditor* aEditor);
		void Redo(TextEditor* aEditor);

		std::string mAdded;
		Coordinates mAddedStart;
		Coordinates mAddedEnd;

		std::string mRemoved;
		Coordinates mRemovedStart;
		Coordinates mRemovedEnd;

		EditorState mBefore;
		EditorState mAfter;
	};
	struct SearchStringPos
	{
		Coordinates pos;
	};

	typedef std::vector<UndoRecord> UndoBuffer;

	void ProcessInputs();
	void Colorize(int aFromLine = 0, int aCount = -1);
	void ColorizeLine(int &line, int &column);
	void ColorizeRange(int aFromLine = 0, int aToLine = 0);
	void ColorizeInternal();
	float TextDistanceToLineStart(const Coordinates& aFrom) const;
	void EnsureCursorVisible();
	int GetPageSize() const;
	Coordinates GetActualCursorCoordinates() const;
	Coordinates SanitizeCoordinates(const Coordinates& aValue) const;
	void Advance(Coordinates& aCoordinates) const;
	void GoBackOne(Coordinates& aCoordinates) const;
	void AddUndo(UndoRecord& aValue);
	Coordinates ScreenPosToCoordinates(const ImVec2& aPosition) const;
	Coordinates FindWordStart(const Coordinates& aFrom) const;
	Coordinates FindWordEnd(const Coordinates& aFrom) const;
	Coordinates FindNextWord(const Coordinates& aFrom) const;
	Coordinates TextEditor::FindPrevWordStart(const Coordinates& aFrom) const;
	int GetCharacterColumn(int aLine, int aIndex) const;
	int GetLineCharacterCount(int aLine) const;
	bool IsOnWordBoundary(const Coordinates& aAt) const;
	void RemoveLine(int aStart, int aEnd);
	void RemoveLine(int aIndex);
	Line& InsertLine(int aIndex);
	void EnterCharacter(ImWchar aChar, bool aShift);
	void Backspace();
	void BasicMovs(int);

	YankBuffer *GetYankBuffer(char ch)
	{
		YankBuffer* found = nullptr;
		switch(ch)
		{
		case 'a':
		{
			return &yank[0];
		}break;
		case 'o':
		{
			return &yank[1];
		}break;
		case 'l':
		{
			return &yank[2];
		}break;
		case 'e':
		{
			return &yank[4];
		}break;
		case 'h':
		{
			return &yank[5];
		}break;
		case '\0':
		{
			return &yank[6];
		}break;
		default:
			assert(0);
		}
	}

	char OppositeLevelCharacter(char ch)
	{
		switch (ch)
		{
		case '{':
			return'}';
		break;
		case '}':
			return'{';
		break;
		case '(':
			return')';
		break;
		case ')':
			return'(';
		break;

		}
	}
	void SelectInnerLevel(char ch, Coordinates coor)
	{
		int start_line, start_column, end_line, end_column;
		int level = 0;
		bool has_start = CheckMatchLevelsOfChar(ch, coor.mLine, coor.mColumn, &start_line, &start_column);
		char opposite = OppositeLevelCharacter(ch);
		bool has_end = CheckMatchLevelsOfChar(opposite, coor.mLine, coor.mColumn, &end_line, &end_column);

		if(has_start && has_end)
		{
			Coordinates start(start_line, start_column);
			Coordinates end(end_line, end_column);
			Advance(start);
			GoBackOne(end);
			SetSelectionStart(start);
			SetSelectionEnd(end);
		}
	}
	void DeleteSelection();
	std::string GetWordAt(const Coordinates& aCoords) const;
	ImU32 GetGlyphColor(const Glyph& aGlyph) const;

	void HandleKeyboardInputs();
	void HandleMouseInputs();
	void Render();

	float mLineSpacing;

	UndoBuffer mUndoBuffer;
	UndoBuffer mUndoBuffer2;
	int mUndoIndex;
	std::string auxInsertBuffer;

	int mTabSize;
	bool mOverwrite;
	bool mReadOnly;
	bool mWithinRender;
	bool mScrollToCursor;
	bool mScrollToTop;
	bool mTextChanged;
	bool mColorizerEnabled;
	float mTextStart;                   // position (in pixels) where a code line starts relative to the left of the TextEditor.
	int  mLeftMargin;
	bool mCursorPositionChanged;
	int mColorRangeMin, mColorRangeMax;
	SelectionMode mSelectionMode;
	bool mHandleKeyboardInputs;
	bool mHandleMouseInputs;
	bool mIgnoreImGuiChild;
	bool mShowWhitespaces;
	bool hasCursorFocus;

	Palette mPaletteBase;
	Palette mPalette;
	LanguageDefinition mLanguageDefinition;
	RegexList mRegexList;
	Coordinates originalCPosBeforeSearchString;

	std::vector<SearchStringPos> matchedStrings;

	bool mCheckComments;
	Breakpoints mBreakpoints;
	ErrorMarkers mErrorMarkers;
	ImVec2 mCharAdvance;
	Coordinates mInteractiveStart, mInteractiveEnd, mStartLineVisual;
	std::string mLineBuffer;
	uint64_t mStartTime;

	std::vector<GotoMark> gotoMarks;
	YankBuffer yank[8];

	int selectedSearch = 0xff000066;
	int commentColor = 0xff007700;
	bool IsOpenCommentBlock(int i, Line& l);
	bool IsCloseCommentBlock(int i, Line& l);

	bool searchWord;
	bool isQuotes;

	float mLastClick;
	bool IsLetter(char c)
	{
		return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z';
	}
	bool IsNumber(char c)
	{
		return c >= '0' && c <= '9';
	}
};
