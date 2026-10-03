#pragma once

#include "AuthSession.h"
#include "ThemeManager.h"

namespace SchoolCore
{
    using namespace System;
    using namespace System::Drawing;
    using namespace System::Windows::Forms;

    private ref class FinanceTheme abstract sealed
    {
    public:
        static property Color Surface { Color get() { return Color::FromArgb(248, 250, 252); } }
        static property Color Hover { Color get() { return Color::FromArgb(241, 245, 249); } }
        static property Color Pressed { Color get() { return Color::FromArgb(226, 232, 240); } }
        static property Color TextStrong { Color get() { return Color::FromArgb(15, 23, 42); } }
        static property Color TextHeading { Color get() { return Color::FromArgb(30, 41, 59); } }
        static property Color TextMuted { Color get() { return Color::FromArgb(71, 85, 105); } }

        static initonly Drawing::Font^ Title = gcnew Drawing::Font(L"Segoe UI Semibold", 21.0F);
        static initonly Drawing::Font^ Dialog = gcnew Drawing::Font(L"Segoe UI Semibold", 18.0F);
        static initonly Drawing::Font^ Section = gcnew Drawing::Font(L"Segoe UI Semibold", 13.0F);
        static initonly Drawing::Font^ Metric = gcnew Drawing::Font(L"Segoe UI Semibold", 18.0F);
        static initonly Drawing::Font^ Action = gcnew Drawing::Font(L"Segoe UI Semibold", 10.0F);
        static initonly Drawing::Font^ Body = gcnew Drawing::Font(L"Segoe UI", 10.0F);
        static initonly Drawing::Font^ Small = gcnew Drawing::Font(L"Segoe UI", 9.0F);
    };

    private ref class FinanceOperation sealed
    {
    public:
        FinanceOperation(String^ title, String^ description)
            : Title(title), Description(description) {
        }

        initonly String^ Title;
        initonly String^ Description;
    };

