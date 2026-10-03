#pragma once

#include "ThemeManager.h"

namespace SchoolCore
{
    using namespace System;
    using namespace System::Drawing;
    using namespace System::Windows::Forms;

    public ref class Settings : public Form
    {
    public:
        Settings()
        {
            InitializeComponent();
            ThemeManager::ApplyToForm(this);
        }

    private:
        Label^ lblTitle;
        Label^ lblSubtitle;
        Label^ lblTheme;
        Label^ lblCurrent;
        RadioButton^ rbSystem;
        RadioButton^ rbLight;
        RadioButton^ rbDark;
        Button^ btnApply;
        Button^ btnClose;

        void OnThemeChanged(Object^ sender, EventArgs^ e)
        {
            if (rbSystem->Checked)
                ThemeManager::SetMode(ThemeMode::System);
            else if (rbDark->Checked)
                ThemeManager::SetMode(ThemeMode::Dark);
            else
                ThemeManager::SetMode(ThemeMode::Light);

            ThemeManager::ApplyToOpenForms();
            lblCurrent->Text = L"Current theme: " + ThemeManager::GetModeName();
        }

        void InitializeComponent()
        {
            this->SuspendLayout();

            this->Text = L"SchoolCore - Settings";
            this->StartPosition = FormStartPosition::CenterParent;
            this->ClientSize = Drawing::Size(720, 520);
            this->MinimumSize = Drawing::Size(600, 460);
            this->BackColor = Color::White;
            this->Font = gcnew Drawing::Font(L"Segoe UI", 10.0F);
            this->DoubleBuffered = true;

            TableLayoutPanel^ root = gcnew TableLayoutPanel();
            root->Dock = DockStyle::Fill;
            root->Padding = System::Windows::Forms::Padding(32);
            root->ColumnCount = 1;
            root->RowCount = 5;
            root->RowStyles->Add(gcnew RowStyle(SizeType::Absolute, 48));
            root->RowStyles->Add(gcnew RowStyle(SizeType::Absolute, 48));
            root->RowStyles->Add(gcnew RowStyle(SizeType::Percent, 100));
            root->RowStyles->Add(gcnew RowStyle(SizeType::Absolute, 42));
            root->RowStyles->Add(gcnew RowStyle(SizeType::Absolute, 58));

            lblTitle = gcnew Label();
            lblTitle->Text = L"Settings";
            lblTitle->Dock = DockStyle::Fill;
            lblTitle->Font = gcnew Drawing::Font(L"Segoe UI Semibold", 21.0F);
            lblTitle->TextAlign = ContentAlignment::MiddleLeft;

            lblSubtitle = gcnew Label();
            lblSubtitle->Text = L"Application appearance and display preferences";
            lblSubtitle->Dock = DockStyle::Fill;
            lblSubtitle->ForeColor = Color::DimGray;
            lblSubtitle->TextAlign = ContentAlignment::MiddleLeft;

            Panel^ appearance = gcnew Panel();
            appearance->Dock = DockStyle::Fill;
            appearance->Padding = System::Windows::Forms::Padding(20);
            appearance->BorderStyle = BorderStyle::FixedSingle;

            lblTheme = gcnew Label();
            lblTheme->Text = L"Theme";
            lblTheme->Dock = DockStyle::Top;
            lblTheme->Height = 34;
            lblTheme->Font = gcnew Drawing::Font(L"Segoe UI Semibold", 12.0F);

            FlowLayoutPanel^ choices = gcnew FlowLayoutPanel();
            choices->Dock = DockStyle::Top;
            choices->Height = 70;
            choices->WrapContents = false;
            choices->FlowDirection = FlowDirection::LeftToRight;
            choices->Padding = System::Windows::Forms::Padding(0, 12, 0, 0);

            rbSystem = gcnew RadioButton();
            rbSystem->Text = L"System";
            rbSystem->AutoSize = true;
            rbSystem->Margin = System::Windows::Forms::Padding(0, 0, 28, 0);

            rbLight = gcnew RadioButton();
            rbLight->Text = L"Light";
            rbLight->AutoSize = true;
            rbLight->Margin = System::Windows::Forms::Padding(0, 0, 28, 0);

            rbDark = gcnew RadioButton();
            rbDark->Text = L"Dark";
            rbDark->AutoSize = true;

            choices->Controls->Add(rbSystem);
            choices->Controls->Add(rbLight);
            choices->Controls->Add(rbDark);

            lblCurrent = gcnew Label();
            lblCurrent->Dock = DockStyle::Fill;
            lblCurrent->TextAlign = ContentAlignment::MiddleLeft;
            lblCurrent->Padding = System::Windows::Forms::Padding(0, 10, 0, 0);

            appearance->Controls->Add(choices);
            appearance->Controls->Add(lblTheme);

            Panel^ footer = gcnew Panel();
            footer->Dock = DockStyle::Fill;

            btnClose = gcnew Button();
            btnClose->Text = L"Close";
            btnClose->DialogResult = System::Windows::Forms::DialogResult::Cancel;
            btnClose->Dock = DockStyle::Right;
            btnClose->Width = 110;
            btnClose->Margin = System::Windows::Forms::Padding(8);

            btnApply = gcnew Button();
            btnApply->Text = L"Apply";
            btnApply->Dock = DockStyle::Right;
            btnApply->Width = 110;
            btnApply->Margin = System::Windows::Forms::Padding(8);
            btnApply->Click += gcnew EventHandler(this, &Settings::OnThemeChanged);

            footer->Controls->Add(btnClose);
            footer->Controls->Add(btnApply);

            root->Controls->Add(lblTitle, 0, 0);
            root->Controls->Add(lblSubtitle, 0, 1);
            root->Controls->Add(appearance, 0, 2);
            root->Controls->Add(lblCurrent, 0, 3);
            root->Controls->Add(footer, 0, 4);

            this->Controls->Add(root);

            ThemeMode mode = ThemeManager::GetMode();
            rbSystem->Checked = mode == ThemeMode::System;
            rbLight->Checked = mode == ThemeMode::Light;
            rbDark->Checked = mode == ThemeMode::Dark;
            lblCurrent->Text = L"Current theme: " + ThemeManager::GetModeName();

            this->AcceptButton = btnApply;
            this->CancelButton = btnClose;

            this->ResumeLayout(false);
        }
    };
}
