#pragma once

#define		CB_CLOSE		0
#define		CB_MINIMIZE		1
#define		CB_MAXIMIZE		2

namespace Win32 {
	class Caption
	{
	public:
		struct CaptionButton
		{
			std::wstring m_Text = L"";

			INT m_Command = -1;
			INT m_Width = 50;

			RECT m_Rect;

			CaptionButton(std::wstring text, INT command, INT width = 40)
			{
				m_Command = command;
				m_Text = text;
				m_Width = width;
			}
		};

	private:
		std::list<CaptionButton*> m_CaptionButtons;
		BOOL m_ShowTitle = TRUE;

		// Getters
	public:
		BOOL ShowTitle() { return m_ShowTitle; }
		std::list<CaptionButton*> CaptionButtons() { return m_CaptionButtons; }

		// Setters
	public:
		VOID ShowTitle(BOOL showtitle) { m_ShowTitle = showtitle; }

	public:
		VOID AddCaptionButton(CaptionButton* button);
	};

}
