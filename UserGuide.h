#pragma once

#include "ThemeManager.h"

namespace SchoolCore
{
    using namespace System;
    using namespace System::Drawing;
    using namespace System::Windows::Forms;

    public ref class UserGuide : public Form
    {
    public:
        UserGuide(void)
        {
            InitializeComponent();
            ThemeManager::ApplyToForm(this);
            ApplyGuideStyles();
            UpdateGuideWidths();
        }

    private:
        TableLayoutPanel^ rootLayout;
        Panel^ headerPanel;
        Label^ lblTitle;
        Label^ lblSubtitle;
        Panel^ guideScrollPanel;
        FlowLayoutPanel^ guideList;
        Panel^ footerPanel;
        Button^ btnBack;

        Panel^ CreateGuideSection(
            String^ headingText,
            String^ bodyText,
            int sectionHeight)
        {
            Panel^ section = gcnew Panel();
            section->Name = L"guideSection";
            section->Width = 880;
            section->Height = sectionHeight;
            section->Margin = System::Windows::Forms::Padding(0, 0, 0, 10);
            section->Padding = System::Windows::Forms::Padding(12);
            section->BackColor = Color::White;
            section->BorderStyle = BorderStyle::FixedSingle;

            Label^ heading = gcnew Label();
            heading->Name = L"guideTitle";
            heading->Text = headingText;
            heading->Location = Point(12, 7);
            heading->Size = Drawing::Size(820, 24);
            heading->Font = gcnew Drawing::Font(
                L"Segoe UI Semibold", 10.0F, FontStyle::Bold);
            heading->ForeColor = Color::FromArgb(30, 41, 59);
            heading->TextAlign = ContentAlignment::MiddleLeft;
            heading->Anchor = AnchorStyles::Top | AnchorStyles::Left | AnchorStyles::Right;

            Label^ body = gcnew Label();
            body->Name = L"guideBody";
            body->Text = bodyText;
            body->Location = Point(12, 33);
            body->Size = Drawing::Size(820, sectionHeight - 43);
            body->Font = gcnew Drawing::Font(
                L"Segoe UI", 9.0F, FontStyle::Regular);
            body->ForeColor = Color::FromArgb(71, 85, 105);
            body->TextAlign = ContentAlignment::TopLeft;
            body->Anchor =
                AnchorStyles::Top |
                AnchorStyles::Left |
                AnchorStyles::Right |
                AnchorStyles::Bottom;

            section->Controls->Add(body);
            section->Controls->Add(heading);
            return section;
        }

        void ApplyGuideStyles()
        {
            this->BackColor = Color::FromArgb(248, 250, 252);
            this->ForeColor = Color::FromArgb(15, 23, 42);

            this->headerPanel->BackColor = Color::FromArgb(30, 41, 59);
            this->lblTitle->ForeColor = Color::White;
            this->lblSubtitle->ForeColor = Color::FromArgb(226, 232, 240);

            for each (Control^ item in this->guideList->Controls)
            {
                Panel^ section = dynamic_cast<Panel^>(item);
                if (section == nullptr)
                    continue;

                section->BackColor = Color::White;

                for each (Control^ child in section->Controls)
                {
                    Label^ label = dynamic_cast<Label^>(child);
                    if (label == nullptr)
                        continue;

                    if (String::Equals(label->Name, L"guideTitle"))
                    {
                        label->ForeColor = Color::FromArgb(30, 41, 59);
                        label->Font = gcnew Drawing::Font(
                            L"Segoe UI Semibold", 10.0F, FontStyle::Bold);
                    }
                    else
                    {
                        label->ForeColor = Color::FromArgb(71, 85, 105);
                        label->Font = gcnew Drawing::Font(
                            L"Segoe UI", 9.0F, FontStyle::Regular);
                    }
                }
            }

            this->btnBack->BackColor = Color::FromArgb(29, 78, 216);
            this->btnBack->ForeColor = Color::White;
            this->btnBack->FlatStyle = FlatStyle::Flat;
            this->btnBack->FlatAppearance->BorderSize = 0;
        }

        void UpdateGuideWidths()
        {
            if (this->guideScrollPanel == nullptr ||
                this->guideList == nullptr)
            {
                return;
            }

            int availableWidth = Math::Max(
                360,
                this->guideScrollPanel->ClientSize.Width - 24);

            this->guideList->Width = availableWidth;

            for each (Control^ item in this->guideList->Controls)
            {
                Panel^ section = dynamic_cast<Panel^>(item);
                if (section == nullptr)
                    continue;

                section->Width = Math::Max(340, availableWidth - 12);

                for each (Control^ child in section->Controls)
                {
                    Label^ label = dynamic_cast<Label^>(child);
                    if (label == nullptr)
                        continue;

                    label->Width = Math::Max(
                        300,
                        section->ClientSize.Width - 44);
                }
            }
        }

        System::Void GuideScrollPanel_Resize(
            Object^ sender,
            EventArgs^ e)
        {
            UpdateGuideWidths();
        }

        System::Void BackClicked(
            Object^ sender,
            EventArgs^ e)
        {
            this->Close();
        }

        void InitializeComponent(void)
        {
            this->SuspendLayout();

            this->Text = L"SchoolCore - User Guide";
            this->StartPosition = FormStartPosition::CenterParent;
            this->ClientSize = Drawing::Size(1050, 700);
            this->MinimumSize = Drawing::Size(760, 540);
            this->BackColor = Color::FromArgb(248, 250, 252);
            this->Font = gcnew Drawing::Font(L"Segoe UI", 9.5F);
            this->DoubleBuffered = true;

            this->rootLayout = gcnew TableLayoutPanel();
            this->rootLayout->Dock = DockStyle::Fill;
            this->rootLayout->Padding = System::Windows::Forms::Padding(18);
            this->rootLayout->ColumnCount = 1;
            this->rootLayout->RowCount = 3;
            this->rootLayout->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Percent, 100.0F));
            this->rootLayout->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 78.0F));
            this->rootLayout->RowStyles->Add(
                gcnew RowStyle(SizeType::Percent, 100.0F));
            this->rootLayout->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 54.0F));
            this->rootLayout->BackColor = Color::FromArgb(248, 250, 252);

            this->headerPanel = gcnew Panel();
            this->headerPanel->Dock = DockStyle::Fill;
            this->headerPanel->Padding = System::Windows::Forms::Padding(18, 8, 18, 6);
            this->headerPanel->BackColor = Color::FromArgb(30, 41, 59);

            this->lblTitle = gcnew Label();
            this->lblTitle->Text = L"SchoolCore User Guide";
            this->lblTitle->Dock = DockStyle::Top;
            this->lblTitle->Height = 34;
            this->lblTitle->Font = gcnew Drawing::Font(
                L"Segoe UI Semibold", 18.0F, FontStyle::Bold);
            this->lblTitle->ForeColor = Color::White;
            this->lblTitle->TextAlign = ContentAlignment::MiddleLeft;

            this->lblSubtitle = gcnew Label();
            this->lblSubtitle->Text =
                L"Quick instructions for the main school administration tasks.";
            this->lblSubtitle->Dock = DockStyle::Fill;
            this->lblSubtitle->Font = gcnew Drawing::Font(
                L"Segoe UI", 9.5F, FontStyle::Regular);
            this->lblSubtitle->ForeColor = Color::FromArgb(226, 232, 240);
            this->lblSubtitle->TextAlign = ContentAlignment::MiddleLeft;

            this->headerPanel->Controls->Add(this->lblSubtitle);
            this->headerPanel->Controls->Add(this->lblTitle);

            this->guideScrollPanel = gcnew Panel();
            this->guideScrollPanel->Dock = DockStyle::Fill;
            this->guideScrollPanel->AutoScroll = true;
            this->guideScrollPanel->BackColor = Color::FromArgb(248, 250, 252);
            this->guideScrollPanel->Padding =
                System::Windows::Forms::Padding(4, 12, 4, 8);
            this->guideScrollPanel->Resize +=
                gcnew EventHandler(
                    this, &UserGuide::GuideScrollPanel_Resize);

            this->guideList = gcnew FlowLayoutPanel();
            this->guideList->Dock = DockStyle::Top;
            this->guideList->FlowDirection = FlowDirection::TopDown;
            this->guideList->WrapContents = false;
            this->guideList->AutoSize = true;
            this->guideList->AutoSizeMode = AutoSizeMode::GrowAndShrink;
            this->guideList->Padding = System::Windows::Forms::Padding(0);
            this->guideList->Margin = System::Windows::Forms::Padding(0);
            this->guideList->BackColor = Color::FromArgb(248, 250, 252);

            this->guideList->Controls->Add(CreateGuideSection(
                L"1. Set up the academic year",
                L"Open Academic Years & Terms to create the school year and its terms. Confirm that the correct year and term are active before entering current school records.",
                84));
            this->guideList->Controls->Add(CreateGuideSection(
                L"2. Register and manage students",
                L"Register each student once. Enter their names and personal details, then choose the correct academic year, term, level, class, stream and session. Use Manage Students to search, edit or promote an existing record instead of registering the same student again.",
                96));
            this->guideList->Controls->Add(CreateGuideSection(
                L"3. Set up teachers, classes and streams",
                L"Create teacher records before assigning teachers to lessons. Set up the academic level, classes and streams first; then use those existing records in enrolment, attendance and timetable screens.",
                84));
            this->guideList->Controls->Add(CreateGuideSection(
                L"4. Manage subjects and UNEB setup",
                L"Add each school subject once in Subjects. Use Curriculum Requirements to link subjects to a curriculum. For UCE CBC O-Level, Load UCE Core Codes prepares the seven core mappings and can safely be run again. Configure examination papers separately using confirmed official requirements.",
                96));
            this->guideList->Controls->Add(CreateGuideSection(
                L"5. Record attendance",
                L"Select the academic year, term, class, stream and date before loading students. Review the list, mark attendance, and save. If no students appear, check their active enrolments and the selected class, stream and year.",
                84));
            this->guideList->Controls->Add(CreateGuideSection(
                L"6. Prepare the timetable",
                L"Select the academic year, class and stream, then assign the subject and teacher for each lesson. If a teacher or subject is missing, check that the record is active and correctly linked before editing the timetable.",
                84));
            this->guideList->Controls->Add(CreateGuideSection(
                L"7. Examinations and results",
                L"Create the examination and check its subject setup before entering marks. Verify scores and the selected class, term and year before processing or sharing results. Correct source marks before relying on a report.",
                84));
            this->guideList->Controls->Add(CreateGuideSection(
                L"8. Fees and finance",
                L"Set up the fee structure, assign charges to the correct students, and record payments against the right student. Select the actual payment method and include the transaction reference when one is available. Review balances and payment history after saving.",
                90));
            this->guideList->Controls->Add(CreateGuideSection(
                L"9. Reports",
                L"Choose the report, set its filters and select Generate. Check the academic period and class filters before exporting results. Reports reflect the records and permissions available to the signed-in account.",
                84));
            this->guideList->Controls->Add(CreateGuideSection(
                L"10. Users and permissions",
                L"Give each staff account its own login and only the permissions required for its duties. If a menu or action is not visible, ask an administrator to review the account's role and permissions.",
                84));
            this->guideList->Controls->Add(CreateGuideSection(
                L"Keyboard and troubleshooting tips",
                L"Use Tab to move through input fields and Shift+Tab to move backward. Check active year, term, class and stream when lists are empty. Refresh after changing setup, and read the full validation message before trying again.",
                84));

            this->guideScrollPanel->Controls->Add(this->guideList);

            this->footerPanel = gcnew Panel();
            this->footerPanel->Dock = DockStyle::Fill;
            this->footerPanel->BackColor = Color::FromArgb(248, 250, 252);

            this->btnBack = gcnew Button();
            this->btnBack->Text = L"Back to Dashboard";
            this->btnBack->Size = Drawing::Size(170, 40);
            this->btnBack->Dock = DockStyle::Right;
            this->btnBack->Font = gcnew Drawing::Font(
                L"Segoe UI Semibold", 9.5F, FontStyle::Bold);
            this->btnBack->Click +=
                gcnew EventHandler(this, &UserGuide::BackClicked);
            this->footerPanel->Controls->Add(this->btnBack);

            this->rootLayout->Controls->Add(this->headerPanel, 0, 0);
            this->rootLayout->Controls->Add(this->guideScrollPanel, 0, 1);
            this->rootLayout->Controls->Add(this->footerPanel, 0, 2);

            this->Controls->Add(this->rootLayout);
            this->ResumeLayout(false);
        }
    };
}