    public ref class Finance : public Form
    {
    public:
        Finance()
        {
            InitializeComponent();
                            ThemeManager::ApplyToForm(this);
                    }

    private:
        literal String^ PermView = L"fees.view";
        literal int MetricCount = 4;

        array<Label^>^ metricValues;

        static Label^ CreateLabel(String^ text, Drawing::Font^ font, Color color)
        {
            Label^ label = gcnew Label();
            label->Text = text;
            label->Font = font;
            label->ForeColor = color;
            label->UseMnemonic = false;
            label->AutoEllipsis = true;
            return label;
        }

        static Panel^ CreateHeader(String^ title, String^ subtitle,
            Drawing::Font^ titleFont, int height, int titleHeight)
        {
            Panel^ header = gcnew Panel();
            header->Dock = DockStyle::Top;
            header->Height = height;
            header->BackColor = FinanceTheme::Surface;
            header->Padding = System::Windows::Forms::Padding(28, 18, 28, 12);

            Label^ heading = CreateLabel(title, titleFont, FinanceTheme::TextStrong);
            heading->Dock = DockStyle::Top;
            heading->Height = titleHeight;

            Label^ description = CreateLabel(subtitle, FinanceTheme::Body, FinanceTheme::TextMuted);
            description->Dock = DockStyle::Fill;
            description->AutoEllipsis = false;

            header->Controls->Add(description);
            header->Controls->Add(heading);
            return header;
        }

        Panel^ CreateMetricCard(String^ name, int index)
        {
            Panel^ card = gcnew Panel();
            card->Dock = DockStyle::Fill;
            card->Margin = System::Windows::Forms::Padding(0, 0, 12, 10);
            card->Padding = System::Windows::Forms::Padding(15, 12, 15, 12);
            card->BackColor = FinanceTheme::Surface;
            card->BorderStyle = BorderStyle::FixedSingle;

            Label^ caption = CreateLabel(name, FinanceTheme::Small, FinanceTheme::TextMuted);
            caption->Dock = DockStyle::Top;
            caption->Height = 28;

            Label^ value = CreateLabel(L"UGX 0.00", FinanceTheme::Metric, FinanceTheme::TextStrong);
            value->Dock = DockStyle::Fill;
            value->TextAlign = ContentAlignment::MiddleLeft;
            value->AccessibleName = name;

            metricValues[index] = value;

            card->Controls->Add(value);
            card->Controls->Add(caption);
            return card;
        }

        Button^ CreateActionButton(FinanceOperation^ operation, String^ name)
        {
            Button^ button = gcnew Button();
            button->Name = name;
            button->Text = operation->Title;
            button->Tag = operation;
            button->Dock = DockStyle::Fill;
            button->Margin = System::Windows::Forms::Padding(0, 0, 12, 12);
            button->Padding = System::Windows::Forms::Padding(18, 0, 10, 0);
            button->TextAlign = ContentAlignment::MiddleLeft;
            button->Font = FinanceTheme::Action;
            button->ForeColor = FinanceTheme::TextStrong;
            button->BackColor = Color::White;
            button->UseVisualStyleBackColor = false;
            button->FlatStyle = FlatStyle::Flat;
            button->FlatAppearance->BorderSize = 1;
            button->FlatAppearance->MouseOverBackColor = FinanceTheme::Hover;
            button->FlatAppearance->MouseDownBackColor = FinanceTheme::Pressed;
            button->Enabled = AuthSession::HasPermission(PermView);
            button->Click += gcnew EventHandler(this, &Finance::OnOperationClick);
            return button;
        }

        void SetMetric(int index, Decimal amount)
        {
            if (index < 0 || index >= MetricCount)
                return;

            metricValues[index]->Text =
                L"UGX " + amount.ToString(
                    L"N2",
                    Globalization::CultureInfo::InvariantCulture);
        }

        void ShowRestrictedMetrics()
        {
            for (int i = 0; i < MetricCount; ++i)
                metricValues[i]->Text = L"Restricted";
        }

        void OnOperationClick(Object^ sender, EventArgs^ e)
        {
            Button^ button = dynamic_cast<Button^>(sender);
            if (button == nullptr)
                return;

            FinanceOperation^ operation =
                dynamic_cast<FinanceOperation^>(button->Tag);

            if (operation == nullptr)
                return;

            OpenFinanceOperation(operation);
        }

        void OpenFinanceOperation(FinanceOperation^ operation)
        {
            if (!AuthSession::HasPermission(PermView))
            {
                MessageBox::Show(
                    this,
                    L"You do not have permission to open this section.",
                    L"SchoolCore - Access denied",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning);
                return;
            }

            Form^ view = gcnew Form();

            try
            {
                view->Text = L"SchoolCore - " + operation->Title;
                view->StartPosition = FormStartPosition::CenterParent;
                view->ClientSize = Drawing::Size(900, 560);
                view->MinimumSize = Drawing::Size(800, 500);
                view->BackColor = Color::White;
                view->ShowInTaskbar = false;
                view->MinimizeBox = false;
                view->MaximizeBox = false;

                Panel^ footer = gcnew Panel();
                footer->Dock = DockStyle::Bottom;
                footer->Height = 56;
                footer->Padding =
                    System::Windows::Forms::Padding(28, 10, 28, 10);

                Button^ close = gcnew Button();
                close->Text = L"Close";
                close->Dock = DockStyle::Right;
                close->Width = 110;
                close->DialogResult =
                    System::Windows::Forms::DialogResult::Cancel;

                footer->Controls->Add(close);
                view->CancelButton = close;

                view->Controls->Add(footer);
                view->Controls->Add(
                    CreateHeader(
                        operation->Title,
                        operation->Description,
                        FinanceTheme::Dialog,
                        96,
                        36));

                view->ShowDialog(this);
            }
            finally
            {
                delete view;
            }
        }

        void InitializeComponent()
        {
            this->SuspendLayout();

            this->Text = L"SchoolCore - Fees & Finance";
            this->StartPosition = FormStartPosition::CenterScreen;
            this->AutoScaleDimensions = SizeF(96.0F, 96.0F);
            this->AutoScaleMode =
                System::Windows::Forms::AutoScaleMode::Dpi;
            this->ClientSize = Drawing::Size(1000, 650);
            this->MinimumSize = Drawing::Size(680, 560);
            this->BackColor = Color::White;
            this->DoubleBuffered = true;

            bool canView = AuthSession::HasPermission(PermView);
            metricValues = gcnew array<Label^>(MetricCount);

            TableLayoutPanel^ content = gcnew TableLayoutPanel();
            content->Dock = DockStyle::Fill;
            content->Padding = System::Windows::Forms::Padding(28);
            content->ColumnCount = 1;
            content->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Percent, 100.0F));
            content->RowCount = 5;
            content->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 42.0F));
            content->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 135.0F));
            content->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 45.0F));
            content->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 190.0F));
            content->RowStyles->Add(
                gcnew RowStyle(SizeType::Percent, 100.0F));

            Label^ overview =
                CreateLabel(
                    L"Financial Overview",
                    FinanceTheme::Section,
                    FinanceTheme::TextHeading);
            overview->Dock = DockStyle::Fill;

            TableLayoutPanel^ metrics = gcnew TableLayoutPanel();
            metrics->Dock = DockStyle::Fill;
            metrics->Margin = System::Windows::Forms::Padding(0);
            metrics->ColumnCount = 2;
            metrics->RowCount = 2;

            metrics->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Percent, 50.0F));
            metrics->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Percent, 50.0F));
            metrics->RowStyles->Add(
                gcnew RowStyle(SizeType::Percent, 50.0F));
            metrics->RowStyles->Add(
                gcnew RowStyle(SizeType::Percent, 50.0F));

            array<String^>^ metricNames = gcnew array<String^>
            {
                L"Total Revenue",
                L"Outstanding Fees",
                L"Collected Today",
                L"Pending Approvals"
            };

            for (int i = 0; i < MetricCount; ++i)
                metrics->Controls->Add(
                    CreateMetricCard(metricNames[i], i), i % 2, i / 2);

            if (!canView)
                ShowRestrictedMetrics();

            Label^ operations =
                CreateLabel(
                    L"Finance Operations",
                    FinanceTheme::Section,
                    FinanceTheme::TextHeading);
            operations->Dock = DockStyle::Fill;
            operations->Padding =
                System::Windows::Forms::Padding(0, 8, 0, 0);

            TableLayoutPanel^ actions = gcnew TableLayoutPanel();
            actions->Dock = DockStyle::Fill;
            actions->Margin = System::Windows::Forms::Padding(0);
            actions->ColumnCount = 2;
            actions->RowCount = 2;
            actions->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Percent, 50.0F));
            actions->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Percent, 50.0F));
            actions->RowStyles->Add(
                gcnew RowStyle(SizeType::Percent, 50.0F));
            actions->RowStyles->Add(
                gcnew RowStyle(SizeType::Percent, 50.0F));

            array<FinanceOperation^>^ definitions =
                gcnew array<FinanceOperation^>
            {
                gcnew FinanceOperation(
                    L"Fee Structures",
                    L"Define and manage school fee structures by academic year, term, class and stream."),
                gcnew FinanceOperation(
                    L"Student Charges",
                    L"Review student fee charges, due dates, payment status and outstanding balances."),
                gcnew FinanceOperation(
                    L"Record Payment",
                    L"Record cash, Mobile Money, bank, card or other supported payments with a transaction reference."),
                gcnew FinanceOperation(
                    L"Payment History",
                    L"Review recorded payments, receipts, payment methods and transaction references.")
            };

            array<String^>^ buttonNames = gcnew array<String^>
            {
                L"btnFeeStructures",
                L"btnStudentCharges",
                L"btnRecordPayment",
                L"btnPaymentHistory"
            };

            for (int i = 0; i < definitions->Length; ++i)
            {
                Button^ button =
                    CreateActionButton(definitions[i], buttonNames[i]);

                button->TabIndex = i;
                actions->Controls->Add(
                    button,
                    i % 2,
                    i / 2);
            }

            String^ noteText = L"";

            if (!canView)
            {
                noteText =
                    L"Your account does not have access to financial records. "
                    L"Contact an administrator if you need it.";
            }

            Label^ note =
                CreateLabel(
                    noteText,
                    FinanceTheme::Small,
                    FinanceTheme::TextMuted);
            note->Dock = DockStyle::Fill;
            note->AutoEllipsis = false;
            note->Padding =
                System::Windows::Forms::Padding(0, 18, 0, 0);

            content->Controls->Add(overview, 0, 0);
            content->Controls->Add(metrics, 0, 1);
            content->Controls->Add(operations, 0, 2);
            content->Controls->Add(actions, 0, 3);
            content->Controls->Add(note, 0, 4);

            this->Controls->Add(content);
            this->Controls->Add(
                CreateHeader(
                    L"Fees & Finance",
                    L"Manage fee structures, student charges, payments and financial records.",
                    FinanceTheme::Title,
                    105,
                    42));

            this->ResumeLayout(false);
        }
    };
}
