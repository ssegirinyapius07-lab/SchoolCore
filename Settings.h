#pragma once

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
        }

    private:
        Label^ lblTitle;
        Label^ lblSubtitle;
        Label^ lblStatus;
        Button^ btnClose;

        void InitializeComponent()
        {
            this->SuspendLayout();

            this->Text = L"SchoolCore - Settings";
            this->StartPosition = FormStartPosition::CenterParent;
            this->ClientSize = Drawing::Size(720, 460);
            this->MinimumSize = Drawing::Size(600, 400);
            this->BackColor = Color::White;
            this->ForeColor = Color::FromArgb(15, 23, 42);
            this->Font = gcnew Drawing::Font(L"Segoe UI", 10.0F);
            this->DoubleBuffered = true;

            TableLayoutPanel^ root = gcnew TableLayoutPanel();
            root->Dock = DockStyle::Fill;
            root->Padding = System::Windows::Forms::Padding(32);
            root->ColumnCount = 1;
            root->RowCount = 4;
            root->RowStyles->Add(gcnew RowStyle(SizeType::Absolute, 52));
            root->RowStyles->Add(gcnew RowStyle(SizeType::Absolute, 48));
            root->RowStyles->Add(gcnew RowStyle(SizeType::Percent, 100));
            root->RowStyles->Add(gcnew RowStyle(SizeType::Absolute, 58));

            lblTitle = gcnew Label();
            lblTitle->Text = L"Settings";
            lblTitle->Dock = DockStyle::Fill;
            lblTitle->Font = gcnew Drawing::Font(L"Segoe UI Semibold", 21.0F);
            lblTitle->ForeColor = Color::FromArgb(15, 23, 42);
            lblTitle->TextAlign = ContentAlignment::MiddleLeft;

            lblSubtitle = gcnew Label();
            lblSubtitle->Text = L"Application settings and configuration";
            lblSubtitle->Dock = DockStyle::Fill;
            lblSubtitle->ForeColor = Color::FromArgb(71, 85, 105);
            lblSubtitle->TextAlign = ContentAlignment::MiddleLeft;

            Panel^ content = gcnew Panel();
            content->Dock = DockStyle::Fill;
            content->Padding = System::Windows::Forms::Padding(20);
            content->BorderStyle = BorderStyle::FixedSingle;
            content->BackColor = Color::FromArgb(248, 250, 252);

            lblStatus = gcnew Label();
            lblStatus->Dock = DockStyle::Fill;
            lblStatus->Text =
                L"Appearance settings are currently using the standard light interface.";
            lblStatus->ForeColor = Color::FromArgb(71, 85, 105);
            lblStatus->Font = gcnew Drawing::Font(L"Segoe UI", 10.0F);
            lblStatus->TextAlign = ContentAlignment::TopLeft;

            content->Controls->Add(lblStatus);

            btnClose = gcnew Button();
            btnClose->Text = L"Close";
            btnClose->DialogResult = System::Windows::Forms::DialogResult::Cancel;
            btnClose->Dock = DockStyle::Right;
            btnClose->Width = 110;
            btnClose->Margin = System::Windows::Forms::Padding(8);

            root->Controls->Add(lblTitle, 0, 0);
            root->Controls->Add(lblSubtitle, 0, 1);
            root->Controls->Add(content, 0, 2);
            root->Controls->Add(btnClose, 0, 3);

            this->Controls->Add(root);

            this->AcceptButton = btnClose;
            this->CancelButton = btnClose;

            this->ResumeLayout(false);
        }
    };
}
